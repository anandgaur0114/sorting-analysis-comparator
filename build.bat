@echo off
echo ===================================================
echo   Compiling Sorting Analysis Comparator (SAC)
echo ===================================================

set SFML_DIR=C:\Users\HP\Desktop\CGM PROJECT\SFML-2.5.1

g++ -std=c++14 -O3 -Wall -Wextra -Iinclude -I"%SFML_DIR%\include" -L"%SFML_DIR%\lib" src/sorts.cpp src/benchmark.cpp src/analysis.cpp src/visualizer.cpp src/main.cpp -lsfml-graphics -lsfml-window -lsfml-system -o sac.exe

if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] Build completed: sac.exe created successfully with SFML integration.
    echo Run "sac.exe --help" for available options.
) else (
    echo.
    echo [ERROR] Build failed with error code %ERRORLEVEL%.
)
