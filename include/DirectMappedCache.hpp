#ifndef DIRECTMAPPEDCACHE
#define DIRECTMAPPEDCACHE

#include <vector>
#include "Cache.hpp"
#include "Stats.hpp"

class DirectMappedCache : public Cache {

private:
struct CacheEntry {
    int block_id;
    bool dirty;
    bool valid;  // Indica si la entrada contiene datos válidos
};

std::vector<CacheEntry> cache_entries;  // Usamos un vector para acceso directo

public:
    
    DirectMappedCache(int size);

    bool access(int block_id, AdvancedStats& stats) override;

    void add_block(int block_id) override;

    void mark_dirty(int block_id) override;
};
#endif