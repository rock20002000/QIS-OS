#ifndef __LIST_H__
#define __LIST_H__
#include <types.h>
#include <utils/include/print_macros.h>
#include <stdint.h>

#define LSDBG(fmt, ...)

//  Usage:
//
//    struct element {
//        struct list_head next;
//        int foo;
//    };
//
//    LIST_FOR_EACH(elem, &elems, struct element, next) {
//        printf("foo: %d", elem->foo);
//    }
//
#define LIST_CONTAINER(head, container, field)                                              \
    ((container *) ((vaddr_t)(head) -offsetof(container, field)))
#define LIST_FOR_EACH(elem, list, container, field)                                         \
    for (container *elem = LIST_CONTAINER((list)->next, container, field),                  \
                   *__next = NULL;                                                          \
         (&elem->field != (list)                                                            \
          && (__next = (container*)LIST_CONTAINER(elem->field.next, container, field)));    \
         elem = __next)

struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

typedef struct list_head list_t;
typedef struct list_head list_elem_t;
typedef struct list_head ListHead;

static inline int listEmpty(list_t *list)
{
   return (list->next == list) && (list->prev == list);
}

static inline bool list_is_empty(list_t *list) {
    return list->next == list;
}

#define listEntry(ptr,type,member) LIST_CONTAINER(ptr,type,member)

static inline size_t list_len(list_t *list) {
    size_t len = 0;
    struct list_head *node = list->next;
    while (node != list) {
        len++;
        node = node->next;
    }

    return len;
}

static inline bool list_contains(list_t *list, list_elem_t *elem) {
    list_elem_t *node = list->next;
    while (node != list) {
        if (node == elem) {
            return true;
        }
        node = node->next;
    }

    return false;
}

// Inserts a new element between `prev` and `next`.
static inline void list_insert(list_elem_t *prev, list_elem_t *next,
                               list_elem_t *new) {
	LSDBG("%s new:0x%p prev:0x%p\n", __func__, new, prev );
    new->prev = prev;
  //  LSDBG("%s \n", __func__);
    new->next = next;
  //  LSDBG("%s \n", __func__);
    next->prev = new;
    prev->next = new;
   // LSDBG("%s \n", __func__);
}

// Initializes a list.
static inline void list_init(list_t *list) {
    list->prev = list;
    list->next = list;
}

// Invalidates a list element.
static inline void list_nullify(list_elem_t *elem) {
    elem->prev = NULL;
    elem->next = NULL;
}

// Removes a element from the list.
static inline void list_remove(list_elem_t *elem) {
    if (!elem->next) {
        // The element is not in a list.
        return;
    }

    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;

    // Invalidate the element as they're no longer in the list.
    list_nullify(elem);
}

// Appends a element into the list.
static inline void list_push_back(list_t *list, list_elem_t *new_tail) {
    DEBUG_ASSERT(!list_contains(list, new_tail));
    list_insert(list->prev, list, new_tail);
}

// Get and removes the first element from the list.
static inline list_t *list_pop_front(list_t *list) {
    struct list_head *head = list->next;
    if (head == list) {
        return NULL;
    }

    // list <-> head <-> next => list <-> next
    struct list_head *next = head->next;
    list->next = next;
    next->prev = list;

    // Invalidate the element as they're no longer in the list.
    list_nullify(head);
    return head;
}

static inline int listAddTail(ListHead *new,ListHead *next)
{
    list_insert(next->prev,next,new);
    return 0;
}


/**
 * @brief insert a node after a list
 *
 * @param l list to insert it
 * @param n new node to be inserted
 */
static inline void list_insert_after(list_t *l, list_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/**
 * @brief insert a node before a list
 *
 * @param n new node to be inserted
 * @param l list to insert it
 */

static inline void list_insert_before(list_t *l, list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}



#define LIST_POP_FRONT(list, container, field)                                 \
    ({                                                                         \
        list_elem_t *__elem = list_pop_front(list);                            \
        (__elem) ? LIST_CONTAINER(__elem, container, field) : NULL;            \
    })

#endif
