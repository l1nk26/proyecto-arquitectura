#pragma once
#include "JournalingMode.hpp"
#include "Stats.hpp"

// base abstracta wasa
class FileSystem {
    protected:
        int block_size;
        JournalingMode journal_mode;
        bool use_extents;
        
    public:
        virtual ~FileSystem() {}
        virtual void read(int address, AdvancedStats& stats) = 0;
        virtual void write(int address, AdvancedStats& stats) = 0;
        virtual void set_journal_mode(JournalingMode mode) = 0;
};