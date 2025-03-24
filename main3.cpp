#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <chrono>
#include <random>
#include <cmath>
#include <iomanip>
#include "stats.h"

using namespace std;
using namespace std::chrono;

// Políticas de Journaling
enum JournalingMode {
    NO_JOURNALING,
    METADATA_JOURNALING,
    FULL_JOURNALING
};

// Clase base abstracta para sistemas de archivos
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

// Implementación de caché con política LRU
class LRUCache {
    private:
        struct CacheEntry {
            int block_id;
            bool dirty;
            list<int>::iterator lru_it;
        };
    
        int capacity;
        list<int> lru_list;
        unordered_map<int, CacheEntry> cache_map;
    
    public:
        LRUCache(int size) : capacity(size) {}
    
        bool access(int block_id, AdvancedStats& stats) {
            auto it = cache_map.find(block_id);
            if (it != cache_map.end()) {
                // Actualizar LRU
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
            if (cache_map.size() >= capacity) {
                // Eliminar el menos recientemente usado
                int lru = lru_list.back();
                cache_map.erase(lru);
                lru_list.pop_back();
            }
            
            lru_list.push_front(block_id);
            cache_map[block_id] = {block_id, false, lru_list.begin()};
        }
    
        void mark_dirty(int block_id) {
            auto it = cache_map.find(block_id);
            if (it != cache_map.end()) {
                it->second.dirty = true;
            }
        }
    };

// Implementación para ext3
class Ext3 : public FileSystem {
private:
    LRUCache& cache;
    bool has_journal;
    
    void journal_operation(int address, AdvancedStats& stats) {
        stats.journal_ops++;
        // Acceso al journal (bloque especial 0)
        if (!cache.access(0, stats)) {
            cache.add_block(0);
            stats.disk_reads++;
        }
    }

public:
    Ext3(LRUCache& c, int bs) : cache(c) {
        block_size = bs;
        use_extents = false;
        journal_mode = METADATA_JOURNALING;
    }

    void read(int address, AdvancedStats& stats) override {
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

    void write(int address, AdvancedStats& stats) override {
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

    void set_journal_mode(JournalingMode mode) override {
        journal_mode = mode;
    }
};

// Implementación para ext4
class Ext4 : public FileSystem {
private:
    LRUCache& cache;
    bool delayed_allocation;
    
    void extent_access(int address, AdvancedStats& stats) {
        // Simular acceso por extensiones (4 bloques contiguos)
        int base_block = (address / block_size) & ~3;
        for (int i = 0; i < 4; ++i) {
            if (!cache.access(base_block + i, stats)) {
                cache.add_block(base_block + i);
                stats.disk_reads++;
            }
        }
    }

public:
    Ext4(LRUCache& c, int bs) : cache(c) {
        block_size = bs;
        use_extents = true;
        delayed_allocation = true;
    }

    void read(int address, AdvancedStats& stats) override {
        // Acceso mediante extensiones
        extent_access(address, stats);
    }

    void write(int address, AdvancedStats& stats) override {
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

    void set_journal_mode(JournalingMode mode) override {
        // Ext4 siempre usa journaling con checksum
    }
};

// Generador de patrones de acceso
vector<int> generate_access_pattern(int num_ops, bool sequential) {
    vector<int> addresses;
    random_device rd;
    mt19937 gen(rd());
    
    if (sequential) {
        for (int i = 0; i < num_ops; ++i) {
            addresses.push_back(i * 4096);  // Bloques de 4K
        }
    } else {
        uniform_int_distribution<> dist(0, 1 << 24);
        for (int i = 0; i < num_ops; ++i) {
            addresses.push_back(dist(gen));
        }
    }
    return addresses;
}

// Función de simulación
void run_simulation(FileSystem& fs, vector<int>& addresses, AdvancedStats& stats) {
    auto start = high_resolution_clock::now();
    
    for (int addr : addresses) {
        if (addr % 5 == 0) {  // 20% escrituras
            fs.write(addr, stats);
        } else {
            fs.read(addr, stats);
        }
    }
    
    auto end = high_resolution_clock::now();
    stats.total_latency = duration_cast<milliseconds>(end - start).count();
    stats.avg_access_time = stats.total_latency / addresses.size();
}

void print_stats(const AdvancedStats& stats, const string& fs_name) {
    cout << "Estadísticas para " << fs_name << ":\n";
    cout << "--------------------------------\n";
    cout << "Aciertos de caché: " << stats.cache_hits << "\n";
    cout << "Fallos de caché: " << stats.cache_misses << "\n";
    cout << "Lecturas de disco: " << stats.disk_reads << "\n";
    cout << "Escrituras de disco: " << stats.disk_writes << "\n";
    cout << "Operaciones de journal: " << stats.journal_ops << "\n";
    cout << fixed << setprecision(2);
    cout << "Latencia total: " << stats.total_latency << " ms\n";
    cout << "Tiempo medio por acceso: " << stats.avg_access_time << " ms\n\n";
}

int main() {
    const int CACHE_SIZE = 512;  // Bloques en caché
    const int BLOCK_SIZE = 4096; // 4KB
    const int NUM_OPS = 10000;
    
    // Configurar caché
    LRUCache cache(CACHE_SIZE);
    
    // Crear sistemas de archivos
    Ext3 ext3(cache, BLOCK_SIZE);
    Ext4 ext4(cache, BLOCK_SIZE);
    
    // Generar patrones de acceso
    vector<int> seq_access = generate_access_pattern(NUM_OPS, true);
    vector<int> rand_access = generate_access_pattern(NUM_OPS, false);
    
    // Ejecutar simulaciones
    AdvancedStats stats_ext3 = {}, stats_ext4 = {};
    
    cout << "=== Simulación con acceso secuencial ===\n";
    run_simulation(ext3, seq_access, stats_ext3);
    run_simulation(ext4, seq_access, stats_ext4);
    print_stats(stats_ext3, "ext3");
    print_stats(stats_ext4, "ext4");
    
    cout << "=== Simulación con acceso aleatorio ===\n";
    AdvancedStats stats_ext3_rand = {}, stats_ext4_rand = {};
    run_simulation(ext3, rand_access, stats_ext3_rand);
    run_simulation(ext4, rand_access, stats_ext4_rand);
    print_stats(stats_ext3_rand, "ext3");
    print_stats(stats_ext4_rand, "ext4");
    
    return 0;
}