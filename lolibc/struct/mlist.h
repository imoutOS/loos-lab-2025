#ifndef __MLIST_H__
#define __MLIST_H__

struct mlist_element {
    struct mlist_element *prev;
    struct mlist_element *next;
    void *data;
};

struct mlist {
    struct mlist_element *head;
    struct mlist_element *tail;
};

struct mlist *mlist_init();
bool mlist_is_empty(struct mlist*);
void mlist_push(struct mlist*, void*);
void *mlist_pop(struct mlist*);
void mlist_append(struct mlist*, void*);
void mlist_remove(struct mlist*, void*);
void *mlist_find(struct mlist*, bool (*func)(void*));
void mlist_print(struct mlist *);

#endif
