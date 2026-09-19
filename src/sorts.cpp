#include "sorts.hpp"
#include <algorithm>
#include <chrono>
#include <climits>

// ============================================================================
// FAST VARIANTS (Optimized for timing benchmarks, zero logging overhead)
// ============================================================================

// --- Merge Sort ---
namespace {
void merge_fast(std::vector<int>& arr, std::vector<int>& aux, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            aux[k++] = arr[i++];
        } else {
            aux[k++] = arr[j++];
        }
    }
    while (i <= mid) aux[k++] = arr[i++];
    while (j <= right) aux[k++] = arr[j++];

    for (int p = left; p <= right; ++p) {
        arr[p] = aux[p];
    }
}

void merge_sort_fast_impl(std::vector<int>& arr, std::vector<int>& aux, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    merge_sort_fast_impl(arr, aux, left, mid);
    merge_sort_fast_impl(arr, aux, mid + 1, right);
    merge_fast(arr, aux, left, mid, right);
}
} // namespace

void merge_sort(std::vector<int>& arr) {
    if (arr.size() <= 1) return;
    std::vector<int> aux(arr.size());
    merge_sort_fast_impl(arr, aux, 0, static_cast<int>(arr.size()) - 1);
}

// --- Quick Sort ---
namespace {
void pick_median_of_three(std::vector<int>& arr, int low, int high) {
    int mid = low + (high - low) / 2;
    if (arr[mid] < arr[low]) std::swap(arr[low], arr[mid]);
    if (arr[high] < arr[low]) std::swap(arr[low], arr[high]);
    if (arr[mid] < arr[high]) std::swap(arr[mid], arr[high]);
    // Now arr[low] <= arr[high] <= arr[mid]; median of three is at arr[high]
}

int lomuto_partition_fast(std::vector<int>& arr, int low, int high) {
    if (high - low >= 2) {
        pick_median_of_three(arr, low, high);
    } else if (arr[high] < arr[low]) {
        std::swap(arr[low], arr[high]);
    }
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (arr[j] <= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quick_sort_fast_impl(std::vector<int>& arr, int low, int high) {
    while (low < high) {
        int p = lomuto_partition_fast(arr, low, high);
        // Tail-recursion elimination to ensure O(log n) stack depth
        if (p - low < high - p) {
            quick_sort_fast_impl(arr, low, p - 1);
            low = p + 1;
        } else {
            quick_sort_fast_impl(arr, p + 1, high);
            high = p - 1;
        }
    }
}
} // namespace

void quick_sort(std::vector<int>& arr) {
    if (arr.size() <= 1) return;
    quick_sort_fast_impl(arr, 0, static_cast<int>(arr.size()) - 1);
}

// --- Heap Sort ---
namespace {
void sift_down_fast(std::vector<int>& arr, int n, int i) {
    while (true) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && arr[left] > arr[largest]) {
            largest = left;
        }
        if (right < n && arr[right] > arr[largest]) {
            largest = right;
        }

        if (largest == i) break;
        std::swap(arr[i], arr[largest]);
        i = largest;
    }
}
} // namespace

void heap_sort(std::vector<int>& arr) {
    int n = static_cast<int>(arr.size());
    if (n <= 1) return;

    // Build max heap in O(n)
    for (int i = n / 2 - 1; i >= 0; --i) {
        sift_down_fast(arr, n, i);
    }

    // Repeatedly extract root to end
    for (int i = n - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);
        sift_down_fast(arr, i, 0);
    }
}

// --- Radix Sort (LSD Base 10) ---
void radix_sort(std::vector<int>& arr) {
    int n = static_cast<int>(arr.size());
    if (n <= 1) return;

    // Handle potential negative numbers by shifting to non-negative domain
    int min_val = arr[0];
    int max_val = arr[0];
    for (int i = 1; i < n; ++i) {
        if (arr[i] < min_val) min_val = arr[i];
        if (arr[i] > max_val) max_val = arr[i];
    }

    long long shift = 0;
    if (min_val < 0) {
        shift = -static_cast<long long>(min_val);
        for (int i = 0; i < n; ++i) {
            arr[i] = static_cast<int>(arr[i] + shift);
        }
        max_val = static_cast<int>(max_val + shift);
    }

    std::vector<int> output(n);
    for (long long exp = 1; max_val / exp > 0; exp *= 10) {
        int count[10] = {0};

        for (int i = 0; i < n; ++i) {
            int digit = (arr[i] / exp) % 10;
            count[digit]++;
        }

        for (int i = 1; i < 10; ++i) {
            count[i] += count[i - 1];
        }

        for (int i = n - 1; i >= 0; --i) {
            int digit = (arr[i] / exp) % 10;
            output[--count[digit]] = arr[i];
        }

        for (int i = 0; i < n; ++i) {
            arr[i] = output[i];
        }
    }

    // Restore original values if shifted
    if (shift > 0) {
        for (int i = 0; i < n; ++i) {
            arr[i] = static_cast<int>(arr[i] - shift);
        }
    }
}

// ============================================================================
// INSTRUMENTED VARIANTS (Counting & optional step logging)
// ============================================================================

// --- Instrumented Merge Sort ---
namespace {
void merge_instrumented(std::vector<int>& arr, std::vector<int>& aux,
                        int left, int mid, int right,
                        SortMetrics& metrics, bool record_steps) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        metrics.comparisons++;
        if (record_steps) {
            metrics.steps.push_back({StepType::COMPARE, i, j, 0});
        }

        if (arr[i] <= arr[j]) {
            aux[k++] = arr[i++];
        } else {
            aux[k++] = arr[j++];
        }
    }

    while (i <= mid) {
        aux[k++] = arr[i++];
    }
    while (j <= right) {
        aux[k++] = arr[j++];
    }

    for (int p = left; p <= right; ++p) {
        arr[p] = aux[p];
        metrics.swaps++; // Count element placement/assignment
        if (record_steps) {
            metrics.steps.push_back({StepType::ASSIGN, p, -1, aux[p]});
        }
    }
}

void merge_sort_instr_impl(std::vector<int>& arr, std::vector<int>& aux,
                           int left, int right,
                           SortMetrics& metrics, bool record_steps) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    merge_sort_instr_impl(arr, aux, left, mid, metrics, record_steps);
    merge_sort_instr_impl(arr, aux, mid + 1, right, metrics, record_steps);
    merge_instrumented(arr, aux, left, mid, right, metrics, record_steps);
}
} // namespace

SortMetrics merge_sort_instrumented(std::vector<int>& arr, bool record_steps) {
    SortMetrics metrics;
    if (arr.size() <= 1) return metrics;

    auto start_time = HighResClock::now();
    std::vector<int> aux(arr.size());
    merge_sort_instr_impl(arr, aux, 0, static_cast<int>(arr.size()) - 1, metrics, record_steps);
    auto end_time = HighResClock::now();

    metrics.duration_ms = HighResClock::duration_ms(start_time, end_time);
    return metrics;
}

// --- Instrumented Quick Sort ---
namespace {
void pick_median_of_three_instr(std::vector<int>& arr, int low, int high,
                                SortMetrics& metrics, bool record_steps) {
    int mid = low + (high - low) / 2;

    metrics.comparisons++;
    if (record_steps) metrics.steps.push_back({StepType::COMPARE, low, mid, 0});
    if (arr[mid] < arr[low]) {
        std::swap(arr[low], arr[mid]);
        metrics.swaps++;
        if (record_steps) metrics.steps.push_back({StepType::SWAP, low, mid, 0});
    }

    metrics.comparisons++;
    if (record_steps) metrics.steps.push_back({StepType::COMPARE, low, high, 0});
    if (arr[high] < arr[low]) {
        std::swap(arr[low], arr[high]);
        metrics.swaps++;
        if (record_steps) metrics.steps.push_back({StepType::SWAP, low, high, 0});
    }

    metrics.comparisons++;
    if (record_steps) metrics.steps.push_back({StepType::COMPARE, high, mid, 0});
    if (arr[mid] < arr[high]) {
        std::swap(arr[mid], arr[high]);
        metrics.swaps++;
        if (record_steps) metrics.steps.push_back({StepType::SWAP, mid, high, 0});
    }
}

int lomuto_partition_instr(std::vector<int>& arr, int low, int high,
                           SortMetrics& metrics, bool record_steps) {
    if (high - low >= 2) {
        pick_median_of_three_instr(arr, low, high, metrics, record_steps);
    } else {
        metrics.comparisons++;
        if (record_steps) metrics.steps.push_back({StepType::COMPARE, low, high, 0});
        if (arr[high] < arr[low]) {
            std::swap(arr[low], arr[high]);
            metrics.swaps++;
            if (record_steps) metrics.steps.push_back({StepType::SWAP, low, high, 0});
        }
    }
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        metrics.comparisons++;
        if (record_steps) {
            metrics.steps.push_back({StepType::COMPARE, j, high, 0});
        }
        if (arr[j] <= pivot) {
            ++i;
            if (i != j) {
                std::swap(arr[i], arr[j]);
                metrics.swaps++;
                if (record_steps) {
                    metrics.steps.push_back({StepType::SWAP, i, j, 0});
                }
            }
        }
    }

    if (i + 1 != high) {
        std::swap(arr[i + 1], arr[high]);
        metrics.swaps++;
        if (record_steps) {
            metrics.steps.push_back({StepType::SWAP, i + 1, high, 0});
        }
    }
    if (record_steps) {
        metrics.steps.push_back({StepType::MARK_SORTED, i + 1, -1, 0});
    }

    return i + 1;
}

void quick_sort_instr_impl(std::vector<int>& arr, int low, int high,
                           SortMetrics& metrics, bool record_steps) {
    while (low < high) {
        int p = lomuto_partition_instr(arr, low, high, metrics, record_steps);
        if (p - low < high - p) {
            quick_sort_instr_impl(arr, low, p - 1, metrics, record_steps);
            low = p + 1;
        } else {
            quick_sort_instr_impl(arr, p + 1, high, metrics, record_steps);
            high = p - 1;
        }
    }
    if (low == high && record_steps) {
        metrics.steps.push_back({StepType::MARK_SORTED, low, -1, 0});
    }
}
} // namespace

SortMetrics quick_sort_instrumented(std::vector<int>& arr, bool record_steps) {
    SortMetrics metrics;
    if (arr.size() <= 1) return metrics;

    auto start_time = HighResClock::now();
    quick_sort_instr_impl(arr, 0, static_cast<int>(arr.size()) - 1, metrics, record_steps);
    auto end_time = HighResClock::now();

    metrics.duration_ms = HighResClock::duration_ms(start_time, end_time);
    return metrics;
}

// --- Instrumented Heap Sort ---
namespace {
void sift_down_instr(std::vector<int>& arr, int n, int i,
                     SortMetrics& metrics, bool record_steps) {
    while (true) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n) {
            metrics.comparisons++;
            if (record_steps) metrics.steps.push_back({StepType::COMPARE, left, largest, 0});
            if (arr[left] > arr[largest]) {
                largest = left;
            }
        }

        if (right < n) {
            metrics.comparisons++;
            if (record_steps) metrics.steps.push_back({StepType::COMPARE, right, largest, 0});
            if (arr[right] > arr[largest]) {
                largest = right;
            }
        }

        if (largest == i) break;

        std::swap(arr[i], arr[largest]);
        metrics.swaps++;
        if (record_steps) {
            metrics.steps.push_back({StepType::SWAP, i, largest, 0});
        }

        i = largest;
    }
}
} // namespace

SortMetrics heap_sort_instrumented(std::vector<int>& arr, bool record_steps) {
    SortMetrics metrics;
    int n = static_cast<int>(arr.size());
    if (n <= 1) return metrics;

    auto start_time = HighResClock::now();

    // Build max-heap
    for (int i = n / 2 - 1; i >= 0; --i) {
        sift_down_instr(arr, n, i, metrics, record_steps);
    }

    // Extract elements
    for (int i = n - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);
        metrics.swaps++;
        if (record_steps) {
            metrics.steps.push_back({StepType::SWAP, 0, i, 0});
            metrics.steps.push_back({StepType::MARK_SORTED, i, -1, 0});
        }
        sift_down_instr(arr, i, 0, metrics, record_steps);
    }
    if (record_steps) {
        metrics.steps.push_back({StepType::MARK_SORTED, 0, -1, 0});
    }

    auto end_time = HighResClock::now();
    metrics.duration_ms = HighResClock::duration_ms(start_time, end_time);
    return metrics;
}

// --- Instrumented Radix Sort ---
SortMetrics radix_sort_instrumented(std::vector<int>& arr, bool record_steps) {
    SortMetrics metrics;
    int n = static_cast<int>(arr.size());
    if (n <= 1) return metrics;

    auto start_time = HighResClock::now();

    // Radix sort is non-comparative: comparisons remains 0
    int min_val = arr[0];
    int max_val = arr[0];
    for (int i = 1; i < n; ++i) {
        if (arr[i] < min_val) min_val = arr[i];
        if (arr[i] > max_val) max_val = arr[i];
    }

    long long shift = 0;
    if (min_val < 0) {
        shift = -static_cast<long long>(min_val);
        for (int i = 0; i < n; ++i) {
            arr[i] = static_cast<int>(arr[i] + shift);
        }
        max_val = static_cast<int>(max_val + shift);
    }

    std::vector<int> output(n);
    for (long long exp = 1; max_val / exp > 0; exp *= 10) {
        int count[10] = {0};

        for (int i = 0; i < n; ++i) {
            int digit = (arr[i] / exp) % 10;
            count[digit]++;
        }

        for (int i = 1; i < 10; ++i) {
            count[i] += count[i - 1];
        }

        for (int i = n - 1; i >= 0; --i) {
            int digit = (arr[i] / exp) % 10;
            output[--count[digit]] = arr[i];
        }

        for (int i = 0; i < n; ++i) {
            arr[i] = output[i];
            metrics.swaps++; // Track element reassignments / movements
            if (record_steps) {
                metrics.steps.push_back({StepType::ASSIGN, i, -1, arr[i]});
            }
        }
    }

    if (shift > 0) {
        for (int i = 0; i < n; ++i) {
            arr[i] = static_cast<int>(arr[i] - shift);
            if (record_steps) {
                metrics.steps.push_back({StepType::ASSIGN, i, -1, arr[i]});
            }
        }
    }

    auto end_time = HighResClock::now();
    metrics.duration_ms = HighResClock::duration_ms(start_time, end_time);
    return metrics;
}
