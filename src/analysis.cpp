#include "analysis.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <algorithm>

std::vector<GrowthRecord> ComplexityAnalyzer::analyze(const std::vector<BenchmarkResult>& results) {
    std::vector<GrowthRecord> records;

    // Group results by key: (algorithm, input_type) -> vector of results sorted by n
    std::map<std::pair<std::string, std::string>, std::vector<BenchmarkResult>> grouped;
    for (const auto& res : results) {
        grouped[{res.algorithm, res.input_type}].push_back(res);
    }

    for (auto& pair : grouped) {
        auto& list = pair.second;
        std::sort(list.begin(), list.end(), [](const BenchmarkResult& a, const BenchmarkResult& b) {
            return a.n < b.n;
        });

        for (size_t i = 1; i < list.size(); ++i) {
            const auto& prev = list[i - 1];
            const auto& curr = list[i];

            if (prev.n <= 0 || curr.n <= 0) continue;

            GrowthRecord gr;
            gr.algorithm = curr.algorithm;
            gr.input_type = curr.input_type;
            gr.n1 = prev.n;
            gr.n2 = curr.n;
            gr.t1_ms = prev.avg_time_ms;
            gr.t2_ms = curr.avg_time_ms;

            // Theoretical ratios
            double ratio_linear = static_cast<double>(curr.n) / prev.n;
            double n1_logn1 = prev.n * (std::log2(static_cast<double>(prev.n)));
            double n2_logn2 = curr.n * (std::log2(static_cast<double>(curr.n)));
            double ratio_nlogn = n2_logn2 / n1_logn1;
            double ratio_n2 = ratio_linear * ratio_linear;

            gr.expected_ratio_linear = ratio_linear;
            gr.expected_ratio_nlogn = ratio_nlogn;
            gr.expected_ratio_n2 = ratio_n2;

            // Observed time ratio & empirical alpha
            if (prev.avg_time_ms > 0.0001 && curr.avg_time_ms > 0.0001) {
                gr.observed_time_ratio = curr.avg_time_ms / prev.avg_time_ms;
                double log_scale = std::log(static_cast<double>(curr.n) / prev.n);
                if (log_scale > 1e-6) {
                    gr.empirical_alpha = std::log(gr.observed_time_ratio) / log_scale;
                }
            } else {
                gr.observed_time_ratio = 0.0;
                gr.empirical_alpha = 0.0;
            }

            // Comparisons ratio
            if (prev.avg_comparisons > 0) {
                gr.comp_ratio = static_cast<double>(curr.avg_comparisons) / prev.avg_comparisons;
            } else {
                gr.comp_ratio = 0.0; // E.g. Radix Sort
            }

            records.push_back(gr);
        }
    }

    return records;
}

void ComplexityAnalyzer::print_summary_table(const std::vector<GrowthRecord>& records) {
    std::cout << "\n";
    std::cout << "=========================================================================================================\n";
    std::cout << "                   EMPIRICAL VS. THEORETICAL COMPLEXITY GROWTH ANALYSIS TABLE                            \n";
    std::cout << "=========================================================================================================\n";
    std::cout << std::left
              << std::setw(12) << "Algorithm"
              << std::setw(16) << "Distribution"
              << std::setw(15) << "n1 -> n2"
              << std::setw(12) << "Obs Time (R)"
              << std::setw(13) << "Exp O(nlogn)"
              << std::setw(11) << "Exp O(n^2)"
              << std::setw(11) << "Exp O(n)"
              << std::setw(12) << "Obs Comps"
              << std::setw(10) << "Est. alpha"
              << "\n";
    std::cout << "---------------------------------------------------------------------------------------------------------\n";

    std::string last_algo = "";
    std::string last_type = "";

    for (const auto& r : records) {
        if (!last_algo.empty() && (last_algo != r.algorithm || last_type != r.input_type)) {
            std::cout << "---------------------------------------------------------------------------------------------------------\n";
        }
        last_algo = r.algorithm;
        last_type = r.input_type;

        std::string n_range = std::to_string(r.n1) + "->" + std::to_string(r.n2);

        std::cout << std::left
                  << std::setw(12) << r.algorithm
                  << std::setw(16) << r.input_type
                  << std::setw(15) << n_range;

        if (r.observed_time_ratio > 0.0) {
            std::cout << std::setw(12) << std::fixed << std::setprecision(2) << r.observed_time_ratio;
        } else {
            std::cout << std::setw(12) << "<0.01 ms";
        }

        std::cout << std::setw(13) << std::fixed << std::setprecision(2) << r.expected_ratio_nlogn
                  << std::setw(11) << std::fixed << std::setprecision(1) << r.expected_ratio_n2
                  << std::setw(11) << std::fixed << std::setprecision(1) << r.expected_ratio_linear;

        if (r.algorithm == "Radix Sort") {
            std::cout << std::setw(12) << "N/A (O(n+k))";
        } else {
            std::cout << std::setw(12) << std::fixed << std::setprecision(2) << r.comp_ratio;
        }

        if (r.empirical_alpha > 0.0) {
            std::cout << std::setw(10) << std::fixed << std::setprecision(2) << r.empirical_alpha;
        } else {
            std::cout << std::setw(10) << "N/A";
        }

        std::cout << "\n";
    }

    std::cout << "=========================================================================================================\n";
    std::cout << "Notes:\n";
    std::cout << "  - Obs Time (R): Ratio of execution time T(n2) / T(n1)\n";
    std::cout << "  - Exp O(nlogn): (n2*log2(n2)) / (n1*log2(n1))\n";
    std::cout << "  - Est. alpha: Fitted power exponent in T(n) = c * n^alpha. Alpha ~ 1.0-1.2 indicates O(n) / O(n log n).\n";
    std::cout << "  - Radix Sort comparisons are strictly 0 (non-comparative distribution sort).\n";
    std::cout << "=========================================================================================================\n\n";
}

void ComplexityAnalyzer::print_complexity_overview() {
    std::cout << "======================================================================\n";
    std::cout << "               THEORETICAL BIG-O COMPLEXITY SUMMARY                  \n";
    std::cout << "======================================================================\n";
    std::cout << " Algorithm   | Best Case     | Average Case  | Worst Case    | Space   \n";
    std::cout << "-------------+---------------+---------------+---------------+--------\n";
    std::cout << " Merge Sort  | O(n log n)    | O(n log n)    | O(n log n)    | O(n)    \n";
    std::cout << " Quick Sort  | O(n log n)    | O(n log n)    | O(n^2)        | O(log n)\n";
    std::cout << " Heap Sort   | O(n log n)    | O(n log n)    | O(n log n)    | O(1)    \n";
    std::cout << " Radix Sort  | O(d * (n + k))| O(d * (n + k))| O(d * (n + k))| O(n + k)\n";
    std::cout << "======================================================================\n\n";
}
