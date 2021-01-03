#ifndef TOMATOS_LIST_H
#define TOMATOS_LIST_H

#define CR(ptr, type, member) \
    ((type*)((char*)(ptr) - (char*)offsetof(type, member)))

/**
 * an entry in a list
 */
typedef struct list_entry {
    struct list_entry* next;
    struct list_entry* prev;
} list_entry_t;

/**
 * The head of a list
 */
typedef list_entry_t list_t;

/*
 * Initialize a list to empty. Because these are circular lists, an "empty"
 * list is an entry where both links point to itself. This makes insertion
 * and removal simpler because they don't need any branches.
 */
void list_init(list_t* list);

/*
 * Append the provided entry to the end of the list. This assumes the entry
 * isn't in a list already because it overwrites the linked list pointers.
 */
void list_push(list_t* list, list_entry_t* entry);

/*
 * Remove the provided entry from whichever list it's currently in. This
 * assumes that the entry is in a list. You don't need to provide the list
 * because the lists are circular, so the list's pointers will automatically
 * be updated if the first or last entries are removed.
 */
void list_remove(list_entry_t* entry);

/*
 * Remove and return the first entry in the list or NULL if the list is empty.
 */
list_entry_t* list_pop(list_t* list);

#endif //TOMATOS_LIST_H
