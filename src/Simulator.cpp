#include "Simulator.hpp"
#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <tabulate/table.hpp>
#include <string>

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

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9); // Generar números entre 0 y 9

    auto start = std::chrono::high_resolution_clock::now();

    for (int addr : addresses) {
        int operation = dist(gen); // Generar operación aleatoria
        if (operation < 2) {  // 20% escrituras
            fs.write(addr, stats);
        } else {
            fs.read(addr, stats);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    stats.total_latency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    stats.avg_access_time = stats.total_latency / addresses.size();
}

void print_stats(const AdvancedStats& stats, const std::string& fs_name, COLOR c) {

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

void print_stats_table(const AdvancedStats& stats_ext3, const AdvancedStats& stats_ext4) {
	using namespace tabulate;
	using Row_t = Table::Row_t;
	
	Table stats;
	stats.format().border_color(Color::yellow);
	
	stats.add_row(Row_t{"Sistemas de archivos", "Ext3", "Ext4"});
	stats.add_row(Row_t{"Aciertos de cache", std::to_string(stats_ext3.cache_hits), std::to_string(stats_ext4.cache_hits)});
	stats.add_row(Row_t{"Fallos de cache", std::to_string(stats_ext3.cache_misses), std::to_string(stats_ext4.cache_misses)});
	stats.add_row(Row_t{"Lecturas de disco", std::to_string(stats_ext3.disk_reads), std::to_string(stats_ext4.disk_reads)});
	stats.add_row(Row_t{"Escrituras de disco", std::to_string(stats_ext3.disk_writes), std::to_string(stats_ext4.disk_writes)});
	stats.add_row(Row_t{"Operaciones de journal", std::to_string(stats_ext3.journal_ops), std::to_string(stats_ext4.journal_ops)});
	stats.add_row(Row_t{"Latencia Total", std::to_string(stats_ext3.total_latency), std::to_string(stats_ext4.total_latency)});
	stats.add_row(Row_t{"Tiempo medio por acceso", std::to_string(stats_ext3.avg_access_time), std::to_string(stats_ext4.avg_access_time)});
	
	std::cout << stats << "\n\n";
}

