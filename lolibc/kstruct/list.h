#ifndef __KSTRUCT_LIST_H__
#define __KSTRUCT_LIST_H__

// #include <stdio.h>
#include <stddef.h>

struct list_head
{
    struct list_head *next;
    struct list_head *prev;
};

#define LIST_HEAD_INIT(name)     {&(name), &(name)}
#ifdef LIST_HEAD
#undef LIST_HEAD
#define LIST_HEAD(name)          struct list_head name = LIST_HEAD_INIT(name)
#endif

#if !defined(offsetof)
#define offsetof(type, member)     ((size_t) &((type *) 0)->member)
#endif

#define container_of(ptr, type, member) \
        ({const typeof(((type *) 0)->member) *_mptr = (ptr); \
        (type *)((char *)_mptr - offsetof(type, member));})

// ptr is a struct list_head *, and will return a type* given struct name which list_head is in,
// and the member name of the list_head * type;
#define list_entry(ptr, type, member)   container_of(ptr, type, member)
// 


#define list_for_each_entry(pos, head, member)  \
        for (pos = list_entry((head)->next, typeof(*pos), member); \
        &(pos->member) != (head); \
        pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member) \
        for (pos = list_entry((head)->next, typeof(*pos), member), \
        n = list_entry(pos->member.next, typeof(*pos), member); \
        &(pos->member) != (head); \
        pos = n, n = list_entry(n->member.next, typeof(*n), member))

#define list_for_each_entry_reverse(pos, head, member)  \
        for (pos = list_entry((head)->prev, typeof(*pos), member); \
        &(pos->member) != (head); \
        pos = list_entry(pos->member.prev,typeof(*pos), member))

#define list_for_each_entry_safe_reverse(pos, n, head, member) \
        for (pos = list_entry((head)->prev, typeof(*pos), member), \
        n = list_entry(pos -> member.prev,typeof(*pos), member); \
        &(pos->member) != (head); \
        pos = n, n = list_entry(n->member.prev,typeof(*n), member))

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

// static inline void                 list_add(struct list_head *head, struct list_head *new);
// static inline void                 list_add_tail(struct list_head *head, struct list_head *new);
// static inline void                 list_del(struct list_head *entry);
// static inline void                 list_replace(struct list_head *old, struct list_head *new);
// static inline void                 list_replace_init(struct list_head *old, struct list_head *new);
// static inline void                 list_move(struct list_head *list, struct list_head *head);
// static inline void                 list_move_tail(struct list_head *list, struct list_head *head);
// static inline int                  list_is_empty(struct list_head *head);
// static inline int                  list_is_last(struct list_head *list,struct list_head *head);
// static inline void                 list_splice(struct list_head *list, struct list_head *head);
// static inline void                 list_splice_init(struct list_head *list, struct list_head *head);
// static inline struct list_head *   list_pop(struct list_head *head);
// static inline struct list_head *   list_pop_tail(struct list_head *head);

// static inline void                 __list_del(struct list_head *prev, struct list_head *next);
// static inline void                 __list_add(struct list_head *next, struct list_head *prev, struct list_head *new);


static inline void init_list_head(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}


static inline void _list_add(struct list_head *next, struct list_head *prev, struct list_head *new)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}
static inline void list_add(struct list_head *head, struct list_head *new)
{
    _list_add(head->next, head, new);
}

static inline void list_add_tail(struct list_head *head, struct list_head *new)
{
    _list_add(head, head->prev, new);
}


static inline void _list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}
static inline void list_del(struct list_head *entry)
{
    _list_del(entry->prev, entry->next);
    entry->next = entry;
    entry->prev = entry;
}

static inline void list_replace(struct list_head *old, struct list_head *new)
{
    new         ->next   = old->next;
    new->next   ->prev   = new;
    new         ->prev   = old->prev;
    new->prev   ->next   = new;
}
static inline void list_replace_init(struct list_head *old, struct list_head *new)
{
    list_replace(old, new);
    init_list_head(old);
}


static inline void list_move(struct list_head *list, struct list_head *head)
{
    _list_del(list->prev, list->next);
    list_add(head,list);
}
static inline void list_move_tail(struct list_head *list, struct list_head *head)
{
    _list_del(list->prev, list->next);
    list_add_tail(head,list);
}


static inline int list_is_empty(struct list_head *head)
{
    return head->next == head;
}
static inline int list_is_last(struct list_head *list,struct list_head *head)
{
    return list->next == head;
}


static inline void list_splice(struct list_head *list, struct list_head *head)
{
    head->prev  ->  next  = list->next;
    list->next  ->  prev  = head->prev;
    list->prev  ->  next  = head;
    head        ->  prev  = list->prev;
}
static inline void list_splice_init(struct list_head *list, struct list_head *head)
{
    list_splice(list, head);
    init_list_head(list);
}


static inline struct list_head *list_pop(struct list_head *head)
{
    if ( list_is_empty(head) ) return NULL;
    struct list_head * result = head->next;
    list_del(result);
    return result; 
}
static inline struct list_head *list_pop_tail(struct list_head *head)
{
    if ( list_is_empty(head) ) return NULL;
    struct list_head * result = head->prev;
    list_del(result);
    return result; 
}

// static inline struct list_head * list_head_value()

void init_list_head(struct list_head *list);


#endif
