#include "klib.h"
#include "struct/mlist.h"

#define panic_on(cond, msg) \
do { \
  if ((cond)) { \
    putstr(msg); \
    halt(1); \
  } \
} while(0)

struct mlist *mlist_init() {
    struct mlist *list = (struct mlist*)malloc(sizeof(struct mlist));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

static struct mlist_element *new_mlist_elem(void *data) {
    struct mlist_element *elem = (struct mlist_element*)malloc(sizeof(struct mlist_element));
    elem->prev = NULL;
    elem->next = NULL;
    elem->data = data;
    return elem;
}
bool inline mlist_is_empty(struct mlist *list) {
    panic_on(!list, "list not init");
    //assert((list->head == list->tail && list->head == NULL) || list->head != list->tail);
    return list && (list->head == NULL);
}

void mlist_push(struct mlist *list, void *data) {
    struct mlist_element *elem = new_mlist_elem(data);
    if (mlist_is_empty(list)) {
        list->head = list->tail = elem;
    } else {
        elem->next = list->head;
        elem->next->prev = elem;
        list->head = elem;
    }
}

void *mlist_pop(struct mlist *list) {
    if (mlist_is_empty(list))
        return NULL;
    void *data;
    struct mlist_element *elem = list->head;
    data = elem->data;
    // only one element
    if (list->head == list->tail) {
        list->head = list->tail = NULL;
    } else {
        list->head = list->head->next;
    }
    free(elem);
    return data;
}
void mlist_append(struct mlist *list, void *data) {
    struct mlist_element *elem = new_mlist_elem(data);
    if (mlist_is_empty(list)) {
        list->head = list->tail = elem;
    } else {
        elem->prev = list->tail;
        elem->prev->next = elem;
        list->tail = elem;
    }
}
void mlist_remove(struct mlist *list, void *data) {
    if (mlist_is_empty(list))
        goto fail;
    struct mlist_element *elem = list->head;
    while (elem) {
        if (elem->data == data) {
            if (elem == list->head)
                list->head = elem->next;
            if (elem == list->tail)
                list->tail = elem->prev;
            free(elem);
            return;
        }
        elem = elem->next;
    }
fail:
    assert(0);
}


void *mlist_find(struct mlist *list, bool (*func)(void*)) {
    if (mlist_is_empty(list))
        goto fail;
    struct mlist_element *elem = list->head;
    while (elem) {
        if (func(elem->data)) {
            return elem->data;
        }
        elem = elem->next;
    }
fail:
    return NULL;
}

void mlist_print(struct mlist *list) {
    struct mlist_element *elem = list->head;
    printf("head: %p, tail: %p\nelem: ", list->head, list->tail);
    while (elem) {
        printf("%p ", elem->data);
        elem = elem->next;
    }
    printf("\n");
}

