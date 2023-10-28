#include <libc/include/string.h>
#include <libc/include/stdio.h>

#include "kernel.h"
#include "task.h"
#include "arch_task.h"

/* Copy n(=size) bytes from userspace to kernel */
void ipc_copy_in(THREAD_t* p_thread, vaddr_t src, vaddr_t dst, size_t size)
{
    uint16_t remain_bytes_in_page = PAGE_SIZE - (src & 0xFFF);
    paddr_t source_paddr = arch_vm_v2p_addr(&p_thread->process->arch.mmu, src);
    vaddr_t dest_vaddr = dst;

    if(remain_bytes_in_page >= size)
    {     
        memcpy((void*)dest_vaddr, (void*)source_paddr, size);
    }
    else
    {
        unsigned copied_bytes = remain_bytes_in_page;

        memcpy((void*)dest_vaddr, (void*)source_paddr, remain_bytes_in_page);

        for( ; copied_bytes + PAGE_SIZE > size; copied_bytes += PAGE_SIZE )
        {
            source_paddr = arch_vm_v2p_addr(&p_thread->process->arch.mmu, src + copied_bytes);

            memcpy((void*)(dest_vaddr + copied_bytes), (void*)source_paddr, PAGE_SIZE);
        }

        unsigned remain_bytes = size - copied_bytes;

        source_paddr = arch_vm_v2p_addr(&p_thread->process->arch.mmu, src + copied_bytes);

        memcpy((void*)(dest_vaddr + copied_bytes), (void*)source_paddr, remain_bytes);
    }

}
