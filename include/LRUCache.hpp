#pragma once
#include <unordered_map>
#include <list>
#include "Stats.hpp"

// Declaracion de caché con política LRU
class LRUCache {
    private:
        struct CacheEntry {
            int block_id;
            bool dirty;
            std::list<int>::iterator lru_it;
        };
    
        int capacity;
        std::list<int> lru_list;
        std::unordered_map<int, CacheEntry> cache_map;
    
    public:
        LRUCache(int size);
    
        bool access(int block_id, AdvancedStats& stats);
    
        void add_block(int block_id);
    
        void mark_dirty(int block_id);
};