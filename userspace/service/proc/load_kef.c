#include <boot/include/kirk_image_def.h>
#include <libc/include/stdlib.h>
#include <libc/include/string.h>
#include <platform/include/platform.h>
#include <utils/include/print_macros.h>

#define ELF_PF_X    (1L)    // (1L << 32)
#ifdef ELF_PF_X
 #define ELF_PF_W   (ELF_PF_X << 1)
#endif

#define get_arg(s)  strtoul(s, NULL, 0)

static unsigned char app_nums;
static const application_hdr_t* p_header;
static const unsigned char* kef_data;
static vaddr_t lma_off;

typedef struct kef_map_info
{
    vaddr_t start;    
    vaddr_t end;    
    size_t  size;    
} kef_map_info_t;

static int load_kef_section(const program_hdr_t* sect, p_proc_create_attr_t pca, kef_map_info_t* p_map)
{
    vaddr_t vaddr;
    uint32_t attr = sect->attr >> 32;
    size_t size = sect->file_size;
    int idx = pca->map.nums;

    pca->map.params[idx].vaddr = sect->vma;
    pca->map.params[idx].flags = MAP_SHARED | PROT_READ;

    if (attr & ELF_PF_X)
    {
        pca->map.params[idx].flags |= PROT_EXEC;
    }
    if (attr & ELF_PF_W)
    {
        pca->map.params[idx].flags |= PROT_READ_WRITE;
    }

    if (size)
    {
        vaddr = mmap(NULL_VADDR, size, PROT_READ_WRITE, MAP_SHARED, NULL_ID, 0);
        if (MAP_FAILED == vaddr)
        {
            return ERR_NO_MEMORY;
        }
        memcpy((void*)vaddr, (const void*)(sect->lma + lma_off), size);
        pca->map.params[idx].size = size;
        pca->map.params[idx].id = 0;  // proc
        pca->map.params[idx].vmem = vaddr;
    }
    else
    {
        size = sect->mem_size;
        pca->map.params[idx].size = size;
        pca->map.params[idx].flags |= MAP_ANON | MAP_LAZY; // bss
        pca->map.params[idx].id = NOFD;
        pca->map.params[idx].vmem = 0;
    }
    // xprintf("load_kef_section: vaddr:0x%x + 0x%x, flags:%x\n",
    //     pca->map.params[idx].vmem,
    //     pca->map.params[idx].size,
    //     pca->map.params[idx].flags);
    if (p_map && size)
    {
        p_map->start = vaddr;
        p_map->end = vaddr + size;
        p_map->size = sect->file_size;
    }
    pca->map.nums++;
    return EOK;
}

static void unload_kef_section(const program_hdr_t* sect)
{
    if (!sect->file_size)    
        return;
}

// #define PCA_BUF_SIZE    512
static int load_kef_app(const application_hdr_t* app, pid_t parent, int stat, const char* cmdl)
{
    error_t err = EOK;
    bool copy_cmdl= true;
    kef_map_info_t map[CONFIG_MAX_MODULES-1];
    p_proc_create_attr_t pca;
    size_t pca_size = sizeof(*pca) + (app->ph_nmbers-1)*sizeof(pca->map.params[0]);
#ifdef PCA_BUF_SIZE
    if (pca_size > PCA_BUF_SIZE)
    {
        xprintf("load_kef_app(%s) proc_create_attr size %d > %d !!\n", app->name, pca_size, PCA_BUF_SIZE);
        return ERR_NO_MEMORY;
    }

    char pca_buf[PCA_BUF_SIZE];

    pca = (p_proc_create_attr_t)pca_buf;
#else
    pca = malloc(pca_size);
    if (!pca)
    {
        xprintf("malloc proc_create_attr_t:0x%x failed\n", pca_size);
        return ERR_NO_MEMORY;
    }
#endif
    pca->parent = parent;
    pca->stat = stat;
    strncpy(pca->name, app->name, sizeof(pca->name));
    pca->map.entry = app->entry;
    pca->map.nums = 0;
    xprintf("load_kef_app pca: %p\n", pca);
    for(uint8_t i = 0; i < app->ph_nmbers && EOK == err; i++)
    {
        map[i].start = map[i].end = NULL_PADDR;
        map[i].size = 0;
        err = load_kef_section(&app->prog_hdr[i], pca, &map[i]);
    }

    if (EOK == err)
    {
        for(uint8_t i = 0; i < app->ph_nmbers && EOK == err; i++)
        {
            if (copy_cmdl && map[i].end != NULL_PADDR)
            {
                if (!(app->prog_hdr[i].attr & ELF_PF_W))    // first rodata
                {
                    char* proc_cmdl = (char*)PROC_CMDLINE_ADDR(map[i].end);

                    // xprintf("cmdl%d: %p => %p, %s\n", i, map[i].end, proc_cmdl, app->name);
                    strcpy(proc_cmdl, app->name);

                    if (cmdl)
                    {
                        char* param = proc_cmdl + strlen(proc_cmdl);

                        *param++ = ' ';
                        strcpy(param, cmdl);    // append parameters
                    }
                    copy_cmdl = false;
                }
            }
        }
        // xprintf("exec: stat=%d, name=%s, map.nums=%d, pca_size:0x%x\n",
        //     pca->stat, pca->name, pca->map.nums, pca_size);
        err = init_process_main_thread(pca);
    }

    for(uint8_t i = 0; i < app->ph_nmbers; i++)
    {
        if (!map[i].size)
            break;
        munmap((void*)map[i].start, map[i].size);
    }
#ifdef PCA_BUF_SIZE
    free(pca);
#endif
    return err;
}

void load_kef_init(int argc, char* argv[])
{
    // app_nums = 0;
    // p_header = NULL;
    // lma_off = 0;

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-apps"))
        {
            app_nums = (unsigned char)atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-header"))
        {
            p_header = (const application_hdr_t*)get_arg(argv[++i]);
        }
        else if (!strcmp(argv[i], "-lma_off"))
        {
            lma_off = (vaddr_t)get_arg(argv[++i]);
        }
    }
    xprintf("apps: %d, header: %p, lma_off: %p\n", app_nums, p_header, lma_off);
    for (uint8_t i = 0; i < app_nums; i++)
    {
        xprintf(" app[%d] = %s\n", i, p_header[i].name);
    }
}

int load_kef_by_id(unsigned char app_id, pid_t parent, int run, const char* cmdl)
{
    if (app_id >= app_nums)
        return ERR_NOT_FOUND;

    return load_kef_app(&p_header[app_id], parent, run, cmdl);
}

int load_kef_by_name(const char* app_name, pid_t parent, int run, const char* cmdl)
{
    for (uint8_t i = 0; i < app_nums; i++)
    {
        if (!strcmp(app_name, p_header[i].name))
            return load_kef_app(&p_header[i], parent, run, cmdl);
    }
    return ERR_NOT_FOUND;
}
