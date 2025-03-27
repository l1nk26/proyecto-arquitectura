#pragma once
#include "FileSystem.hpp"
#include "LRUCache.hpp"
#include "Cache.hpp"

// Declaracion
class Ext3 : public FileSystem {
    private:
        Cache& cache;
        bool has_journal;
        
        void journal_operation(int address, AdvancedStats& stats);
    public:
        Ext3(Cache& c, int bs);
    
        void read(int address, AdvancedStats& stats) override;
    
        void write(int address, AdvancedStats& stats) override;
    
        void set_journal_mode(JournalingMode mode) override;
    };