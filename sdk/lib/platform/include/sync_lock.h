/*
 * synclock.h
 *
 *  Created on: 2022-9-20
 *      Author: Test
 */

#ifndef SDK_LIB_PLATFORM_INCLUDE_SYNC_LOCK_H_
#define SDK_LIB_PLATFORM_INCLUDE_SYNC_LOCK_H_
#include <types.h>
#include <platform/include/compiler_types.h>
#include <platform/include/kirk_task.h>
#include <arch_common.h>



void 	spinlock_init_rawuser(struct spinlock_user * lock, int iInitLockStatus);
void 	spinlock_acquire_rawuser(struct spinlock_user * lock);
int 	spinlock_tryacquire_rawuser(struct spinlock_user * lock);
void 	spinlock_release_rawuser(struct spinlock_user * lock);

int 	pthread_spin_init( pthread_spinlock_t * spinner,  int pshared );
int 	pthread_spin_destroy( pthread_spinlock_t * spinner );
int 	pthread_spin_lock( pthread_spinlock_t * spinner );
int 	pthread_spin_unlock( pthread_spinlock_t * spinner );
int 	pthread_spin_trylock( pthread_spinlock_t * spinner );

int 	pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr );
int 	pthread_mutex_lock( pthread_mutex_t* mutex );
int 	pthread_mutex_trylock( pthread_mutex_t* pMutex );
int 	pthread_mutex_unlock( pthread_mutex_t* mutex );
int 	pthread_mutex_destroy( pthread_mutex_t* mutex );


#endif /* SDK_LIB_PLATFORM_INCLUDE_SYNC_LOCK_H_ */
