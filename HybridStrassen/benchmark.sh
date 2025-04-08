#!/bin/bash
set -e  # Exit immediately if any command fails

# Check if cutoff parameter was provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <cutoff_value>"
    echo "  cutoff_value: Size threshold below which standard multiplication is used"
    exit 1
fi

CUTOFF=$1
echo "Using Strassen cutoff value: $CUTOFF"

echo "Compiling with -pg and -O3..."
gcc -pg -O3 hybrid_strassen.c -lm  ../matrix_operation/matrix.c -o hybrid || { echo "Compilation failed."; exit 1; }

mkdir -p performance
mkdir -p analysis

# Include cutoff value in the filename
PERFORMANCE_FILE="performance/performance_cutoff_${CUTOFF}.csv"
echo "Matrix Size,Time (seconds)" > "$PERFORMANCE_FILE"

for power in {2..12}; do
    size=$((2 ** power))
    echo "Running test for size ${size}x${size} with cutoff $CUTOFF"
    rm -f gmon.out
    output=$(./hybrid "$size" "$CUTOFF")
    echo "$output" >> "$PERFORMANCE_FILE"
    if [[ -f gmon.out ]]; then
        gprof hybrid gmon.out > "analysis/analysis_${size}_cutoff_${CUTOFF}.txt"
        echo "Profiling saved to analysis/analysis_${size}_cutoff_${CUTOFF}.txt"
    else
        echo "⚠️  gmon.out not generated for size $size"
    fi
    echo
done

echo "✅ All tests and profiling completed with cutoff value $CUTOFF."
read -p "Display a graph of the benchmark data? (Y/n): " response
if [[ "$response" =~ ^[Yy]$ || "$response" == "" ]]; then
    echo "Executing the Python script..."
    python3 plot.py "$PERFORMANCE_FILE" "$CUTOFF"
else
    echo "The graph display will not be executed."
fi
