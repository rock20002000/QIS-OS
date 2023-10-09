#include "kernel.h"
#if 0
typedef struct pathobj_node
{
    path_object_t           object;
    struct rb_node          rbt;
    struct rb_root          children;
    struct pathobj_node*    parent;
    int                     count;  // children count
    char                    name[1];
} pathobj_node_t;
#endif
typedef struct pathobj_node
{
    path_object_t           object;
    struct pathobj_node*    parent;
    struct pathobj_node*    sibling;
    struct pathobj_node*    children[256];
    int                     count;  // children count
    char                    name[1];
} pathobj_node_t;

extern error_t shm_create(path_object_t* p_obj);
extern error_t shm_delete(path_object_t* p_obj);
extern error_t shm_ioctl(path_object_t* p_obj, const void* param, size_t size);

struct path_object_interface {
	error_t (*create)(path_object_t* p_obj);
	error_t (*delete)(path_object_t* p_obj);
	error_t (*ioctl)(path_object_t* p_obj, const void* data, size_t size);
} pathobj_intf[] =
{
    {NULL}, // PATH_OBJECT_NONE
    {NULL}, // PATH_OBJECT_CHANNEL
    // PATH_OBJECT_SHM
    {shm_create, shm_delete, shm_ioctl}
};

static pathobj_node_t   pathobj_root;
static spinlock_t       pathobj_lock;

#define PATHOBJ_SIZE(name)      (sizeof(pathobj_node_t) + strlen(name))
#define PATHOBJ_NSIZE(nsize)    (sizeof(pathobj_node_t) + nsize)

#define PATH_CALLBACK_EXIT      1

#if 0
static uint8_t str2crc(const char *ptr)
{
	uint8_t crc = 0;

    while (*ptr)
    {
        uint8_t byte = (uint8_t)*ptr++;

		crc^=*ptr++;
		for (int i = 0; i < 8; i++)
		{
			if (crc & 0x01)
                crc = (crc>>1) ^ 0x8C;
			else
                crc >>= 1;
		}
    }
	return crc;
}
#endif
static uint8_t strn2crc(const char *ptr, size_t size)
{
	uint8_t crc = 0;
    //	const char* path = ptr;

    for (size_t i = 0; i < size; i++)
    {
        uint8_t byte = (uint8_t)*ptr++;

		crc ^= byte;
		for (int i = 0; i < 8; i++)
		{
			if (crc & 0x01)
                crc = (crc>>1) ^ 0x8C;
			else
                crc >>= 1;
		}
    }
	return crc;
}

static pathobj_node_t* pathobj_find(pathobj_node_t* parent,
    const char* name, size_t nsize, uint8_t* p_crc, pathobj_node_t** p_prev)
{
    uint8_t crc = strn2crc(name, nsize);

    if (p_crc)
        *p_crc = crc;

    pathobj_node_t* found = NULL;
    pathobj_node_t* prev = NULL;

    for (pathobj_node_t* node = parent->children[crc]; node; node = node->sibling)
    {
        if (!strncmp(node->name, name, nsize))
        {
            found = node;
            break;
        }
        prev = node;
    }

    if (p_prev)
        *p_prev = prev;
    return found;
}

static pathobj_node_t* pathobj_alloc(pathobj_node_t* parent, const char* name, size_t nsize, uint8_t crc)
{
    pathobj_node_t* node = kmallocz(PATHOBJ_NSIZE(nsize));

    if (!node)
        return NULL;

    node->parent = parent;
    // node->object.type = PATH_OBJECT_NONE;
    node->object.pid = NULL_ID;
    memcpy(node->name, name, nsize);
    node->name[nsize] = 0;
    node->sibling = parent->children[crc];
    parent->children[crc] = node;
    parent->count++;

    return node;
}

#ifdef PATH_CALLBACK_EXIT
typedef error_t (pathobj_callback_t)(pathobj_node_t* parent,  const char* name, size_t nsize,
                    uint8_t crc, pathobj_node_t* prev, pathobj_node_t** p_node, void* param);

static error_t pathobj_get(pathobj_node_t* parent,  const char* name, size_t nsize,
                    uint8_t crc, pathobj_node_t* prev, pathobj_node_t** p_node, void* param)
{
    pathobj_node_t* node = *p_node;

    if (!node)
        return ERR_NOT_FOUND;

    if (!name[nsize]) // leaf
    {
        path_object_t** pp_obj = (path_object_t**)param;
        *pp_obj = &node->object;
        return PATH_CALLBACK_EXIT;
    }
    return EOK;
}

static error_t pathobj_remove(pathobj_node_t* parent,  const char* name, size_t nsize,
                    uint8_t crc, pathobj_node_t* prev, pathobj_node_t** p_node, void* param)
{
    pathobj_node_t* node = *p_node;

    if (!node)
        return ERR_NOT_FOUND;

    if (!name[nsize]) // leaf
    {
        // KDBG("pathobj_remove: %p, name=%s, crc=%d\n", node, node->name, crc);
        if (!prev)
        {
            parent->children[crc] = node->sibling;
        }
        else
        {
            node = prev->sibling;
            prev->sibling = node->sibling;
        }
        parent->count--;
        kfree(node);
        return PATH_CALLBACK_EXIT;
    }
    return EOK;
}

static error_t pathobj_append(pathobj_node_t* parent,  const char* name, size_t nsize,
                    uint8_t crc, pathobj_node_t* prev, pathobj_node_t** p_node, void* param)
{
    bool new_node = false;
    pathobj_node_t* node = *p_node;

    if (!node)
    {
        node = pathobj_alloc(parent, name, nsize, crc);
        if (!node)
            return ERR_NO_MEMORY;
        new_node = true;
        *p_node = node;
    }

    if (!name[nsize]) // leaf
    {
        if (new_node)
        {
            const path_object_t* p_obj = (const path_object_t*)param;

            node->object.type = p_obj->type;
            node->object.pid = p_obj->pid;
            node->object.handle = p_obj->handle;
            // KDBG("pathobj_append: %p, name=%s, crc=%d, pid=%d, coid=%d\n",
            //     node, node->name, crc, node->object.pid, node->object.coid);
            if ((pathobj_intf[p_obj->type]).create)
            {
                error_t err = (pathobj_intf[p_obj->type]).create(&node->object);

                if (EOK != err)
                {
                    KWARN("path_object(%d, %d, %d) create:%p failed!",
                        p_obj->type, p_obj->pid, p_obj->handle, (pathobj_intf[p_obj->type]).create);
                    pathobj_remove(parent, name, nsize, crc, prev, p_node, NULL);
                    *p_node = NULL;
                    return err;
                }
            }
            return PATH_CALLBACK_EXIT;
        }
        else
        {
            return ERR_ALREADY_EXISTS;
        }
    }
    return EOK;
}

static error_t pathmgr_walk(const char* path, pathobj_callback_t cb, void* param)
{
    error_t err = EOK;
    const char* start = path + 1;
    pathobj_node_t* parent =  &pathobj_root;

    lock_acq(&pathobj_lock);
    for (;;)
    {
        const char* end = start + 1;

        while (*end && *end != PATH_DIR_CHAR)
            end++;

        uint8_t crc;
        pathobj_node_t* prev = NULL;
        pathobj_node_t* node = pathobj_find(parent, start, end - start, &crc, &prev);

        err = cb(parent, start, end - start, crc, prev, &node, param);
        if (err)
        {
            if (PATH_CALLBACK_EXIT == err)
                err = EOK;
            break;
        }

        parent = node;
        start = end + 1;
    }
    lock_rel(&pathobj_lock);

    return err;
}
#endif

static bool check_path(const char* path)
{
    if (!path || !path[0])
        return false;
    if (PATH_DIR_CHAR != path[0]) // only support absolute dir now.
        return false;
    if (PATH_DIR_CHAR == path[strlen(path) - 1])  // not a leaf
        return false;
    return true;
}

void pathmgr_init(void)
{
    spinlock_init(&pathobj_lock);
    memset(&pathobj_root, 0, sizeof(pathobj_root));
    pathobj_root.object.pid = NULL_ID;
}

error_t pathmgr_link(const char* path, path_object_t* p_obj, bool reg_obj)
{
    if (!check_path(path) || !p_obj ||
        p_obj->type < PATH_OBJECT_NONE || p_obj->type >= PATH_OBJECT_MAXTYPE)
        return ERR_INVALID_ARG;

#ifdef PATH_CALLBACK_EXIT
    KDBG("pathmgr_link: path=%s, type=%d, pid=%d, handle=%x\n",
        path, p_obj->type, p_obj->pid, p_obj->handle);
    error_t err = pathmgr_walk(path, pathobj_append, p_obj);
    if (EOK == err && reg_obj)
    {
        pathmgr_walk(path, pathobj_get, &p_obj);
        err = fd_set(NULL, -1, p_obj);
    }
#else
    error_t err = EOK;

    const char* start = path + 1;
    pathobj_node_t* parent =  &pathobj_root;

    lock_acq(&pathobj_lock);
    for (;;)
    {
        const char* end = start + 1;

        while (*end && *end != PATH_DIR_CHAR)
            end++;

        uint8_t crc;
        bool new_node = false;
        pathobj_node_t* node = pathobj_find(parent, start, end - start, &crc, NULL);

        if (!node)
        {
            node = pathobj_alloc(parent, start, end - start, crc);
            if (!node)
            {
                err = ERR_NO_MEMORY;
                break;
            }
            new_node = true;
        }

        if (!*end) // leaf
        {
            if (new_node)
            {
                memcpy(&node->object, p_obj, sizeof(*p_obj))
                p_obj = &node->object;
            }
            else
            {
                err = ERR_ALREADY_EXISTS;
            }
            break;
        }

        parent = node;
        start = end + 1;
    }
    lock_rel(&pathobj_lock);
    if (EOK == err && reg_obj)
    {
        err = fd_set(NULL, -1, p_obj);
    }
#endif
    return err;
}

error_t pathmgr_unlink(const char* path)
{
    if (!check_path(path))
        return ERR_INVALID_ARG;

#ifdef PATH_CALLBACK_EXIT
    error_t err = pathmgr_walk(path, pathobj_remove, NULL);
#else
    error_t err = EOK;

    const char* start = path + 1;
    pathobj_node_t* parent =  &pathobj_root;

    lock_acq(&pathobj_lock);
    for (;;)
    {
        const char* end = start + 1;

        while (*end && *end != PATH_DIR_CHAR)
            end++;

        uint8_t crc;
        pathobj_node_t* prev = NULL;
        pathobj_node_t* node = pathobj_find(parent, start, end - start, &crc, &prev);

        if (!node)
        {
            err = ERR_NOT_FOUND;
            break;
        }
        if (!*end) // leaf
        {
            if (!prev)
            {
                parent->children[crc] = node->sibling;
            }
            else
            {
                node = prev->sibling;
                prev->sibling = node->sibling;
            }
            parent->count--;
            kfree(node);
            break;
        }

        parent = node;
        start = end + 1;
    }
    lock_rel(&pathobj_lock);
#endif
    return err;
}

path_object_t* pathmgr_object(const char* path)
{
    if (!check_path(path))
        return NULL;

    path_object_t* obj = NULL;
#ifdef PATH_CALLBACK_EXIT
    pathmgr_walk(path, pathobj_get, &obj);
    if (obj)
        KDBG("pathmgr_object: path=%s, pid=%d, handle=%x\n", path, obj->pid, obj->handle);
    else
        KDBG("pathmgr_object: path=%s not found!\n", path);
#else
    const char* start = path + 1;
    pathobj_node_t* parent = &pathobj_root;

    lock_acq(&pathobj_lock);
    for (;;)
    {
        const char* end = start + 1;

        while (*end && *end != PATH_DIR_CHAR)
            end++;

        pathobj_node_t* node = pathobj_find(parent, start, end - start, NULL, NULL);

        if (!node)
            break;
        if (!*end) // leaf
        {
            obj = &node->object;
            break;
        }

        parent = node;
        start = end + 1;
    }
    lock_rel(&pathobj_lock);
#endif
    return obj;
}

char* path_from_user(PROCESS_t* proc,
    const void* user_path, size_t len, bool absolute, char* kernel_path)
{
    if (absolute)
    {
        if (!kernel_path)
            return (char*)copy_user_data(proc, user_path, len);
        if (EOK != copy_from_user(proc, user_path, len, kernel_path))
            return NULL;
        return kernel_path;
    }

    if (!proc)
        proc = current_process(NULL);

    bool alloc = false;
    size_t plen = strlen(process_path(proc));

    if (!kernel_path)
    {
        kernel_path = kmalloc(plen + len, KMEM);
        if (!kernel_path)
            return NULL;
        alloc = true;
    }
    memcpy(kernel_path, process_path(proc), plen);
    if (EOK != copy_from_user(proc, user_path, len, kernel_path + plen))
    {
        if (alloc)
            kfree(kernel_path);
        kernel_path = NULL;
    }
    return kernel_path;
}

int pathobj_open(const char* path)
{
    path_object_t* obj = pathmgr_object(path);

    // KDBG("pathobj_open(%s), proc=%p, obj=%p\n",
    //     path, current_process(NULL), obj);
    if (!obj)
        return ERR_NOT_FOUND;

    int fd = fd_set(NULL, -1, obj);
    return fd;
}

error_t pathobj_close(int fd)
{
    return fd_set(NULL, fd, NULL);
}

error_t pathobj_ioctl(int fd, const void* param, size_t size)
{
    path_object_t* p_obj = (path_object_t*)fd_get(NULL, fd);
    // KDBG("pathobj_ioctl(%p, %x) %p\n", p_obj, fd, p_user);
    if (!p_obj)
        return ERR_NOT_FOUND;
    if (p_obj->type < PATH_OBJECT_NONE || p_obj->type >= PATH_OBJECT_MAXTYPE)
        return ERR_INVALID_ARG;
    if ((pathobj_intf[p_obj->type]).ioctl)
        return (pathobj_intf[p_obj->type]).ioctl(p_obj, param, size);
    return EOK;
}
