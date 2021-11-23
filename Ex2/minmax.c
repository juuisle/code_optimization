#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void minmax(int a[], int b[], int n)
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] > b[i])
        {
            int tmp = a[i];
            a[i] = b[i];
            b[i] = tmp;
        }
    }
}

void print_array(int *a, const int size)
{
    for (int i = 0; i < size; i++)
        printf("%d ", a[i]);
    printf("\n");
}

int main(int argc, char** argv)
{
    int size = 10;
    // Check if an array size is given on the command line
    if (argc > 1)
      size = atoi(argv[1]);

    // Allocate integers arrays
    int* a = (int*) malloc(size * sizeof(int));
    int* b = (int*) malloc(size * sizeof(int));
    // Fill arrays
    for (int i = 0; i < size; i++)
    {
        a[i] = i;
        b[i] = size-i-1;
    }

    // Print initialized arrays
    printf("Initial array A:\n");
    print_array(a, size);
    printf("Initial array B:\n");
    print_array(b, size);

    // Sort them
    minmax(a, b, size);

    // Print result
    printf("After sort array A:\n");
    print_array(a, size);
    printf("After sort array B:\n");
    print_array(b, size);

    // Free arrays
    free(a); free(b);
    exit(0);
}
