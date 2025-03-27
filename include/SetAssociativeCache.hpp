#ifndef SETASSOCIATIVECACHE
#define SETASSOCIATIVECACHE

#include <list>
#include <vector>
#include <unordered_map>
#include "Stats.hpp"
#include "Cache.hpp"

class SetAssociativeCache : public Cache {
private:
    struct CacheEntry {
        int block_id;
        bool dirty;
        std::list<int>::iterator lru_it;  // Iterador para la lista LRU del conjunto
    };

    int capacity;      // Capacidad total del caché (número de bloques)
    int num_sets;      // Número de conjuntos (sets)
    int ways;          // Número de vías (ways) por conjunto
    std::vector<std::list<int>> lru_lists;  // Listas LRU por conjunto
    std::vector<std::unordered_map<int, CacheEntry>> cache_maps;  // Mapas por conjunto

public:
    SetAssociativeCache(int size, int num_ways);

    bool access(int block_id, AdvancedStats& stats) override;

    void add_block(int block_id) override;

    void mark_dirty(int block_id) override;
};

#endif