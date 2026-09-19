#pragma once

#include <string>
#include <vector>
#include "types.hpp"

// Single aggregated benchmark record
struct BenchmarkResult {
    std::string algorithm;
    std::string input_type;
    int n = 0;
    double avg_time_ms = 0.0;
    unsigned long long avg_comparisons = 0;
    unsigned long long avg_swaps = 0;
};

// Data generator functions
std::vector<int> generate_data(InputType type, int n, unsigned int seed = 42);

// Benchmark configuration
struct BenchmarkConfig {
    std::vector<int> sizes = {100, 500, 1000, 5000, 10000, 50000};
    std::vector<InputType> input_types = {
        InputType::RANDOM,
        InputType::NEARLY_SORTED,
        InputType::REVERSE_SORTED,
        InputType::MANY_DUPLICATES
    };
    int trials_per_config = 5;
    std::string csv_filename = "results.csv";
};

// Benchmark suite runner
class BenchmarkRunner {
public:
    explicit BenchmarkRunner(BenchmarkConfig config = BenchmarkConfig());

    // Runs full suite across all algorithms, input distributions, and scales
    std::vector<BenchmarkResult> run_all(bool verbose = true);

    // Save results to CSV
    static bool save_to_csv(const std::vector<BenchmarkResult>& results, const std::string& filename);

    // Load results from CSV
    static std::vector<BenchmarkResult> load_from_csv(const std::string& filename);

private:
    BenchmarkConfig config_;
};
