
#include <libc/include/string.h>
#include "kernel.h"

void spinlock_init(struct spinlock * lock)
{
    memset(lock, 0x0, sizeof(struct spinlock));
    lock->lock_owner = 0xff;  
    lock->lock_state = UNLOCKED;
}

void lock_acq(struct spinlock * lock)
{
    while (!__sync_val_compare_and_swap(&lock->lock_state, UNLOCKED, LOCKED)) {
    	arch_pause();
    }
    lock->lock_owner = get_cpuvar()->cpu_id;
}

void lock_rel(struct spinlock * lock)
{
    lock->lock_owner = NO_LOCK_OWNER;

    __sync_val_compare_and_swap(&lock->lock_state, LOCKED, UNLOCKED);
}

/*int pthread_spin_init( pthread_spinlock_t * spinner,  int pshared ){
	spinlock_init(spinner);
	return 1;
}

int pthread_spin_destroy( pthread_spinlock_t * spinner ){
	return 1;
}

int pthread_spin_lock( pthread_spinlock_t * spinner ){
	lock_acq(spinner);
}

int pthread_spin_unlock( pthread_spinlock_t * spinner ){
	lock_rel(spinner);
}

int pthread_spin_trylock( pthread_spinlock_t * spinner ){
	if(spinner == NULL) return -1;
    if (!__sync_val_compare_and_swap(&spinner->lock_state, UNLOCKED, LOCKED)) {
        return -1;
    }
    spinner->lock_owner = get_cpuvar()->cpu_id;
    return 0;
}
*/
