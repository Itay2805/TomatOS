#ifndef __UTIL_LIST_H__
#define __UTIL_LIST_H__

#include <stdbool.h>

#define CR(record, type, field) ((type*)((char*)(record) - (char*)offsetof(type, field)))

#define INIT_LIST_ENTRY(var) ((list_entry_t){ &var, &var })

#define FOR_EACH_IN_LIST(list, link) \
    for(list_entry_t* link = (list).next; link != &(list); link = link->next)

typedef struct list_entry {
    struct list_entry* next;
    struct list_entry* prev;
} list_entry_t;

bool is_node_in_list(list_entry_t* head, list_entry_t* node);

list_entry_t* initialize_list_had(list_entry_t* head);

list_entry_t* insert_head_list(list_entry_t* head, list_entry_t* entry);

list_entry_t* insert_tail_list(list_entry_t* head, list_entry_t* entry);

list_entry_t* get_first_node(list_entry_t* list);

list_entry_t* get_next_node(list_entry_t* node);

list_entry_t* get_prev_node(list_entry_t* node);

bool is_list_empty(list_entry_t* head);

bool is_null(list_entry_t* list, list_entry_t* node);

bool is_node_at_end(list_entry_t* list, list_entry_t* node);

list_entry_t* swap_list_entries(list_entry_t* first, list_entry_t* second);

list_entry_t* remove_entry_list(list_entry_t* entry);

#endif //__UTIL_LIST_H__
