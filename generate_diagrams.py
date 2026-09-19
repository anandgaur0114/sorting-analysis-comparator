import matplotlib.pyplot as plt
import matplotlib.patches as patches
import pandas as pd
import numpy as np
import os

os.makedirs("docs", exist_ok=True)

# Set global style
plt.rcParams['font.sans-serif'] = 'Arial'
plt.rcParams['font.family'] = 'sans-serif'

def create_block_diagram():
    fig, ax = plt.subplots(figsize=(12, 7), dpi=300)
    ax.set_facecolor('#0f141d')
    fig.patch.set_facecolor('#0f141d')
    ax.set_xlim(0, 12)
    ax.set_ylim(0, 7)
    ax.axis('off')

    title = "SORTING ANALYSIS COMPARATOR (SAC) - SYSTEM ARCHITECTURE BLOCK DIAGRAM"
    ax.text(6, 6.5, title, color='#00d4ff', fontsize=14, fontweight='bold', ha='center')

    # Draw boxes
    boxes = [
        ("Input Generation Layer", 
         "• Synthetic Presets:\n  - Uniform Random\n  - Nearly Sorted (~5% swaps)\n  - Reverse Sorted\n  - Many Duplicates\n• Custom User Array Dialog",
         0.6, 1.8, 3.0, 4.0, '#1a2233', '#00b4d8'),
        
        ("Algorithmic & Timing Core",
         "• Fast Variants (Pure Speed, -O3):\n  - Merge Sort (Aux buffer)\n  - Quick Sort (Median-of-3)\n  - Heap Sort (In-place sift-down)\n  - Radix Sort (LSD base-10)\n• Windows QPC Hardware Timer\n  (Sub-microsecond resolution)",
         4.2, 1.8, 3.6, 4.0, '#1a2233', '#f5a623'),
        
        ("Telemetry & Empirical Engine",
         "• Instrumented Variants:\n  - Comparison & Swap counting\n  - AnimationStep Event Stream\n• Empirical Ratio Fitting:\n  - R_obs = T(n2) / T(n1)\n  - Power Exponent alpha\n• results.csv Dataset Exporter",
         8.4, 3.8, 3.0, 2.0, '#1a2233', '#2ecc71'),
         
        ("SFML Interactive Visualizer",
         "• Clickable Mouse UI Toolbar\n• Single Sort View (Live Number Labels)\n• 4-Way Split-Screen Race Mode\n• Benchmark Time vs N Plotter",
         8.4, 1.2, 3.0, 2.2, '#1a2233', '#e74c3c')
    ]

    for title, desc, x, y, w, h, bg, border in boxes:
        rect = patches.FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.15", 
                                      facecolor=bg, edgecolor=border, linewidth=2)
        ax.add_patch(rect)
        ax.text(x + w/2, y + h - 0.35, title, color=border, fontsize=11, fontweight='bold', ha='center')
        ax.text(x + 0.2, y + h - 0.7, desc, color='#d0d8e8', fontsize=8.5, va='top')

    # Connecting Arrows
    arrow_style = dict(arrowstyle="->", color='#00d4ff', lw=2, mutation_scale=15)
    
    # Input -> Algorithmic Core
    ax.annotate("", xy=(4.2, 3.8), xytext=(3.6, 3.8), arrowprops=arrow_style)
    # Core -> Telemetry
    ax.annotate("", xy=(8.4, 4.8), xytext=(7.8, 4.8), arrowprops=arrow_style)
    # Core -> Visualizer
    ax.annotate("", xy=(8.4, 2.3), xytext=(7.8, 2.3), arrowprops=arrow_style)
    # Telemetry -> Visualizer (CSV data feed to plotter)
    ax.annotate("", xy=(9.9, 3.4), xytext=(9.9, 3.8), arrowprops=dict(arrowstyle="->", color='#2ecc71', lw=1.5, mutation_scale=12))

    plt.tight_layout()
    plt.savefig("docs/block_diagram.png", facecolor=fig.get_facecolor(), edgecolor='none', bbox_inches='tight')
    plt.close()
    print("Created docs/block_diagram.png")

def create_uml_diagram():
    fig, ax = plt.subplots(figsize=(13, 8), dpi=300)
    ax.set_facecolor('#0f141d')
    fig.patch.set_facecolor('#0f141d')
    ax.set_xlim(0, 13)
    ax.set_ylim(0, 8)
    ax.axis('off')

    ax.text(6.5, 7.6, "SORTING ANALYSIS COMPARATOR (SAC) - UML CLASS DIAGRAM", 
            color='#00d4ff', fontsize=14, fontweight='bold', ha='center')

    classes = [
        ("HighResClock",
         "+ time_point : long long\n+ now() : time_point\n+ duration_ms(start, end) : double",
         0.5, 5.0, 3.2, 1.8, '#4a90e2'),
        
        ("AnimationStep",
         "+ type : StepType\n+ index1 : int\n+ index2 : int\n+ value : int",
         0.5, 2.6, 3.2, 1.8, '#f39c12'),

        ("SortMetrics",
         "+ comparisons : uint64_t\n+ swaps : uint64_t\n+ duration_ms : double\n+ steps : vector<AnimationStep>",
         0.5, 0.4, 3.2, 1.8, '#2ecc71'),

        ("SortRunnerState",
         "+ name : string\n+ initial_array : vector<int>\n+ current_array : vector<int>\n+ steps : vector<AnimationStep>\n+ step_index : size_t\n+ comparisons, swaps : uint64_t\n+ finished : bool\n+ finish_rank : int",
         4.2, 0.4, 3.8, 3.6, '#9b59b6'),

        ("UIButton",
         "+ id, label : string\n+ bounds : FloatRect\n+ is_active : bool\n+ is_hovered : bool\n+ contains(pos) : bool",
         4.2, 4.6, 3.8, 2.2, '#e67e22'),

        ("SortingVisualizer",
         "+ window_ : RenderWindow\n+ current_mode_ : VisualizerMode\n+ single_runner_ : SortRunnerState\n+ race_runners_ : vector<SortRunnerState>\n+ buttons_ : vector<UIButton>\n+ run() : void\n+ handle_events() : void\n+ update(dt) : void\n+ render() : void\n+ render_bars() : void\n+ confirm_custom_array() : void",
         8.5, 0.4, 4.0, 6.4, '#1abc9c')
    ]

    for title, content, x, y, w, h, color in classes:
        rect = patches.FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.1", 
                                      facecolor='#18202c', edgecolor=color, linewidth=2)
        ax.add_patch(rect)
        # Header box
        header_rect = patches.Rectangle((x, y + h - 0.5), w, 0.5, facecolor=color)
        ax.add_patch(header_rect)
        ax.text(x + w/2, y + h - 0.35, title, color='#ffffff', fontsize=10.5, fontweight='bold', ha='center')
        ax.text(x + 0.15, y + h - 0.7, content, color='#e0e6f0', fontsize=8, family='monospace', va='top')

    # Relationship connectors
    line_kw = dict(color='#7f8c8d', lw=1.5, linestyle='--')
    # AnimationStep in SortMetrics
    ax.annotate("", xy=(2.1, 2.2), xytext=(2.1, 2.6), arrowprops=dict(arrowstyle="->", color='#2ecc71', lw=1.5))
    # SortMetrics in SortRunnerState
    ax.annotate("", xy=(4.2, 1.3), xytext=(3.7, 1.3), arrowprops=dict(arrowstyle="->", color='#9b59b6', lw=1.5))
    # SortRunnerState in SortingVisualizer
    ax.annotate("", xy=(8.5, 2.2), xytext=(8.0, 2.2), arrowprops=dict(arrowstyle="->", color='#1abc9c', lw=1.5))
    # UIButton in SortingVisualizer
    ax.annotate("", xy=(8.5, 5.7), xytext=(8.0, 5.7), arrowprops=dict(arrowstyle="->", color='#1abc9c', lw=1.5))

    plt.tight_layout()
    plt.savefig("docs/uml_diagram.png", facecolor=fig.get_facecolor(), edgecolor='none', bbox_inches='tight')
    plt.close()
    print("Created docs/uml_diagram.png")

def create_benchmark_curves():
    if not os.path.exists("results.csv"):
        print("results.csv not found, skipping curve generation.")
        return

    df = pd.read_csv("results.csv")
    fig, ax = plt.subplots(figsize=(10, 6), dpi=300)
    ax.set_facecolor('#0f141d')
    fig.patch.set_facecolor('#0f141d')

    # Filter by Random distribution
    df_rnd = df[df['input_type'] == 'Random']

    colors = {
        'Merge Sort': '#00d4ff',
        'Quick Sort': '#ffd700',
        'Heap Sort': '#ff8c00',
        'Radix Sort': '#2ecc71'
    }

    for algo in df_rnd['algorithm'].unique():
        sub = df_rnd[df_rnd['algorithm'] == algo].sort_values('n')
        ax.plot(sub['n'], sub['avg_time_ms'], marker='o', lw=2.2, label=algo, color=colors.get(algo, 'white'))

    ax.set_title("Empirical Execution Time vs. Input Scale N (Random Distribution)", color='#00d4ff', fontsize=13, fontweight='bold')
    ax.set_xlabel("Array Scale N (elements)", color='#d0d8e8', fontsize=11)
    ax.set_ylabel("Execution Time (ms)", color='#d0d8e8', fontsize=11)
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.tick_params(colors='#8898aa')
    ax.grid(True, which='both', color='#243044', linestyle=':', lw=1)

    legend = ax.legend(facecolor='#18202c', edgecolor='#334460', labelcolor='#ffffff', fontsize=10)
    
    plt.tight_layout()
    plt.savefig("docs/benchmark_curves.png", facecolor=fig.get_facecolor(), edgecolor='none', bbox_inches='tight')
    plt.close()
    print("Created docs/benchmark_curves.png")

if __name__ == '__main__':
    create_block_diagram()
    create_uml_diagram()
    create_benchmark_curves()
