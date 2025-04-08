#!/bin/bash
set -e  # Exit immediately if any command fails

echo "Compiling with -pg and -O3..."
gcc -pg -O3 mul.c -lm ../matrix_operation/matrix.c -o mul || { echo "Compilation failed."; exit 1; }

mkdir -p performance
mkdir -p analysis

echo "Matrix Size,Time (seconds)" > performance/performance.csv

for power in {2..12}; do
    size=$((2 ** power))
    echo "Running test for size ${size}x${size}..."

    rm -f gmon.out

    output=$(./mul "$size")
    echo "$output" >> performance/performance.csv

    if [[ -f gmon.out ]]; then
        gprof mul gmon.out > "analysis/analysis_${size}.txt"
        echo "Profiling saved to analysis/analysis_${size}.txt"
    else
        echo "⚠️  gmon.out not generated for size $size"
    fi

    echo
done

echo "✅ All tests and profiling completed."
read -p "Display a graph of the benchmark data? (Y/n): " response

if [[ "$response" =~ ^[Yy]$ || "$response" == "" ]]; then
    echo "Executing the Python script..."
    python3 plot.py
else
    echo "The graph display will not be executed."
fi
