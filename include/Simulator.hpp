#pragma once
#include <vector>
#include <string>
#include "FileSystem.hpp"
#include "Stats.hpp"

enum Color {
    DEFAULT = 0,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36
};

std::vector<int> generate_access_pattern(int num_ops, bool sequential);
void run_simulation(FileSystem& fs, std::vector<int>& addresses, AdvancedStats& stats);
void print_stats(const AdvancedStats& stats, const std::string& fs_name);