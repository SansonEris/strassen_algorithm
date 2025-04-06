# Strassen Algorithm Implementation with Three Temporary Matrices

This project implements the Strassen matrix multiplication algorithm using only three temporary matrices 
The algorithm handles square matrices of any size, including those that are not multiples of two, 
as it pads the matrices to the next power of two before performing the multiplication.

## Compilation

To compile the program, use the following command:

```bash
gcc -pg -O3 strassen.c -lm -o strassen
```

## Running the Program

To run the Strassen matrix multiplication, execute the following command:

```bash
./strassen <matrix_size>
```
Where <matrix_size> is the dimension (n x n) of the square matrices to multiply. For example, to multiply two 5x5 matrices, use 5.

## Running a Benchmark

To run a benchmark that measures the performance of the algorithm, use the following command:

```bash
./benchmark.sh
```
The benchmark will test matrix sizes from 2^0 to 2^12. 
If you want to modify the maximum matrix size, you can do so by changing the value in the benchmark.sh script.

## Performance Graph

Here is the performance graph showing the execution times of the Strassen algorithm for various matrix sizes:
![alt text](strassen_performance.png)
