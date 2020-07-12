#ifndef __TOMATOS_KERNEL_UTIL_LIST_H__
#define __TOMATOS_KERNEL_UTIL_LIST_H__

#include <stddef.h>
#include <stdbool.h>

typedef struct list_entry {
    struct list_entry* next;
    struct list_entry* prev;
} list_entry_t;

#define CR(ptr, type, member) \
    ((type*)((char*)(ptr) - (char*)offsetof(type, member)))

#define INIT_LIST(var) \
    ((list_entry_t){ &(var), &(var) })

void list_add(list_entry_t* head, list_entry_t* new);

void list_add_tail(list_entry_t* head, list_entry_t* new);

void list_del(list_entry_t* new);

bool list_is_empty(list_entry_t* head);

#define LIST_NEXT(cur, member) \
    CR((cur)->member.next, typeof(*(cur)), member)

#define LIST_PREV(cur, member) \
    CR((cur)->member.prev, typeof(*(cur)), member)

#define FOR_EACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define FOR_EACH_ENTRY(pos, head, member) \
    for (pos = CR((head)->next, typeof(*(pos)), member); &pos->member != (head); pos = LIST_NEXT(pos, member))

#endif //__TOMATOS_KERNEL_UTIL_LIST_H__
