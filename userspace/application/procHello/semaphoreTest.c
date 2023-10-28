#include <types.h>
#include <semaphore.h>

#include <pthread.h>

#include <platform/include/syscalls.h>
#include <platform/include/mmap.h>
#include <stat.h>
#include <libc/include/string.h>


enum { SEM_SELF, SEM_THREAD, SEM_PROCESS, SEM_REMOTE };

sem_t sem, semaphore;
int SyncTypeCreate(unsigned long __type, sem_t* semp, const struct _sync_attr *__attr);
int sem_init(sem_t *sem, int pshared, unsigned int value) {
	struct _sync_attr		attr;


	attr.__protocol = (int)value;
	attr.__flags = pshared ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE;
	attr.__prioceiling = 0;

	xprintf("\n attr.__protocol:%d \n", attr.__protocol);
	
	if(SyncTypeCreate(_NTO_SYNC_SEM, (sem_t*)sem, &attr) == -1) {
		
		return(-1);
	}
	return(0);
}

#define SEM_NAME "/dev/sem"
int SyncTypeTake(sem_t* semp);
static int semTake(sem_t *sem) {
	// Is it a named semaphore.
	return(SyncTypeTake(sem));
}

int semPost(sem_t *sem) {
	return(SyncSemPost(sem));
}

int semDestroy(sem_t *sem) {
	// Force an EINVAL if the semaphore is "statically" initialized.
	return(SyncDestroy( sem));
}

int sem_getvalue(sem_t *sem)
{
    if (!sem )
    {
        return -1;
    }
    return  SyncGetValue(sem);   
}

void semaphoreTest( )
{
    int		loop, mode, named, unavail, fd, opt, result;
   pid_t    child;
	sem_t* semp, sem;
	int semValue;
	
   char		pass[80], fqpn[128], semname[256], exename[256], *arg[4];
   mode = SEM_PROCESS, named = 0, unavail = 0;

   strcpy(sem.namePath,SEM_NAME);

  xprintf("\n [%s:%d]sem.namePath:%s\n",__func__, __LINE__,sem.namePath);

   sem_init(&sem,PTHREAD_PROCESS_SHARED,3);

   semp = semaphoreOpen(SEM_NAME, strlen(SEM_NAME)+1,O_ACCMODE | O_RDWR , S_IRUSR | S_IWUSR, 0);
   xprintf("\n [%s:%d] Open semp:0x%p\n",__func__,__LINE__, semp);
   semValue= semTake(semp);
   
   xprintf("\n [%s:%d]Take semValue  %d\n",__func__, __LINE__, semValue );

   semValue= semPost(semp);   
   xprintf("\n [%s:%d] Post semValue  %d\n",__func__, __LINE__, semValue );

   semValue= sem_getvalue(semp);   
   xprintf("\n [%s:%d] Sync semValue  %d\n",__func__, __LINE__, semValue );

   semDestroy(semp);   
   xprintf("\n [%s:%d] sem Destroy\n",__func__, __LINE__ );



#if 0

   	if (named) {
		fd = NOFD;
	//	sem = sem_open(SEM_NAME, O_ACCMODE | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);	
	}
	else if (mode == SEM_PROCESS) {
	//	fd = shm_open(SEM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
		//ASSERT(fd != -1);
	//	result = ftruncate(fd, sizeof(sem_t));
		//ASSERT(result != -1);
		sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		ASSERT(sem != MAP_FAILED);
		result = sem_init(sem, !0, 0);
		ASSERT(result == EOK);
	}else {
		fd = NOFD;
		result = sem_init(sem = &semaphore, 0, 0);
		ASSERT(result == EOK);
	}

    switch (mode) {
	   
        case SEM_PROCESS:
	
		    SYNC_INIT("sem", 0);
		    if (!(child = fork())) {
			    SYNC_PRIORITY(-1, NULL);
		    	SYNC_COORDINATE(0, +1);
			    for (loop = limit; loop != 0; --loop) {
				    sem_post(sem);
			    }
			    _exit(0);
		    }
		    assert(child != -1);
		    SYNC_PRIORITY(0, NULL);
		    SYNC_COORDINATE(!0, +1);
		    start = myclock();
		    for (loop = limit; loop != 0; --loop) {
			    sem_wait(sem);
		    }
		    stop = myclock();
		    waitpid(child, NULL, 0);
		    SYNC_DESTROY();
		
	
		    break;

    }

#endif


}
