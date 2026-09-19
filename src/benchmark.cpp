#include "benchmark.hpp"
#include "sorts.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>
#include <algorithm>
#include <cassert>

std::vector<int> generate_data(InputType type, int n, unsigned int seed) {
    if (n <= 0) return {};
    std::vector<int> data(n);
    if (n == 1) {
        data[0] = 1;
        return data;
    }
    std::mt19937 rng(seed);

    switch (type) {
        case InputType::RANDOM: {
            std::uniform_int_distribution<int> dist(1, 100000);
            for (int i = 0; i < n; ++i) {
                data[i] = dist(rng);
            }
            break;
        }
        case InputType::NEARLY_SORTED: {
            for (int i = 0; i < n; ++i) {
                data[i] = i + 1;
            }
            // Swap ~5% of elements randomly
            int num_swaps = std::max(1, static_cast<int>(n * 0.05));
            std::uniform_int_distribution<int> dist_idx(0, n - 1);
            for (int i = 0; i < num_swaps; ++i) {
                int idx1 = dist_idx(rng);
                int idx2 = dist_idx(rng);
                std::swap(data[idx1], data[idx2]);
            }
            break;
        }
        case InputType::REVERSE_SORTED: {
            for (int i = 0; i < n; ++i) {
                data[i] = n - i;
            }
            break;
        }
        case InputType::MANY_DUPLICATES: {
            std::uniform_int_distribution<int> dist(1, 10); // small range [1, 10] creates many collisions
            for (int i = 0; i < n; ++i) {
                data[i] = dist(rng);
            }
            break;
        }
    }
    return data;
}

BenchmarkRunner::BenchmarkRunner(BenchmarkConfig config)
    : config_(config) {}

std::vector<BenchmarkResult> BenchmarkRunner::run_all(bool verbose) {
    std::vector<BenchmarkResult> results;

    struct AlgoEntry {
        std::string name;
        void (*fast_func)(std::vector<int>&);
        SortMetrics (*instr_func)(std::vector<int>&, bool);
    };

    std::vector<AlgoEntry> algorithms = {
        {"Merge Sort", merge_sort, merge_sort_instrumented},
        {"Quick Sort", quick_sort, quick_sort_instrumented},
        {"Heap Sort",  heap_sort,  heap_sort_instrumented},
        {"Radix Sort", radix_sort, radix_sort_instrumented}
    };

    if (verbose) {
        std::cout << "======================================================================\n";
        std::cout << "           STARTING SORTING ANALYSIS COMPARATOR BENCHMARK             \n";
        std::cout << "======================================================================\n";
        std::cout << "Trials per configuration: " << config_.trials_per_config << "\n";
        std::cout << "Scales: ";
        for (int s : config_.sizes) std::cout << s << " ";
        std::cout << "\n\n";
    }

    for (const auto& algo : algorithms) {
        if (verbose) {
            std::cout << ">>> Benchmarking: " << algo.name << " <<<\n";
        }

        for (auto input_type : config_.input_types) {
            std::string type_name = input_type_to_string(input_type);

            for (int n : config_.sizes) {
                double total_time_ms = 0.0;
                unsigned long long total_comparisons = 0;
                unsigned long long total_swaps = 0;

                for (int trial = 0; trial < config_.trials_per_config; ++trial) {
                    unsigned int seed = 42 + trial * 1000 + n;
                    auto base_data = generate_data(input_type, n, seed);

                    // 1. Time the fast variant (adaptive repetition for sub-microsecond precision on small N)
                    {
                        int repeats = (n <= 500 ? 50 : (n <= 2000 ? 10 : 1));
                        std::vector<std::vector<int>> copies(repeats, base_data);

                        auto t_start = HighResClock::now();
                        for (int r = 0; r < repeats; ++r) {
                            algo.fast_func(copies[r]);
                        }
                        auto t_end = HighResClock::now();

                        for (int r = 0; r < repeats; ++r) {
                            if (!std::is_sorted(copies[r].begin(), copies[r].end())) {
                                std::cerr << "ERROR: " << algo.name << " failed to sort array of size " << n << "!\n";
                                break;
                            }
                        }

                        double elapsed = HighResClock::duration_ms(t_start, t_end) / repeats;
                        total_time_ms += elapsed;
                    }

                    // 2. Obtain operation counts from the instrumented variant (record_steps = false)
                    {
                        auto arr_metric = base_data;
                        SortMetrics metrics = algo.instr_func(arr_metric, false);

                        if (!std::is_sorted(arr_metric.begin(), arr_metric.end())) {
                            std::cerr << "ERROR: Instrumented " << algo.name << " failed to sort array of size " << n << "!\n";
                        }

                        total_comparisons += metrics.comparisons;
                        total_swaps += metrics.swaps;
                    }
                }

                BenchmarkResult res;
                res.algorithm = algo.name;
                res.input_type = type_name;
                res.n = n;
                res.avg_time_ms = total_time_ms / config_.trials_per_config;
                res.avg_comparisons = total_comparisons / config_.trials_per_config;
                res.avg_swaps = total_swaps / config_.trials_per_config;

                results.push_back(res);

                if (verbose) {
                    std::cout << std::left << std::setw(12) << algo.name << " | "
                              << std::setw(15) << type_name << " | "
                              << "N = " << std::setw(6) << n << " | "
                              << "Time: " << std::fixed << std::setprecision(4) << std::setw(8) << res.avg_time_ms << " ms | "
                              << "Comps: " << std::setw(9) << res.avg_comparisons << " | "
                              << "Swaps: " << std::setw(9) << res.avg_swaps << "\n";
                }
            }
        }
        if (verbose) std::cout << "\n";
    }

    if (!config_.csv_filename.empty()) {
        save_to_csv(results, config_.csv_filename);
        if (verbose) {
            std::cout << "All benchmark results successfully exported to: " << config_.csv_filename << "\n\n";
        }
    }

    return results;
}

bool BenchmarkRunner::save_to_csv(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open CSV file for writing: " << filename << "\n";
        return false;
    }

    file << "algorithm,input_type,n,avg_time_ms,comparisons,swaps\n";
    for (const auto& r : results) {
        file << r.algorithm << ","
             << r.input_type << ","
             << r.n << ","
             << std::fixed << std::setprecision(6) << r.avg_time_ms << ","
             << r.avg_comparisons << ","
             << r.avg_swaps << "\n";
    }
    file.close();
    return true;
}

std::vector<BenchmarkResult> BenchmarkRunner::load_from_csv(const std::string& filename) {
    std::vector<BenchmarkResult> results;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open CSV file: " << filename << "\n";
        return results;
    }

    std::string line;
    // Skip header
    if (!std::getline(file, line)) return results;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string algo, type_str, n_str, time_str, comps_str, swaps_str;

        if (std::getline(ss, algo, ',') &&
            std::getline(ss, type_str, ',') &&
            std::getline(ss, n_str, ',') &&
            std::getline(ss, time_str, ',') &&
            std::getline(ss, comps_str, ',') &&
            std::getline(ss, swaps_str, ',')) {

            BenchmarkResult r;
            r.algorithm = algo;
            r.input_type = type_str;
            r.n = std::stoi(n_str);
            r.avg_time_ms = std::stod(time_str);
            r.avg_comparisons = std::stoull(comps_str);
            r.avg_swaps = std::stoull(swaps_str);
            results.push_back(r);
        }
    }
    file.close();
    return results;
}
