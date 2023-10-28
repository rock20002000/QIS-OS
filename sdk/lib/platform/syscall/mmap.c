#include <types.h>
#include <arch_syscall.h>
#include <platform/include/platform.h>
#include <libc/include/string.h>
#include <fcntl.h>

extern void* malloc(size_t size);
static const char* shm_full_name(const char* name)
{
#define SHM_ROOT_DIR	"/dev/shmem/"
	if (!name || !name[0])
		return NULL;

	char* full_path = NULL;
	const char* shm_path = name;

	if (PATH_DIR_CHAR != name[0])
	{
		full_path = malloc(sizeof(SHM_ROOT_DIR) + strlen(name));
		strcpy(full_path, SHM_ROOT_DIR);
		strcat(full_path, name);
		shm_path = full_path;
	}
	return shm_path;
}

void* mmap(void* addr, size_t len, int prot, int flags, id_t id, offset_t off)
{
	if (!len)
		return NULL;

	flags &= ~(PROT_MASK | MAP_RESERVMASK);
	prot &= PROT_MASK;
	
	return (void*)syscall(SYSCALL_MMAP, (long)addr, (long)len, (long)(flags|prot), (long)id, (long)off); 
}

int munmap(void* addr, size_t len)
{
	if (!addr || !len) 
		return ERR_INVALID_ARG;
	return syscall(SYSCALL_MUNMAP, (long)addr, (long)len, 0, 0, 0); 
}

uintptr_t mmap_device_io(size_t len, paddr_t io)
{
#define MAP_PROT_IO		(PROT_NOCACHE | PROT_READ_WRITE)
#define MAP_FLAGS_IO	(MAP_PHYS | MAP_SHARED)

	if (NULL_PADDR == io || !len)
		return 0L;
	return (uintptr_t)mmap(NULL, len, MAP_PROT_IO, MAP_FLAGS_IO, NOFD, (offset_t)io);
}

int moffset(const void* addr, size_t len, offset_t* p_off, size_t* p_contiglen)
{
	if (!addr || ! len)
		return ERR_INVALID_ARG;
	return syscall(SYSCALL_MOFFSET, (long)addr, (long)len, (long)p_off, (long)p_contiglen, 0); 
}

int shm_open(const char* name, int oflag, int mode)
{
	int fd;
	const char* shm_path = shm_full_name(name);

	if (!shm_path)
		return ERR_INVALID_ARG;

	if (O_CREAT & oflag)
	{
		fd = syscall(SYSCALL_PATHMGR_LINK,
			(long)shm_path, (long)PATH_OBJECT_SHM, (long)0, (long)1, (long)0);
	}
	else
		fd = PathobjOpen(shm_path);

	if (shm_path != name)
		free((void*)shm_path);

	return fd;
}

int shm_close(int fd)
{
	if (fd < 0)
		return ERR_INVALID_ARG;

	return PathobjClose(fd);
}

int shm_ctl(int fd, int flags, paddr_t paddr, size_t size)
{
	if (fd < 0)
		return ERR_INVALID_ARG;

	shm_block_t shmb = {paddr, size, flags};

	return PathobjCtl(fd, &shmb, sizeof(shmb));
}

int shm_unlink(const char* name)
{
	const char* shm_path = shm_full_name(name);

	if (!shm_path)
		return ERR_INVALID_ARG;

	int err = PathmgrUnlink(shm_path);

	if (shm_path != name)
		free((void*)shm_path);

	return err;
}
