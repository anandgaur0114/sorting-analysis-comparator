#include "visualizer.hpp"
#include "sorts.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

SortingVisualizer::SortingVisualizer() {
    window_.create(sf::VideoMode(1280, 720), "Sorting Analysis Comparator (SAC) - Interactive Visualizer", sf::Style::Close | sf::Style::Titlebar);
    window_.setFramerateLimit(60);

    if (!font_.loadFromFile("arial.ttf")) {
        if (!font_.loadFromFile("C:/Users/HP/Desktop/CGM PROJECT/SFML-2.5.1/arial.ttf")) {
            font_.loadFromFile("C:/Windows/Fonts/arial.ttf");
        }
    }
    font_loaded_ = true;

    load_plot_data();

    setup_buttons();
    setup_modal_buttons();

    init_single_sort();
    init_race_mode();
}

std::vector<int> SortingVisualizer::parse_array_string(const std::string& text) {
    std::vector<int> result;
    std::string clean = text;
    for (char& c : clean) {
        if (c == ',' || c == ';' || c == '[' || c == ']') c = ' ';
    }
    std::stringstream ss(clean);
    long long val;
    while (ss >> val) {
        if (val < 1) val = 1;
        if (val > 999999) val = 999999;
        result.push_back(static_cast<int>(val));
    }
    return result;
}

void SortingVisualizer::setup_buttons() {
    buttons_.clear();

    // Row 1: View Navigation Modes (y = 12, h = 32)
    buttons_.push_back({"mode_single", "Single Sort", sf::FloatRect(390.0f, 12.0f, 105.0f, 32.0f), false, false});
    buttons_.push_back({"mode_race", "Race Mode", sf::FloatRect(502.0f, 12.0f, 105.0f, 32.0f), false, false});
    buttons_.push_back({"mode_plot", "Benchmark Plot", sf::FloatRect(614.0f, 12.0f, 130.0f, 32.0f), false, false});

    // Row 1: Primary Action Controls (Custom Insert Array, Randomize, Pause)
    buttons_.push_back({"action_custom", "✍ Insert Array", sf::FloatRect(758.0f, 12.0f, 135.0f, 32.0f), false, false});
    buttons_.push_back({"action_random", "🎲 Randomize", sf::FloatRect(900.0f, 12.0f, 115.0f, 32.0f), false, false});
    buttons_.push_back({"action_pause", "❚❚ Pause", sf::FloatRect(1022.0f, 12.0f, 95.0f, 32.0f), false, false});

    // Row 2: Secondary Toolbar (y = 52, h = 28)
    // Algorithm selection
    buttons_.push_back({"algo_merge", "Merge", sf::FloatRect(100.0f, 52.0f, 65.0f, 28.0f), false, false});
    buttons_.push_back({"algo_quick", "Quick", sf::FloatRect(170.0f, 52.0f, 65.0f, 28.0f), false, false});
    buttons_.push_back({"algo_heap", "Heap", sf::FloatRect(240.0f, 52.0f, 65.0f, 28.0f), false, false});
    buttons_.push_back({"algo_radix", "Radix", sf::FloatRect(310.0f, 52.0f, 65.0f, 28.0f), false, false});

    // Data distribution presets
    buttons_.push_back({"dist_rand", "Random", sf::FloatRect(435.0f, 52.0f, 75.0f, 28.0f), false, false});
    buttons_.push_back({"dist_near", "Nearly Sorted", sf::FloatRect(515.0f, 52.0f, 105.0f, 28.0f), false, false});
    buttons_.push_back({"dist_rev", "Reverse", sf::FloatRect(625.0f, 52.0f, 75.0f, 28.0f), false, false});
    buttons_.push_back({"dist_dup", "Duplicates", sf::FloatRect(705.0f, 52.0f, 85.0f, 28.0f), false, false});

    // Scale N buttons
    buttons_.push_back({"size_dec", "-", sf::FloatRect(865.0f, 52.0f, 28.0f, 28.0f), false, false});
    buttons_.push_back({"size_inc", "+", sf::FloatRect(955.0f, 52.0f, 28.0f, 28.0f), false, false});

    // Speed buttons
    buttons_.push_back({"speed_dec", "-", sf::FloatRect(1075.0f, 52.0f, 28.0f, 28.0f), false, false});
    buttons_.push_back({"speed_inc", "+", sf::FloatRect(1185.0f, 52.0f, 28.0f, 28.0f), false, false});
}

void SortingVisualizer::setup_modal_buttons() {
    modal_buttons_.clear();

    // Presets
    modal_buttons_.push_back({"modal_ex1", "Ex 1: 64, 34, 25, 12, 22, 11, 90", sf::FloatRect(370.0f, 340.0f, 260.0f, 30.0f), false, false});
    modal_buttons_.push_back({"modal_ex2", "Ex 2: 9, 8, 7, 6, 5, 4, 3, 2, 1", sf::FloatRect(645.0f, 340.0f, 240.0f, 30.0f), false, false});
    modal_buttons_.push_back({"modal_ex3", "Ex 3: 5, 2, 8, 5, 2, 8, 5, 2", sf::FloatRect(370.0f, 380.0f, 220.0f, 30.0f), false, false});
    modal_buttons_.push_back({"modal_ex4", "Ex 4: 10, 50, 20, 40, 30", sf::FloatRect(605.0f, 380.0f, 200.0f, 30.0f), false, false});

    // Action buttons
    modal_buttons_.push_back({"modal_sort", "✔ Trace & Sort Array", sf::FloatRect(370.0f, 455.0f, 200.0f, 42.0f), false, false});
    modal_buttons_.push_back({"modal_random", "🎲 Random 16", sf::FloatRect(585.0f, 455.0f, 140.0f, 42.0f), false, false});
    modal_buttons_.push_back({"modal_cancel", "✖ Cancel", sf::FloatRect(740.0f, 455.0f, 125.0f, 42.0f), false, false});
}

void SortingVisualizer::update_button_states() {
    for (auto& btn : buttons_) {
        if (btn.id == "mode_single") btn.is_active = (current_mode_ == VisualizerMode::SINGLE_SORT);
        else if (btn.id == "mode_race") btn.is_active = (current_mode_ == VisualizerMode::RACE_MODE);
        else if (btn.id == "mode_plot") btn.is_active = (current_mode_ == VisualizerMode::RESULTS_PLOT);
        else if (btn.id == "action_custom") btn.is_active = input_modal_open_;
        else if (btn.id == "action_pause") {
            btn.label = (is_paused_ ? "▶ Resume" : "❚❚ Pause");
            btn.is_active = is_paused_;
        }
        else if (btn.id == "algo_merge") btn.is_active = (current_algo_ == "Merge Sort");
        else if (btn.id == "algo_quick") btn.is_active = (current_algo_ == "Quick Sort");
        else if (btn.id == "algo_heap") btn.is_active = (current_algo_ == "Heap Sort");
        else if (btn.id == "algo_radix") btn.is_active = (current_algo_ == "Radix Sort");
        else if (btn.id == "dist_rand") btn.is_active = (!has_custom_array_ && input_type_ == InputType::RANDOM);
        else if (btn.id == "dist_near") btn.is_active = (!has_custom_array_ && input_type_ == InputType::NEARLY_SORTED);
        else if (btn.id == "dist_rev") btn.is_active = (!has_custom_array_ && input_type_ == InputType::REVERSE_SORTED);
        else if (btn.id == "dist_dup") btn.is_active = (!has_custom_array_ && input_type_ == InputType::MANY_DUPLICATES);
    }
}

void SortingVisualizer::set_mode(VisualizerMode mode) {
    current_mode_ = mode;
}

void SortingVisualizer::set_array_size(int n) {
    has_custom_array_ = false;
    array_size_ = std::max(8, std::min(n, 512));
    init_single_sort();
    init_race_mode();
}

void SortingVisualizer::set_input_type(InputType type) {
    has_custom_array_ = false;
    input_type_ = type;
    plot_input_type_ = type;
    init_single_sort();
    init_race_mode();
}

void SortingVisualizer::load_plot_data() {
    plot_results_ = BenchmarkRunner::load_from_csv("results.csv");
    if (plot_results_.empty()) {
        BenchmarkRunner runner;
        plot_results_ = runner.run_all(false);
    }
}

void SortingVisualizer::confirm_custom_array() {
    auto parsed = parse_array_string(input_buffer_);
    if (parsed.size() < 2) {
        parsed = {64, 34, 25, 12, 22, 11, 90};
    }
    custom_array_ = parsed;
    has_custom_array_ = true;
    array_size_ = static_cast<int>(custom_array_.size());
    input_modal_open_ = false;
    is_paused_ = false;

    init_single_sort();
    init_race_mode();
}

void SortingVisualizer::init_single_sort() {
    single_runner_.name = current_algo_;
    std::vector<int> base_data;

    if (has_custom_array_ && !custom_array_.empty()) {
        base_data = custom_array_;
    } else {
        base_data = generate_data(input_type_, array_size_, 1000 + std::rand() % 9000);
    }

    single_runner_.initial_array = base_data;
    single_runner_.current_array = base_data;
    single_runner_.step_index = 0;
    single_runner_.comparisons = 0;
    single_runner_.swaps = 0;
    single_runner_.finished = false;
    single_runner_.finish_rank = 0;
    single_runner_.is_sorted.assign(base_data.size(), false);
    single_runner_.active_comp1 = single_runner_.active_comp2 = -1;
    single_runner_.active_swap1 = single_runner_.active_swap2 = -1;
    single_runner_.active_assign = -1;

    auto to_sort = base_data;
    SortMetrics m;
    if (current_algo_ == "Merge Sort") {
        m = merge_sort_instrumented(to_sort, true);
    } else if (current_algo_ == "Quick Sort") {
        m = quick_sort_instrumented(to_sort, true);
    } else if (current_algo_ == "Heap Sort") {
        m = heap_sort_instrumented(to_sort, true);
    } else if (current_algo_ == "Radix Sort") {
        m = radix_sort_instrumented(to_sort, true);
    }
    single_runner_.steps = std::move(m.steps);
}

void SortingVisualizer::init_race_mode() {
    race_runners_.clear();
    race_runners_.resize(4);
    finished_count_ = 0;

    std::vector<std::string> names = {"Merge Sort", "Quick Sort", "Heap Sort", "Radix Sort"};
    std::vector<int> base_data;

    if (has_custom_array_ && !custom_array_.empty()) {
        base_data = custom_array_;
    } else {
        base_data = generate_data(input_type_, array_size_, 2000 + std::rand() % 9000);
    }

    for (size_t i = 0; i < 4; ++i) {
        race_runners_[i].name = names[i];
        race_runners_[i].initial_array = base_data;
        race_runners_[i].current_array = base_data;
        race_runners_[i].step_index = 0;
        race_runners_[i].comparisons = 0;
        race_runners_[i].swaps = 0;
        race_runners_[i].finished = false;
        race_runners_[i].finish_rank = 0;
        race_runners_[i].is_sorted.assign(base_data.size(), false);
        race_runners_[i].active_comp1 = race_runners_[i].active_comp2 = -1;
        race_runners_[i].active_swap1 = race_runners_[i].active_swap2 = -1;
        race_runners_[i].active_assign = -1;

        auto to_sort = base_data;
        SortMetrics m;
        if (i == 0) m = merge_sort_instrumented(to_sort, true);
        else if (i == 1) m = quick_sort_instrumented(to_sort, true);
        else if (i == 2) m = heap_sort_instrumented(to_sort, true);
        else if (i == 3) m = radix_sort_instrumented(to_sort, true);

        race_runners_[i].steps = std::move(m.steps);
    }
}

void SortingVisualizer::step_runner(SortRunnerState& runner, int steps_to_advance) {
    if (runner.finished) return;

    for (int s = 0; s < steps_to_advance; ++s) {
        if (runner.step_index >= runner.steps.size()) {
            runner.finished = true;
            if (runner.finish_rank == 0) {
                runner.finish_rank = ++finished_count_;
            }
            std::fill(runner.is_sorted.begin(), runner.is_sorted.end(), true);
            runner.active_comp1 = runner.active_comp2 = -1;
            runner.active_swap1 = runner.active_swap2 = -1;
            runner.active_assign = -1;
            break;
        }

        const auto& step = runner.steps[runner.step_index++];
        runner.active_comp1 = runner.active_comp2 = -1;
        runner.active_swap1 = runner.active_swap2 = -1;
        runner.active_assign = -1;

        switch (step.type) {
            case StepType::COMPARE:
                runner.active_comp1 = step.index1;
                runner.active_comp2 = step.index2;
                runner.comparisons++;
                break;
            case StepType::SWAP:
                runner.active_swap1 = step.index1;
                runner.active_swap2 = step.index2;
                if (step.index1 >= 0 && step.index1 < static_cast<int>(runner.current_array.size()) &&
                    step.index2 >= 0 && step.index2 < static_cast<int>(runner.current_array.size())) {
                    std::swap(runner.current_array[step.index1], runner.current_array[step.index2]);
                }
                runner.swaps++;
                break;
            case StepType::ASSIGN:
                runner.active_assign = step.index1;
                if (step.index1 >= 0 && step.index1 < static_cast<int>(runner.current_array.size())) {
                    runner.current_array[step.index1] = step.value;
                }
                runner.swaps++;
                break;
            case StepType::MARK_SORTED:
                if (step.index1 >= 0 && step.index1 < static_cast<int>(runner.is_sorted.size())) {
                    runner.is_sorted[step.index1] = true;
                }
                break;
        }
    }
}

void SortingVisualizer::handle_mouse_move(float x, float y) {
    if (input_modal_open_) {
        for (auto& btn : modal_buttons_) {
            btn.is_hovered = btn.bounds.contains(x, y);
        }
        return;
    }

    for (auto& btn : buttons_) {
        btn.is_hovered = btn.bounds.contains(x, y);
    }
}

void SortingVisualizer::handle_mouse_click(float x, float y) {
    if (input_modal_open_) {
        for (const auto& btn : modal_buttons_) {
            if (btn.bounds.contains(x, y)) {
                if (btn.id == "modal_sort") {
                    confirm_custom_array();
                } else if (btn.id == "modal_cancel") {
                    input_modal_open_ = false;
                } else if (btn.id == "modal_random") {
                    input_buffer_ = "45, 12, 89, 3, 21, 67, 4, 98, 54, 32, 76, 19, 83, 61, 28, 7";
                } else if (btn.id == "modal_ex1") {
                    input_buffer_ = "64, 34, 25, 12, 22, 11, 90";
                } else if (btn.id == "modal_ex2") {
                    input_buffer_ = "9, 8, 7, 6, 5, 4, 3, 2, 1";
                } else if (btn.id == "modal_ex3") {
                    input_buffer_ = "5, 2, 8, 5, 2, 8, 5, 2";
                } else if (btn.id == "modal_ex4") {
                    input_buffer_ = "10, 50, 20, 40, 30";
                }
                break;
            }
        }
        return;
    }

    for (const auto& btn : buttons_) {
        if (btn.bounds.contains(x, y)) {
            // View Navigation
            if (btn.id == "mode_single") current_mode_ = VisualizerMode::SINGLE_SORT;
            else if (btn.id == "mode_race") current_mode_ = VisualizerMode::RACE_MODE;
            else if (btn.id == "mode_plot") current_mode_ = VisualizerMode::RESULTS_PLOT;

            // Custom Array Insertion
            else if (btn.id == "action_custom") {
                input_modal_open_ = true;
                is_paused_ = true;
            }

            // Quick Randomize
            else if (btn.id == "action_random") {
                has_custom_array_ = false;
                if (current_mode_ == VisualizerMode::SINGLE_SORT) init_single_sort();
                else if (current_mode_ == VisualizerMode::RACE_MODE) init_race_mode();
            }

            // Execution Controls
            else if (btn.id == "action_pause") is_paused_ = !is_paused_;

            // Algorithm Selection
            else if (btn.id == "algo_merge") {
                current_algo_ = "Merge Sort";
                if (current_mode_ == VisualizerMode::SINGLE_SORT) init_single_sort();
            }
            else if (btn.id == "algo_quick") {
                current_algo_ = "Quick Sort";
                if (current_mode_ == VisualizerMode::SINGLE_SORT) init_single_sort();
            }
            else if (btn.id == "algo_heap") {
                current_algo_ = "Heap Sort";
                if (current_mode_ == VisualizerMode::SINGLE_SORT) init_single_sort();
            }
            else if (btn.id == "algo_radix") {
                current_algo_ = "Radix Sort";
                if (current_mode_ == VisualizerMode::SINGLE_SORT) init_single_sort();
            }

            // Data Distribution
            else if (btn.id == "dist_rand") set_input_type(InputType::RANDOM);
            else if (btn.id == "dist_near") set_input_type(InputType::NEARLY_SORTED);
            else if (btn.id == "dist_rev") set_input_type(InputType::REVERSE_SORTED);
            else if (btn.id == "dist_dup") set_input_type(InputType::MANY_DUPLICATES);

            // Scale N
            else if (btn.id == "size_dec") {
                if (array_size_ > 8) set_array_size(array_size_ / 2);
            }
            else if (btn.id == "size_inc") {
                if (array_size_ < 512) set_array_size(array_size_ * 2);
            }

            // Speed
            else if (btn.id == "speed_dec") speed_level_ = std::max(1, speed_level_ - 1);
            else if (btn.id == "speed_inc") speed_level_ = std::min(7, speed_level_ + 1);

            break;
        }
    }
}

void SortingVisualizer::handle_events() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
        } else if (event.type == sf::Event::MouseMoved) {
            handle_mouse_move(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                handle_mouse_click(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
            }
        } else if (event.type == sf::Event::TextEntered) {
            if (input_modal_open_) {
                if (event.text.unicode == 13) { // Enter key
                    confirm_custom_array();
                } else if (event.text.unicode == 8) { // Backspace
                    if (!input_buffer_.empty()) input_buffer_.pop_back();
                } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                    if (input_buffer_.size() < 120) {
                        input_buffer_.push_back(static_cast<char>(event.text.unicode));
                    }
                }
            }
        } else if (event.type == sf::Event::KeyPressed) {
            if (input_modal_open_) {
                if (event.key.code == sf::Keyboard::Escape) {
                    input_modal_open_ = false;
                }
                continue;
            }

            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    window_.close();
                    break;
                case sf::Keyboard::Tab:
                    if (current_mode_ == VisualizerMode::SINGLE_SORT) current_mode_ = VisualizerMode::RACE_MODE;
                    else if (current_mode_ == VisualizerMode::RACE_MODE) current_mode_ = VisualizerMode::RESULTS_PLOT;
                    else current_mode_ = VisualizerMode::SINGLE_SORT;
                    break;
                case sf::Keyboard::Space:
                    is_paused_ = !is_paused_;
                    break;
                case sf::Keyboard::Return:
                case sf::Keyboard::BackSpace:
                    input_modal_open_ = true;
                    is_paused_ = true;
                    break;
                case sf::Keyboard::Num1:
                case sf::Keyboard::M:
                    current_algo_ = "Merge Sort";
                    if (current_mode_ == VisualizerMode::SINGLE_SORT) init_single_sort();
                    break;
                case sf::Keyboard::Num2:
                case sf::Keyboard::Q:
                    current_algo_ = "Quick Sort";
                    if (current_mode_ == VisualizerMode::SINGLE_SORT) init_single_sort();
                    break;
                case sf::Keyboard::Num3:
                case sf::Keyboard::H:
                    current_algo_ = "Heap Sort";
                    if (current_mode_ == VisualizerMode::SINGLE_SORT) init_single_sort();
                    break;
                case sf::Keyboard::Num4:
                case sf::Keyboard::X:
                case sf::Keyboard::R:
                    current_algo_ = "Radix Sort";
                    if (current_mode_ == VisualizerMode::SINGLE_SORT) init_single_sort();
                    break;
                case sf::Keyboard::T: {
                    int next_type = (static_cast<int>(input_type_) + 1) % 4;
                    set_input_type(static_cast<InputType>(next_type));
                    break;
                }
                case sf::Keyboard::Up:
                    speed_level_ = std::min(7, speed_level_ + 1);
                    break;
                case sf::Keyboard::Down:
                    speed_level_ = std::max(1, speed_level_ - 1);
                    break;
                case sf::Keyboard::Right:
                    if (array_size_ < 512) set_array_size(array_size_ * 2);
                    break;
                case sf::Keyboard::Left:
                    if (array_size_ > 8) set_array_size(array_size_ / 2);
                    break;
                default:
                    break;
            }
        }
    }
}

void SortingVisualizer::update(float dt) {
    cursor_timer_ += dt;
    if (cursor_timer_ >= 1.0f) cursor_timer_ = 0.0f;

    update_button_states();

    if (input_modal_open_ || is_paused_) return;

    int steps_to_advance = 0;
    if (speed_level_ <= 3) {
        step_timer_ += dt * 1000.0f;
        float delay = (speed_level_ == 1 ? 50.0f : (speed_level_ == 2 ? 20.0f : 8.0f));
        if (step_timer_ >= delay) {
            steps_to_advance = static_cast<int>(step_timer_ / delay);
            step_timer_ -= steps_to_advance * delay;
        }
    } else {
        steps_to_advance = (speed_level_ == 4 ? 1 : (speed_level_ == 5 ? 4 : (speed_level_ == 6 ? 15 : 50)));
    }

    if (steps_to_advance > 0) {
        if (current_mode_ == VisualizerMode::SINGLE_SORT) {
            step_runner(single_runner_, steps_to_advance);
        } else if (current_mode_ == VisualizerMode::RACE_MODE) {
            for (auto& runner : race_runners_) {
                step_runner(runner, steps_to_advance);
            }
        }
    }
}

void SortingVisualizer::render_button(const UIButton& btn, sf::Color custom_accent) {
    sf::RectangleShape box(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    box.setPosition(btn.bounds.left, btn.bounds.top);

    sf::Color bg_color(26, 32, 44);
    sf::Color border_color(55, 68, 88);
    sf::Color text_color(190, 205, 225);

    if (btn.is_active) {
        bg_color = custom_accent;
        border_color = sf::Color(80, 200, 255);
        text_color = sf::Color::White;
    } else if (btn.is_hovered) {
        bg_color = sf::Color(42, 54, 74);
        border_color = sf::Color(100, 125, 160);
        text_color = sf::Color::White;
    }

    box.setFillColor(bg_color);
    box.setOutlineThickness(1.0f);
    box.setOutlineColor(border_color);
    window_.draw(box);

    if (font_loaded_) {
        sf::Text txt;
        txt.setFont(font_);
        txt.setString(btn.label);
        txt.setCharacterSize(12);
        txt.setStyle(btn.is_active ? sf::Text::Bold : sf::Text::Regular);
        txt.setFillColor(text_color);

        sf::FloatRect text_bounds = txt.getLocalBounds();
        float tx = btn.bounds.left + (btn.bounds.width - text_bounds.width) / 2.0f - text_bounds.left;
        float ty = btn.bounds.top + (btn.bounds.height - text_bounds.height) / 2.0f - text_bounds.top;
        txt.setPosition(tx, ty);
        window_.draw(txt);
    }
}

void SortingVisualizer::render_bars(const SortRunnerState& runner, sf::FloatRect bounds, bool is_race) {
    size_t n = runner.current_array.size();
    if (n == 0) return;

    sf::RectangleShape bg(sf::Vector2f(bounds.width, bounds.height));
    bg.setPosition(bounds.left, bounds.top);
    bg.setFillColor(sf::Color(16, 20, 28));
    bg.setOutlineThickness(1.0f);
    bg.setOutlineColor(runner.finished ? sf::Color(46, 204, 113, 180) : sf::Color(50, 62, 80));
    window_.draw(bg);

    int max_val = 1;
    for (int v : runner.current_array) {
        if (v > max_val) max_val = v;
    }

    float bar_spacing = (n > 128 ? 0.0f : (n <= 32 ? 3.0f : 1.0f));
    float total_spacing = bar_spacing * (n - 1);
    float bar_width = (bounds.width - 24.0f - total_spacing) / n;
    float start_x = bounds.left + 12.0f;
    float base_y = bounds.top + bounds.height - 12.0f;
    float max_bar_h = bounds.height - (is_race ? 52.0f : 75.0f);

    for (size_t i = 0; i < n; ++i) {
        float h = std::max(4.0f, (static_cast<float>(runner.current_array[i]) / max_val) * max_bar_h);
        float x = start_x + i * (bar_width + bar_spacing);
        float y = base_y - h;

        sf::RectangleShape bar(sf::Vector2f(std::max(1.0f, bar_width), h));
        bar.setPosition(x, y);

        // Color coding
        if (runner.finished || (i < runner.is_sorted.size() && runner.is_sorted[i])) {
            bar.setFillColor(sf::Color(46, 204, 113)); // Emerald Green
        } else if (static_cast<int>(i) == runner.active_swap1 || static_cast<int>(i) == runner.active_swap2) {
            bar.setFillColor(sf::Color(231, 76, 60)); // Crimson Red
        } else if (static_cast<int>(i) == runner.active_assign) {
            bar.setFillColor(sf::Color(155, 89, 182)); // Purple
        } else if (static_cast<int>(i) == runner.active_comp1 || static_cast<int>(i) == runner.active_comp2) {
            bar.setFillColor(sf::Color(241, 196, 15)); // Vivid Yellow
        } else {
            bar.setFillColor(sf::Color(52, 152, 219)); // Cool Blue
        }

        window_.draw(bar);

        // Display exact numbers on bars for custom/small arrays!
        if (n <= 25 && font_loaded_) {
            sf::Text num_txt;
            num_txt.setFont(font_);
            num_txt.setCharacterSize(n <= 12 ? 14 : 11);
            num_txt.setStyle(sf::Text::Bold);
            num_txt.setFillColor(sf::Color::White);
            num_txt.setString(std::to_string(runner.current_array[i]));
            sf::FloatRect nb = num_txt.getLocalBounds();
            float nx = x + (bar_width - nb.width) / 2.0f - nb.left;
            float ny = y - (is_race ? 18.0f : 22.0f);
            if (ny < bounds.top + (is_race ? 42.0f : 55.0f)) {
                ny = y + 4.0f; // Inside bar if bar is very tall
                num_txt.setFillColor(sf::Color::Black);
            }
            num_txt.setPosition(nx, ny);
            window_.draw(num_txt);
        }
    }

    // Card Header Overlay
    if (font_loaded_) {
        sf::Text title;
        title.setFont(font_);
        title.setCharacterSize(is_race ? 16 : 22);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);
        title.setString(runner.name);
        title.setPosition(bounds.left + 14.0f, bounds.top + 8.0f);
        window_.draw(title);

        sf::Text stats;
        stats.setFont(font_);
        stats.setCharacterSize(is_race ? 12 : 15);
        stats.setFillColor(sf::Color(180, 195, 215));
        std::stringstream ss;
        ss << "Comps: " << runner.comparisons << "  |  Swaps: " << runner.swaps;
        stats.setString(ss.str());
        stats.setPosition(bounds.left + 14.0f, bounds.top + (is_race ? 28.0f : 34.0f));
        window_.draw(stats);

        // Rank Badge in Race Mode
        if (is_race && runner.finished && runner.finish_rank > 0) {
            sf::RectangleShape badge(sf::Vector2f(95.0f, 24.0f));
            badge.setPosition(bounds.left + bounds.width - 110.0f, bounds.top + 8.0f);
            sf::Color badge_col = sf::Color(100, 100, 100);
            std::string rank_str = "FINISHED";
            if (runner.finish_rank == 1) { badge_col = sf::Color(241, 196, 15); rank_str = "1st PLACE"; }
            else if (runner.finish_rank == 2) { badge_col = sf::Color(189, 195, 199); rank_str = "2nd PLACE"; }
            else if (runner.finish_rank == 3) { badge_col = sf::Color(211, 84, 0); rank_str = "3rd PLACE"; }
            else if (runner.finish_rank == 4) { badge_col = sf::Color(127, 140, 141); rank_str = "4th PLACE"; }

            badge.setFillColor(badge_col);
            window_.draw(badge);

            sf::Text rank_txt;
            rank_txt.setFont(font_);
            rank_txt.setCharacterSize(11);
            rank_txt.setStyle(sf::Text::Bold);
            rank_txt.setFillColor(sf::Color::Black);
            rank_txt.setString(rank_str);
            rank_txt.setPosition(badge.getPosition().x + 10.0f, badge.getPosition().y + 5.0f);
            window_.draw(rank_txt);
        }
    }
}

void SortingVisualizer::render_single_mode() {
    sf::FloatRect bounds(25.0f, 95.0f, 1230.0f, 570.0f);
    render_bars(single_runner_, bounds, false);
}

void SortingVisualizer::render_race_mode() {
    float top_y = 95.0f;
    float row_h = 280.0f;
    float col_w = 605.0f;

    sf::FloatRect bounds_merge(25.0f, top_y, col_w, row_h);
    sf::FloatRect bounds_quick(650.0f, top_y, col_w, row_h);
    sf::FloatRect bounds_heap(25.0f, top_y + row_h + 10.0f, col_w, row_h);
    sf::FloatRect bounds_radix(650.0f, top_y + row_h + 10.0f, col_w, row_h);

    if (race_runners_.size() >= 4) {
        render_bars(race_runners_[0], bounds_merge, true);
        render_bars(race_runners_[1], bounds_quick, true);
        render_bars(race_runners_[2], bounds_heap, true);
        render_bars(race_runners_[3], bounds_radix, true);
    }
}

void SortingVisualizer::render_results_plot() {
    sf::FloatRect plot_area(60.0f, 95.0f, 1160.0f, 570.0f);

    sf::RectangleShape bg(sf::Vector2f(plot_area.width, plot_area.height));
    bg.setPosition(plot_area.left, plot_area.top);
    bg.setFillColor(sf::Color(16, 20, 28));
    bg.setOutlineThickness(1.0f);
    bg.setOutlineColor(sf::Color(50, 62, 80));
    window_.draw(bg);

    std::string type_str = input_type_to_string(plot_input_type_);

    if (font_loaded_) {
        sf::Text plot_title;
        plot_title.setFont(font_);
        plot_title.setCharacterSize(18);
        plot_title.setFillColor(sf::Color::White);
        plot_title.setStyle(sf::Text::Bold);
        plot_title.setString("Execution Time vs. Input Scale (N)  -  Dataset: " + type_str);
        plot_title.setPosition(plot_area.left + 25.0f, plot_area.top + 15.0f);
        window_.draw(plot_title);
    }

    std::vector<int> sizes = {100, 500, 1000, 5000, 10000, 50000};
    std::vector<std::string> algos = {"Merge Sort", "Quick Sort", "Heap Sort", "Radix Sort"};
    std::vector<sf::Color> algo_colors = {
        sf::Color(0, 220, 255),   // Cyan
        sf::Color(255, 220, 0),   // Yellow
        sf::Color(255, 140, 0),   // Orange
        sf::Color(46, 204, 113)   // Green
    };

    double max_time = 0.1;
    for (const auto& r : plot_results_) {
        if (r.input_type == type_str && r.avg_time_ms > max_time) {
            max_time = r.avg_time_ms;
        }
    }
    max_time *= 1.15;

    float origin_x = plot_area.left + 75.0f;
    float origin_y = plot_area.top + plot_area.height - 50.0f;
    float graph_w = plot_area.width - 110.0f;
    float graph_h = plot_area.height - 100.0f;

    // Grid lines & X tick labels
    for (size_t i = 0; i < sizes.size(); ++i) {
        float x = origin_x + (static_cast<float>(i) / (sizes.size() - 1)) * (graph_w - 50.0f);

        sf::Vertex grid_line[] = {
            sf::Vertex(sf::Vector2f(x, origin_y), sf::Color(35, 45, 60)),
            sf::Vertex(sf::Vector2f(x, origin_y - graph_h), sf::Color(35, 45, 60))
        };
        window_.draw(grid_line, 2, sf::Lines);

        if (font_loaded_) {
            sf::Text tick_txt;
            tick_txt.setFont(font_);
            tick_txt.setCharacterSize(13);
            tick_txt.setFillColor(sf::Color(160, 175, 195));
            tick_txt.setString(std::to_string(sizes[i]));
            tick_txt.setPosition(x - 15.0f, origin_y + 10.0f);
            window_.draw(tick_txt);
        }
    }

    // Y ticks
    int num_y_ticks = 5;
    for (int i = 0; i <= num_y_ticks; ++i) {
        float y = origin_y - (static_cast<float>(i) / num_y_ticks) * graph_h;
        double val = (static_cast<double>(i) / num_y_ticks) * max_time;

        sf::Vertex grid_line[] = {
            sf::Vertex(sf::Vector2f(origin_x, y), sf::Color(35, 45, 60)),
            sf::Vertex(sf::Vector2f(origin_x + graph_w - 50.0f, y), sf::Color(35, 45, 60))
        };
        window_.draw(grid_line, 2, sf::Lines);

        if (font_loaded_) {
            sf::Text y_txt;
            y_txt.setFont(font_);
            y_txt.setCharacterSize(12);
            y_txt.setFillColor(sf::Color(160, 175, 195));
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << val << " ms";
            y_txt.setString(ss.str());
            y_txt.setPosition(origin_x - 65.0f, y - 8.0f);
            window_.draw(y_txt);
        }
    }

    // Lines & Nodes
    for (size_t a = 0; a < algos.size(); ++a) {
        std::vector<sf::Vector2f> points;

        for (size_t i = 0; i < sizes.size(); ++i) {
            int n_target = sizes[i];
            double time_ms = 0.0;
            for (const auto& r : plot_results_) {
                if (r.algorithm == algos[a] && r.input_type == type_str && r.n == n_target) {
                    time_ms = r.avg_time_ms;
                    break;
                }
            }

            float x = origin_x + (static_cast<float>(i) / (sizes.size() - 1)) * (graph_w - 50.0f);
            float y = origin_y - static_cast<float>(time_ms / max_time) * graph_h;
            points.push_back(sf::Vector2f(x, y));
        }

        for (size_t i = 1; i < points.size(); ++i) {
            sf::Vertex line[] = {
                sf::Vertex(points[i - 1], algo_colors[a]),
                sf::Vertex(points[i], algo_colors[a])
            };
            window_.draw(line, 2, sf::Lines);
        }

        for (const auto& pt : points) {
            sf::CircleShape dot(4.5f);
            dot.setOrigin(4.5f, 4.5f);
            dot.setPosition(pt);
            dot.setFillColor(algo_colors[a]);
            window_.draw(dot);
        }
    }

    // Legend
    float legend_x = plot_area.left + plot_area.width - 230.0f;
    float legend_y = plot_area.top + 20.0f;

    for (size_t a = 0; a < algos.size(); ++a) {
        sf::RectangleShape box(sf::Vector2f(14.0f, 14.0f));
        box.setPosition(legend_x, legend_y + a * 24.0f);
        box.setFillColor(algo_colors[a]);
        window_.draw(box);

        if (font_loaded_) {
            sf::Text leg_txt;
            leg_txt.setFont(font_);
            leg_txt.setCharacterSize(14);
            leg_txt.setFillColor(sf::Color::White);
            leg_txt.setString(algos[a]);
            leg_txt.setPosition(legend_x + 24.0f, legend_y + a * 24.0f - 2.0f);
            window_.draw(leg_txt);
        }
    }
}

void SortingVisualizer::render_toolbar() {
    sf::RectangleShape top_bar(sf::Vector2f(1280.0f, 88.0f));
    top_bar.setFillColor(sf::Color(10, 14, 20));
    window_.draw(top_bar);

    sf::RectangleShape divider(sf::Vector2f(1280.0f, 1.0f));
    divider.setPosition(0.0f, 88.0f);
    divider.setFillColor(sf::Color(40, 50, 68));
    window_.draw(divider);

    if (font_loaded_) {
        // App Title
        sf::Text main_title;
        main_title.setFont(font_);
        main_title.setCharacterSize(19);
        main_title.setStyle(sf::Text::Bold);
        main_title.setFillColor(sf::Color(0, 220, 255));
        main_title.setString("SAC VISUALIZER");
        main_title.setPosition(25.0f, 16.0f);
        window_.draw(main_title);

        // Section labels
        sf::Text lbl_algo;
        lbl_algo.setFont(font_);
        lbl_algo.setCharacterSize(13);
        lbl_algo.setFillColor(sf::Color(140, 155, 175));
        lbl_algo.setString("Algorithm:");
        lbl_algo.setPosition(25.0f, 57.0f);
        window_.draw(lbl_algo);

        sf::Text lbl_dist;
        lbl_dist.setFont(font_);
        lbl_dist.setCharacterSize(13);
        lbl_dist.setFillColor(sf::Color(140, 155, 175));
        lbl_dist.setString("Data:");
        lbl_dist.setPosition(395.0f, 57.0f);
        window_.draw(lbl_dist);

        // Size Display
        sf::Text lbl_size;
        lbl_size.setFont(font_);
        lbl_size.setCharacterSize(13);
        lbl_size.setFillColor(sf::Color(140, 155, 175));
        lbl_size.setString("Size:");
        lbl_size.setPosition(825.0f, 57.0f);
        window_.draw(lbl_size);

        sf::Text val_size;
        val_size.setFont(font_);
        val_size.setCharacterSize(13);
        val_size.setStyle(sf::Text::Bold);
        val_size.setFillColor(sf::Color::White);
        val_size.setString(std::to_string(array_size_));
        val_size.setPosition(905.0f, 57.0f);
        window_.draw(val_size);

        // Speed Display
        sf::Text lbl_speed;
        lbl_speed.setFont(font_);
        lbl_speed.setCharacterSize(13);
        lbl_speed.setFillColor(sf::Color(140, 155, 175));
        lbl_speed.setString("Speed:");
        lbl_speed.setPosition(1025.0f, 57.0f);
        window_.draw(lbl_speed);

        sf::Text val_speed;
        val_speed.setFont(font_);
        val_speed.setCharacterSize(13);
        val_speed.setStyle(sf::Text::Bold);
        val_speed.setFillColor(sf::Color::White);
        std::stringstream ss_spd;
        ss_spd << speed_level_ << "/7";
        val_speed.setString(ss_spd.str());
        val_speed.setPosition(1115.0f, 57.0f);
        window_.draw(val_speed);
    }

    for (const auto& btn : buttons_) {
        sf::Color accent = sf::Color(0, 145, 215);
        if (btn.id == "action_custom") accent = sf::Color(46, 204, 113); // Highlight Insert Array in green
        render_button(btn, accent);
    }
}

void SortingVisualizer::render_bottom_hud() {
    sf::RectangleShape btm_bar(sf::Vector2f(1280.0f, 45.0f));
    btm_bar.setPosition(0.0f, 675.0f);
    btm_bar.setFillColor(sf::Color(10, 14, 20));
    window_.draw(btm_bar);

    sf::RectangleShape divider(sf::Vector2f(1280.0f, 1.0f));
    divider.setPosition(0.0f, 675.0f);
    divider.setFillColor(sf::Color(40, 50, 68));
    window_.draw(divider);

    if (!font_loaded_) return;

    sf::Text status;
    status.setFont(font_);
    status.setCharacterSize(13);
    status.setFillColor(sf::Color(170, 185, 205));

    std::stringstream ss;
    if (current_mode_ == VisualizerMode::SINGLE_SORT) {
        ss << "Current: " << current_algo_
           << (has_custom_array_ ? " [CUSTOM ARRAY TRACE]" : " [" + input_type_to_string(input_type_) + "]")
           << "  |  Comparisons: " << single_runner_.comparisons
           << "  |  Swaps/Moves: " << single_runner_.swaps
           << "  |  State: " << (single_runner_.finished ? "[SORT COMPLETE]" : (is_paused_ ? "[PAUSED]" : "[SORTING...]"));
    } else if (current_mode_ == VisualizerMode::RACE_MODE) {
        ss << "4-Way Race Mode" << (has_custom_array_ ? " [ON CUSTOM ARRAY]" : " [ON IDENTICAL ARRAY]")
           << "  |  Finished: " << finished_count_ << "/4"
           << "  |  State: " << (finished_count_ == 4 ? "[ALL FINISHED]" : (is_paused_ ? "[PAUSED]" : "[RACING...]"));
    } else if (current_mode_ == VisualizerMode::RESULTS_PLOT) {
        ss << "Benchmark Curves View  |  Click any Data button above to switch dataset";
    }

    status.setString(ss.str());
    status.setPosition(25.0f, 688.0f);
    window_.draw(status);
}

void SortingVisualizer::render_input_modal() {
    // Dark Backdrop
    sf::RectangleShape backdrop(sf::Vector2f(1280.0f, 720.0f));
    backdrop.setFillColor(sf::Color(0, 0, 0, 190));
    window_.draw(backdrop);

    // Modal Card
    sf::RectangleShape card(sf::Vector2f(600.0f, 410.0f));
    card.setPosition(340.0f, 120.0f);
    card.setFillColor(sf::Color(20, 26, 36));
    card.setOutlineThickness(2.0f);
    card.setOutlineColor(sf::Color(0, 200, 255));
    window_.draw(card);

    if (!font_loaded_) return;

    // Header Title
    sf::Text title;
    title.setFont(font_);
    title.setCharacterSize(18);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color::White);
    title.setString("INSERT CUSTOM ARRAY TO SORT & TRACE");
    title.setPosition(370.0f, 140.0f);
    window_.draw(title);

    // Subtitle
    sf::Text subtitle;
    subtitle.setFont(font_);
    subtitle.setCharacterSize(13);
    subtitle.setFillColor(sf::Color(160, 175, 195));
    subtitle.setString("Type integers separated by commas or spaces:");
    subtitle.setPosition(370.0f, 175.0f);
    window_.draw(subtitle);

    // Text Input Field Box
    sf::RectangleShape inputBox(sf::Vector2f(540.0f, 44.0f));
    inputBox.setPosition(370.0f, 202.0f);
    inputBox.setFillColor(sf::Color(12, 16, 24));
    inputBox.setOutlineThickness(1.5f);
    inputBox.setOutlineColor(sf::Color(0, 180, 255));
    window_.draw(inputBox);

    // Text with blinking cursor
    std::string display_str = input_buffer_;
    if (cursor_timer_ < 0.5f) {
        display_str += "|";
    }

    sf::Text inputText;
    inputText.setFont(font_);
    inputText.setCharacterSize(16);
    inputText.setFillColor(sf::Color::White);
    inputText.setString(display_str);
    inputText.setPosition(380.0f, 212.0f);
    window_.draw(inputText);

    // Live preview of parsed elements
    auto parsed = parse_array_string(input_buffer_);
    sf::Text preview;
    preview.setFont(font_);
    preview.setCharacterSize(13);

    if (parsed.size() >= 2) {
        preview.setFillColor(sf::Color(46, 204, 113));
        std::stringstream ss;
        ss << "✓ Detected " << parsed.size() << " elements: [ ";
        for (size_t i = 0; i < std::min<size_t>(parsed.size(), 10); ++i) {
            ss << parsed[i] << (i + 1 < parsed.size() ? ", " : "");
        }
        if (parsed.size() > 10) ss << "...";
        ss << " ]";
        preview.setString(ss.str());
    } else {
        preview.setFillColor(sf::Color(241, 196, 15));
        preview.setString("⚠ Please enter at least 2 numbers (e.g. 64, 34, 25, 12, 22)");
    }
    preview.setPosition(370.0f, 260.0f);
    window_.draw(preview);

    // Preset helper label
    sf::Text lbl_presets;
    lbl_presets.setFont(font_);
    lbl_presets.setCharacterSize(12);
    lbl_presets.setFillColor(sf::Color(140, 155, 175));
    lbl_presets.setString("Or click a quick example preset:");
    lbl_presets.setPosition(370.0f, 312.0f);
    window_.draw(lbl_presets);

    // Render modal action & preset buttons
    for (const auto& btn : modal_buttons_) {
        sf::Color accent = sf::Color(0, 145, 215);
        if (btn.id == "modal_sort") accent = sf::Color(46, 204, 113); // Green
        else if (btn.id == "modal_cancel") accent = sf::Color(231, 76, 60); // Red
        render_button(btn, accent);
    }
}

void SortingVisualizer::render() {
    window_.clear(sf::Color(8, 10, 15));

    render_toolbar();

    if (current_mode_ == VisualizerMode::SINGLE_SORT) {
        render_single_mode();
    } else if (current_mode_ == VisualizerMode::RACE_MODE) {
        render_race_mode();
    } else if (current_mode_ == VisualizerMode::RESULTS_PLOT) {
        render_results_plot();
    }

    render_bottom_hud();

    if (input_modal_open_) {
        render_input_modal();
    }

    window_.display();
}

void SortingVisualizer::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        float dt = clock.restart().asSeconds();
        handle_events();
        update(dt);
        render();
    }
}
