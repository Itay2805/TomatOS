#include "list.h"

void list_add(list_entry_t* head, list_entry_t* entry) {
    entry->next = head->next;
    entry->prev = head;
    entry->next->prev = entry;
    head->next = entry;
}

void list_add_tail(list_entry_t* head, list_entry_t* entry) {
    entry->next = head;
    entry->prev = head->prev;
    entry->prev->next = entry;
    head->prev = entry;
}

void list_del(list_entry_t* entry) {
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
    entry->next = (void*)0xCAFE000000000000ull;
    entry->prev = (void*)0xBABE000000000000ull;
}

bool list_is_empty(list_entry_t* head) {
    return head->next == head;
}
