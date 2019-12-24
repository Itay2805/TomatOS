#pragma once

#include <stb/stb_ds.h>

namespace util {

    template<typename Key, typename Value>
    class map {
    private:
        struct {
            Key key;
            Value value;
        }* stb_map = nullptr;

    public:

        ~map() {
            hmfree(stb_map);
        }

        [[nodiscard]] inline int len() const {
            return hmlen(stb_map);
        }

        inline int index_of(Key& key) const {
            return hmgeti(stb_map, key);
        }

        inline Key& key_at(int index) {
            return stb_map[index].key;
        }

        inline const Key& key_at(int index) const {
            return stb_map[index].key;
        }

        inline Value& value_at(int index) {
            return stb_map[index].value;
        }

        inline const Value& value_at(int index) const {
            return stb_map[index].value;
        }

        inline Value& get(Key key) {
            return hmget(stb_map, key);
        }

        inline const Value& get(Key key) const {
            return hmget(stb_map, key);
        }

        inline Value& get(Key& key) {
            return hmget(stb_map, key);
        }

        inline const Value& get(Key& key) const {
            return hmget(stb_map, key);
        }

        inline void set_default(Value& value) const {
            hmdefault(stb_map, value);
        }

        inline void put(const Key& key, const Value& value) {
            hmput(stb_map, key, value);
        }

        inline void del(const Key& key) {
            auto entry = &stb_map[hmgeti(stb_map, key)];
            entry->key.~Key();
            entry->value.~Value();
            hmdel(stb_map, key);
        }

        Value& operator[](Key& key) {
            return this->get(key);
        }

        const Value& operator[](Key& key) const {
            return this->get(key);
        }

    };

    template<typename Value>
    class string_map {
    private:
        struct {
            const char* key;
            Value value;
        }* stb_map = nullptr;

    public:

        ~string_map() {
            smfree(stb_map);
        }

        [[nodiscard]] inline int length() const {
            return smlen(stb_map);
        }

        inline int index_of(const char* key) const {
            return smgeti(stb_map, key);
        }

        inline Value& at(int index) {
            return stb_map[index].value;
        }

        inline const Value& at(int index) const {
            return stb_map[index].value;
        }

        inline Value& get(const char* key) {
            return smget(stb_map, key);
        }

        inline const Value& get(const char* key) const {
            return smget(stb_map, key);
        }

        inline void set_default(Value& value) const {
            smdefault(stb_map, value);
        }

        inline void put(const char* key, Value& value) {
            smput(stb_map, key, value);
        }

        inline void del(const char* key) {
            smdel(stb_map, key);
        }

        Value& operator[](const char* key) {
            return this->get(key);
        }

        const Value& operator[](const char* key) const {
            return this->get(key);
        }

    };

}
