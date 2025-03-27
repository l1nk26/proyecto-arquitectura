#include "DirectMappedCache.hpp"

DirectMappedCache::DirectMappedCache(int size) : Cache(size) {
    cache_entries.resize(size, {-1, false, false});  // Inicializar entradas como inválidas
}

bool DirectMappedCache::access(int block_id, AdvancedStats& stats) {
    int index = block_id % capacity;  // Función de correspondencia directa
    if (cache_entries[index].valid && cache_entries[index].block_id == block_id) {
        stats.cache_hits++;
        return true;
    }
    stats.cache_misses++;
    return false;
}

void DirectMappedCache::add_block(int block_id) {
    int index = block_id % capacity;
    cache_entries[index] = {block_id, false, true};  // Reemplazar directamente
}

void DirectMappedCache::mark_dirty(int block_id) {
    int index = block_id % capacity;
    if (cache_entries[index].valid && cache_entries[index].block_id == block_id) {
        cache_entries[index].dirty = true;
    }
}