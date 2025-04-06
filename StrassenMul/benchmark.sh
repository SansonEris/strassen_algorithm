#!/bin/bash
echo "Compiling with -pg and -O3..."
gcc -pg -O3 strassen.c -lm -o strassen

echo "Matrix Size,Time (seconds)" > performance.csv

for power in {2..12}; do
    size=$((2 ** power))
    echo "Running test for size ${size}x${size}..."

    rm -f gmon.out

    time=$(./strassen "$size")
    
    echo "${size},${time}" >> strassen_results.csv

    # gprof profiling
    if [[ -f gmon.out ]]; then
        gprof strassen gmon.out > "analysis_${size}.txt"
        echo "Profiling saved to analysis_${size}.txt"
    else
        echo "⚠️  gmon.out not generated for size $size"
    fi

    echo
done

echo "✅ All tests and profiling completed."
