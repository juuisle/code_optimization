#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000

int main()
{
    // Generate random matrix a
    int a[N][N];
    int b[N][N];
    int c[N][N];

    srand(time(NULL));
    for(int i = 0; i<N ; i++)
        for(int k = 0; k<N; k++)
            a[i][k] = rand();

    // Generate random matrix b
    for(int i = 0; i<N; i++)
        for(int k = 0; k<N; k++)
            b[i][k] = rand();
    
    // Matrix mutiplication for (i, k, j) orders
    for (int i=0; i<N; i++) {
        for (int k=0; k<N; k++) {
            double r = a[k][j];
            for (int j=0; j<N;j++) {
                c[i][j] += r*b[i][k];
            }
        }
    }

    // Matrix mutiplication for (j, k, i) orders
    for (int j=0; j<N; j++) {
        for (int k=0; k<N; k++) {
            double r = b[k][j];
            for (int i=0; i<N;i++) {
                c[i][j] += a[i][k]*r;
            }
        }
    }

   return 0;
}