#include "Ext4.hpp"

// Implementación ext4

Ext4::Ext4(Cache& c, int bs) : FileSystem(bs), cache(c) {
    use_extents = true;
    delayed_allocation = true;
}

void Ext4::extent_access(int address, AdvancedStats& stats) {
    // Simular acceso por extensiones (4 bloques contiguos)
    int base_block = (address / block_size) & ~3;
    for (int i = 0; i < 4; ++i) {
        if (!cache.access(base_block + i, stats)) {
            cache.add_block(base_block + i);
            stats.disk_reads++;
        }
    }
}

void Ext4::read(int address, AdvancedStats& stats){
    // Acceso mediante extensiones
    extent_access(address, stats);
}

void Ext4::write(int address, AdvancedStats& stats){
    // Escritura diferida
    if (delayed_allocation) {
        int block_id = address / block_size;
        if (!cache.access(block_id, stats)) {
            cache.add_block(block_id);
        }
        cache.mark_dirty(block_id);
    } else {
        // Similar a ext3 pero con extensiones
        extent_access(address, stats);
        stats.disk_writes++;
    }
}

void Ext4::set_journal_mode(JournalingMode mode){
    // Ext4 siempre usa journaling con checksum
}