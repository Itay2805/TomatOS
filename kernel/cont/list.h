#ifndef TOMATOS_LIST_H
#define TOMATOS_LIST_H

typedef struct list_entry {
    struct list_entry* next;
    struct list_entry* prev;
} list_entry_t;

#endif //TOMATOS_LIST_H
