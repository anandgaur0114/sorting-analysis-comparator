#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include "types.hpp"
#include "sorts.hpp"
#include "benchmark.hpp"
#include "analysis.hpp"
#include "visualizer.hpp"

// Self-test suite to verify mathematical and operational correctness of all algorithms
bool run_self_tests() {
    std::cout << ">>> Running algorithm correctness self-tests...\n";

    struct TestAlgo {
        std::string name;
        void (*fast_sort)(std::vector<int>&);
        SortMetrics (*instr_sort)(std::vector<int>&, bool);
    };

    std::vector<TestAlgo> algos = {
        {"Merge Sort", merge_sort, merge_sort_instrumented},
        {"Quick Sort", quick_sort, quick_sort_instrumented},
        {"Heap Sort",  heap_sort,  heap_sort_instrumented},
        {"Radix Sort", radix_sort, radix_sort_instrumented}
    };

    std::vector<int> test_sizes = {0, 1, 2, 10, 100, 1000};
    std::vector<InputType> test_types = {
        InputType::RANDOM,
        InputType::NEARLY_SORTED,
        InputType::REVERSE_SORTED,
        InputType::MANY_DUPLICATES
    };

    for (const auto& a : algos) {
        for (int sz : test_sizes) {
            for (auto t : test_types) {
                auto data1 = generate_data(t, sz, 123 + sz);
                auto data2 = data1;

                // Test fast sort
                a.fast_sort(data1);
                if (!std::is_sorted(data1.begin(), data1.end())) {
                    std::cerr << "FAIL: " << a.name << " fast sort failed on size " << sz << "!\n";
                    return false;
                }

                // Test instrumented sort
                SortMetrics m = a.instr_sort(data2, sz <= 50);
                if (!std::is_sorted(data2.begin(), data2.end())) {
                    std::cerr << "FAIL: " << a.name << " instrumented sort failed on size " << sz << "!\n";
                    return false;
                }

                if (sz <= 50 && sz > 1) {
                    // Check that steps were generated when requested
                    if (m.steps.empty() && a.name != "Radix Sort") {
                        std::cerr << "WARNING: " << a.name << " produced 0 animation steps on size " << sz << "\n";
                    }
                }
            }
        }
    }

    std::cout << ">>> All algorithms passed 100% of correctness tests!\n\n";
    return true;
}

void print_help(const char* prog_name) {
    std::cout << "Sorting Analysis Comparator (SAC) - Analysis of Algorithms\n";
    std::cout << "Usage: " << prog_name << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --all         (Default) Run self-tests, full benchmark, and print complexity analysis\n";
    std::cout << "  --test        Run correctness self-tests on all 4 sorting algorithms\n";
    std::cout << "  --benchmark   Run benchmark suite and export to results.csv\n";
    std::cout << "  --analyze     Load results.csv and display empirical vs theoretical complexity\n";
    std::cout << "  --visualize   Launch SFML visualizer module (Phase 2)\n";
    std::cout << "  --help        Show this help message\n";
}

int main(int argc, char* argv[]) {
    std::string mode = "";
    if (argc > 1) {
        mode = argv[1];
    } else {
        std::cout << "======================================================================\n";
        std::cout << "           SORTING ANALYSIS COMPARATOR (SAC) - MAIN MENU              \n";
        std::cout << "======================================================================\n";
        std::cout << "  [1] Launch SFML Interactive Visualizer (Single, Race & Plots)\n";
        std::cout << "  [2] Run Full Benchmark Suite (Export to results.csv)\n";
        std::cout << "  [3] Show Empirical vs. Theoretical Complexity Table\n";
        std::cout << "  [4] Run Correctness Self-Tests (All Algorithms & Distributions)\n";
        std::cout << "  [5] Run Complete Suite (Tests + Benchmarks + Analysis)\n";
        std::cout << "  [0] Exit\n";
        std::cout << "======================================================================\n";
        std::cout << "Select an option [1-5]: ";

        int choice = 1;
        if (!(std::cin >> choice)) {
            choice = 1;
        }

        switch (choice) {
            case 1: mode = "--visualize"; break;
            case 2: mode = "--benchmark"; break;
            case 3: mode = "--analyze";   break;
            case 4: mode = "--test";      break;
            case 5: mode = "--all";       break;
            default: return 0;
        }
    }

    if (mode == "--help" || mode == "-h") {
        print_help(argv[0]);
        return 0;
    }

    if (mode == "--test") {
        if (!run_self_tests()) return 1;
        return 0;
    }

    if (mode == "--visualize") {
        SortingVisualizer viz;
        viz.run();
        return 0;
    }

    if (mode == "--analyze") {
        ComplexityAnalyzer::print_complexity_overview();
        auto results = BenchmarkRunner::load_from_csv("results.csv");
        if (results.empty()) {
            std::cerr << "No results found in results.csv. Running quick benchmark first...\n";
            BenchmarkConfig cfg;
            cfg.sizes = {100, 500, 1000, 5000, 10000, 50000};
            BenchmarkRunner runner(cfg);
            results = runner.run_all(false);
        }
        auto growth = ComplexityAnalyzer::analyze(results);
        ComplexityAnalyzer::print_summary_table(growth);
        return 0;
    }

    if (mode == "--benchmark") {
        BenchmarkConfig config;
        config.sizes = {100, 500, 1000, 5000, 10000, 50000};
        config.trials_per_config = 5;
        config.csv_filename = "results.csv";
        BenchmarkRunner runner(config);
        runner.run_all(true);
        return 0;
    }

    // Default or --all
    if (!run_self_tests()) {
        std::cerr << "Aborting benchmarks due to test failure.\n";
        return 1;
    }

    BenchmarkConfig config;
    config.sizes = {100, 500, 1000, 5000, 10000, 50000};
    config.trials_per_config = 5;
    config.csv_filename = "results.csv";

    BenchmarkRunner runner(config);
    auto results = runner.run_all(true);

    ComplexityAnalyzer::print_complexity_overview();
    auto growth = ComplexityAnalyzer::analyze(results);
    ComplexityAnalyzer::print_summary_table(growth);

    return 0;
}
