/*
 * lock.c
 *
 *  Created on: 2022-9-20
 *      Author: Test
 */
#include <libc/include/string.h>
#include <platform/include/sync_lock.h>	//	#include "../include/sync_lock.h"
#include <platform/include/syscall_vec.h>

/*

#define barrier() __asm__ __volatile__("": : :"memory")
void spinlock_init_rawuser(struct spinlock_user * lock, int iInitLockStatus)
{
    memset(lock, 0x0, sizeof(struct spinlock_user));
    //	lock->cpu = 0xff;
    lock->lock = iInitLockStatus;
}

void spinlock_acquire_rawuser(struct spinlock_user * lock)
{
    while (xchg32(&lock->lock, 1) != 0);
    barrier();
    //lock->cpu = cpu();
}

int spinlock_tryacquire_rawuser(struct spinlock_user * lock)
{
    int iRslt = xchg32(&lock->lock, 1);
    barrier();
    return iRslt;
    //lock->cpu = cpu();
}


void spinlock_release_rawuser(struct spinlock_user * lock)
{
	//	ASSERT(lock->cpu == cpu() && lock->lock == 1);
    //lock->cpu = 0xff;
    barrier();
    xchg32(&lock->lock, 0);
    //	ASSERT(xchg32(&lock->lock, 0) == 1);
}
*/

#define LOCKED              0x12ab
#define UNLOCKED            0xc0be

void spinlock_init(pthread_spinlock_t * lock)
{
    memset(lock, 0x0, sizeof(pthread_spinlock_t));
    //lock->lock_owner = 0xff;
    lock->lock = UNLOCKED;
}

void lock_acq(pthread_spinlock_t * lock)
{
    while (!__sync_val_compare_and_swap(&lock->lock, UNLOCKED, LOCKED)) {
    	;//arch_pause();
    }
    //lock->lock_owner = get_cpuvar()->cpu_id;
}

int lock_try_acq(pthread_spinlock_t * lock)
{
    return (__sync_val_compare_and_swap(&lock->lock, UNLOCKED, LOCKED)==LOCKED);
    //lock->lock_owner = get_cpuvar()->cpu_id;
}

void lock_rel(pthread_spinlock_t * lock)
{
    //lock->lock_owner = NO_LOCK_OWNER;

    __sync_val_compare_and_swap(&lock->lock, LOCKED, UNLOCKED);
}

int pthread_spin_init( pthread_spinlock_t * spinner,  int pshared ){
	spinlock_init(spinner);	//spinlock_init_rawuser(spinner, pshared);
	return EOK;	//1;
}

int pthread_spin_destroy( pthread_spinlock_t * spinner ){
	return EOK;	//1;
}

int pthread_spin_lock( pthread_spinlock_t * spinner ){
	if(spinner==NULL) return -1;
	lock_acq(spinner);	//	spinlock_acquire_rawuser(spinner);
	return EOK;	//1;
}

int pthread_spin_unlock( pthread_spinlock_t * spinner ){
	if(spinner==NULL) return -1;
	lock_rel(spinner);		//spinlock_release_rawuser(spinner);
	return EOK;	//1;
}

int pthread_spin_trylock( pthread_spinlock_t * spinner ){
	return lock_try_acq(spinner);	//spinlock_tryacquire_rawuser(spinner);
	//if(spinner == NULL) return -1;
    //if (!__sync_val_compare_and_swap(&spinner->lock_state, UNLOCKED, LOCKED)) {
    //    return -1;
    //}
    //spinner->lock_owner = get_cpuvar()->cpu_id;
    //return 0;
}

//
//int 	pthread_mutex_init(pthread_mutex_t* pMutex, const pthread_mutexattr_t* attr ){
//	if(pMutex==NULL) return -1;
//	return syscall(SYSCALL_MUTEX_INIT, (long)pMutex, (long)attr, (long)0, (long)0, (long)0);
//}
//int 	pthread_mutex_lock( pthread_mutex_t* pMutex ){
//	if(pMutex==NULL) return -1;
//	return syscall(SYSCALL_MUTEX_LOCK, (long)pMutex, (long)0, (long)0, (long)0, (long)0);
//}
//int 	pthread_mutex_trylock( pthread_mutex_t* pMutex ){
//	if(pMutex==NULL) return -1;
//	return syscall(SYSCALL_MUTEX_TRYLOCK, (long)pMutex, (long)0, (long)0, (long)0, (long)0);
//}
//int 	pthread_mutex_unlock( pthread_mutex_t* pMutex ){
//	if(pMutex==NULL) return -1;
//	return syscall(SYSCALL_MUTEX_UNLOCK, (long)pMutex, (long)0, (long)0, (long)0, (long)0);
//}
//int 	pthread_mutex_destroy( pthread_mutex_t* pMutex ){
//	if(pMutex==NULL) return -1;
//	return syscall(SYSCALL_MUTEX_DESTROY, (long)pMutex, (long)0, (long)0, (long)0, (long)0);
//}
//


int 	pthread_mutex_init(pthread_mutex_t* pMutex, const pthread_mutexattr_t* attr ){
	if(pMutex==NULL) return -1;
	return pthread_spin_init(&(pMutex->wait_lock), (attr!=NULL)?attr->__mutexkind:0);
}
int 	pthread_mutex_lock( pthread_mutex_t* pMutex ){
	return pthread_spin_lock(&(pMutex->wait_lock));
}
int 	pthread_mutex_trylock( pthread_mutex_t* pMutex ){
	return pthread_spin_trylock(&(pMutex->wait_lock));
}
int 	pthread_mutex_unlock( pthread_mutex_t* pMutex ){
	return pthread_spin_unlock(&(pMutex->wait_lock));
}
int 	pthread_mutex_destroy( pthread_mutex_t* mutex ){
	return EOK;	//1;
}
