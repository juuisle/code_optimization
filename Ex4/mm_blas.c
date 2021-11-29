
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cblas.h>   

/*
  - Load the module OpenBLAS: module load OpenBLAS
  - Compile with:  gcc -O3 mm_blas.c -o mm_blas -lopenblas
 */


/* Allocates a 2D matrix of size rows*cols as a contiguous block */
  double ** allocate_matrix_cont(int rows, int cols) {
    /* Allocate row pointers */
    double **M = (double **) malloc(rows*sizeof(double *));
    if (M==NULL) {
      printf("Could not allocate memory for row pointers\n");
      exit(0);
    }
    /* Allocate a contiguous block of rows*cols elements */
    double *Mb = (double *) malloc(rows*cols*sizeof(double));
    if (Mb==NULL) {
      printf("Could not allocate memory for matrix\n");
      exit(0);
    }
    /* Initialize pointers to rows in the matrix */
    for (int i=0; i<rows; i++) {
      M[i] = Mb + i*cols;
    }
    return M;
  }

/* Frees a 2D matrix */
  void free_matrix_cont(double **M) {
    free(M[0]);
    free(M);
  }

/* Writes a matrix M of size r*c to the file fn in readable format.
   Returns zero if the file couldn't be opened, otherwise 1       */
int fwrite_matrix(double **M, const int r, const int c, char *fn) {
  FILE *fp;
  /* Open the file */
  if ((fp=fopen(fn, "w")) == NULL) {
    printf("Couldn't open file %s\n", fn);
    return 0;
  }
  /* Write the matrix to the file in ASCII format */
  for (int i=0; i<r; i++) {
      for (int j=0; j<c; j++) {
	fprintf(fp, "%.1f ", M[i][j]);
      }
      fprintf(fp, "\n");
  }
  fclose(fp);  /* Close the file */
  return(1);
}

/* Prints out the first limit*limit elements of a 2D matrix */
  void print_matrix(double **M, const int rows, const int cols, const int limit) {
    int r = (rows<limit) ? rows : limit;
    int c = (cols<limit) ? cols : limit;
    for (int i=0; i<r; i++) {
      for (int j=0; j<c; j++) {
	printf("%3.1f ", M[i][j]);
      }
      printf("\n");
    }
  }

/*
  Multiply the matrices A and B and place the result in C.
  The matrices are of dimension r*c
*/
void matrixmult(double **A, double **B, double **C, const int r, const int c) {
  for (int i=0; i<r; i++)  {
    for (int j=0; j<c; j++) {
      double sum = 0.0;
      for (int k=0; k<c; k++) 
	sum += A[i][k]*B[k][j];
      C[i][j] = sum;
    }
  }
}



  /* BLAS xgemm calculates C=alpha*A * B + beta*C, where A, B and C are 2D matrices
     and alpha and beta are scalar values.

  void cblas_dgemm ( 
    const enum CBLAS_ORDER Order,       // CblasRowMajor or CblasColMajor  
    const enum CBLAS_TRANSPOSE TransA,  // CblasTrans, CblasNoTrans or CblasConjTrans
    const enum CBLAS_TRANSPOSE TransB,  // Same as above
    const int M,                        // Number of rows in matrix A
    const int N,                        // Number of columns in matrix B
    const int K,                        // Number of columns in matrix C and rows in B
    const SCALAR alpha,                 // alpha value
    const TYPE * A,                     // Matrix A
    const int lda,                      // Leading dimension of A (nr. of elements in a row)
    const TYPE * B,                     // Matrix B
    const int ldb,                      // Leading dimension of B (nr. of elements in a column)
    const SCALAR beta,                  // beta value
    TYPE * C,                           // Matrix C
   const int ldc)                       // Leading dimension of C (nr. of elements in a column)

   Source https://www.physicsforums.com/threads/understanding-blas-dgemm-in-c.543110/
  */

void matrixmult_BLAS(double **A, double **B, double **C, const int r, const int c) {
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, r, c, c, 1.0,
	      A[0], r, B[0], r, 0.0, C[0], r);
}


int main(int argc, char** argv) {
  
  int size = 1000;          // Default matrix size is 1000*1000

  // Check if a matrix size is given on the command line
  if (argc>1) {
    size = atoi(argv[1]);
    printf("Size is %d \n", size);
  }

  double **A = allocate_matrix_cont(size, size);
  double **B = allocate_matrix_cont(size, size);
  double **C = allocate_matrix_cont(size, size);
  
  printf("Allocated matrices of size %d\n\n", size);

  // Initilaize the matrices
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
	A[i][j] = (double)i;
	B[i][j] = (double)j;
	C[i][j] = 0.0;
    }
  }

  // Print how many threads and cores are used
  printf("Number of threads = %d\n", openblas_get_num_threads() );
  printf("Number of processors = %d\n", openblas_get_num_procs() );

  clock_t start1 = clock();    /* Start measuring time */
  // Do the matrix multiplication with BLAS dgemm
  matrixmult_BLAS(A, B, C, size, size);
  printf("Time for BLAS dgemm  %6.2f seconds\n\n", ((clock()-start1)/1000000.0));

  printf("Matrix C:\n");
  print_matrix(C, size, size, 10);

  // Write the result to a file
  fwrite_matrix(C, size, size, "result_dgemm.txt");

  free_matrix_cont(A); free_matrix_cont(B); free_matrix_cont(C);
  printf("Done\n");
  exit(0);
}
