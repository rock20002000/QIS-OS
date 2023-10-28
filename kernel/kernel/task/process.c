#include <libc/include/string.h>
#include <libc/include/stdio.h>
#include <platform/include/kirk_task.h>

#include "task.h"
#include "arch_task.h"
#include "kernel.h"

DYNARRAY_EXTERN(PROCESS_t*, process_vector);

static const char gProcPath[] = {PATH_DIR_CHAR, '\0'};

PROCESS_t* process_create(const char* name)
{
    size_t process_id = 0;
    uint8_t step_success_state = 0;

    //Step 1: check if thread is null
    PROCESS_t* process = (PROCESS_t*)kmallocz(sizeof(PROCESS_t));
    if(NULL != process)
    {
        step_success_state++;
    }
    else
    {
        goto exception_exit;
    }

    //Step 2: alloc an id for this process
    process_id = DYNARRAY_INSERT(process_vector, process);
    
    if(INVALID_ID != process_id)
    {
        step_success_state++;
    }
    else
    {
        goto exception_exit;
    }
    process->pid = process_id;
    process->pager = process_id;

    //Step 3: Set process name!
    if (name != NULL) 
    {
        ASSERT(strlen(name) < CONFIG_PROC_NAME_LEN);

        strcpy(process->name, name);
    } 
    else 
    {
        snprintf(process->name, CONFIG_PROC_NAME_LEN, "proc-%u", process_id);
    }

    vm_init(process);

    yieldlock_init(&process->lock);

    process->parent = NULL;

    DYNARRAY_NULLIFY(process->threads);
    DYNARRAY_NULLIFY(process->children);

    id_pool_init(&process->thread_id_pool,MAX_THREAD_PER_PROC);

    process_lookuplist_push(process);

    // set current path is root;
    process->path = gProcPath;

    process->__state = PROCESS_STATE_NORMAL;
    // process->run_time.ticks = 0;
    // process->run_time.cycles = 0;
    get_systime(&process->start_time);
    process->run_start.ticks = process->start_time.ticks;
    process->run_start.cycles = process->start_time.cycles;

    return process;

exception_exit:
    if (process)
    {
        kfree(process);
    }
    return NULL;
}

//bind a thread to an existing process
void process_bind_thread(PROCESS_t* prp, THREAD_t* trp)
{
    trp->process = prp;
    
    yieldlock_acq(&prp->lock);

    trp->tid = prp->threads.length;

    DYNARRAY_PUSHBACK(prp->threads, trp);	//DYNARRAY_INSERT(prp->threads, trp);

    yieldlock_rel(&prp->lock);

    //trp->tid = id_pool_allocate_id(&prp->thread_id_pool);
}


void process_unbind_thread(PROCESS_t* prp, THREAD_t* trp)
{
    yieldlock_acq(&prp->lock);

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(prp->threads, trp);

    yieldlock_rel(&prp->lock);

    trp->process = NULL;

    //todo: check lock ok & free tid from thread_id_pool
}
#ifdef CONFIG_TASK_HEAP
size_t process_heap_size(PROCESS_t* proc)
{
    return TASK_HEAP_SIZE;
}
#endif
error_t process_init_env(PROCESS_t* proc)
{
    error_t err = EOK;
    PROCESS_ENV* env = NULL;
    vaddr_t vaddr = PROC_ENV_ADDR(proc->addr.rodata_end);
#ifdef KERNEL_VMAP_SIZE
    env = (PROCESS_ENV*)kernel_map_user(proc, (const void*)vaddr, PROCESS_ENV_SIZE, KERNEL_MAP_FLAGS);

    if (!env)
    {
        WARN("process_init_env can't map env vaddr: %p\n", vaddr);
        return ERR_NOT_FOUND;
    }
#else
    paddr_t paddr = arch_vm_v2p_addr(&proc->arch.mmu, vaddr);

    if (NULL_PADDR == paddr)
    {
        WARN("process_init_env can't find env vaddr: %p\n", vaddr);
        return ERR_NOT_FOUND;
    }

    env = (PROCESS_ENV*)paddr2ptr(paddr);
#endif
    if (NULL_PADDR != proc->addr.bss_start)
    {
        const char* cmdl = (const char*)PROC_CMDLINE_PTR(env);

        // KDBG("copy cmdline k:%p => u:%p : %s\n", cmdl, proc->addr.bss_start, cmdl);

        err = copy_kernel_data(proc, (void*)proc->addr.bss_start, strlen(cmdl)+1, cmdl);    // copy cmdline
    }
    env->size = 0;
    // fill env data
    env->pid = proc->pid;
    env->vdso_data = proc->addr.vdso_data;
    env->vdso_text = proc->addr.vdso_text;
    env->cycles_per_sec = g_syspage_ptr->systime.cycles_per_sec;
    env->systimer_freq = g_syspage_ptr->systime.timer_freq;
    // fill complete
    env->size = sizeof(*env);
    // init complete
    // KDBG("Process%d, env user:%p\n", proc->pid, vaddr);
#ifdef KERNEL_VMAP_SIZE
    kernel_unmap(env, PROCESS_ENV_SIZE);
#endif
    return err;
}

error_t process_set_path(PROCESS_t* proc, const char* path)
{
    if (!proc)
        proc = current_process(NULL);
    if (!proc)
        return ERR_INVALID_TASK;
    if (!path || !path[0])
        return ERR_INVALID_ARG;
    if (PATH_DIR_CHAR != path[0]) // only support absolute dir now.
        return ERR_INVALID_ARG;
    if (PATH_DIR_CHAR != path[strlen(path) - 1])  // not a dir
        return ERR_INVALID_ARG;

    proc->path = path;
    return EOK;
}

void process_destroy(PROCESS_t* process)
{
    if (process->parent)
    {
        DYNARRAY_REMOVE_BY_VALUE_AND_PACK(process->parent->children, process);
    }

    yieldlock_acq(&process->lock);

    for (size_t i =0 ; i < process->threads.length; i++)
    {
        destroy_thread(process->threads.storage[i], false);
    }
    DYNARRAY_DEL(process->threads);

    for (size_t i =0 ; i < process->children.length; i++)
    {
        process->children.storage[i]->parent = NULL;
    }
    DYNARRAY_DEL(process->children);

    id_pool_deinit(&process->thread_id_pool);

    vm_free(process);
    if (process->fdtab)
        kfree(process->fdtab);

    yieldlock_rel(&process->lock);
}

int fd_set(PROCESS_t* proc, int fd, void* ptr)
{
    if (!proc)
        proc = current_process(NULL);
    if (!proc)
        return ERR_INVALID_TASK;
    if (fd >= PROCESS_FDMAX)
        return ERR_TOO_LARGE;
    if (!proc->fdtab)
        proc->fdtab = (void**)kmallocz(sizeof(void*) * PROCESS_FDMAX);
    if (!proc->fdtab)
        return ERR_NO_MEMORY;
    if (fd < 0)
    {
        for (int i = 0; i < PROCESS_FDMAX; i++)
        {
            if (!proc->fdtab[i])
            {
                fd = i;
                break;
            }
        }
    }
    if (fd < 0)
        return ERR_NO_MEMORY;
    proc->fdtab[fd] = ptr;
    // KDBG("fd_set, proc=%p, fdtab=%p, fd=%x, ptr=%p\n", proc, proc->fdtab, fd, proc->fdtab[fd]);
    return fd;
}

void* fd_get(PROCESS_t* proc, int fd)
{
    if (fd < 0 || fd >= PROCESS_FDMAX)
        return NULL;
    if (!proc)
        proc = current_process(NULL);
    if (!proc)
        return NULL;
    if (!proc->fdtab)
        return NULL;
    // KDBG("fd_get, proc=%p, fdtab=%p, fd=%x, ptr=%p\n", proc, proc->fdtab, fd, proc->fdtab[fd]);
    return proc->fdtab[fd];
}

pid_t get_user_pid(PROCESS_t* proc)
{
    if (!proc)
        return NULL_ID;
    if (is_root_process(proc))
        return proc->pid;
    return proc->pid + USR_PID_START;
}

THREAD_t* get_main_thread(PROCESS_t* proc)
{
    if (!proc)
        return NULL;
    return proc->threads.storage[0];
}

static void get_ps_info(PROCESS_t* proc, ps_info_t* ps_info)
{
    strcpy(ps_info->name, proc->name);
    ps_info->pid = get_user_pid(proc);
    if (proc->parent)
        ps_info->ppid = get_user_pid(proc->parent);
    else
        ps_info->ppid = 0;
    ps_info->tid = get_main_thread(proc)->tid;
    ps_info->vsz = proc->ms.vmem_pages;
    ps_info->rss = proc->ms.pmem_pages;
    ps_info->code = proc->addr.code_pages;
    ps_info->data = proc->addr.data_pages;
#if 0
    ps_info->start = proc->start_time.ticks;
    ps_info->time = proc->run_time.ticks;
#else
    ps_info->start = systime_to_us(&proc->start_time);
    ps_info->time = systime_to_us(&proc->run_time);
#endif
    ps_info->priority = get_main_thread(proc)->priority;
    ps_info->stat = proc->__state;
    ps_info->threads = proc->thread_id_pool.count;
}

error_t get_sysinfo(sysinfo_type_e type, uintptr_t param, void* p_sysinfo, size_t size)
{

    if (p_sysinfo && size)
    {
        p_sysinfo = kernel_map_user(NULL, p_sysinfo, size, KERNEL_MAP_WFLAGS);
        if (!p_sysinfo)
        {
            KWARN("kernel_map_user failed in map sysinfo!\n");
            return ERR_NO_MEMORY;
        }
    }
    else
    {
        p_sysinfo = NULL;
        size = 0;
    }


    int err = ERR_NOT_FOUND;

    if (SYSINFO_VERSION == type)
    {
        if (p_sysinfo)
            strncpy(p_sysinfo, g_version, size);
        err = size > (strlen(g_version) + 1) ? size : (strlen(g_version) + 1);
    }
    else if (SYSINFO_MEMORY == type)
    {
        if (p_sysinfo && size >= sizeof(mem_info_t))
            get_meminfo((mem_info_t*)p_sysinfo);
        err = size > sizeof(mem_info_t) ? size : sizeof(mem_info_t);
    }
    else if (SYSINFO_PROCESS == type)
    {
        pid_t pid = (pid_t)param;

        if (pid != NULL_ID)
        {
            if (pid)
                pid -= USR_PID_START;
            if (!is_active_process(getProcessWithPid(pid)))
                err = ERR_INVALID_ARG;
            else
            {
                if (p_sysinfo)
                    get_ps_info(getProcessWithPid(pid), (ps_info_t*)p_sysinfo);
                err = sizeof(ps_info_t);
            }
        }
        else
        {
            int active = 0, max_infos = process_vector.length;
            ps_info_t* ps_info = (ps_info_t*)p_sysinfo;

            if (size)
                max_infos =  size / sizeof(ps_info_t);
            for (int i = 0; i < process_vector.length; i++)
            {
                PROCESS_t* proc = process_vector.storage[i];

                if (is_active_process(proc))
                {
                    if (ps_info)
                    {
                        get_ps_info(proc, ps_info++);
                    }
                    active++;
                    if (active >= max_infos)
                        break;
                }
            }
            if (active)
                err = active * sizeof(ps_info_t);
        }
    }

    if (p_sysinfo)
        kernel_unmap(p_sysinfo, size);

    return err;
}
