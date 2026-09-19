#pragma once

#include <string>
#include <vector>

// Type of operations performed during sorting (used for animation playback)
enum class StepType {
    COMPARE,     // Comparing elements at index1 and index2
    SWAP,        // Swapping elements at index1 and index2
    ASSIGN,      // Assigning value to index1 (e.g. in merge sort / radix sort)
    MARK_SORTED  // Indicating index1 is in its final sorted position
};

// Represents a single visual step for SFML animation
struct AnimationStep {
    StepType type;
    int index1;
    int index2; // -1 if not applicable
    int value;  // Value written (if ASSIGN), or auxiliary payload
};

// Metrics collected during sorting
struct SortMetrics {
    unsigned long long comparisons = 0;
    unsigned long long swaps = 0;
    double duration_ms = 0.0;
    std::vector<AnimationStep> steps; // Populated only if step recording is enabled
};

// Data distributions for benchmarking
enum class InputType {
    RANDOM,
    NEARLY_SORTED,
    REVERSE_SORTED,
    MANY_DUPLICATES
};

// Helper to convert InputType enum to string
inline std::string input_type_to_string(InputType type) {
    switch (type) {
        case InputType::RANDOM:          return "Random";
        case InputType::NEARLY_SORTED:   return "Nearly Sorted";
        case InputType::REVERSE_SORTED:  return "Reverse Sorted";
        case InputType::MANY_DUPLICATES: return "Many Duplicates";
    }
    return "Unknown";
}

// Helper to convert string to InputType enum
inline InputType string_to_input_type(const std::string& str) {
    if (str == "Random") return InputType::RANDOM;
    if (str == "Nearly Sorted") return InputType::NEARLY_SORTED;
    if (str == "Reverse Sorted") return InputType::REVERSE_SORTED;
    if (str == "Many Duplicates") return InputType::MANY_DUPLICATES;
    return InputType::RANDOM;
}

// High-precision clock utility:
// On Windows MinGW, std::chrono::high_resolution_clock is limited to 15.6ms resolution (GCC Bug 63683).
// Using QueryPerformanceCounter guarantees true hardware sub-microsecond timer resolution.
#ifdef _WIN32
#include <windows.h>
struct HighResClock {
    using time_point = long long;
    static inline time_point now() {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return t.QuadPart;
    }
    static inline double duration_ms(time_point start, time_point end) {
        static LARGE_INTEGER freq;
        static bool init = false;
        if (!init) {
            QueryPerformanceFrequency(&freq);
            init = true;
        }
        return static_cast<double>(end - start) * 1000.0 / freq.QuadPart;
    }
};
#else
#include <chrono>
struct HighResClock {
    using time_point = std::chrono::high_resolution_clock::time_point;
    static inline time_point now() { return std::chrono::high_resolution_clock::now(); }
    static inline double duration_ms(time_point start, time_point end) {
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
};
#endif

