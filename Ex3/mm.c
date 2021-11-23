
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
/* (i, j, k) order */
void matrixmult_ijk(double **A, double **B, double **C, const int N)
{
  for(int i = 0; i < N; i++)      // For each row
  {
    for(int j = 0; j < N; j++)    // For each column
    {
      double sum = 0;             // Init local sum
      for(int k = 0; k < N; k++)  // Add each A and B coefficients multiplication
        sum += A[i][k] * B[k][j];
      C[i][j] = sum;
    }
  }
}
/* (i, k, j) order */
void matrixmult_ikj(double **A, double **B, double **C, const int N)
{
  for(int i = 0; i < N; i++)
  {
    for(int k = 0; k < N; k++)
    {
      double r = A[i][k];
      for(int j = 0; j < N; j++)
        C[i][j] += r * B[k][j];
    }
  }
}
/* (j, k, i) order */
void matrixmult_jki(double **A, double **B, double **C, const int N)
{
  for(int j = 0; j < N; j++)
  {
    for(int k = 0; k < N; k++)
    {
      double r = B[k][j];
      for(int i = 0; i < N; i++)
        C[i][j] += A[i][k] * r;
    }
  }
}


void compareMatrices(double **M1, double **M2, size);
{
  int same = 1;
  for(int i = 0; i < N; i++)
    for(int j = 0; j < N; j++)
      same = (M1[i][j] != M2[i][j]) ? 0 : 1;

  if (same != 1)
    printf(" Different.\n");
  else
    printf(" Identical.\n");
}


int main(int argc, char** argv)
{
  int size = 1000;          // Default matrix size is 1000*1000
  clock_t start1, start2, start3;

  // Check if a matrix size is given on the command line
  if (argc > 1)
    size = atoi(argv[1]);

  // Allocate the matrices
  double **A = allocate_matrix(size, size);
  double **B = allocate_matrix(size, size);
  double **Cijk = allocate_matrix(size, size);
  double **Cikj = allocate_matrix(size, size);
  double **Cjki = allocate_matrix(size, size);
  printf("Allocated matrices of size %d\n\n", size);

  // Initialize the matrices with some values
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      A[i][j] = (double) i;
      B[i][j] = (double) j;
      C[i][j] = 0.0;
    }
  }

  // Test (i, j, k) order algorithm
  start1 = clock();                                   // Start measuring time
  matrixmult_ijk(A, B, Cijk, size);                   // Do the matrix multiplication
  float time = ((clock() - start1) / 1000000.0);      // Compute time
  printf("IJK: Time for matrix multiplication %6.2f seconds\n", time);
  fwrite_matrix(Cijk, 10, 10, "resultIJK.txt", time);

  // Test (i, k, j) order algorithm
  start2 = clock();                                   // Start measuring time
  matrixmult_ikj(A, B, Cikj, size);                   // Do the matrix multiplication
  time = ((clock() - start2) / 1000000.0);            // Compute time
  printf("IKJ: Time for matrix multiplication %6.2f seconds\n", time);
  fwrite_matrix(Cikj, 10, 10, "resultIKJ.txt", time);

  // Test (j, k, i) order algorithm
  start3 = clock();                                   // Start measuring time
  matrixmult_jki(A, B, Cjki, size);                   // Do the matrix multiplication
  time = ((clock() - start3) / 1000000.0);            // Compute time
  printf("JKI: Time for matrix multiplication %6.2f seconds\n", time);
  fwrite_matrix(Cjki, 10, 10, "resultJKI.txt", time);

  // Print shortened result
  printf("Matrix C:\n");
  print_matrix(Cijk, size, size, 10);

  // Verify results (need to do that once)
/*  printf("Compare IJK and IKJ matrices: ");
  compareMatrices(Cijk, Cikj, size);
  printf("Compare IJK and JKI matrices: ");
  compareMatrices(Cijk, Cjki, size);*/

  free_matrix(A); free_matrix(B); free_matrix(C);
  exit(0);
}
