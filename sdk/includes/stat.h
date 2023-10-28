#pragma once

/*
 *  Mask for file access modes.
 */

#ifndef O_RDONLY
# define O_RDONLY	0	/* only allow read */
#endif
#ifndef O_WRONLY
# define O_WRONLY	1	/* only allow write */
#endif
#ifndef O_RDWR
# define O_RDWR		2	/* both are allowed */
#endif
#ifndef O_ACCMODE
# define O_ACCMODE (O_RDONLY | O_RDWR | O_WRONLY)
#endif
				/* The rest can be OR-ed in to the above: */
#ifndef O_CREAT
# define O_CREAT	8	/* create file if needed */
#endif

#ifndef O_EXCL
# define O_EXCL		16	/* file cannot already exist */
#endif

#ifndef S_IRUSR
# define S_IRUSR 0000400
#endif
#ifndef S_IWUSR
# define S_IWUSR 0000200
#endif

#define O_CLOEXEC   020000  /*  File descriptor will be close-on-exec */
#define O_REALIDS   040000  /*  Use real uid/gid for permision checking */
#define O_LARGEFILE 0100000 /*  Largest off_t can be 64 bits */
#define O_ASYNC     0200000 /*  Set async i/o               */