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
    
    // Configurar caché
    DirectMappedCache dmCache(CACHE_SIZE);
    SetAssociativeCache saCache(CACHE_SIZE, 4);

    // Crear sistemas de archivos
    Ext3 ext3(dmCache, BLOCK_SIZE);
    Ext4 ext4(dmCache, BLOCK_SIZE);
    
    // Generar patrones de acceso
    vector<int> seq_access = generate_access_pattern(NUM_OPS, true);
    vector<int> rand_access = generate_access_pattern(NUM_OPS, false);
    
    // Ejecutar simulaciones
    AdvancedStats stats_ext3 = {}, stats_ext4 = {};
    
    cout << "=== Simulación con acceso secuencial ===\n";
    run_simulation(ext3, seq_access, stats_ext3);
    run_simulation(ext4, seq_access, stats_ext4);
    print_stats(stats_ext3, "ext3 con cache directamente mapeada");
    print_stats(stats_ext4, "ext4 con cache directamente mapeada");

    
    
    cout << "=== Simulación con acceso aleatorio ===\n";
    AdvancedStats stats_ext3_rand = {}, stats_ext4_rand = {};
    run_simulation(ext3, rand_access, stats_ext3_rand);
    run_simulation(ext4, rand_access, stats_ext4_rand);
    print_stats(stats_ext3_rand, "ext3");
    print_stats(stats_ext4_rand, "ext4");
    
    return 0;
}