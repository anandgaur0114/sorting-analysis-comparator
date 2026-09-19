# Sorting Analysis Comparator (SAC) 📊⚡

An advanced **Analysis of Algorithms (AOA)** benchmarking, empirical complexity analysis, and interactive animated visualization suite built in **C++17** and **SFML 2.5**.

SAC compares **Merge Sort**, **Quick Sort**, **Heap Sort**, and **Radix Sort** — evaluating actual measured execution times, operation counts (comparisons & swaps), and theoretical Big-O complexity ratios against real-world hardware performance across multiple data distributions and scales.

---

## 🌟 Features

### 1. Dual-Variant Algorithm Implementations (C++17)
- **Fast Variant**: Pure vectorized implementations optimized with `-O3`, zero logging overhead, preallocated buffers, and tail-call optimization for accurate hardware timing.
- **Instrumented Variant**: Tracks exact comparison and swap/assignment counts, and logs step-by-step state events (`AnimationStep`) for graphical playback.
- **Algorithms Covered**:
  - **Merge Sort**: Top-down recursion with auxiliary scratch buffer $\Theta(n \log n)$.
  - **Quick Sort**: Median-of-three pivot selection and tail-recursion elimination to prevent $O(n^2)$ recursion stack overflow on sorted inputs.
  - **Heap Sort**: In-place binary max-heap build in $\Theta(n)$ bottom-up sift-down followed by repeated root extraction $\Theta(n \log n)$.
  - **Radix Sort**: Least Significant Digit (LSD) base-10 non-comparative distribution sort $\Theta(d \cdot (n + k))$.

### 2. High-Precision Benchmarking Module
- **Input Scales ($N$)**: $100, 500, 1000, 5000, 10000, 50000$.
- **4 Data Distributions**:
  - `Random`: Uniform random integers in range $[1, 100000]$.
  - `Nearly Sorted`: Pre-sorted array with $\sim 5\%$ random index swaps.
  - `Reverse Sorted`: Strictly descending $[N, \dots, 1]$.
  - `Many Duplicates`: Values in small domain $[1, 10]$ to test key collision handling.
- **Sub-Microsecond Hardware Timing**: Utilizes Windows `QueryPerformanceCounter` (QPC) via the `HighResClock` cross-platform abstraction to bypass MinGW's 15.6 ms resolution limitation.
- **CSV Data Export**: Multi-trial averaged results automatically saved to `results.csv`.

### 3. Empirical vs. Theoretical Big-O Analysis
- Computes empirical growth ratios between successive input sizes $n_1 \to n_2$:
  $$R_{obs} = \frac{T(n_2)}{T(n_1)}, \quad \alpha = \frac{\ln(R_{obs})}{\ln(n_2 / n_1)}$$
- Compares observed growth against theoretical models:
  - $O(n \log n)$: $\frac{n_2 \log_2 n_2}{n_1 \log_2 n_1}$
  - $O(n^2)$: $(n_2 / n_1)^2$
  - $O(n)$: $n_2 / n_1$
- Formats a clean summary table for lab reports directly in the console.

### 4. Interactive SFML Visualizer (GUI)
- **Clickable UI Toolbar**: Fully mouse-controllable interface with buttons for mode switching, algorithm selection, distributions, speed, and scale.
- **Single Sort View**: Bar-chart visualization with color-coded operations:
  - 🔵 **Blue**: Default unsorted elements
  - 🟡 **Yellow**: Active comparison indices (`COMPARE`)
  - 🔴 **Red**: Swapped indices (`SWAP`)
  - 🟣 **Purple**: Overwritten/moved elements (`ASSIGN`)
  - 🟢 **Green**: Confirmed sorted elements (`MARK_SORTED`)
- **Live Number Labels**: For arrays with $N \le 25$, the exact integer values are displayed directly on top of each bar so you can watch numbers exchange positions.
- **4-Way Race Mode**: $2 \times 2$ Quad split-screen animating Merge, Quick, Heap, and Radix sort concurrently on the **exact same initial array** with real-time placement badges (`1st PLACE`, `2nd PLACE`, etc.).
- **Benchmark Plotter View**: Graphical 2D chart plotting Execution Time (ms) vs. Scale $N$ curves directly from `results.csv`.
- **✍ Custom Array Insertion**: Click `[✍ Insert Array]` to type or paste any custom numbers (e.g. `45, 12, 89, 3, 21, 67, 4`) and trace the exact sorting steps!

---

## 📁 Project Structure

```
AOA project/
├── include/
│   ├── types.hpp         # StepType, AnimationStep, SortMetrics, HighResClock
│   ├── sorts.hpp         # Declarations for fast & instrumented sorting variants
│   ├── benchmark.hpp     # Benchmarking suite & data generators
│   ├── analysis.hpp      # Empirical complexity ratio fitting & table generator
│   └── visualizer.hpp    # SFML visualizer architecture & state runners
├── src/
│   ├── sorts.cpp         # Implementations of Merge, Quick, Heap, Radix sorts
│   ├── benchmark.cpp     # High-precision hardware timing harness & CSV exporter
│   ├── analysis.cpp      # Big-O ratio fitting & theoretical comparisons
│   ├── visualizer.cpp    # Full SFML visualizer (Single Sort, Race Mode, Results Plot)
│   └── main.cpp          # Application entry point with interactive menu & CLI
├── build.bat             # 1-Click build script for Windows MinGW GCC
├── CMakeLists.txt        # Full CMake build configuration
├── results.csv           # Benchmark results dataset across all 96 configurations
├── arial.ttf             # Font asset for GUI text and labels
├── sfml-*.dll            # SFML 2.5.1 runtime libraries
└── .gitignore            # Git ignore file for binaries and build artifacts
```

---

## 🛠️ Build & Installation

### Prerequisites
- **Compiler**: GCC / MinGW with C++14/C++17 support (or MSVC / Clang).
- **SFML 2.5.1**: Headers and libraries (runtime DLLs already included).

### Method 1: Using `build.bat` (Recommended for MinGW on Windows)
Simply double-click `build.bat` or run in PowerShell:
```powershell
cmd.exe /c build.bat
```

### Method 2: Using CMake
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

---

## 🚀 Running the Application

### Interactive Menu
Running without arguments opens the interactive terminal menu:
```powershell
.\sac.exe
```
```
======================================================================
           SORTING ANALYSIS COMPARATOR (SAC) - MAIN MENU              
======================================================================
  [1] Launch SFML Interactive Visualizer (Single, Race & Plots)
  [2] Run Full Benchmark Suite (Export to results.csv)
  [3] Show Empirical vs. Theoretical Complexity Table
  [4] Run Correctness Self-Tests (All Algorithms & Distributions)
  [5] Run Complete Suite (Tests + Benchmarks + Analysis)
  [0] Exit
======================================================================
```

### Command-Line Arguments
```powershell
.\sac.exe --visualize    # Launch SFML Visualizer directly
.\sac.exe --benchmark    # Run full benchmark matrix and export results.csv
.\sac.exe --analyze      # Display theoretical vs empirical Big-O table
.\sac.exe --test         # Run algorithm correctness unit tests
.\sac.exe --all          # Run tests, benchmarks, and analysis
```

---

## 🎮 Visualizer Controls

### On-Screen Buttons (Mouse Clickable):
- **View Navigation Tabs**: `[Single Sort]`, `[Race Mode]`, `[Benchmark Plot]`
- **Actions**: `[✍ Insert Array]`, `[🎲 Randomize]`, `[❚❚ Pause / ▶ Resume]`
- **Algorithm Selector**: `[Merge]`, `[Quick]`, `[Heap]`, `[Radix]`
- **Dataset Selector**: `[Random]`, `[Nearly Sorted]`, `[Reverse]`, `[Duplicates]`
- **Scale $N$ Controls**: `[-]` and `[+]` ($8 \leftrightarrow 512$)
- **Speed Controls**: `[-]` and `[+]` ($1/7 \leftrightarrow 7/7$)

### Optional Keyboard Shortcuts:
| Key | Action |
|---|---|
| `Tab` | Switch active view (`Single Sort` $\to$ `Race Mode` $\to$ `Results Plot`) |
| `Space` | Pause / Resume animation |
| `Enter` / `BackSpace` | Open custom array insertion dialog |
| `1` / `2` / `3` / `4` | Select Merge, Quick, Heap, or Radix Sort |
| `T` | Cycle data distribution |
| `Left` / `Right` | Decrease / Increase array size $N$ |
| `Up` / `Down` | Decrease / Increase animation speed |
| `Esc` | Exit visualizer |

---

## 📈 Sample Empirical Analysis Results

Sample excerpt from the empirical complexity analysis ($10,000 \to 50,000$ elements):

```
=========================================================================================================
                   EMPIRICAL VS. THEORETICAL COMPLEXITY GROWTH ANALYSIS TABLE                            
=========================================================================================================
Algorithm    Distribution    n1 -> n2       Obs Time (R) Exp O(nlogn) Exp O(n^2) Exp O(n)   Obs Comps   Est. alpha
---------------------------------------------------------------------------------------------------------
Heap Sort    Random          10000->50000   5.91        5.87         25.0       5.0        5.99        1.10      
Merge Sort   Random          10000->50000   5.70        5.87         25.0       5.0        5.96        1.08      
Quick Sort   Random          10000->50000   5.70        5.87         25.0       5.0        6.02        1.08      
Quick Sort   Many Duplicates 10000->50000   20.69       5.87         25.0       5.0        24.74       1.88      
Radix Sort   Random          10000->50000   5.24        5.87         25.0       5.0        N/A (O(n+k))1.03      
Radix Sort   Reverse Sorted  10000->50000   5.03        5.87         25.0       5.0        N/A (O(n+k))1.00      
=========================================================================================================
```

### Key Insights:
1. **$O(n \log n)$ Verification**: For Heap Sort, Merge Sort, and Quick Sort on random inputs, observed time ratio ($5.70 - 5.91\times$) and comparison ratio ($5.96 - 6.02\times$) closely track the theoretical prediction ($5.87\times$), with fitted $\alpha \approx 1.08 - 1.10$.
2. **$O(n^2)$ Degradation**: Quick Sort on many duplicates degrades quadratically ($24.74\times$ comparisons increase on a $5\times$ scale increase, matching $5^2 = 25$), resulting in an observed time ratio of $20.69\times$ and $\alpha \approx 1.88$.
3. **Linear Complexity**: Radix Sort exhibits an observed growth ratio of $5.03\times$ (theoretical linear is $5.00\times$) with $\alpha = 1.00$ and zero comparisons.

---

## 📜 License

Distributed under the MIT License. See `LICENSE` for details.
