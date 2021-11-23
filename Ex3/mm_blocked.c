
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

// Matrixmult_blocked order (i, j, k)
void matrixmult_blocked_ijk(double **A, double **B, double **C, const int N, const int blocksize)
{
  for (int i=0; i<N; i+=blocksize) 
  {
    for (int j=0; j<N; j+=blocksize)
    {
      for (int k=0; k<N; k+=blocksize)

        for (int ii=i; ii<i+blocksize; ii++)
        {
          for (int jj=j; jj<j+blocksize; jj++)
          {
            double sum = 0.0;
            for (int kk=k; kk<k+blocksize; kk++)
            {
              sum += A[ii][kk]*B[kk][jj];
            }
            C[ii][jj] += sum;
          }
        }
    }
  }
}

// Matrixmult_blocked order (i, k, j)
void matrixmult_blocked_ikj(double **A, double **B, double **C, const int N, const int blocksize)
{
  for (int i=0; i<N; i+=blocksize) 
  {
    for (int k=0; k<N; k+=blocksize)
    {
      for (int j=0; j<N; j+=blocksize)

        for (int ii=i; ii<i+blocksize; ii++)
        {
          for (int kk=k; kk<k+blocksize; kk++)
          {
            double r = A[ii][kk];
            for (int jj=j; jj<j+blocksize; jj++)
            {
              C[ii][jj] += r*B[kk][jj];
            }
          }
        }
    }
  }
}

// Matrixmult_blocked order (j, k, i)
void matrixmult_blocked_jki(double **A, double **B, double **C, const int N, const int blocksize)
{
  for (int j=0;  j<N;  j+=blocksize) 
  {
    for (int k=0; k<N; k+=blocksize)
    {
      for (int i=0; i<N; i+=blocksize)

        for (int jj=j; jj<j+blocksize; jj++)
        {
          for (int kk=k; kk<k+blocksize; kk++)
          {
            double r = B[kk][jj];
            for (int ii=i; ii<i+blocksize; ii++)
            {
              C[ii][jj] += A[ii][kk] * r;
            }
          }
        }
    }
  }
}

void matrixreset(double **M, const int N)
{
  for(int i = 0; i < N; i++)
    for(int j = 0; j < N; j++)
      M[i][j] = 0;
}


int main(int argc, char** argv)
{
  int size = 4000;          // Default matrix size is 1000*1000
  int blocksize = 40;
  clock_t start1, start2, start3;

  // Check if a matrix size is given on the command line
  if (argc > 1)
    size = atoi(argv[1]);

  // Allocate the matrices
  double **A = allocate_matrix(size, size);
  double **B = allocate_matrix(size, size);
  double **C = allocate_matrix(size, size);
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

  // Test matrix multiplication blocked algorithm (i, j, k)
  start1 = clock();                                   // Start measuring time
  matrixmult_blocked_ijk(A, B, C, size, blocksize);       // Do the matrix multiplication
  float time = ((clock() - start1) / 1000000.0);      // Compute time
  printf("IJK Blocked: Time for matrix multiplication %6.2f seconds\n", time);
  // Reset matrix C
  matrixreset(C, size);

  // Test (i, k, j) order algorithm
  start2 = clock();                                   // Start measuring time
  matrixmult_blocked_ikj(A, B, C, size, blocksize);                      // Do the matrix multiplication
  time = ((clock() - start2) / 1000000.0);      // Compute time
  printf("IKJ Blocked: Time for matrix multiplication %6.2f seconds\n", time);
  // Reset matrix C
  matrixreset(C, size);

  // Test (j, k, i) order algorithm
  start3 = clock();                                   // Start measuring time
  matrixmult_blocked_jki(A, B, C, size, blocksize);                      // Do the matrix multiplication
  time = ((clock() - start3) / 1000000.0);      // Compute time
  printf("JKI Blocked: Time for matrix multiplication %6.2f seconds\n", time);

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
