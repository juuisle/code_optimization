
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
  - Load the module gcc: module load gcc
  - Compile with:  gcc -O3 -o mm
*/


/* Allocates a 2D matrix of size rows*cols as a contiguous block */
double ** allocate_matrix(int rows, int cols)
{
  /* Allocate row pointers */
  double **M = (double **) malloc(rows*sizeof(double *));
  if (M == NULL)
  {
    printf("Could not allocate memory for row pointers\n");
    exit(0);
  }
  /* Allocate a contiguous block of rows*cols elements */
  double *Mb = (double *) malloc(rows*cols*sizeof(double));
  if (Mb == NULL)
  {
    printf("Could not allocate memory for matrix\n");
    exit(0);
  }
  /* Initialize pointers to rows in the matrix */
  for (int i = 0; i < rows; i++)
    M[i] = Mb + i*cols;

  return M;
}

/* Frees a 2D matrix */
void free_matrix(double **M)
{
  free(M[0]);
  free(M);
}


/* Writes a matrix M of size r*c to the file fn in readable format.
   Returns zero if the file couldn't be opened, otherwise 1       */
int fwrite_matrix(double **M, const int rows, const int cols, char *fn, float time)
{
  FILE *fp;
  /* Open the file */
  if ((fp = fopen(fn, "w")) == NULL)
  {
    printf("Couldn't open file %s\n", fn);
    return 0;
  }
  /* Print time */
  fprintf(fp, "Time %6.2f \n", time);
  /* Write the matrix to the file in ASCII format */
  for (int i = 0; i < rows; i++)
  {
      for (int j = 0; j < cols; j++)
	     fprintf(fp, "%.1f ", M[i][j]);
      fprintf(fp, "\n");
  }
  fclose(fp);  /* Close the file */
  return(1);
}

/* Prints out the first limit*limit elements of a 2D matrix */
  void print_matrix(double **M, const int rows, const int cols, const int limit)
  {
    int r = (rows < limit) ? rows : limit;
    int c = (cols < limit) ? cols : limit;
    for (int i = 0; i < r; i++)
    {
      for (int j = 0; j < c; j++)
	      printf("%3.1f ", M[i][j]);
      printf("\n");
    }
  }


/*
  Multiply the matrices A and B and place the result in C.
  The matrices are assumed to be square of size N*N
*/
void matrixmult(double **A, double **B, double **C, const int N)
{
  // For each row
  for(int r = 0; r < N; r++)
  {
    // For each column
    for(int c = 0; c < N; c++)
    {
      // Init C coefficient
      C[r][c] = 0;
      // Add each A and B coefficients multiplication
      for(int k = 0; k < N; k++)
        C[r][c] += A[r][k] * B[k][c];
    }
  }
}


int main(int argc, char** argv)
{
  int size = 1000;          // Default matrix size is 1000*1000
  clock_t start1;

  // Check if a matrix size is given on the command line
  if (argc > 1)
    size = atoi(argv[1]);

  // Allocate the matrices
  double **A = allocate_matrix(size, size);
  double **B = allocate_matrix(size, size);
  double **C = allocate_matrix(size, size);
  
  printf("Allocated matrices of size %d\n\n", size);

  // Initilaize the matrices with some values
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
    	A[i][j] = (double) i;
    	B[i][j] = (double) j;
    	C[i][j] = 0.0;
    }
  }

  start1 = clock();    /* Start measuring time */
  // Do the matrix multiplication
  matrixmult(A, B, C, size);
  float time = ((clock() - start1) / 1000000.0);
  printf("Time for matrix multiplication %6.2f seconds\n", time);

  /*
  printf("Matrix A:\n");
  print_matrix(A, size, size, 10);

  printf("Matrix B:\n");
  print_matrix(B, size, size, 10);
  */

  printf("Matrix C:\n");
  print_matrix(C, size, size, 10);

  // Write the result to a file
  fwrite_matrix(C, size, size, "result.txt", time);

  free_matrix(A); free_matrix(B); free_matrix(C);
  exit(0);
}
