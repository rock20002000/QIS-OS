#include "kernel.h"


void yieldlock_init(struct yieldlock* lock) 
{
    lock->lock_state = UNLOCKED;
}

void yieldlock_acq(struct yieldlock* lock) 
{
    while (!__sync_val_compare_and_swap(&lock->lock_state, UNLOCKED, LOCKED)) 
    {
        // Todo: do yield to release CPU resource
        // yield();
        arch_pause();	//__asm__ __volatile__("pause");
    }
}


void yieldlock_rel(struct yieldlock* lock) 
{
    __sync_val_compare_and_swap(&lock->lock_state, LOCKED, UNLOCKED);
}
