#!/bin/bash
echo "Compiling with -pg and -O3..."
gcc -pg -O3 strassen.c -lm -o strassen

echo "Matrix Size,Time (seconds)" > performance/performance.csv

for power in {2..12}; do
    size=$((2 ** power))
    echo "Running test for size ${size}x${size}..."

    rm -f gmon.out

    time=$(./strassen "$size")
    
    echo "${size},${time}" >> performance/performance.csv

    # gprof profiling
    if [[ -f gmon.out ]]; then
        gprof strassen gmon.out > "analysis/analysis_${size}.txt"
        echo "Profiling saved to analysis/analysis_${size}.txt"
    else
        echo "⚠️  gmon.out not generated for size $size"
    fi

    echo
done

echo "✅ All tests and profiling completed."
read -p "display a graph of the benchmark data? (Y/n): " response

if [[ "$response" =~ ^[Yy]$ || "$response" == "" ]]; then
    echo "Executing the Python script..."
    python3 plot.py
else
    echo "The graph display will not be executed."
fi
