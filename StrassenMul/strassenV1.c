#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MaxRandVal 9 
#define matrixElem(matrix, r_index, c_index, side) matrix[(r_index) * (side) + (c_index)]

/******************************************
//  Funzioni di allocazione, free, print, fill
 *******************************************/

int* alloc_matrix(int side) {
	int* matrix = malloc(sizeof(int) * side * side);
	return matrix;
}


void fill_matrix_rand(int* matrix, int side) {
	for (int i = 0; i < side; i++){
		for (int j = 0; j < side; j++){
			matrixElem(matrix, i, j, side) = rand() % (MaxRandVal + 1);
		}
	}
}

void print_matrix(int* matrix, int side) {
	for (int i = 0; i < side; i++){
		for (int j = 0; j < side; j++){
			printf("(%d,%d): %4d  ", i, j, matrixElem(matrix, i, j, side));
		}
		printf("\n");
	}
}



/**********************************************
  Funzioni helper per operazioni sulle matrici
  I parametri sono:
  - block_size: numero di elementi da iterare (lato del blocco)
  - side_source: la lunghezza del lato della matrice da cui si legge
  - side_dest:   la lunghezza del lato della matrice in cui si scrive
 **********************************************/

// Somma: C = A + B
int sum_matrix(int* A, int* B, int* C,int rowA, int colA, int rowB, int colB, int rowC, int colC,int block_size, int side_source, int side_dest) {
	for (int i = 0; i < block_size; i++){
		for (int j = 0; j < block_size; j++){
			matrixElem(C, i + rowC, j + colC, side_dest) =
				matrixElem(A, i + rowA, j + colA, side_source) +
				matrixElem(B, i + rowB, j + colB, side_source);
		}
	}
	return 0;
}

// Sottrazione: C = A - B
int sub_matrix(int* A, int* B, int* C,int rowA, int colA,int rowB, int colB,int rowC, int colC,int block_size, int side_source, int side_dest) {
	for (int i = 0; i < block_size; i++){
		for (int j = 0; j < block_size; j++){
			matrixElem(C, i + rowC, j + colC, side_dest) =
				matrixElem(A, i + rowA, j + colA, side_source) -
				matrixElem(B, i + rowB, j + colB, side_source);
		}
	}
	return 0;
}

// Aggiunge (in-place) la submatrice A a B
int add_submatrix(int* A, int* B,int rowB, int colB,int block_size, int side_A, int side_B) {
	for (int i = 0; i < block_size; i++){
		for (int j = 0; j < block_size; j++){
			matrixElem(B, i + rowB, j + colB, side_B) += matrixElem(A, i, j, side_A);
		}
	}
	return 0;
}

// Sottrae (in-place) la submatrice A da B
int sub_submatrix(int* A, int* B,int rowB, int colB,int block_size, int side_A, int side_B) {
	for (int i = 0; i < block_size; i++){
		for (int j = 0; j < block_size; j++){
			matrixElem(B, i + rowB, j + colB, side_B) -= matrixElem(A, i, j, side_A);
		}
	}
	return 0;
}

// Copia una submatrice da A in C
int copy_submatrix(int* A, int rowA, int colA,int* C, int rowC, int colC,int block_size, int side_A, int side_C) {
	for (int i = 0; i < block_size; i++){
		for (int j = 0; j < block_size; j++){
			matrixElem(C, i + rowC, j + colC, side_C) = matrixElem(A, i + rowA, j + colA, side_A);
		}
	}
	return 0;
}

void init_matrix_zeros(int* C, int side){
	for (int i = 0; i < side; i++) {
		for (int j = 0; j < side; j++) {
			matrixElem(C, i, j, side) = 0;
		}
	}
}

//funzione per aggiungere righe e colonne in caso la matrice 
//non fosse di taglia 2^n 
// Calcola la potenza di 2 successiva più vicina a m
int next_power_of_two(int n) {
	if (n <= 0) return 1;

	if (!(n & (n - 1))) return n;

	// shift a sinistra equivale a fare 2^ceil(log2(m))
	return 1 << ((int)ceil(log2(n)));
}

/*********************************************
  Strassen algorithm with 3 temporary matrix 

  A1 = (A12 − A22 )
  A2 = (A11 + A22 )
  A3 = (A11 − A21 )
  A4 = (A11 + A12 )
  A5 = A11
  A6 = A22
  A7 = (A21 + A22 )

  B1 = (B21 + B22 )
  B2 = (B11 + B22 )
  B3 = (B11 + B12 )
  B4 = B22
  B5 = (B12 − B22 )
  B6 = (B21 − B11 )
  B7 = B11 

  C11 = P1 + P2 − P4 + P6
  C12 = P4 + P5
  C21 = P6 + P7
  C22 = P2 − P3 + P5 − P7

  this algorithm works with squered matrix only
 *********************************************/

int* strassen_mul(int* A, int* B, int* C, int side) {
	if (side == 1) {
		matrixElem(C, 0, 0, side) = matrixElem(A, 0, 0, side) * matrixElem(B, 0, 0, side);
		return C;
	}

	int new_side = side / 2;

	int* temp1 = alloc_matrix(new_side);
	int* temp2 = alloc_matrix(new_side);
	int* P     = alloc_matrix(new_side);

	init_matrix_zeros(C,side);

	//P1 = (A12 - A22) * (B21 + B22)
	sub_matrix(A, A, temp1, 0, new_side, new_side, new_side, 0, 0, new_side, side, new_side);
	sum_matrix(B, B, temp2, new_side, 0, new_side, new_side, 0, 0, new_side, side, new_side);
	strassen_mul(temp1, temp2, P, new_side);
	printf("\nP1 (A12 - A22) * (B21 + B22):\n");
	print_matrix(P, new_side);

	// C11 = P1
	copy_submatrix(P, 0, 0, C, 0, 0, new_side, new_side, side);
	printf("\nC after P1 update (C11 = P1):\n");
	print_matrix(C, side);

	//P2 = (A11 + A22) * (B11 + B22)
	sum_matrix(A, A, temp1, 0, 0, new_side, new_side, 0, 0, new_side, side, new_side);
	sum_matrix(B, B, temp2, 0, 0, new_side, new_side, 0, 0, new_side, side, new_side);
	strassen_mul(temp1, temp2, P, new_side);
	printf("\nP2 (A11 + A22) * (B11 + B22):\n");
	print_matrix(P, new_side);

	// C11 += P2, C22 = P2
	add_submatrix(P, C, 0, 0, new_side, new_side, side);
	copy_submatrix(P, 0, 0, C, new_side, new_side, new_side, new_side, side);
	printf("\nC after P2 update (C11 += P2, C22 = P2):\n");
	print_matrix(C, side);

	// P3 = (A11 - A21) * (B11 + B12)
	sub_matrix(A, A, temp1, 0, 0, new_side, 0, 0, 0, new_side, side, new_side);
	sum_matrix(B, B, temp2, 0, 0, 0, new_side, 0, 0, new_side, side, new_side);
	strassen_mul(temp1, temp2, P, new_side);
	printf("\nP3 = (A11 - A21) * (B11 + B12):\n");
	print_matrix(P, new_side);

	// C22 -= P3 
	sub_submatrix(P, C, new_side, new_side, new_side, new_side, side);
	printf("\nC after P3 update (C22 -= P3):\n");
	print_matrix(C, side);

	// P4 = (A11 + A12) * B22
	sum_matrix(A, A, temp1, 0, 0, 0, new_side, 0, 0, new_side, side, new_side);
	copy_submatrix(B, new_side, new_side, temp2, 0, 0, new_side, side, new_side);
	strassen_mul(temp1, temp2, P, new_side);
	printf("\nP4 = (A11 + A21) * B22:\n");
	print_matrix(P, new_side);

	// C11 -= P4, C12 = P4
	sub_submatrix(P, C, 0, 0, new_side, new_side, side);
	copy_submatrix(P, 0, 0, C, 0, new_side, new_side, new_side, side);
	printf("\nC after P4 update (C11 -= P4, C12 = P4):\n");
	print_matrix(C, side);

	// P5 = A11 * (B12 - B22)
	copy_submatrix(A, 0, 0, temp1, 0, 0, new_side, side, new_side);
	sub_matrix(B, B, temp2, 0, new_side, new_side, new_side, 0, 0, new_side, side, new_side);
	strassen_mul(temp1, temp2, P, new_side);
	printf("\nP5 A11 * (B12 - B22):\n");
	print_matrix(P, new_side);

	// C12 += P5, C22 += P5
	add_submatrix(P, C, 0, new_side, new_side, new_side, side);
	add_submatrix(P, C, new_side, new_side, new_side, new_side, side);
	printf("\nC after P5 update (C12 += P5, C22 += P5):\n");
	print_matrix(C, side);

	// P6 = A22 * (B21 - B11)
	copy_submatrix(A, new_side, new_side, temp1, 0, 0, new_side, side, new_side);
	sub_matrix(B, B, temp2, new_side, 0, 0, 0, 0, 0, new_side, side, new_side);
	strassen_mul(temp1, temp2, P, new_side);
	printf("\nP6 A22 * (B21 - B11):\n");
	print_matrix(P, new_side);

	// C11 += P6, C21 = P6
	add_submatrix(P, C, 0, 0, new_side, new_side, side);
	copy_submatrix(P, 0, 0, C, new_side, 0, new_side, new_side, side);
	printf("\nC after P6 update (C11 += P6, C21 = P6):\n");
	print_matrix(C, side);

	// P7 = (A21 + A22) * B11
	sum_matrix(A, A, temp1, new_side, 0, new_side, new_side, 0, 0, new_side, side, new_side);
	copy_submatrix(B, 0, 0, temp2, 0, 0, new_side, side, new_side);
	strassen_mul(temp1, temp2, P, new_side);
	printf("\nP7 (A21 + A22) * B11:\n");
	print_matrix(P, new_side);

	// C21 += P7, C22 -= P7
	add_submatrix(P, C, new_side, 0, new_side, new_side, side);
	sub_submatrix(P, C, new_side, new_side, new_side, new_side, side);
	printf("\nC after P7 update (C21 += P7, C22 -= P7):\n");
	print_matrix(C, side);

	free(temp1);
	free(temp2);
	free(P);

	return C;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: %s <matrix_size>\n", argv[0]);
		return 1;
	}

	int original_side = atoi(argv[1]);
	int padded_side = next_power_of_two(original_side);

	printf("Testing with matrix size %d x %d...\n", original_side, original_side);

	printf("Original matrix size: %d x %d\n", original_side, original_side);
	printf("Padded matrix size: %d x %d\n", padded_side, padded_side);

	int* A = alloc_matrix(padded_side);
	int* B = alloc_matrix(padded_side);
	int* C = alloc_matrix(padded_side);

	if (!A || !B || !C) {
		fprintf(stderr, "Memory allocation failed for size %d\n", original_side);
		return 1;
	}

	for (int i = 0; i < padded_side; i++){
		for (int j = 0; j < padded_side; j++){
			if( i < original_side && j < original_side){
				matrixElem(A, i, j, padded_side) = 1;
				matrixElem(B, i, j, padded_side) = 1;
			} else {
				matrixElem(A, i, j, padded_side) = 0;
				matrixElem(B, i, j, padded_side) = 0;
			}
		}
	}

	printf("Matrix A:\n");
	print_matrix(A, padded_side);
	printf("\nMatrix B:\n");
	print_matrix(B, padded_side);

	clock_t t = clock();
	strassen_mul(A, B, C, padded_side);
	t = clock() - t;
	double time_taken = ((double)t) / CLOCKS_PER_SEC;

	printf("\nResult Matrix C = A * B (Strassen):\n");

	for (int i = 0; i < original_side; i++) {
		for (int j = 0; j < original_side; j++) {
			printf("(%d,%d): %4d  ", i, j, matrixElem(C, i, j, padded_side));
		}
		printf("\n");
	}

	printf("\n\nstrassen took %f seconds to execute\n", time_taken);

	free(A);
	free(B);
	free(C);

	return 0;
}
