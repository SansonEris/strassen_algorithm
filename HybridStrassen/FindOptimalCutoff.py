#!/usr/bin/env python3
import subprocess
import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from concurrent.futures import ProcessPoolExecutor
import pandas as pd
import multiprocessing

# Configuration
C_PROGRAM = "hybrid"  # Name of your compiled C program
C_SOURCE = "hybrid_strassen.c"  # Your C source file
MATRIX_SIZES = [512, 1024, 2048] # Matrix sizes to test
MIN_CUTOFF = 8  # Minimum cutoff to test
MAX_CUTOFF = 64  # Maximum cutoff to test
CUTOFF_STEP = 4  # Step between cutoff values
TRIALS = 3  # Number of trials for each configuration

MAX_WORKERS = max(1, multiprocessing.cpu_count() - 1)

def compile_program():
    """Compile the C program"""
    print("Compiling C program...")
    try:
        # Removed the -fsanitize=address flag to reduce memory usage
        subprocess.run([
            "gcc", "-pg", "-O3", C_SOURCE, "-o", C_PROGRAM, 
            "-lm", "../matrix_operation/matrix.c", "-g"
        ], check=True)
        print("Compilation successful.")
    except subprocess.CalledProcessError:
        print("Compilation failed.")
        sys.exit(1)

def run_single_test(matrix_size, cutoff):
    """Run a single test with given matrix size and cutoff"""
    times = []
    for _ in range(TRIALS):
        try:
            # Added timeout to prevent hanging processes
            result = subprocess.run(
                [f"./{C_PROGRAM}", str(matrix_size), str(cutoff)],
                capture_output=True,
                text=True,
                check=True,
                timeout=300  # 5-minute timeout
            )
            # Parse the output (assuming it's in the format "size,time")
            output = result.stdout.strip()
            size, time_str = output.split(',')
            times.append(float(time_str))
        except (subprocess.CalledProcessError, ValueError, subprocess.TimeoutExpired) as e:
            print(f"Error running test with size={matrix_size}, cutoff={cutoff}: {e}")
            return matrix_size, cutoff, None
    
    avg_time = sum(times) / len(times)
    return matrix_size, cutoff, avg_time

def find_optimal_cutoff():
    """Test different cutoff values to find the optimal one for each matrix size"""
    # Make sure the program is compiled
    compile_program()
    
    results = []
    
    # Create a progress counter
    total_tests = len(MATRIX_SIZES) * ((MAX_CUTOFF - MIN_CUTOFF) // CUTOFF_STEP + 1)
    completed = 0
    
    print(f"Running {total_tests} tests across {len(MATRIX_SIZES)} matrix sizes with {MAX_WORKERS} workers...")
    
    # Run tests in parallel with limited workers
    with ProcessPoolExecutor(max_workers=MAX_WORKERS) as executor:
        futures = []
        # Test smaller matrices first to get quicker results
        for size in sorted(MATRIX_SIZES):
            for cutoff in range(MIN_CUTOFF, MAX_CUTOFF + 1, CUTOFF_STEP):
                futures.append(executor.submit(run_single_test, size, cutoff))
        
        # Process results as they complete
        for future in futures:
            size, cutoff, time = future.result()
            if time is not None:
                results.append((size, cutoff, time))
                # Save intermediate results in case of crash
                temp_df = pd.DataFrame(results, columns=['matrix_size', 'cutoff', 'time'])
                temp_df.to_csv('strassen_results_temp.csv', index=False)
            
            # Update progress
            completed += 1
            print(f"Progress: {completed}/{total_tests} tests completed", end="\r")
    
    print("\nAll tests completed.")
    return results

def analyze_results(results):
    """Analyze results to find optimal cutoff for each matrix size"""
    # Convert results to DataFrame for easier analysis
    df = pd.DataFrame(results, columns=['matrix_size', 'cutoff', 'time'])
    
    # Find optimal cutoff for each matrix size
    optimal_cutoffs = {}
    for size in MATRIX_SIZES:
        size_df = df[df['matrix_size'] == size]
        if not size_df.empty:
            best_row = size_df.loc[size_df['time'].idxmin()]
            optimal_cutoffs[size] = (int(best_row['cutoff']), best_row['time'])
    
    return df, optimal_cutoffs

def plot_results(df, optimal_cutoffs):
    """Plot the results"""
    # Create a plot for each matrix size
    fig, axes = plt.subplots(len(MATRIX_SIZES), 1, figsize=(10, 3*len(MATRIX_SIZES)))
    
    if len(MATRIX_SIZES) == 1:
        axes = [axes]  # Make it iterable when there's only one subplot
    
    for i, size in enumerate(MATRIX_SIZES):
        size_df = df[df['matrix_size'] == size]
        if size_df.empty:
            continue
            
        ax = axes[i]
        # Sort by cutoff for proper line graph
        size_df = size_df.sort_values('cutoff')
        ax.plot(size_df['cutoff'], size_df['time'], 'o-')
        
        # Highlight the optimal point
        if size in optimal_cutoffs:
            opt_cutoff, opt_time = optimal_cutoffs[size]
            ax.plot(opt_cutoff, opt_time, 'ro', markersize=10)
            ax.text(opt_cutoff, opt_time*1.05, 
                    f'Optimal: {opt_cutoff}\n({opt_time:.4f}s)',
                    horizontalalignment='center')
        
        ax.set_title(f'Matrix Size: {size}x{size}')
        ax.set_xlabel('Cutoff Value')
        ax.set_ylabel('Time (seconds)')
        ax.grid(True)
    
    plt.tight_layout()
    plt.savefig('optimal_cutoff_results.png')
    print("Results plot saved as 'optimal_cutoff_results.png'")
    plt.close()

def main():
    try:
        # If there's a temporary results file from a previous run, ask to use it
        if os.path.exists('strassen_results_temp.csv'):
            response = input("Found temporary results file. Use it instead of running tests again? (y/n): ")
            if response.lower() == 'y':
                print("Loading previous results...")
                df = pd.read_csv('strassen_results_temp.csv')
                results = [tuple(row) for row in df.values]
            else:
                results = find_optimal_cutoff()
        else:
            results = find_optimal_cutoff()
        
        # Analyze results
        df, optimal_cutoffs = analyze_results(results)
        
        # Print results
        print("\nOptimal cutoff values for each matrix size:")
        for size, (cutoff, time) in optimal_cutoffs.items():
            print(f"Matrix size {size}x{size}: Optimal cutoff = {cutoff} (Time: {time:.6f}s)")
        
        # Save results to CSV
        df.to_csv('strassen_results.csv', index=False)
        print("Detailed results saved to 'strassen_results.csv'")
        
        # Plot results
        plot_results(df, optimal_cutoffs)
        
        # Calculate a general recommendation
        optimal_values = [cutoff for cutoff, _ in optimal_cutoffs.values()]
        if optimal_values:
            avg_optimal = sum(optimal_values) / len(optimal_values)
            print(f"\nRecommended general cutoff value: {int(avg_optimal)}")
            
    except KeyboardInterrupt:
        print("\nProcess interrupted by user. Saving any available results...")
        if 'results' in locals() and results:
            temp_df = pd.DataFrame(results, columns=['matrix_size', 'cutoff', 'time'])
            temp_df.to_csv('strassen_results_interrupted.csv', index=False)
            print("Partial results saved to 'strassen_results_interrupted.csv'")

if __name__ == "__main__":
    main()
