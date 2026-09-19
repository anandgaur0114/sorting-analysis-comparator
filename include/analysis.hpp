#pragma once

#include <vector>
#include <string>
#include "benchmark.hpp"

// Record of growth comparison between two successive input sizes n1 -> n2
struct GrowthRecord {
    std::string algorithm;
    std::string input_type;
    int n1 = 0;
    int n2 = 0;
    double t1_ms = 0.0;
    double t2_ms = 0.0;
    double observed_time_ratio = 0.0;
    double expected_ratio_nlogn = 0.0;
    double expected_ratio_n2 = 0.0;
    double expected_ratio_linear = 0.0;
    double empirical_alpha = 0.0; // alpha in T(n) ~ c * n^alpha
    double comp_ratio = 0.0;      // comparisons ratio C(n2) / C(n1)
    double expected_comp_ratio = 0.0;
};

// Complexity analysis engine
class ComplexityAnalyzer {
public:
    // Computes growth ratios from benchmark results
    static std::vector<GrowthRecord> analyze(const std::vector<BenchmarkResult>& results);

    // Prints beautiful formatted comparison table to stdout
    static void print_summary_table(const std::vector<GrowthRecord>& records);

    // Prints overall theoretical complexity conclusions
    static void print_complexity_overview();
};
