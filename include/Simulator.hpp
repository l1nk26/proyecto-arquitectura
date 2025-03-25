#pragma once
#include <vector>
#include <string>
#include "FileSystem.hpp"
#include "Stats.hpp"

std::vector<int> generate_access_pattern(int num_ops, bool sequential);
void run_simulation(FileSystem& fs, std::vector<int>& addresses, AdvancedStats& stats);
void print_stats(const AdvancedStats& stats, const std::string& fs_name);