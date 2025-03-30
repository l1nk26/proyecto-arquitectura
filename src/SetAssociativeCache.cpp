#include "SetAssociativeCache.hpp"

    SetAssociativeCache::SetAssociativeCache(int size, int num_ways) : Cache(size), ways(num_ways) {
        num_sets = capacity / ways;
        lru_lists.resize(num_sets);
        cache_maps.resize(num_sets);
    }

    SetAssociativeCache::SetAssociativeCache(SetAssociativeCache const &c) : Cache(c) , ways(c.ways){
        num_sets = capacity / ways;
        lru_lists = c.lru_lists;
        cache_maps = c.cache_maps;
    }

    bool SetAssociativeCache::access(int block_id, AdvancedStats& stats) {
        int set_index = block_id % num_sets;  // Determinar el conjunto
        auto& cache_map = cache_maps[set_index];
        auto& lru_list = lru_lists[set_index];

        auto it = cache_map.find(block_id); 
        if (it != cache_map.end()) { // se verifica si la clave existe en el mapa
            // Actualizar LRU dentro del conjunto para que el valor accedido sea el mas recientemente utilizado
            lru_list.erase(it->second.lru_it);
            lru_list.push_front(block_id);
            it->second.lru_it = lru_list.begin();
            stats.cache_hits++;
            return true;
        }
        stats.cache_misses++;
        return false;
    }

    void SetAssociativeCache::add_block(int block_id) {
        int set_index = block_id % num_sets;
        auto& cache_map = cache_maps[set_index];
        auto& lru_list = lru_lists[set_index];

        if (cache_map.size() >= ways) {
            // Eliminar el menos recientemente usado (LRU) del conjunto
            int lru_block = lru_list.back();
            cache_map.erase(lru_block);
            lru_list.pop_back();
        }

        // Insertar el nuevo bloque
        lru_list.push_front(block_id);
        cache_map[block_id] = {block_id, false, lru_list.begin()};
    }

    void SetAssociativeCache::mark_dirty(int block_id) {
        int set_index = block_id % num_sets;
        auto& cache_map = cache_maps[set_index];

        auto it = cache_map.find(block_id);
        if (it != cache_map.end()) {
            it->second.dirty = true;
        }
    }