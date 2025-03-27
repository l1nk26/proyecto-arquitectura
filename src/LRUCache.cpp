#include "LRUCache.hpp"


// Implementación de caché con política LRU
LRUCache::LRUCache(int size) : Cache(size) {}

bool LRUCache::access(int block_id, AdvancedStats& stats) {
    auto it = cache_map.find(block_id);
    if (it != cache_map.end()) {
        // Actualizar LRU
        lru_list.erase(it->second.lru_it);
        lru_list.push_front(block_id);
        it->second.lru_it = lru_list.begin();
        stats.cache_hits++;
        return true;
    }
    stats.cache_misses++;
    return false;
}

void LRUCache::add_block(int block_id) {
    if (cache_map.size() >= (long unsigned int)capacity) {
        // Eliminar el menos recientemente usado
        int lru = lru_list.back();
        cache_map.erase(lru);
        lru_list.pop_back();
    }
    
    lru_list.push_front(block_id);
    cache_map[block_id] = {block_id, false, lru_list.begin()};
}

void LRUCache::mark_dirty(int block_id) {
    auto it = cache_map.find(block_id);
    if (it != cache_map.end()) {
        it->second.dirty = true;
    }
}