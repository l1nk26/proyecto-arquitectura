#pragma once
#include <unordered_map>
#include <list>
#include "Stats.hpp"
#include "Cache.hpp"

// Declaracion de caché con política LRU
class LRUCache : public Cache {
    private:
        struct CacheEntry {
            int block_id;
            bool dirty;
            std::list<int>::iterator lru_it;
        };
    
        std::list<int> lru_list;
        std::unordered_map<int, CacheEntry> cache_map;
    
    public:
        LRUCache(int size);
    
        bool access(int block_id, AdvancedStats& stats) override;
    
        void add_block(int block_id) override;
    
        void mark_dirty(int block_id) override;
};