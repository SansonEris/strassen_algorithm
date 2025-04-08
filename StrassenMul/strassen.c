#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../matrix_operation/matrix.h"

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

    printf("%d,%f\n", originalSide, timeTaken);

    freeMatrix(&A);
    freeMatrix(&B);
    freeMatrix(&C);

    return 0;
}
