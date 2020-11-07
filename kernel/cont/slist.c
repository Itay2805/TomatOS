#include "slist.h"

void slist_push(slist_entry_t* head, slist_entry_t* new) {
    new->next = head->next;
    head->next = new;
}

slist_entry_t* slist_pop(slist_entry_t* head) {
    slist_entry_t* poped = head->next;
    head->next = poped->next;
    return poped;
}

bool slist_empty(slist_entry_t* head) {
    return head->next == NULL;
}
