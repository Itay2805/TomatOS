#pragma once

namespace util {

    #define CR(record, type, field) ((type*)((char*)(record) - (char*)offsetof(type, field)))

    struct list_entry {
        list_entry* next;
        list_entry* prev;

        // insert at the tail
        void insert_tail(list_entry* entry);

        // insert at the head
        void insert_head(list_entry* entry);

        // remove this entry
        void remove();

        // is the list empty
        bool is_empty();

        // make sure the list is valid
        bool is_valid();
    };

}