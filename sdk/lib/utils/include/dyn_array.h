#ifndef __LIB__DYNARRAY_H__
#define __LIB__DYNARRAY_H__

#include <types.h>

#ifdef KERNEL
 #define alloc  kmallocz
 #define free   kfree
#endif

#define DYNARRAY_STRUCT(TYPE) \
    struct {                  \
        TYPE  *storage;       \
        size_t storage_size;  \
        size_t length;        \
    }

#define DYNARRAY_EXTERN(TYPE, THIS) \
    extern DYNARRAY_STRUCT(TYPE) THIS

#define DYNARRAY_GLOBAL(THIS) \
    typeof(THIS) THIS = {0}

#define DYNARRAY_STATIC(TYPE, THIS) \
    static DYNARRAY_STRUCT(TYPE) THIS = {0}

#define DYNARRAY_NEW(TYPE, THIS) \
    DYNARRAY_STRUCT(TYPE) THIS = {0}

#define DYNARRAY_NULLIFY(THIS) \
    (THIS).storage = NULL; 

#define DYNARRAY_INIT(THIS, INITIAL_SIZE) ({                                       \
    (THIS).storage_size = INITIAL_SIZE;                                            \
    (THIS).storage = alloc((THIS).storage_size * sizeof(typeof(*(THIS).storage))); \
})

#define DYNARRAY_DEL(THIS) ({ \
    free((THIS).storage);     \
})

#ifdef KERNEL
 //#include "common/page.h"
 #define DYNARRAY_INIT_NEW(THIS) DYNARRAY_INIT(THIS, PAGE_SIZE/sizeof(typeof(*(THIS).storage)))
#else
 #define DYNARRAY_INIT_NEW(THIS) DYNARRAY_INIT(THIS, 1)		//	DYNARRAY_INIT(THIS, 1)
#endif

#define DYNARRAY_GROW(THIS) ({                                                           \
    if ((THIS).storage == NULL) {                                                        \
        DYNARRAY_INIT_NEW(THIS);                                                         \
    } else {                                                                             \
        (THIS).storage_size *= 2;                                                        \
        (THIS).storage = realloc((THIS).storage,                                         \
                                 (THIS).storage_size * sizeof(typeof(*(THIS).storage))); \
    }                                                                                    \
})

#define DYNARRAY_PUSHBACK(THIS, ITEM) ({      \
    if ((THIS).length >= (THIS).storage_size) \
        DYNARRAY_GROW(THIS);                  \
    (THIS).storage[(THIS).length++] = ITEM;   \
    (THIS).length - 1;                        \
})

#define DYNARRAY_INSERT(THIS, ITEM) ({     \
    bool found = false;                    \
    size_t iDynaInsertTmp;                              \
    for (iDynaInsertTmp = 0; iDynaInsertTmp < (THIS).length; iDynaInsertTmp++) {  \
        if ((THIS).storage[iDynaInsertTmp] == NULL) {   \
            (THIS).storage[iDynaInsertTmp] = ITEM;      \
            found = true;                  \
            break;                         \
        }                                  \
    }                                      \
    if (found == false) {                  \
        iDynaInsertTmp = DYNARRAY_PUSHBACK(THIS, ITEM); \
    }                                      \
    iDynaInsertTmp;                                     \
})

#define DYNARRAY_INSERT_AT(THIS, ITEM, AT) ({         \
    if ((AT) >= (THIS).storage_size) {                \
        (THIS).storage_size = (AT) + 1;               \
        (THIS).storage =                              \
            realloc((THIS).storage,                   \
                    (THIS).storage_size               \
                  * sizeof(typeof(*(THIS).storage))); \
    }                                                 \
    (THIS).storage[(AT)] = (ITEM);                    \
    if ((AT) >= (THIS).length)                        \
        (THIS).length = (AT) + 1;                     \
    (AT);                                             \
})

#define DYNARRAY_INSERT_LEAST(THIS, ITEM, AT) ({ \
    bool found = false;                          \
    size_t iDynaInsertLeastTmp;                                    \
    if ((AT) >= (THIS).length) {                 \
        iDynaInsertLeastTmp = DYNARRAY_INSERT_AT(THIS, ITEM, AT);  \
    } else {                                     \
        for (iDynaInsertLeastTmp = (AT); iDynaInsertLeastTmp < (THIS).length; iDynaInsertLeastTmp++) { \
            if ((THIS).storage[iDynaInsertLeastTmp] == NULL) {     \
                (THIS).storage[iDynaInsertLeastTmp] = ITEM;        \
                found = true;                    \
                break;                           \
            }                                    \
        }                                        \
        if (found == false) {                    \
            iDynaInsertLeastTmp = DYNARRAY_PUSHBACK(THIS, ITEM);   \
        }                                        \
    }                                            \
    i;                                           \
})

#define DYNARRAY_GET_INDEX_BY_VALUE(THIS, VALUE) ({ \
    size_t iDynaGetIndexByValueTmp;                                      \
    for (iDynaGetIndexByValueTmp = 0; iDynaGetIndexByValueTmp < (size_t)(THIS).length; iDynaGetIndexByValueTmp++) {  \
        if ((THIS).storage[iDynaGetIndexByValueTmp] == (VALUE))           \
            break;                                  \
    }                                               \
    if (iDynaGetIndexByValueTmp == (size_t)((THIS).length))              \
        iDynaGetIndexByValueTmp = -1;                                     \
    iDynaGetIndexByValueTmp;                                              \
})

#define DYNARRAY_REMOVE_BY_VALUE(THIS, VALUE) ({   \
    size_t iDynaRemoveByValueTmp;                                     \
    for (iDynaRemoveByValueTmp = 0; iDynaRemoveByValueTmp < (size_t)(THIS).length; iDynaRemoveByValueTmp++) { \
        if ((THIS).storage[iDynaRemoveByValueTmp] == (VALUE)) {        \
            (THIS).storage[iDynaRemoveByValueTmp] = NULL;              \
            break;                                 \
        }                                          \
    }                                              \
    if (iDynaRemoveByValueTmp == (size_t)((THIS).length))             \
        iDynaRemoveByValueTmp = -1;                                    \
    iDynaRemoveByValueTmp;                                             \
})

#define DYNARRAY_REMOVE_BY_VALUE_AND_PACK(THIS, VALUE) ({    \
    size_t iDynaRemovePackByValueTmp;                                               \
    for (iDynaRemovePackByValueTmp = 0; iDynaRemovePackByValueTmp < (size_t)(THIS).length; iDynaRemovePackByValueTmp++) {           \
        if ((THIS).storage[iDynaRemovePackByValueTmp] == (VALUE)) {                  \
            for (size_t jiDynaRemovePackByValueTmp = iDynaRemovePackByValueTmp + 1; jiDynaRemovePackByValueTmp < (THIS).length; jiDynaRemovePackByValueTmp++) { \
                (THIS).storage[jiDynaRemovePackByValueTmp-1] = (THIS).storage[jiDynaRemovePackByValueTmp];     \
            }                                                \
            break;                                           \
        }                                                    \
    }                                                        \
    if (iDynaRemovePackByValueTmp == (size_t)((THIS).length))                       \
        iDynaRemovePackByValueTmp = -1;                                              \
    else                                                    \
        --(THIS).length;                                     \
    iDynaRemovePackByValueTmp;                                                       \
})

#define DYNARRAY_REMOVE_AND_PACK(THIS, INDEX) ({           \
    for (size_t iDynaRemovePackTmp = (INDEX) + 1; iDynaRemovePackTmp < (THIS).length; iDynaRemovePackTmp++) { \
        (THIS).storage[iDynaRemovePackTmp-1] = (THIS).storage[iDynaRemovePackTmp];           \
    }                                                      \
    --(THIS).length;                                       \
})

#define DYNARRAY_ADD_FIRST_ITEM(THIS, ITEM) ({           \
    if ((THIS).length >= (THIS).storage_size) \
        DYNARRAY_GROW(THIS);	\
    for (size_t iDynaAddFirstTmp = (THIS).length; iDynaAddFirstTmp >0; iDynaAddFirstTmp--) { \
        (THIS).storage[iDynaAddFirstTmp] = (THIS).storage[iDynaAddFirstTmp-1];           \
    }                                                      \
    (THIS).storage[0] = ITEM;                                       \
	++(THIS).length;                                       \
	(THIS).length - 1;                        \
})


#endif
