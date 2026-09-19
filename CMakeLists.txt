cmake_minimum_required(VERSION 3.10)
project(SortingAnalysisComparator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler optimization flags
if (MSVC)
    add_compile_options(/O2 /W4)
else()
    add_compile_options(-O3 -Wall -Wextra)
endif()

include_directories(include)

# Core SAC executable (CLI, Benchmarks, Empirical Analysis)
set(SAC_SOURCES
    src/sorts.cpp
    src/benchmark.cpp
    src/analysis.cpp
    src/visualizer.cpp
    src/main.cpp
)

add_executable(sac ${SAC_SOURCES})

# SFML Configuration
set(SFML_DIR "C:/Users/HP/Desktop/CGM PROJECT/SFML-2.5.1/lib/cmake/SFML" CACHE PATH "Path to SFML")
find_package(SFML 2.5 COMPONENTS graphics window system OPTIONAL_COMPONENTS)

if (SFML_FOUND)
    message(STATUS "SFML found! Enabling GUI visualizer targets.")
    target_link_libraries(sac sfml-graphics sfml-window sfml-system)
else()
    message(STATUS "SFML not found or not required for CLI benchmark suite.")
endif()
