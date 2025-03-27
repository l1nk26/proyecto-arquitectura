#ifndef CACHE
#define CACHE

#include "Stats.hpp"

class Cache {

    protected:
        unsigned int capacity; // numero de bloques
        
    public:

        Cache(int size) : capacity(size) {}

        Cache(Cache const &c) : capacity(c.capacity) {}
    
        virtual bool access(int block_id, AdvancedStats& stats) = 0;
    
        virtual void add_block(int block_id) = 0;
    
        virtual void mark_dirty(int block_id) = 0;
};

#endif