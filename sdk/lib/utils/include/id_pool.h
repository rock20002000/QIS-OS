#ifndef __LIB_ID_POOL_H__
#define __LIB_ID_POOL_H__

#include <utils/include/bitmap.h>


#define INVALID_ID      0xFFFFFFFFFFFFFFFF

typedef struct
{
    size_t      size;
    size_t      last_index;
    size_t      count;
    void*       bitmap;
    // yieldlock_t lock;
}id_pool_t;


extern void id_pool_free_id(id_pool_t* this, size_t id_idx);
extern size_t id_pool_allocate_id(id_pool_t* this);
extern void id_pool_init(id_pool_t* this, size_t size);
extern void id_pool_deinit(id_pool_t* this);

#endif
