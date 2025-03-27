#pragma once
#include "FileSystem.hpp"
#include "LRUCache.hpp"

// Declaracion ext4
class Ext4 : public FileSystem {
    private:
        LRUCache& cache;
        bool delayed_allocation;
        
        void extent_access(int address, AdvancedStats& stats);
    public:
        Ext4(LRUCache& c, int bs);
    
        void read(int address, AdvancedStats& stats) override;
    
        void write(int address, AdvancedStats& stats) override;
    
        void set_journal_mode(JournalingMode mode) override;
    };