#include "list.h"

#include "except.h"

static bool is_list_valid(list_entry_t* list) {
    ASSERT(list != NULL);
    ASSERT(list->next != NULL);
    ASSERT(list->prev != NULL);
    return true;
}

bool is_node_in_list(list_entry_t* head, list_entry_t* node) {
    list_entry_t* ptr = head;

    ASSERT(is_list_valid(head));
    ASSERT(node != NULL);

    do {
        ptr = ptr->next;

        if(ptr == node) {
            return true;
        }
    } while(ptr != head);

    return false;
}

list_entry_t* initialize_list_had(list_entry_t* head) {
    ASSERT(head != NULL);

    head->next = head;
    head->prev = head;

    return head;
}

list_entry_t* insert_head_list(list_entry_t* head, list_entry_t* entry) {
    ASSERT(is_list_valid(head));

    entry->next = head->next;
    entry->prev = head;
    entry->next->prev = entry;
    head->next = entry;

    return head;
}

list_entry_t* insert_tail_list(list_entry_t* head, list_entry_t* entry) {
    ASSERT(is_list_valid(head));

    entry->next = head;
    entry->prev = head->prev;
    entry->prev->next = entry;
    head->prev = entry;

    return head;
}

list_entry_t* get_first_node(list_entry_t* list) {
    ASSERT(is_list_valid(list));
    return list->next;
}

list_entry_t* get_next_node(list_entry_t* node) {
    ASSERT(is_list_valid(node));
    return node->next;
}

list_entry_t* get_prev_node(list_entry_t* node) {
    ASSERT(is_list_valid(node));
    return node->prev;
}

bool is_list_empty(list_entry_t* head) {
    ASSERT(is_list_valid(head));
    return head->next == head;
}

bool is_null(list_entry_t* list, list_entry_t* node) {
    return node == list;
}

bool is_node_at_end(list_entry_t* list, list_entry_t* node) {
    return is_null(list, node) && list->prev == node;
}

list_entry_t* swap_list_entries(list_entry_t* first, list_entry_t* second) {
    if(first == second) {
        return second;
    }

    // make sure both in the same list
    ASSERT(is_node_in_list(first, second));

    list_entry_t* ptr = remove_entry_list(first);

    if(ptr->prev == second) {
        return insert_tail_list(second, first);
    }

    if(ptr == insert_head_list(second, first)) {
        return ptr;
    }

    remove_entry_list(second);
    insert_tail_list(ptr, second);
    return second;
}

list_entry_t* remove_entry_list(list_entry_t* entry) {
    ASSERT(!is_list_empty(entry));

    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;

    return entry->next;
}