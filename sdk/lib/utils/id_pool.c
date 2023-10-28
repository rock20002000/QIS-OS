#include <utils/include/id_pool.h>

/* alloc() should be implemented by the project with will link this lib */
#ifdef KERNEL
 extern void *kmalloc(size_t size);
 extern void kfree(void *ptr);
 #define alloc  kmalloc
 #define free   kfree
#else
 extern void *alloc(size_t size);
 extern void free(void *ptr);
#endif

 extern void *alloc(size_t size);


void id_pool_init(id_pool_t* this, size_t size) 
{
    this->size = size;
    this->count = 0;
    this->last_index = 0;

    //bitmap_init(&this->id_map, nullptr, this->size);

    this->bitmap = alloc(BITMAP_SIZE(size));
    // memset(this->bitmap, 0, BITMAP_SIZE(size));
    bitmap_fill((uint8_t *)this->bitmap, BITMAP_SIZE(size), 0);

    // yieldlock_init(&this->lock);
}


//Return the an usable index of id pool
size_t id_pool_allocate_id(id_pool_t* this)
{
    // yieldlock_acq(&this->lock);

    while(this->last_index < this->size)
    {
        if(!bitmap_get(this->bitmap, BITMAP_SIZE(this->size), this->last_index))
        {
            size_t found_idx = this->last_index;
            bitmap_set(this->bitmap, BITMAP_SIZE(this->size), this->last_index);
            if(++this->last_index == this->size)
            {
                this->last_index = 0;
            }
            this->count++;
            // yieldlock_rel(&this->lock);
            return (uint64_t)found_idx;            
        }
        else
        {
            this->last_index++;
        }
    }

    if(this->last_index == this->size)
    {
        this->last_index = 0;
    }
    // yieldlock_rel(&this->lock);
    return INVALID_ID;
}


void id_pool_free_id(id_pool_t* this,size_t id_idx)
{
    // yieldlock_acq(&this->lock);
    if (bitmap_get(this->bitmap, BITMAP_SIZE(this->size), id_idx))
        this->count--;
    bitmap_clear(this->bitmap, BITMAP_SIZE(this->size), id_idx);
    // yieldlock_rel(&this->lock);
}

void id_pool_deinit(id_pool_t* this)
{
    if (this->bitmap)
    {
        free(this->bitmap);
        this->bitmap = NULL;
        this->size = 0;
        this->last_index = 0;
        this->count = 0;
    }
}
