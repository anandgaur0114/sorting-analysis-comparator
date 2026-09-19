#pragma once

#include <vector>
#include "types.hpp"

// ============================================================================
// FAST VARIANTS (Optimized for timing benchmarks, zero logging overhead)
// ============================================================================

// Standard Merge Sort with preallocated auxiliary buffer
void merge_sort(std::vector<int>& arr);

// Quick Sort with median-of-three pivot selection & tail-call optimization
void quick_sort(std::vector<int>& arr);

// Heap Sort using in-place binary max heap
void heap_sort(std::vector<int>& arr);

// LSD (Least Significant Digit) Radix Sort (Base 10)
void radix_sort(std::vector<int>& arr);

// ============================================================================
// INSTRUMENTED VARIANTS (Operation counting & step log for animation)
// ============================================================================

// Instrumented Merge Sort: counts comparisons & assignments, records steps if requested
SortMetrics merge_sort_instrumented(std::vector<int>& arr, bool record_steps = false);

// Instrumented Quick Sort: counts comparisons & swaps, records steps if requested
SortMetrics quick_sort_instrumented(std::vector<int>& arr, bool record_steps = false);

// Instrumented Heap Sort: counts comparisons & swaps, records steps if requested
SortMetrics heap_sort_instrumented(std::vector<int>& arr, bool record_steps = false);

// Instrumented Radix Sort: counts digit passes & bucket moves (comparisons = 0)
SortMetrics radix_sort_instrumented(std::vector<int>& arr, bool record_steps = false);
