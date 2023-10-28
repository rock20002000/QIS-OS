#ifndef __PATHOBJ_H__
#define __PATHOBJ_H__

#include <types.h>

#define PATH_DIR_CHAR   '/'

enum {
	PATH_OBJECT_NONE,
	PATH_OBJECT_CHANNEL,
	PATH_OBJECT_SHM,
	PATH_OBJECT_MAXTYPE
};

typedef struct
{
    int             type;
    pid_t           pid;
    union
    {
        int         fd;
        int         chid;
        handle_t    handle;
    };
} path_object_t;

#endif
