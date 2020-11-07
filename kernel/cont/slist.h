#ifndef TOMATOS_SLIST_H
#define TOMATOS_SLIST_H

#include <util/defs.h>

typedef struct slist_entry {
    struct slist_entry* next;
} slist_entry_t;

#define INIT_SLIST ((slist_entry_t) { .next = NULL })

void slist_push(slist_entry_t* head, slist_entry_t* new);

slist_entry_t* slist_pop(slist_entry_t* head);

bool slist_empty(slist_entry_t* head);

#endif //TOMATOS_SLIST_H
