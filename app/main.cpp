#include "Simulator.hpp"
#include "Ext3.hpp"
#include "Ext4.hpp"
#include "SetAssociativeCache.hpp"
#include "DirectMappedCache.hpp"
#include <iostream>

using namespace std;

int main() {
    const int CACHE_SIZE = 512;  // Bloques en caché
    const int BLOCK_SIZE = 4096; // 4KB
    const int NUM_OPS = 10000;

    vector<int> seq_access = generate_access_pattern(NUM_OPS, true);
    vector<int> rand_access = generate_access_pattern(NUM_OPS, false);

    DirectMappedCache dmCache_ext3(CACHE_SIZE);
    DirectMappedCache dmCache_ext4(CACHE_SIZE);

    Ext3 ext3_dm(dmCache_ext3, BLOCK_SIZE);
    Ext4 ext4_dm(dmCache_ext4, BLOCK_SIZE);

    SetAssociativeCache saCache_ext3(CACHE_SIZE, 4);
    SetAssociativeCache saCache_ext4(CACHE_SIZE, 4);

    Ext3 ext3_sa(saCache_ext3, BLOCK_SIZE);
    Ext4 ext4_sa(saCache_ext4, BLOCK_SIZE);

    AdvancedStats stats_ext3 = {}, stats_ext4 = {};
    
    cout << "=== Simulación con acceso secuencial ===\n";
    run_simulation(ext3_dm, seq_access, stats_ext3);
    run_simulation(ext4_dm, seq_access, stats_ext4);
    print_stats(stats_ext3, "ext3 con cache directamente mapeada", RED);
    print_stats(stats_ext4, "ext4 con cache directamente mapeada", GREEN);

    run_simulation(ext3_sa, seq_access, stats_ext3);
    run_simulation(ext4_sa, seq_access, stats_ext4);
    print_stats(stats_ext3, "ext3 con cache asociativa por conjuntos de 4 vías", RED);
    print_stats(stats_ext4, "ext4 con cache asociativa por conjuntos de 4 vías", GREEN);

    cout << "=== Simulación con acceso aleatorio ===\n";
    run_simulation(ext3_dm, rand_access, stats_ext3);
    run_simulation(ext4_dm, rand_access, stats_ext4);
    print_stats(stats_ext3, "ext3 con cache directamente mapeada", RED);
    print_stats(stats_ext4, "ext4 con cache directamente mapeada", GREEN);

    run_simulation(ext3_sa, rand_access, stats_ext3);
    run_simulation(ext4_sa, rand_access, stats_ext4);
    print_stats(stats_ext3, "ext3 con cache asociativa por conjuntos de 4 vías", RED);
    print_stats(stats_ext4, "ext4 con cache asociativa por conjuntos de 4 vías", GREEN);

    print_stats_table(stats_ext3, stats_ext4);

    return 0;
}