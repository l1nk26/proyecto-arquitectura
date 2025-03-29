#include "Ext3.hpp"

// Implementación Ext3

Ext3::Ext3(Cache& c, int bs) : FileSystem(bs), cache(c) {
    use_extents = false;
    journal_mode = METADATA_JOURNALING;
}

void Ext3::journal_operation(int address, AdvancedStats& stats) {
    stats.journal_ops++;
    // Acceso al journal (bloque especial 0)
    if (!cache.access(0, stats)) {
        cache.add_block(0);
        stats.disk_reads++;
    }
}
    
void Ext3::read(int address, AdvancedStats& stats){
    int block_id = address / block_size;
    
    // Acceso a metadatos (bloque 1)
    if (!cache.access(1, stats)) {
        cache.add_block(1);
        stats.disk_reads++;
    }

    if (cache.access(block_id, stats)) {
        return;
    }
    
    cache.add_block(block_id);
    stats.disk_reads++;
}
    
void Ext3::write(int address, AdvancedStats& stats){
    int block_id = address / block_size;
    
    // Journaling
    if (journal_mode != NO_JOURNALING) {
        journal_operation(address, stats);
    }

    // Acceso a metadatos
    if (!cache.access(1, stats)) {
        cache.add_block(1);
        stats.disk_reads++;
    }

    if (!cache.access(block_id, stats)) {
        cache.add_block(block_id);
        stats.disk_reads++;
    }
    
    cache.mark_dirty(block_id);
    stats.disk_writes++;
}
    
void Ext3::set_journal_mode(JournalingMode mode){
    journal_mode = mode;
}