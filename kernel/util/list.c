#include "list.h"

void list_add(list_entry_t* head, list_entry_t* new) {
    new->next = head->next;
    new->prev = head;
    new->next->prev = new;
    head->next = new;
}

void list_add_tail(list_entry_t* head, list_entry_t* new) {
    new->next = head;
    new->prev = head->prev;
    new->prev->next = new;
    head->prev = new;
}

void list_del(list_entry_t* new) {
    new->next->prev = new->prev;
    new->prev->next = new->next;
    new->next = (void*)0xCAFE000000000000ull;
    new->prev = (void*)0xBABE000000000000ull;
}

bool list_is_empty(list_entry_t* head) {
    return head->next == head;
}
