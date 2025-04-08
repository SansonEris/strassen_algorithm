#ifndef matrix_H_
#define matrix_H_ 

/**
 * Helper macro to access elements in a matrix
 * Matrix elements are stored in row-major order (C standard)
 * This macro converts 2D coordinates to the appropriate 1D array index
 * 
 * @param m         Pointer to matrix data
 * @param r_index   Row index (zero-based)
 * @param c_index   Column index (zero-based)
 * @param cols      Number of columns in the matrix
 * @return          The element at the specified position
 */
#define matrixElem(m, r_index, c_index, cols) m[(r_index) * (cols) + (c_index)]


/**
 * Matrix structure definition
 * Contains the matrix data as a 1D array and dimensions information
 */
struct Matrix {
    int* matrix;   /* 1D array to store matrix elements in row-major order */
    int row;       /* Number of rows in the matrix */
    int col;       /* Number of columns in the matrix */
};
/**
 * Allocates memory for a square matrix
 * Creates and initializes a new Matrix struct with allocated memory
 * 
 * @param side  Side length of the square matrix
 * @return      A newly allocated Matrix struct
 */
struct Matrix allocMatrix(int side);


/** 
 * Frees the memory allocated for a matrix
 * Releases both the data array and the Matrix struct itself
 * 
 * @param mat   Pointer to the Matrix to free
 */
void freeMatrix(struct Matrix* mat);

/******************************************
 * Matrix allocation, initialization and utility functions
 *******************************************/

/* Declaration of allocMatrix and freeMatrix is duplicated in the header */

/**
 * Fills a matrix with random values between 0 and MaxRandVal
 * MaxRandVal is assumed to be defined elsewhere
 * 
 * @param mat   Pointer to the Matrix to fill with random values
 */
void fillMatrixRand(struct Matrix* mat);

/**
 * Initializes a matrix with zeros
 * Sets all elements in the matrix to 0
 * 
 * @param mat   Pointer to the Matrix to initialize
 */
void initMatrixZeros(struct Matrix* mat);

/**
 * Prints a matrix to standard output
 * Format is not specified but typically shows elements in a grid
 * 
 * @param mat   Pointer to the Matrix to print
 */
void printMatrix(struct Matrix* mat);

/**
 * Calculates the next power of two greater than or equal to n
 * Used for padding matrices to dimensions suitable for Strassen's algorithm
 * 
 * @param n     Input value
 * @return      Next power of two (e.g., 5 -> 8, 8 -> 8, 9 -> 16)
 */
int nextPowerOfTwo(int n);

/**********************************************
 * Helper functions for matrix operations
 * These functions typically operate on submatrices (blocks)
 * of larger matrices to support divide-and-conquer algorithms
 **********************************************/

/**
 * Adds two submatrices A and B and stores the result in C
 * C[rowC:rowC+blockSize][colC:colC+blockSize] = 
 *   A[rowA:rowA+blockSize][colA:colA+blockSize] + 
 *   B[rowB:rowB+blockSize][colB:colB+blockSize]
 *
 * Used as a building block for Strassen's matrix multiplication
 *
 * @param A          First source matrix
 * @param rowA       Starting row index in A
 * @param colA       Starting column index in A
 * @param B          Second source matrix
 * @param rowB       Starting row index in B
 * @param colB       Starting column index in B
 * @param C          Destination matrix
 * @param rowC       Starting row index in C
 * @param colC       Starting column index in C
 * @param blockSize  Size of the square block to operate on
 * @return           0 on success, non-zero on failure
 */
int sumMatrix(struct Matrix* A, int rowA, int colA, 
              struct Matrix* B, int rowB, int colB,
              struct Matrix* C, int rowC, int colC,
              int blockSize); 

/**
 * Subtracts submatrix B from A and stores the result in C
 * C[rowC:rowC+blockSize][colC:colC+blockSize] = 
 *   A[rowA:rowA+blockSize][colA:colA+blockSize] - 
 *   B[rowB:rowB+blockSize][colB:colB+blockSize]
 *
 * Used as a building block for Strassen's matrix multiplication
 *
 * @param A          First source matrix
 * @param rowA       Starting row index in A
 * @param colA       Starting column index in A
 * @param B          Second source matrix
 * @param rowB       Starting row index in B
 * @param colB       Starting column index in B
 * @param C          Destination matrix
 * @param rowC       Starting row index in C
 * @param colC       Starting column index in C
 * @param blockSize  Size of the square block to operate on
 * @return           0 on success, non-zero on failure
 */
int subMatrix(struct Matrix* A, int rowA, int colA, 
              struct Matrix* B, int rowB, int colB,
              struct Matrix* C, int rowC, int colC,
              int blockSize);

/**
 * Adds a submatrix A to a submatrix of B (in-place operation)
 * B[rowB:rowB+blockSize][colB:colB+blockSize] += 
 *   A[0:blockSize][0:blockSize]
 *
 * Note: Assumes the source matrix A starts at index [0,0]
 * Used for constructing the final result in Strassen's algorithm
 *
 * @param A          Source matrix (entire matrix is used)
 * @param B          Destination matrix
 * @param rowB       Starting row index in B
 * @param colB       Starting column index in B
 * @param blockSize  Size of the square block to operate on
 * @return           0 on success, non-zero on failure
 */
int addSubmatrix(struct Matrix* A, struct Matrix* B, int rowB, int colB, int blockSize);

/**
 * Subtracts a submatrix A from a submatrix of B (in-place operation)
 * B[rowB:rowB+blockSize][colB:colB+blockSize] -= 
 *   A[0:blockSize][0:blockSize]
 *
 * Note: Assumes the source matrix A starts at index [0,0]
 * Used for constructing the final result in Strassen's algorithm
 *
 * @param A          Source matrix (entire matrix is used)
 * @param B          Destination matrix
 * @param rowB       Starting row index in B
 * @param colB       Starting column index in B
 * @param blockSize  Size of the square block to operate on
 * @return           0 on success, non-zero on failure
 */
int subSubmatrix(struct Matrix* A, struct Matrix* B, int rowB, int colB, int blockSize);

/**
 * Copies a submatrix from A to C
 * C[rowC:rowC+blockSize][colC:colC+blockSize] = 
 *   A[rowA:rowA+blockSize][colA:colA+blockSize]
 *
 * Used to extract submatrices for divide-and-conquer operations
 *
 * @param A          Source matrix
 * @param rowA       Starting row index in A
 * @param colA       Starting column index in A
 * @param C          Destination matrix
 * @param rowC       Starting row index in C
 * @param colC       Starting column index in C
 * @param blockSize  Size of the square block to operate on
 * @return           0 on success, non-zero on failure
 */
int copySubmatrix(struct Matrix* A, int rowA, int colA,
                  struct Matrix* C, int rowC, int colC,
                  int blockSize);

/**
 * Performs conventional matrix multiplication
 * Computes C = A * B using the standard O(n³) algorithm
 *
 * @param A      First input matrix
 * @param B      Second input matrix
 * @param C      Output matrix (must be pre-allocated)
 * @return       Pointer to the result matrix C
 */
struct Matrix* mul(struct Matrix* A, struct Matrix* B, struct Matrix* C);

/*********************************************
 * Strassen algorithm with 3 temporary matrices
 *
 * The algorithm computes the product C = A * B using
 * 7 recursive multiplications instead of 8 in the naive approach
 * Complexity: O(n^log₂7) ≈ O(n^2.81)
 *
 * The formulas used are:
 * P1 = (A12 − A22) * (B21 + B22)
 * P2 = (A11 + A22) * (B11 + B22)
 * P3 = (A11 − A21) * (B11 + B12)
 * P4 = (A11 + A12) * B22
 * P5 = A11 * (B12 − B22)
 * P6 = A22 * (B21 − B11)
 * P7 = (A21 + A22) * B11
 *
 * Then the result matrix C is computed as:
 * C11 = P1 + P2 − P4 + P6
 * C12 = P4 + P5
 * C21 = P6 + P7
 * C22 = P2 − P3 + P5 − P7
 *
 * This algorithm works with squared matrices of size 2^n
 * For other sizes, padding is required
 *********************************************/

/**
 * Performs Strassen's matrix multiplication algorithm
 * Computes C = A * B using the recursive divide-and-conquer approach
 *
 * Note: For optimal performance, matrices should have dimensions that are
 * powers of 2. If not, they are internally padded to the next power of 2.
 *
 * @param A      First input matrix
 * @param B      Second input matrix
 * @param C      Output matrix (must be pre-allocated)
 * @return       Pointer to the result matrix C
 */
struct Matrix* strassenMul(struct Matrix* A, struct Matrix* B, struct Matrix* C);

/**
 * Hybrid Strassen-conventional matrix multiplication
 * Uses Strassen's algorithm for large matrices and switches to
 * conventional multiplication below the cutoff threshold
 *
 * This often provides better performance as the overhead of 
 * Strassen's algorithm is not beneficial for small matrices
 *
 * @param A          First input matrix
 * @param B          Second input matrix
 * @param C          Output matrix (must be pre-allocated)
 * @param cutoff     Size threshold below which to use conventional multiplication
 * @return           Pointer to the result matrix C
 */
struct Matrix* strassenMul_hybrid(struct Matrix* A, struct Matrix* B, struct Matrix* C, int cutoff);

/**
 * Function to find the optimal cutoff value for hybrid Strassen algorithm
 * Tests performance with different cutoff values and returns the optimal cutoff
 * 
 * @param minCutoff       Minimum cutoff value to test
 * @param maxCutoff       Maximum cutoff value to test
 * @param step            Step size between cutoff values
 * @param originalSide    Original matrix size
 * @param paddedSide      Padded matrix size (power of 2)
 * @return                The cutoff value that produced the fastest execution time
 */
int findOptimalCutoff(int minCutoff, int maxCutoff, int step, int originalSide, int paddedSide);

#endif /* matrix_H_ */
