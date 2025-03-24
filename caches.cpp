#include <vector>
#include "stats.h"

class DirectMappedCache {
    
    private:
    struct CacheEntry {
        int block_id;
        bool dirty;
        bool valid;  // Indica si la entrada contiene datos válidos
    };
    
    int capacity;
    std::vector<CacheEntry> cache_entries;  // Usamos un vector para acceso directo
    
    public:
        
        DirectMappedCache(int size) : capacity(size) {
            cache_entries.resize(size, {-1, false, false});  // Inicializar entradas como inválidas
        }
    
        bool access(int block_id, AdvancedStats& stats) {
            int index = block_id % capacity;  // Función de correspondencia directa
            if (cache_entries[index].valid && cache_entries[index].block_id == block_id) {
                stats.cache_hits++;
                return true;
            }
            stats.cache_misses++;
            return false;
        }
    
        void add_block(int block_id) {
            int index = block_id % capacity;
            cache_entries[index] = {block_id, false, true};  // Reemplazar directamente
        }
    
        void mark_dirty(int block_id) {
            int index = block_id % capacity;
            if (cache_entries[index].valid && cache_entries[index].block_id == block_id) {
                cache_entries[index].dirty = true;
            }
        }
    };

#include <list>
#include <unordered_map>
   
    
class SetAssociativeCache {
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
    SetAssociativeCache(int size, int num_ways) 
        : capacity(size), ways(num_ways) {
        num_sets = capacity / ways;
        lru_lists.resize(num_sets);
        cache_maps.resize(num_sets);
    }

    bool access(int block_id, AdvancedStats& stats) {
        int set_index = block_id % num_sets;  // Determinar el conjunto
        auto& cache_map = cache_maps[set_index];
        auto& lru_list = lru_lists[set_index];

        auto it = cache_map.find(block_id);
        if (it != cache_map.end()) {
            // Actualizar LRU dentro del conjunto
            lru_list.erase(it->second.lru_it);
            lru_list.push_front(block_id);
            it->second.lru_it = lru_list.begin();
            stats.cache_hits++;
            return true;
        }
        stats.cache_misses++;
        return false;
    }

    void add_block(int block_id) {
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

    void mark_dirty(int block_id) {
        int set_index = block_id % num_sets;
        auto& cache_map = cache_maps[set_index];

        auto it = cache_map.find(block_id);
        if (it != cache_map.end()) {
            it->second.dirty = true;
        }
    }
};