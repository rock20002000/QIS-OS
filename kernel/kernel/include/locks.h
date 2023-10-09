#ifndef __SYSTEM_LOCKS_H__
#define __SYSTEM_LOCKS_H__

#include <utils/include/dyn_array.h>
#include <types.h>

//Spinlock PRE-DEFINES
#define LOCKED              0x12ab
#define UNLOCKED            0xc0be
#define NO_LOCK_OWNER       -1

struct spinlock
{
    /* data */
    volatile int16_t  lock_owner;
    volatile uint16_t lock_state;    
};
typedef struct spinlock spinlock_t;
//typedef struct spinlock pthread_spinlock_t;

// yieldlock gives up CPU when it cannot get lock.
// Since a thread 'yield' action is involved, it apparently can NOT be used in interrupt context.
struct yieldlock 
{
    volatile uint16_t lock_state;
};
typedef struct yieldlock yieldlock_t;
// arch/(ARCH)/page.c


// spinlock.c
void spinlock_init(spinlock_t * lock);
void lock_acq(spinlock_t * lock);
void lock_rel(spinlock_t * lock);

//int pthread_spin_init( pthread_spinlock_t * spinner,  int pshared );
//int pthread_spin_destroy( pthread_spinlock_t * spinner );
//int pthread_spin_lock( pthread_spinlock_t * spinner );
//int pthread_spin_unlock( spinlock_t * spinner );
//int pthread_spin_trylock( pthread_spinlock_t * spinner );

// yieldlock.c
void yieldlock_init(yieldlock_t* lock);
void yieldlock_acq(yieldlock_t* lock);
void yieldlock_rel(yieldlock_t* lock);

#endif
