#include "Simulator.hpp"
#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>

void initialize_stat(AdvancedStats& stats) {
    stats.cache_hits = 0;
    stats.cache_misses = 0;
    stats.disk_reads = 0;
    stats.disk_writes = 0;
    stats.journal_ops = 0;
    stats.total_latency = 0;
    stats.avg_access_time = 0.0;
}

std::vector<int> generate_access_pattern(int num_ops, bool sequential) {
    std::vector<int> addresses;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (sequential) {
        for (int i = 0; i < num_ops; ++i) {
            addresses.push_back(i * 4096);  // Bloques de 4K
        }
    } else {
        std::uniform_int_distribution<> dist(0, 1 << 24);
        for (int i = 0; i < num_ops; ++i) {
            addresses.push_back(dist(gen));
        }
    }
    return addresses;
}

// Función de simulación
void run_simulation(FileSystem& fs, std::vector<int>& addresses, AdvancedStats& stats) {

    initialize_stat(stats);

    auto start = std::chrono::high_resolution_clock::now();
    
    for (int addr : addresses) {
        if (addr % 5 == 0) {  // 20% escrituras
            fs.write(addr, stats);
        } else {
            fs.read(addr, stats);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    stats.total_latency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    stats.avg_access_time = stats.total_latency / addresses.size();
}

void print_stats(const AdvancedStats& stats, const std::string& fs_name, Color c) {

    std::cout << "\033[" << c << "m";

    std::cout << "Estadísticas para " << fs_name << ":\n";
    std::cout << "--------------------------------\n";
    std::cout << "Aciertos de caché: " << stats.cache_hits << "\n";
    std::cout << "Fallos de caché: " << stats.cache_misses << "\n";
    std::cout << "Lecturas de disco: " << stats.disk_reads << "\n";
    std::cout << "Escrituras de disco: " << stats.disk_writes << "\n";
    std::cout << "Operaciones de journal: " << stats.journal_ops << "\n";
    std::cout << std::fixed << std::setprecision(8);
    std::cout << "Latencia total: " << stats.total_latency << " ms\n";
    std::cout << "Tiempo medio por acceso: " << stats.avg_access_time << " ms\n\n";

    std::cout << "\033[0m";
}