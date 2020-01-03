#pragma once

#include <frg/hash_map.hpp>
#include <frg/slab.hpp>

#include <mem/mm.hpp>
#include "spinlock.hpp"

namespace util {

    template<typename Key, typename Value>
    using frg_hash_map = frg::hash_map<Key, Value, frg::hash<Key>, frg::slab_allocator<mem::allocator_policy, util::spinlock>>;

    template<typename Key, typename Value>
    class map : public frg_hash_map<Key, Value> {
    private:
        frg::hash<Key> hasher;

    public:
        map()
            : frg_hash_map<Key, Value>(hasher, mem::allocator)
        {}
    };

//    template<typename Key, typename Value>
//    frg::hash<Key> map<Key, Value>::hasher;

}
