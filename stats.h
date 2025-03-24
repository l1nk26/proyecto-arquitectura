#ifndef STATS
#define STATS

struct AdvancedStats {
    int cache_hits;
    int cache_misses;
    int disk_reads;
    int disk_writes;
    int journal_ops;
    double total_latency;
    double avg_access_time;
};
#endif