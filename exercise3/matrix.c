#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000


int main()
{
    int a[N][N];
    int b[N][N];
    int matrix_c_case_1[N][N];
    int matrix_c_case_2[N][N];
    int matrix_c_case_3[N][N];

    srand(time(NULL));
    // Generate random matrix a
    for(int i = 0; i<N ; i++)
        for(int k = 0; k<N; k++)
            a[i][k] = rand();

    // Generate random matrix b
    for(int i = 0; i<N; i++)
        for(int k = 0; k<N; k++)
            b[i][k] = rand();
    

    // Matrix mutiplication for (i, j, k) orders
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            double sum = 0.0;
            for (int k=0; k<N;k++) {
                sum += a[i][k] * b[k][j];
                matrix_c_case_1[i][j] = sum;
            }
        }
    }
    // Matrix mutiplication for (i, k, j) orders
    for (int i=0; i<N; i++) {
        for (int k=0; k<N; k++) {
            double r = a[i][k];
            for (int j=0; j<N; j++) {
                matrix_c_case_2[i][j] += r*b[k][j];
            }
        }
    }

    // Matrix mutiplication for (j, k, i) orders
    for (int j=0; j<N; j++) {
        for (int k=0; k<N; k++) {
            double r = b[k][j];
            for (int i=0; i<N;i++) {
                matrix_c_case_3[i][j] += a[i][k]*r;
            }
        }
    }    

   return 0;
}