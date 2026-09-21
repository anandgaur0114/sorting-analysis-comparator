#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "types.hpp"
#include "benchmark.hpp"

// View mode in the Visualizer
enum class VisualizerMode {
    SINGLE_SORT, // Bar-chart animation of a single algorithm
    RACE_MODE,   // Side-by-side comparison race of all 4 algorithms on identical array
    RESULTS_PLOT // Graphical plot of benchmark timing curves (Time vs Input Size)
};

// Clickable UI Button
struct UIButton {
    std::string id;
    std::string label;
    sf::FloatRect bounds;
    bool is_active = false;
    bool is_hovered = false;
};

// State of an active sorting animation instance
struct SortRunnerState {
    std::string name;
    std::vector<int> initial_array;
    std::vector<int> current_array;
    std::vector<AnimationStep> steps;
    size_t step_index = 0;
    unsigned long long comparisons = 0;
    unsigned long long swaps = 0;
    std::vector<bool> is_sorted;
    int active_comp1 = -1;
    int active_comp2 = -1;
    int active_swap1 = -1;
    int active_swap2 = -1;
    int active_assign = -1;
    bool finished = false;
    int finish_rank = 0; // 1 for 1st, 2 for 2nd, etc.
};

class SortingVisualizer {
public:
    SortingVisualizer();

    // Launch the visualizer window and run the main interactive loop
    void run();

    void set_mode(VisualizerMode mode);
    void set_array_size(int n);
    void set_input_type(InputType type);

private:
    sf::RenderWindow window_;
    sf::Font font_;
    bool font_loaded_ = false;

    VisualizerMode current_mode_ = VisualizerMode::SINGLE_SORT;
    int array_size_ = 64;
    int speed_level_ = 3; // 1 (slow) to 7 (ultra fast)
    InputType input_type_ = InputType::RANDOM;
    std::string current_algo_ = "Merge Sort";
    bool is_paused_ = false;

    // Timing accumulator for sub-frame pacing
    float step_timer_ = 0.0f;

    // Custom Array Input Modal
    bool input_modal_open_ = false;
    std::string input_buffer_ = "64, 34, 25, 12, 22, 11, 90";
    std::vector<int> custom_array_;
    bool has_custom_array_ = false;
    float cursor_timer_ = 0.0f;
    std::vector<UIButton> modal_buttons_;

    // Animation runners
    SortRunnerState single_runner_;
    std::vector<SortRunnerState> race_runners_; // Merge, Quick, Heap, Radix
    int finished_count_ = 0;

    // Benchmark plot data
    std::vector<BenchmarkResult> plot_results_;
    InputType plot_input_type_ = InputType::RANDOM;
    bool plot_log_scale_ = true;
    sf::Vector2f mouse_pos_{0.0f, 0.0f};

    // UI Buttons
    std::vector<UIButton> buttons_;
    void setup_buttons();
    void setup_modal_buttons();
    void update_button_states();
    void render_button(const UIButton& btn, sf::Color custom_accent = sf::Color(0, 145, 215));
    void handle_mouse_click(float x, float y);
    void handle_mouse_move(float x, float y);

    // Initialization helpers
    void init_single_sort();
    void init_race_mode();
    void load_plot_data();
    void confirm_custom_array();
    static std::vector<int> parse_array_string(const std::string& text);

    // Loop steps
    void handle_events();
    void update(float dt);
    void render();

    // Execution & rendering helpers
    void step_runner(SortRunnerState& runner, int steps_to_advance);
    void render_bars(const SortRunnerState& runner, sf::FloatRect bounds, bool is_race = false);
    void render_single_mode();
    void render_race_mode();
    void render_results_plot();
    void render_toolbar();
    void render_bottom_hud();
    void render_input_modal();
};
