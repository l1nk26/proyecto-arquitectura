#include "Simulator.hpp"
#include "Ext3.hpp"
#include "Ext4.hpp"
#include "SetAssociativeCache.hpp"
#include "DirectMappedCache.hpp"
#include <tabulate/table.hpp>
#include <iostream>

using namespace std;

int main() {

    using namespace tabulate;
    using Row_t = Table::Row_t;

    const int CACHE_SIZE = 512;  // Bloques en caché
    const int BLOCK_SIZE = 4096; // 4KB
    const int NUM_OPS = 10000;
    const int ways = 4;


    vector<int> seq_access = generate_access_pattern(NUM_OPS, true);
    vector<int> rand_access = generate_access_pattern(NUM_OPS, false);

    DirectMappedCache dmCache_ext3(CACHE_SIZE);
    DirectMappedCache dmCache_ext4(CACHE_SIZE);

    Ext3 ext3_dm(dmCache_ext3, BLOCK_SIZE);
    Ext4 ext4_dm(dmCache_ext4, BLOCK_SIZE);

    SetAssociativeCache saCache_ext3(CACHE_SIZE, ways);
    SetAssociativeCache saCache_ext4(CACHE_SIZE, ways);

    Ext3 ext3_sa(saCache_ext3, BLOCK_SIZE);
    Ext4 ext4_sa(saCache_ext4, BLOCK_SIZE);


    Table t_main;
    Table sub_main1;
    Table sub_main2;
    t_main.format().hide_border();
    Table sub_table1;
    Table sub_table2;

    AdvancedStats stats_ext3 = {}, stats_ext4 = {};
    
    t_main.add_row(Row_t{"=== Simulación con acceso secuencial ==="});
    run_simulation(ext3_dm, seq_access, stats_ext3);
    run_simulation(ext4_dm, seq_access, stats_ext4);
    std::string name1 = "Con cache por correspondecia directa";
    sub_table1 = print_stats_table(stats_ext3, stats_ext4, name1);

    run_simulation(ext3_sa, seq_access, stats_ext3);
    run_simulation(ext4_sa, seq_access, stats_ext4);
    std::string name2 = "Con cache asociativa por conjutos";
    sub_table2 = print_stats_table(stats_ext3, stats_ext4, name2);

    sub_main1.add_row(Row_t{sub_table1, sub_table2});
    t_main.add_row(Row_t{sub_main1});

    t_main.add_row(Row_t{"=== Simulación con acceso aleatorio ==="});
    run_simulation(ext3_dm, rand_access, stats_ext3);
    run_simulation(ext4_dm, rand_access, stats_ext4);
    std::string name3 = "Con cache por correspondencia directa";
    sub_table1 = print_stats_table(stats_ext3, stats_ext4, name3);
    
    run_simulation(ext3_sa, rand_access, stats_ext3);
    run_simulation(ext4_sa, rand_access, stats_ext4);
    std::string name4 = "Con cache asociativa por conjutos";
    sub_table2 = print_stats_table(stats_ext3, stats_ext4, name4);

    sub_main2.add_row(Row_t{sub_table1, sub_table2});
    t_main.add_row(Row_t{sub_main2});

    t_main[0].format().font_align(FontAlign::center);

    t_main[1].format()
        .font_align(FontAlign::center)
        .font_color(Color::blue)
        .font_style({FontStyle::italic});

    t_main[2].format().font_align(FontAlign::center);

    t_main[3].format()
        .font_align(FontAlign::center)
        .font_color(Color::green)
        .font_style({FontStyle::italic});
    
    cout << t_main << "\n";
    return 0;
}