#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "matrix.h"

/* Maximum random value for matrix elements when filling matrices with random values */
#define MaxRandVal 9

/******************************************
 * Matrix allocation, initialization and utility functions
 *******************************************/
/**
 * Allocates memory for a square matrix
 * @param side  Side length of the square matrix
 * @return      Pointer to the allocated matrix
 */
struct Matrix allocMatrix(int side) {
    struct Matrix mat;
    mat.matrix = malloc(sizeof(int) * side * side);  /* Allocate memory for matrix elements */
    mat.row = side;
    mat.col = side;
    return mat;
}

/** 
 * Frees the memory allocated for a matrix
 * @param mat   Matrix to free
 */
void freeMatrix(struct Matrix* mat) {
    if (mat->matrix != NULL) {
        free(mat->matrix);        /* Free the allocated memory */
        mat->matrix = NULL;       /* Set pointer to NULL to avoid dangling pointer */
    }
    mat->row = 0;                 /* Reset dimensions */
    mat->col = 0;
}

/**
 * Fills a matrix with random values between 0 and MaxRandVal
 * @param mat   Matrix to fill
 */
void fillMatrixRand(struct Matrix* mat) {
    for (int i = 0; i < mat->row; i++) {
        for (int j = 0; j < mat->col; j++) {
            /* Use macro to access elements in the 1D array as if it were 2D */
            matrixElem(mat->matrix, i, j, mat->col) = rand() % (MaxRandVal + 1);
        }
    }
}

/**
 * Initializes a matrix with zeros
 * @param mat   Matrix to initialize
 */
void initMatrixZeros(struct Matrix* mat) {
    for (int i = 0; i < mat->row; i++) {
        for (int j = 0; j < mat->col; j++) {
            matrixElem(mat->matrix, i, j, mat->col) = 0;
        }
    }
}

/**
 * Prints a matrix to standard output
 * Shows row and column indices along with each element value
 * @param mat   Matrix to print
 */
void printMatrix(struct Matrix* mat) {
    for (int i = 0; i < mat->row; i++) {
        for (int j = 0; j < mat->col; j++) {
            printf("(%d,%d): %4d  ", i, j, matrixElem(mat->matrix, i, j, mat->col));
        }
        printf("\n");
    }
}

/**
 * Calculates the next power of two greater than or equal to n
 * Used for padding matrices to dimensions that work well with Strassen algorithm
 * @param n     Input value
 * @return      Next power of two
 */
int nextPowerOfTwo(int n) {
    if (n <= 0) return 1;  /* Handle edge case */
    
    if (!(n & (n - 1))) return n;  /* Already a power of 2 */
    
    /* Calculate the ceiling of log2(n) and then calculate 2^ceiling */
    return 1 << ((int)ceil(log2(n)));
}

/**********************************************
 * Helper functions for matrix operations
 * These functions operate on submatrices (blocks) of larger matrices
 **********************************************/

/**
 * Adds two submatrices A and B and stores the result in C
 * C[rowC:rowC+blockSize][colC:colC+blockSize] = 
 *   A[rowA:rowA+blockSize][colA:colA+blockSize] + 
 *   B[rowB:rowB+blockSize][colB:colB+blockSize]
 *
 * @param A          First source matrix
 * @param rowA       Starting row in A
 * @param colA       Starting column in A
 * @param B          Second source matrix
 * @param rowB       Starting row in B
 * @param colB       Starting column in B
 * @param C          Destination matrix
 * @param rowC       Starting row in C
 * @param colC       Starting column in C
 * @param blockSize  Size of the block to operate on
 * @return           0 on success
 */
int sumMatrix(struct Matrix* A, int rowA, int colA, 
              struct Matrix* B, int rowB, int colB,
              struct Matrix* C, int rowC, int colC,
              int blockSize) {
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            /* Addition of corresponding elements from A and B, storing in C */
            matrixElem(C->matrix, i + rowC, j + colC, C->col) =
                matrixElem(A->matrix, i + rowA, j + colA, A->col) +
                matrixElem(B->matrix, i + rowB, j + colB, B->col);
        }
    }
    return 0;
}

/**
 * Subtracts submatrix B from A and stores the result in C
 * C[rowC:rowC+blockSize][colC:colC+blockSize] = 
 *   A[rowA:rowA+blockSize][colA:colA+blockSize] - 
 *   B[rowB:rowB+blockSize][colB:colB+blockSize]
 *
 * @param A          First source matrix
 * @param rowA       Starting row in A
 * @param colA       Starting column in A
 * @param B          Second source matrix
 * @param rowB       Starting row in B
 * @param colB       Starting column in B
 * @param C          Destination matrix
 * @param rowC       Starting row in C
 * @param colC       Starting column in C
 * @param blockSize  Size of the block to operate on
 * @return           0 on success
 */
int subMatrix(struct Matrix* A, int rowA, int colA, 
              struct Matrix* B, int rowB, int colB,
              struct Matrix* C, int rowC, int colC,
              int blockSize) {
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            /* Subtraction of B from A, storing in C */
            matrixElem(C->matrix, i + rowC, j + colC, C->col) =
                matrixElem(A->matrix, i + rowA, j + colA, A->col) -
                matrixElem(B->matrix, i + rowB, j + colB, B->col);
        }
    }
    return 0;
}

/**
 * Adds a submatrix A to a submatrix of B (in-place operation)
 * B[rowB:rowB+blockSize][colB:colB+blockSize] += 
 *   A[0:blockSize][0:blockSize]
 *
 * @param A          Source matrix
 * @param B          Destination matrix (modified in-place)
 * @param rowB       Starting row in B
 * @param colB       Starting column in B
 * @param blockSize  Size of the block to operate on
 * @return           0 on success
 */
int addSubmatrix(struct Matrix* A, struct Matrix* B, int rowB, int colB, int blockSize) {
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            /* Add values from A to B in-place */
            matrixElem(B->matrix, i + rowB, j + colB, B->col) += 
                matrixElem(A->matrix, i, j, A->col);
        }
    }
    return 0;
}

/**
 * Subtracts a submatrix A from a submatrix of B (in-place operation)
 * B[rowB:rowB+blockSize][colB:colB+blockSize] -= 
 *   A[0:blockSize][0:blockSize]
 *
 * @param A          Source matrix
 * @param B          Destination matrix (modified in-place)
 * @param rowB       Starting row in B
 * @param colB       Starting column in B
 * @param blockSize  Size of the block to operate on
 * @return           0 on success
 */
int subSubmatrix(struct Matrix* A, struct Matrix* B, int rowB, int colB, int blockSize) {
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            /* Subtract values from A from B in-place */
            matrixElem(B->matrix, i + rowB, j + colB, B->col) -= 
                matrixElem(A->matrix, i, j, A->col);
        }
    }
    return 0;
}

/**
 * Copies a submatrix from A to C
 * C[rowC:rowC+blockSize][colC:colC+blockSize] = 
 *   A[rowA:rowA+blockSize][colA:colA+blockSize]
 *
 * @param A          Source matrix
 * @param rowA       Starting row in A
 * @param colA       Starting column in A
 * @param C          Destination matrix
 * @param rowC       Starting row in C
 * @param colC       Starting column in C
 * @param blockSize  Size of the block to operate on
 * @return           0 on success
 */
int copySubmatrix(struct Matrix* A, int rowA, int colA,
                  struct Matrix* C, int rowC, int colC,
                  int blockSize) {
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            /* Copy values from A to C */
            matrixElem(C->matrix, i + rowC, j + colC, C->col) = 
                matrixElem(A->matrix, i + rowA, j + colA, A->col);
        }
    }
    return 0;
}

/**
 * Standard matrix multiplication algorithm (O(n³))
 * Computes C = A * B
 *
 * @param A      First input matrix
 * @param B      Second input matrix
 * @param C      Output matrix
 * @return       Pointer to the result matrix C
 */
struct Matrix* mul(struct Matrix* A, struct Matrix* B, struct Matrix* C){
    /* For each element in the result matrix */
    for(int i = 0; i < A->row; i++){
        for(int j = 0; j < A->col; j++){
            /* Initialize with first multiplication */
            matrixElem(C->matrix, i, j, C->row) = matrixElem(A->matrix, i, 0, A->row) * matrixElem(B->matrix, 0, j, B->row);
            
            /* Add remaining products for this cell */
            for(int k = 1; k < A->row; k++){
                matrixElem(C->matrix, i, j, C->row) = matrixElem(C->matrix, i, j, C->row) + 
                    matrixElem(A->matrix, i, k, A->row) * matrixElem(B->matrix, k, j, B->row);
            }    
        }
    }
    
    return C;
}    

/*********************************************
 * Strassen algorithm with 3 temporary matrices
 *
 * The algorithm computes the product C = A * B using
 * 7 recursive multiplications instead of 8 in the naive approach
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
 * This algorithm works with squared matrices 
 *********************************************/

/**
 * Performs Strassen's matrix multiplication algorithm
 * Computes C = A * B with time complexity O(n^log₂7) ≈ O(n^2.81)
 *
 * @param A      First input matrix
 * @param B      Second input matrix
 * @param C      Output matrix
 * @return       Pointer to the result matrix C
 */
struct Matrix* strassenMul(struct Matrix* A, struct Matrix* B, struct Matrix* C) {
    /* Base case for recursion - single element matrices */
    if (A->row == 1) {
        matrixElem(C->matrix, 0, 0, C->col) = 
            matrixElem(A->matrix, 0, 0, A->col) * matrixElem(B->matrix, 0, 0, B->col);
        return C;
    }

    /* Calculate new dimension for submatrices */
    int newSide = A->row / 2;

    /* Allocate temporary matrices for calculations */
    struct Matrix temp1 = allocMatrix(newSide);
    struct Matrix temp2 = allocMatrix(newSide);
    struct Matrix P = allocMatrix(newSide);

    /* Initialize result matrix with zeros */
    initMatrixZeros(C);

    /* 
     * Strassen's 7 recursive multiplications with corresponding additions/subtractions
     */
    
    /* P1 = (A12 - A22) * (B21 + B22) */
    subMatrix(A, 0, newSide, A, newSide, newSide, &temp1, 0, 0, newSide);
    sumMatrix(B, newSide, 0, B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C11 = P1 */
    copySubmatrix(&P, 0, 0, C, 0, 0, newSide);

    /* P2 = (A11 + A22) * (B11 + B22) */
    sumMatrix(A, 0, 0, A, newSide, newSide, &temp1, 0, 0, newSide);
    sumMatrix(B, 0, 0, B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C11 += P2, C22 = P2 */
    addSubmatrix(&P, C, 0, 0, newSide);
    copySubmatrix(&P, 0, 0, C, newSide, newSide, newSide);

    /* P3 = (A11 - A21) * (B11 + B12) */
    subMatrix(A, 0, 0, A, newSide, 0, &temp1, 0, 0, newSide);
    sumMatrix(B, 0, 0, B, 0, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C22 -= P3 */
    subSubmatrix(&P, C, newSide, newSide, newSide);

    /* P4 = (A11 + A12) * B22 */
    sumMatrix(A, 0, 0, A, 0, newSide, &temp1, 0, 0, newSide);
    copySubmatrix(B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C11 -= P4, C12 = P4 */
    subSubmatrix(&P, C, 0, 0, newSide);
    copySubmatrix(&P, 0, 0, C, 0, newSide, newSide);

    /* P5 = A11 * (B12 - B22) */
    copySubmatrix(A, 0, 0, &temp1, 0, 0, newSide);
    subMatrix(B, 0, newSide, B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C12 += P5, C22 += P5 */
    addSubmatrix(&P, C, 0, newSide, newSide);
    addSubmatrix(&P, C, newSide, newSide, newSide);

    /* P6 = A22 * (B21 - B11) */
    copySubmatrix(A, newSide, newSide, &temp1, 0, 0, newSide);
    subMatrix(B, newSide, 0, B, 0, 0, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C11 += P6, C21 = P6 */
    addSubmatrix(&P, C, 0, 0, newSide);
    copySubmatrix(&P, 0, 0, C, newSide, 0, newSide);

    /* P7 = (A21 + A22) * B11 */
    sumMatrix(A, newSide, 0, A, newSide, newSide, &temp1, 0, 0, newSide);
    copySubmatrix(B, 0, 0, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C21 += P7, C22 -= P7 */
    addSubmatrix(&P, C, newSide, 0, newSide);
    subSubmatrix(&P, C, newSide, newSide, newSide);

    /* Free temporary matrices to avoid memory leaks */
    freeMatrix(&temp1);
    freeMatrix(&temp2);
    freeMatrix(&P);

    return C;
}

/**
 * Hybrid Strassen's algorithm that switches to standard multiplication for small matrices
 * Uses Strassen for large matrices and standard multiplication when size <= cutoff
 * 
 * @param A          First input matrix
 * @param B          Second input matrix
 * @param C          Output matrix
 * @param cutoff     Size threshold to switch to standard multiplication
 * @return           Pointer to the result matrix C
 */
struct Matrix* strassenMul_hybrid(struct Matrix* A, struct Matrix* B, struct Matrix* C, int cutoff) {
    /* Base case: switch to standard multiplication when size <= cutoff */
    /*
     * Note on optimal cutoff value:
     * For theoretical complexity g(n_0) = (2*n_0 + 5)/(n_0^(log_2(7)-2)))
     * The minimum is at d(g(n_0))/d(n_0) = 0 -> approximately 10.48
     * Without padding we choose the first power before 10 or after 10
     * In this case we choose 8 since g(8) = 3.92 < g(16) = 3.94
     */
    
    /*
    if (A->row <= 8) {
        return mul(A,B,C);
    }
    */
    
    /* With padding we can choose g(10) = 3.89 */
    if (A->row <= cutoff) {
        return mul(A,B,C);
    }

    /* Calculate new dimension for submatrices */
    int newSide = A->row / 2;

    /* Allocate temporary matrices for calculations */
    struct Matrix temp1 = allocMatrix(newSide);
    struct Matrix temp2 = allocMatrix(newSide);
    struct Matrix P = allocMatrix(newSide);

    /* Initialize result matrix with zeros */
    initMatrixZeros(C);

    /* 
     * Strassen's 7 recursive multiplications with corresponding additions/subtractions
     * Same algorithm as strassenMul but using strassenMul_hybrid for recursion
     */
    
    /* P1 = (A12 - A22) * (B21 + B22) */
    subMatrix(A, 0, newSide, A, newSide, newSide, &temp1, 0, 0, newSide);
    sumMatrix(B, newSide, 0, B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C11 = P1 */
    copySubmatrix(&P, 0, 0, C, 0, 0, newSide);

    /* P2 = (A11 + A22) * (B11 + B22) */
    sumMatrix(A, 0, 0, A, newSide, newSide, &temp1, 0, 0, newSide);
    sumMatrix(B, 0, 0, B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C11 += P2, C22 = P2 */
    addSubmatrix(&P, C, 0, 0, newSide);
    copySubmatrix(&P, 0, 0, C, newSide, newSide, newSide);

    /* P3 = (A11 - A21) * (B11 + B12) */
    subMatrix(A, 0, 0, A, newSide, 0, &temp1, 0, 0, newSide);
    sumMatrix(B, 0, 0, B, 0, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C22 -= P3 */
    subSubmatrix(&P, C, newSide, newSide, newSide);

    /* P4 = (A11 + A12) * B22 */
    sumMatrix(A, 0, 0, A, 0, newSide, &temp1, 0, 0, newSide);
    copySubmatrix(B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C11 -= P4, C12 = P4 */
    subSubmatrix(&P, C, 0, 0, newSide);
    copySubmatrix(&P, 0, 0, C, 0, newSide, newSide);

    /* P5 = A11 * (B12 - B22) */
    copySubmatrix(A, 0, 0, &temp1, 0, 0, newSide);
    subMatrix(B, 0, newSide, B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C12 += P5, C22 += P5 */
    addSubmatrix(&P, C, 0, newSide, newSide);
    addSubmatrix(&P, C, newSide, newSide, newSide);

    /* P6 = A22 * (B21 - B11) */
    copySubmatrix(A, newSide, newSide, &temp1, 0, 0, newSide);
    subMatrix(B, newSide, 0, B, 0, 0, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C11 += P6, C21 = P6 */
    addSubmatrix(&P, C, 0, 0, newSide);
    copySubmatrix(&P, 0, 0, C, newSide, 0, newSide);

    /* P7 = (A21 + A22) * B11 */
    sumMatrix(A, newSide, 0, A, newSide, newSide, &temp1, 0, 0, newSide);
    copySubmatrix(B, 0, 0, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);

    /* C21 += P7, C22 -= P7 */
    addSubmatrix(&P, C, newSide, 0, newSide);
    subSubmatrix(&P, C, newSide, newSide, newSide);

    /* Free temporary matrices to avoid memory leaks */
    freeMatrix(&temp1);
    freeMatrix(&temp2);
    freeMatrix(&P);

    return C;
}
