#include <stdbool.h>
#include <types.h>
#include <elf_vDSO.h>
#include <libc/include/string.h>
#include <platform/include/kirk_task.h>
#include <utils/include/print_macros.h>

 extern PROCESS_ENV g_procEnv;

bool elf_checkFile(Elf64_Ehdr *elf)
{
    return (
               elf->e_ident[0] == '\177' &&
               elf->e_ident[1] == 'E'    &&
               elf->e_ident[2] == 'L'    &&
               elf->e_ident[3] == 'F'    &&
               elf->e_ident[4] == 2
           );
}

v_region_t elf_getMemoryBounds(Elf64_Ehdr *elf)
{
    v_region_t  elf_reg;
    vaddr_t     sect_start;
    vaddr_t     sect_end;
    uint32_t    i;
    Elf64_Phdr_t *phdr = (Elf64_Phdr_t *)((paddr_t)elf + elf->e_phoff);

    elf_reg.start = 0x7fffffffffffffffUL;
    elf_reg.end = 0;

    for (i = 0; i < elf->e_phnum; i++) {
        if (phdr[i].p_memsz > 0) {
            sect_start = phdr[i].p_vaddr;
            sect_end = sect_start + phdr[i].p_memsz;
            if (sect_start < elf_reg.start) {
                elf_reg.start = sect_start;
            }
            if (sect_end > elf_reg.end) {
                elf_reg.end = sect_end;
            }
        }
    }

    return elf_reg;
}

void elf_load(Elf64_Ehdr *elf, uint32_t offset)
{
    paddr_t     src;
    paddr_t     dst;
    uint64_t    len;
    uint32_t    i;
    Elf64_Phdr_t *phdr = (Elf64_Phdr_t *)((paddr_t)elf + elf->e_phoff);

    for (i = 0; i < elf->e_phnum; i++) {
        src = (paddr_t)elf + phdr[i].p_offset;
        dst = phdr[i].p_vaddr + offset;
        len = phdr[i].p_filesz;
        memcpy((void *)dst, (char *)src, len);
        dst += len;
        memset((void *)dst, 0, phdr[i].p_memsz - len);
    }
}

#define OK_TYPES (1<<STT_NOTYPE | 1<<STT_OBJECT | 1<<STT_FUNC | 1<<STT_COMMON)
#define OK_BINDS (1<<STB_GLOBAL | 1<<STB_WEAK | 1<<STB_GNU_UNIQUE)


Elf64Entry_STable * getSHStrTab(Elf64_Ehdr * ehdr)
{
    int i = 0, shnum = ehdr->e_shnum;
    char* tmpelf= (char*)ehdr;
 //   xprintf("[%s:%d] ehdr[0]:0x%p ehdr[1]:0x%p ehdr[2]:0x%p ehdr[3]:0x%p\n",__func__,__LINE__,
   //         tmpelf[0], tmpelf[1], tmpelf[2], tmpelf[3]    );
  //  xprintf("[%s:%d] ehdr:0x%p \n",__func__,__LINE__,  ehdr );
    char* nameStr = NULL;
    Elf64Entry_STable * shdr = (Elf64Entry_STable *)((char *)ehdr + ehdr->e_shoff);
 //   xprintf("[%s:%d] shdr:0x%p\n",__func__,__LINE__,shdr);
  //  xprintf("[%s:%d] Elf64_Shdr size:0x%p\n",__func__,__LINE__, sizeof(Elf64Entry_STable));
    for (i = 0; i < shnum; i++, shdr++)
    {
     //   xprintf("[%s:%d] shdr[%d]->sh_type:%d\n",__func__,__LINE__,i,shdr->sh_type);
        if (shdr->sh_type == SHT_STRTAB)
        {
         //   xprintf("[%s:%d] shnum:%d\n",__func__,__LINE__,shnum);
            if (shdr->sh_name >= shdr->sh_size)
            {
                continue;
            }

            nameStr = (char *)ehdr + shdr->sh_offset + shdr->sh_name;
          //   xprintf("[%s:%d] nameStr:%s\n",__func__,__LINE__,nameStr);
            if (strcmp(nameStr, SECTION_NAME_SHSTRTAB) == 0)
            {
          //      xprintf("[%s:%d] found nameStr:%s\n",__func__,__LINE__,nameStr);
                return shdr;
            }
        }
    }
    xprintf("[%s:%d]\n",__func__,__LINE__);
    return NULL;
}

char* getStrTabStr(Elf64_Ehdr * ehdr, Elf64_Shdr * strtabhdr, int idx)
{
    if (idx < strtabhdr->sh_size)
    {
        return ((char *)ehdr + strtabhdr->sh_offset + idx);
    }

    return NULL;
}

Elf64_Shdr * getSHdrByName(Elf64_Ehdr * ehdr, const char * name, Elf64_Word sh_type)
{
    int i = 0, shnum = ehdr->e_shnum;
    char * nameStr = NULL;
    Elf64_Shdr * shstrtabhdr = NULL;
    Elf64_Shdr * shdr = (Elf64_Shdr *)((char *)ehdr + ehdr->e_shoff);
  //  xprintf("[%s:%d] ehdr->e_type:%d\n",__func__,__LINE__,ehdr->e_type);
  //  xprintf("[%s:%d] ehdr->e_shoff:%d\n",__func__,__LINE__,ehdr->e_shoff);

    shstrtabhdr = getSHStrTab(ehdr);
 //   xprintf("[%s:%d]\n",__func__,__LINE__);
    if (shstrtabhdr == NULL)
    {
        xprintf("getSHdrByName Get shstrtabhdr failed name:%s sh_type:%d\n", name, sh_type);
        return NULL;
    }

  //  xprintf("[%s:%d]\n",__func__,__LINE__);

    shdr = (Elf64_Shdr *)((char *)ehdr + ehdr->e_shoff);
    for (i = 0; i < shnum; i++, shdr++)
    {
        if (shdr->sh_type == sh_type)
        {
            nameStr = getStrTabStr(ehdr, shstrtabhdr, shdr->sh_name);
        //    xprintf("[%s:%d] nameStr:%s\n",__func__,__LINE__, nameStr);
            if (nameStr && strcmp(nameStr, name) == 0)
            {
       //         xprintf("[%s:%d] shdr:0x%p\n",__func__,__LINE__, shdr);
                return shdr;
            }
        }
    }
    xprintf("[%s:%d] shdr:0x%p\n",__func__,__LINE__, shdr);
    xprintf("\n GetSHdrByName Get section header failed name:%s sh_type:%d\n", name, sh_type);
    return NULL;
}

int parseSegment(Elf64_Ehdr * ehdr)
{
    int i = 0, j = 0;
    Elf64_Shdr * shdr = NULL;
    Elf64_Phdr * phdr = (Elf64_Phdr *)((char *)ehdr + ehdr->e_phoff);
    Elf64_Shdr * shstrtabhdr = NULL;

    shstrtabhdr = getSHStrTab(ehdr);
    if (shstrtabhdr == NULL)
    {
        printf("parseSectionHeader Get shstrtabhdr failed\n");
        return -1;
    }

    // printf("\nSection to Segment mapping:\n");
    // printf("  Segment Sections...\n");

    for (i = 0; i < ehdr->e_phnum; i++, phdr++)
    {
        // printf("   %2.2d     ", i);
        
        shdr = (Elf64_Shdr *)((char *)ehdr + ehdr->e_shoff);
        for (j = 1; j < ehdr->e_shnum; j++, shdr++)
        {
            if (shdr->sh_size <= 0)
            {
                continue;
            }

            if (shdr->sh_flags & SHF_ALLOC)
            {
                if ((shdr->sh_addr < phdr->p_vaddr) || (shdr->sh_addr + shdr->sh_size > phdr->p_vaddr + phdr->p_memsz))
                {
                    continue;
                }
            }
            else
            {
                if ((shdr->sh_offset < phdr->p_offset) || (shdr->sh_offset + shdr->sh_size > phdr->p_offset + phdr->p_filesz))
                {
                    continue;
                }
            }
            // printf("%s ",getStrTabStr(ehdr, shstrtabhdr, shdr->sh_name));
        }
        // printf("\n");
    }

    return 0;
}


void *__vdsosym( const char *name)
{
	size_t i = 0;
    int  sym_cnt = 0;
    Elf64_Shdr * shdrStrTab = NULL;
    Elf64_Shdr * shdrSymTab = NULL;
    Elf64_Sym * sym = NULL;      
	
	Ehdr *ehdr = (Ehdr *)g_procEnv.vdso_text;
 //   xprintf("$$$$$$$$ [%s:%d] eh->e_phoff:%d  \n",__func__,__LINE__,ehdr->e_phoff );
   
	Phdr *ph = (Phdr *)((char *)ehdr + ehdr->e_phoff);
    size_t *dynv=0, base= g_procEnv.vdso_text;
    for (i=0; i<ehdr->e_phnum; i++, ph=(void *)((char *)ph+ehdr->e_phentsize)) {
   // xprintf("$$$$$$$$$$$$$$$$[%s:%d] ph->p_type:%d \n",__func__,__LINE__, ph->p_type);
		if (ph->p_type == PT_LOAD){
            
			base = (size_t)ehdr + ph->p_offset - ph->p_vaddr;
      //       xprintf("$$$ PT_LOAD [%s:%d]  ehdr:0x%p\n",__func__,__LINE__, ehdr );
      //      xprintf("$$$ PT_LOAD [%s:%d]  ph->p_offset:0x%p\n",__func__,__LINE__, ph->p_offset );
      //       xprintf("$$$ PT_LOAD [%s:%d]   ph->p_vaddr:0x%p\n",__func__,__LINE__, ph->p_vaddr );
       //       xprintf("$$$ PT_LOAD [%s:%d]   base:0x%p\n",__func__,__LINE__, base );
              break;
        }
		else if (ph->p_type == PT_DYNAMIC)
        {
			dynv = (void *)((char *)ehdr + ph->p_offset);
       //     xprintf("$$$ PT_DYNAMIC [%s:%d]  dynv:0x%p\n",__func__,__LINE__, dynv );
       //     xprintf("$$$ PT_DYNAMIC [%s:%d] ph->p_offset:0x%p\n",__func__,__LINE__, ph->p_offset );
        }
	}

    shdrSymTab = getSHdrByName(ehdr, SECTION_NAME_SYMTAB, SHT_SYMTAB);
    if (shdrSymTab == NULL)
    {
        xprintf("parseSymTab getSymTab faild\n");
        return NULL;
    }
    shdrStrTab = getSHdrByName(ehdr, SECTION_NAME_STRTAB, SHT_STRTAB);
    if (shdrStrTab == NULL)
    {
        xprintf("parseSymTab getStrTab faild\n");
        return NULL;
    }
    sym_cnt = shdrSymTab->sh_size / sizeof(Elf64_Sym);
   // xprintf("\n[%s:%d] \n",__func__,__LINE__ );
    // xprintf("\nSymbol table '%s' contains %d entries:\n", SECTION_NAME_SYMTAB, sym_cnt);
    // xprintf("   Num:    Value  Size Type    Bind   Vis      Ndx   Name\n");

    sym = (Elf64_Sym *)((char *)ehdr + shdrSymTab->sh_offset);

    for (i = 0; i < sym_cnt; i++, sym++)
    {
      //  xprintf("\n[%s:%d] \n",__func__,__LINE__ );
      //  xprintf("%s", getStrTabStr(ehdr, shdrStrTab, sym->st_name));
        if (strcmp(name, getStrTabStr(ehdr, shdrStrTab, sym->st_name))==0){
        // xprintf("\n[%s:%d] i:%d base:0x%p sym->st_value:0x%p \n",__func__,__LINE__, 
        //               i,  base, sym->st_value );
            return (void *)(base + sym->st_value);
        }
		//if (versym) 	continue;
		
      //  xprintf("\n[%s:%d] \n",__func__,__LINE__ );
    }

    xprintf("[%s:%d]  \n",__func__,__LINE__ );
	return NULL;
}



size_t LocVdsoStart(size_t vdsoStart, const char *elfHead, const size_t len)
{
    char *head = NULL;
    s32 i;
    s32 loop;
     
    // xprintf("\n %s vdsoStart:0x%p\n", __func__,vdsoStart);

    for (i = 1; i <= MAX_PAGES; i++) {
      //  xprintf("\n $$$$$$ head:0x%p\n", head);
        head = (char *)((vaddr_t)vdsoStart);
        for (loop = len; loop >= 1; loop--) {
      //  xprintf("%d loop:%d *head:0x%p\n",__LINE__, loop, *head);
    //    xprintf("%d loop:%d *(elfHead + len - loop):0x%p\n",__LINE__, loop, *(elfHead + len - loop));
        if (*head++ != *(elfHead + len - loop)) {
                break;
            }
      //  xprintf("%d loop:%d *head:0x%p",__LINE__, loop, *head);
        }
     //   xprintf("\n line: %d  loop:%d \n", __LINE__,loop);

        if (loop) {
            vdsoStart -= PAGE_SIZE;
        } else {
            break;
        }
    }

   // xprintf("\n line:%d \n", __LINE__);

    if (i > MAX_PAGES) {
        return 0;
    }
   //  xprintf("\n line:%d \n", __LINE__);
   // return (vdsoStart - PAGE_SIZE);
   return vdsoStart;
}





