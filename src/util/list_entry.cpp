#include "list_entry.hpp"
#include "debug.hpp"

namespace util {

    bool list_entry::is_valid() {
        return this->next != nullptr && this->prev != nullptr;
    }

    void list_entry::insert_tail(list_entry* entry) {
        ASSERT(this->is_valid());

        // TODO: add locks

        entry->next = this;
        entry->prev = this->prev;
        entry->prev->next = entry;
        this->prev = entry;
    }

    void list_entry::insert_head(list_entry* entry) {
        ASSERT(this->is_valid());

        entry->next = this->next;
        entry->prev = this;
        entry->next->prev = entry;
        this->next = entry;

    }

    void list_entry::remove() {
        ASSERT(this->is_valid());

        this->next->prev = this->prev;
        this->prev->next = this->next;
    }

    bool list_entry::is_empty() {
        ASSERT(this->is_valid());
        return this->next == this;
    }

}
