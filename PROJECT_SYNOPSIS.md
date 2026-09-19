# A Project SYNOPSIS Of SORTING ANALYSIS COMPARATOR (SAC)

**Submitted in partial fulfillment for the award of degree of**  
### Bachelor of Technology  
**Degree Of**  
### Rajasthan Technical University, KOTA  
**2026–2027**

---

### **Submitted By:**
- **Abhinav Bhatra** (Roll No: `24EJCCA009`)
- **Amit kumar** (Roll No: `24EJCCA027`)
- **Anand Gaur** (Roll No: `24EJCCA030`)

### **ITS Coordinator (CC):**
- **Mona Nagar** (Faculty Mentor)

---

**DEPARTMENT OF COMPUTER SCIENCE & ENGINEERING (ARTIFICIAL INTELLIGENCE)**  
**JAIPUR ENGINEERING COLLEGE AND RESEARCH CENTRE**  
*Shri Ram ki Nangal, via Sitapura RIICO, Tonk Road, Sukhpuria, Bambala, Jaipur, Rajasthan*

---

## ABSTRACT

Algorithm analysis in computer science pedagogy is predominantly theoretical, evaluating time and space complexities using asymptotic Big-O notations without direct empirical correlation to underlying physical computing architecture. The **Sorting Analysis Comparator (SAC)** is an engineering and laboratory analysis system built in **C++17** and **SFML 2.5** that rigorously bridges theoretical mathematical predictions and empirical hardware latency.

SAC benchmarks four foundational sorting paradigms—**Merge Sort** (Divide-and-Conquer), **Quick Sort** (Partitioning), **Heap Sort** (Priority Queue Selection), and **Radix Sort** (Non-comparative Distribution)—across six scaling orders ($N = 100$ to $50,000$) and four distinct topologies (Uniform Random, Nearly Sorted, Reverse Sorted, and Many Duplicates). Utilizing Windows `QueryPerformanceCounter` (QPC) for sub-microsecond wall-clock precision and an instrumented telemetry logger for exact comparison and swap counts, the system computes empirical growth ratios $R_{obs} = T(n_2)/T(n_1)$ and fits empirical power exponents $\alpha = \frac{\ln(R_{obs})}{\ln(n_2/n_1)}$.

Empirical results prove strict $O(n \log n)$ convergence ($\alpha \approx 1.08$) for Merge and Heap sorts on random inputs, reveal Quick Sort's quadratic degradation ($\alpha = 1.88 - 2.01$) on duplicate-heavy arrays, and demonstrate Radix Sort's strictly linear scaling ($\alpha = 1.00$) with zero comparisons. Finally, an interactive 60 FPS graphical visualizer provides Single Sort Bar Animation with live on-bar numeric values, an unprecedented 4-Way Split-Screen Race Mode, and execution curve plotting.

---

## INTRODUCTION

### 1. Problem Statement
Students and algorithm engineers frequently encounter a significant disconnect between theoretical Big-O complexity and physical execution time. Textbooks state that Quick Sort and Merge Sort are $O(n \log n)$, yet ignore hardware cache lines, branch miss penalties, and the catastrophic $O(n^2)$ degradation of naive partitioning on duplicate keys. There exists a lack of integrated tools capable of simultaneously recording hardware wall-clock time, counting exact comparison/swap operations, fitting empirical growth exponents, and visually rendering algorithmic steps in real time.

### 2. Scope of the Project
The scope encompasses the complete design and implementation of a C++17 dual-variant benchmarking suite, sub-microsecond hardware timing harnesses, automated CSV reporting, empirical power ratio fitting, and a desktop SFML graphical visualizer supporting Single Sort animation, 4-Way Head-to-Head Race Mode, Benchmark Plotting, and arbitrary Custom Array user input.

### 3. Technologies Used
- **Language**: C++17 (high-performance vectorization, templates, memory buffers)
- **Graphics & Windowing**: SFML 2.5.1 (Simple and Fast Multimedia Library)
- **Timing API**: Windows `QueryPerformanceCounter` (QPC) hardware clock
- **Build Toolchain**: MinGW GCC 6.3+ and CMake 3.10+
- **Data & Plotting**: Python 3.10, Matplotlib, Pandas

### 4. Methodology
Dual-variant implementations ensure benchmark timings remain free of logging overhead (compiled under `-O3`), while an instrumented telemetry engine records comparison counts, swaps, and animation steps. Benchmarks are averaged over 5 stochastic trials per configuration across scales $N \in \{100, 500, 1000, 5000, 10000, 50000\}$. Real-time event playback reconstructs array transitions at 60 FPS.

### 5. Expected Outcome
A production-grade, highly educational laboratory software suite capable of proving asymptotic theoretical sorting complexities, highlighting worst-case degradation scenarios, exporting full statistical CSV datasets, and offering real-time animated sorting races.

### 6. Applications of Project
Serves as an essential teaching instrument for Analysis of Algorithms laboratories, algorithm profiling in embedded systems, database query optimizer verification, and technical interview preparation.

---

## CONCLUSIONS AND FUTURE ENHANCEMENTS

### Conclusions
The experimental results conclusively validate theoretical complexity bounds while uncovering critical hardware-level insights:
1. **Merge Sort & Heap Sort** consistently achieve $O(n \log n)$ scaling across all permutations, with observed growth ratios closely matching theoretical predictions ($5.7\times$ vs $5.87\times$ expected from 10k to 50k).
2. **Quick Sort** exhibits excellent constant-factor performance on uniform random inputs but undergoes severe $O(n^2)$ degradation on high duplicate densities (ratio $20.69\times$ vs $25.0\times$ theoretical quadratic).
3. **Radix Sort** strictly adheres to linear $O(n)$ scaling with an empirical $\alpha = 1.00$ and zero comparisons, confirming the non-comparative advantage.

### Future Enhancements
1. **Parallel GPU Sorting**: Implementation of CUDA and OpenCL kernels for GPU Bitonic Merge Sort and Radix Sort.
2. **Audio Synthesis**: Generation of dynamic audio tones (frequencies mapped to array values) to produce algorithmic soundscapes.
3. **External Memory Sorting**: Benchmarking disk I/O paging mechanisms for datasets exceeding physical RAM ($10^7+$ elements).
4. **Multi-Threaded Parallelism**: Integrating `std::execution::par` policies to evaluate OpenMP multicore scaling.

---

## REFERENCES
1. Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2022). *Introduction to Algorithms* (4th ed.). MIT Press.
2. Knuth, D. E. (1998). *The Art of Computer Programming, Volume 3: Sorting and Searching* (2nd ed.). Addison-Wesley.
3. Sedgewick, R., & Wayne, K. (2011). *Algorithms* (4th ed.). Addison-Wesley Professional.
4. SFML Development Team. (2024). *Simple and Fast Multimedia Library Documentation (Release 2.5.1)*. https://www.sfml-dev.org/
5. ISO/IEC. (2017). *ISO/IEC 14882:2017 - Programming Languages — C++17*. International Organization for Standardization.
6. Hoare, C. A. R. (1962). Quicksort. *The Computer Journal*, 5(1), 10-16.
7. Williams, J. W. J. (1964). Algorithm 232: Heapsort. *Communications of the ACM*, 7(6), 347-348.
