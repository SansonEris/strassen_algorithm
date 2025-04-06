#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MaxRandVal 9

/**
 * Structure to represent a matrix
 * Contains a pointer to the matrix data, number of rows and columns
 */
struct Matrix {
    int* matrix;
    int row;
    int col;
};

/**
 * Helper macro to access elements in a matrix
 * @param m         Pointer to matrix data
 * @param r_index   Row index
 * @param c_index   Column index
 * @param cols      Number of columns in the matrix
 * @return          The element at the specified position
 */
#define matrixElem(m, r_index, c_index, cols) m[(r_index) * (cols) + (c_index)]

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
    mat.matrix = malloc(sizeof(int) * side * side);
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
        free(mat->matrix);
        mat->matrix = NULL;
    }
    mat->row = 0;
    mat->col = 0;
}

/**
 * Fills a matrix with random values between 0 and MaxRandVal
 * @param mat   Matrix to fill
 */
void fillMatrixRand(struct Matrix* mat) {
    for (int i = 0; i < mat->row; i++) {
        for (int j = 0; j < mat->col; j++) {
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
 * @param n     Input value
 * @return      Next power of two
 */
int nextPowerOfTwo(int n) {
    if (n <= 0) return 1;
    
    if (!(n & (n - 1))) return n;  // Already a power of 2
    
    // Calculate the ceiling of log2(n) and then calculate 2^ceiling
    return 1 << ((int)ceil(log2(n)));
}

/**********************************************
 * Helper functions for matrix operations
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
 * @param B          Destination matrix
 * @param rowB       Starting row in B
 * @param colB       Starting column in B
 * @param blockSize  Size of the block to operate on
 * @return           0 on success
 */
int addSubmatrix(struct Matrix* A, struct Matrix* B, int rowB, int colB, int blockSize) {
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
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
 * @param B          Destination matrix
 * @param rowB       Starting row in B
 * @param colB       Starting column in B
 * @param blockSize  Size of the block to operate on
 * @return           0 on success
 */
int subSubmatrix(struct Matrix* A, struct Matrix* B, int rowB, int colB, int blockSize) {
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
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
            matrixElem(C->matrix, i + rowC, j + colC, C->col) = 
                matrixElem(A->matrix, i + rowA, j + colA, A->col);
        }
    }
    return 0;
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
 * Computes C = A * B
 *
 * @param A      First input matrix
 * @param B      Second input matrix
 * @param C      Output matrix
 * @return       Pointer to the result matrix C
 */
struct Matrix* strassenMul(struct Matrix* A, struct Matrix* B, struct Matrix* C) {
    // Base case for recursion
    if (A->row == 1) {
        matrixElem(C->matrix, 0, 0, C->col) = 
            matrixElem(A->matrix, 0, 0, A->col) * matrixElem(B->matrix, 0, 0, B->col);
        return C;
    }

    int newSide = A->row / 2;

    // Allocate temporary matrices
    struct Matrix temp1 = allocMatrix(newSide);
    struct Matrix temp2 = allocMatrix(newSide);
    struct Matrix P = allocMatrix(newSide);

    // Initialize result matrix with zeros
    initMatrixZeros(C);

    // P1 = (A12 - A22) * (B21 + B22)
    subMatrix(A, 0, newSide, A, newSide, newSide, &temp1, 0, 0, newSide);
    sumMatrix(B, newSide, 0, B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);
    //printf("\nP1 (A12 - A22) * (B21 + B22):\n");
    //printMatrix(&P);

    // C11 = P1
    copySubmatrix(&P, 0, 0, C, 0, 0, newSide);
    //printf("\nC after P1 update (C11 = P1):\n");
    //printMatrix(C);

    // P2 = (A11 + A22) * (B11 + B22)
    sumMatrix(A, 0, 0, A, newSide, newSide, &temp1, 0, 0, newSide);
    sumMatrix(B, 0, 0, B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);
    //printf("\nP2 (A11 + A22) * (B11 + B22):\n");
    //printMatrix(&P);

    // C11 += P2, C22 = P2
    addSubmatrix(&P, C, 0, 0, newSide);
    copySubmatrix(&P, 0, 0, C, newSide, newSide, newSide);
    //printf("\nC after P2 update (C11 += P2, C22 = P2):\n");
    //printMatrix(C);

    // P3 = (A11 - A21) * (B11 + B12)
    subMatrix(A, 0, 0, A, newSide, 0, &temp1, 0, 0, newSide);
    sumMatrix(B, 0, 0, B, 0, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);
    //printf("\nP3 = (A11 - A21) * (B11 + B12):\n");
    //printMatrix(&P);

    // C22 -= P3 
    subSubmatrix(&P, C, newSide, newSide, newSide);
    //printf("\nC after P3 update (C22 -= P3):\n");
    //printMatrix(C);

    // P4 = (A11 + A12) * B22
    sumMatrix(A, 0, 0, A, 0, newSide, &temp1, 0, 0, newSide);
    copySubmatrix(B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);
    //printf("\nP4 = (A11 + A12) * B22:\n");
    //printMatrix(&P);

    // C11 -= P4, C12 = P4
    subSubmatrix(&P, C, 0, 0, newSide);
    copySubmatrix(&P, 0, 0, C, 0, newSide, newSide);
    //printf("\nC after P4 update (C11 -= P4, C12 = P4):\n");
    //printMatrix(C);

    // P5 = A11 * (B12 - B22)
    copySubmatrix(A, 0, 0, &temp1, 0, 0, newSide);
    subMatrix(B, 0, newSide, B, newSide, newSide, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);
    //printf("\nP5 A11 * (B12 - B22):\n");
    //printMatrix(&P);

    // C12 += P5, C22 += P5
    addSubmatrix(&P, C, 0, newSide, newSide);
    addSubmatrix(&P, C, newSide, newSide, newSide);
    //printf("\nC after P5 update (C12 += P5, C22 += P5):\n");
    //printMatrix(C);

    // P6 = A22 * (B21 - B11)
    copySubmatrix(A, newSide, newSide, &temp1, 0, 0, newSide);
    subMatrix(B, newSide, 0, B, 0, 0, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);
    //printf("\nP6 A22 * (B21 - B11):\n");
    //printMatrix(&P);

    // C11 += P6, C21 = P6
    addSubmatrix(&P, C, 0, 0, newSide);
    copySubmatrix(&P, 0, 0, C, newSide, 0, newSide);
    //printf("\nC after P6 update (C11 += P6, C21 = P6):\n");
    //printMatrix(C);

    // P7 = (A21 + A22) * B11
    sumMatrix(A, newSide, 0, A, newSide, newSide, &temp1, 0, 0, newSide);
    copySubmatrix(B, 0, 0, &temp2, 0, 0, newSide);
    strassenMul(&temp1, &temp2, &P);
    //printf("\nP7 (A21 + A22) * B11:\n");
    //printMatrix(&P);

    // C21 += P7, C22 -= P7
    addSubmatrix(&P, C, newSide, 0, newSide);
    subSubmatrix(&P, C, newSide, newSide, newSide);
    //printf("\nC after P7 update (C21 += P7, C22 -= P7):\n");
    //printMatrix(C);

    // Free temporary matrices
    freeMatrix(&temp1);
    freeMatrix(&temp2);
    freeMatrix(&P);

    return C;
}

/**
 * Main function
 * @param argc  Number of command line arguments
 * @param argv  Array of command line arguments
 * @return      0 on success, 1 on failure
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        // printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }

    int originalSide = atoi(argv[1]);
    int paddedSide = nextPowerOfTwo(originalSide);

    // Commenta stampe informative
    /*
    printf("Testing with matrix size %d x %d...\n", originalSide, originalSide);
    printf("Original matrix size: %d x %d\n", originalSide, originalSide);
    printf("Padded matrix size: %d x %d\n", paddedSide, paddedSide);
    */

    struct Matrix A = allocMatrix(paddedSide);
    struct Matrix B = allocMatrix(paddedSide);
    struct Matrix C = allocMatrix(paddedSide);

    if (A.matrix == NULL || B.matrix == NULL || C.matrix == NULL) {
        // fprintf(stderr, "Memory allocation failed for size %d\n", originalSide);
        return 1;
    }

    for (int i = 0; i < paddedSide; i++) {
        for (int j = 0; j < paddedSide; j++) {
            if (i < originalSide && j < originalSide) {
                matrixElem(A.matrix, i, j, paddedSide) = 1;
                matrixElem(B.matrix, i, j, paddedSide) = 1;
            } else {
                matrixElem(A.matrix, i, j, paddedSide) = 0;
                matrixElem(B.matrix, i, j, paddedSide) = 0;
            }
        }
    }

    /*
    printf("Matrix A:\n");
    printMatrix(&A);
    printf("\nMatrix B:\n");
    printMatrix(&B);
    */

    clock_t t = clock();
    strassenMul(&A, &B, &C);
    t = clock() - t;
    double timeTaken = ((double)t) / CLOCKS_PER_SEC;

    /*
    printf("\n\nStrassen multiplication took %f seconds to execute\n", timeTaken);
    
    printf("\nResult Matrix C = A * B (Strassen):\n");
    for (int i = 0; i < originalSide; i++) {
        for (int j = 0; j < originalSide; j++) {
            printf("(%d,%d): %4d  ", i, j, matrixElem(C.matrix, i, j, paddedSide));
        }
        printf("\n");
    }
    */

    // 👉 Stampa compatibile con CSV: solo dimensione e tempo
    printf("%d,%f\n", originalSide, timeTaken);

    freeMatrix(&A);
    freeMatrix(&B);
    freeMatrix(&C);

    return 0;
}
