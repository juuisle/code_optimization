# Code Optimization: Assignment N°2 Report

###### Duy Le - Cédric Léonard, 27/11/2021

### Introduction

This third assignment intends to discover the difference between **Matrix Multiplicaton Algorithms** and to understand the effect of memory access and cache organization.

### Question N°1

The first question was about implementing different matrix multiplication althgorims that has several nested for loops. The difference bewwen these algorithms was the orders of the matrix elements. The aim of the assignment was to understand how the order of access affect the performance. Some matrix access was done row-wise, while others done collum-wise. The results was suprissingly different due to how the cache memory was organized. Row-wise operations showed high performance thanks to optimized cache-hits while collumn-wise access leaded to cache-misses and as the result, has significantly poor performance compare to row-wise access algorithm.

###### (i, j, k) or (j, i, k) orders

In this most straight forward implementation, matrix $A$ accesses are done row-wise and matrix $B$ accesses are done column-wise. This leads to a cache miss for matrix $A$ every 8 accesses and a miss each iteration for matrix $B$ (except if the matrix size is $< 8$ which is unlikely).

As explained in the subject, this leads to a $1.125$ cache misses per iteration.

###### (k, i, j) or (i, k, j) orders

In this second implementation: access to elements in matrix $B$ are done row-wise, as similar to an example. Hence, for every 8 iteration, there will be a cache miss. Access to elements in matrix $C$ has the same cache miss as matrix $B$ since it's a row-wise operation. 
In total, there is $0.25$ cache miss in the innermost loop after each iteration.

###### (j, k, i) or (k, j, i) orders

In this last implementation access to the elements in matrix $A$ and $C$ are done column-wise, which will lead to a cache miss on every iteration. The total cache miss of each iteration in the innermost loop is $2$.

##### b. Implement and test the algorithms

|                    | $1000 \times 1000$ | $2000 \times 2000$ | $3000 \times 3000$ | $4000 \times 4000$ |
| ------------------ | ------------------ | ------------------ | ------------------ | ------------------ |
| (i, j, k) (No-opt) | 3.13               | 30.05              | 124.63             | 551.34             |
| (i, k, j) (No-opt) | 2.82               | 22.8               | 77.85              | 185.14             |
| (j, k, i) (No-opt) | 4.01               | 39.56              | 166.29             | 803.63             |
| (i, j, k) (-O3)    | 1.34               | 12.71              | 51.76              | 205.72             |
| (i, k, j) (-O3)    | 0.34               | 4.64               | 21.34              | 53.80              |
| (j, k, i) (-O3)    | 1.49               | 15.93              | 60.23              | 335.67             |

<img src="D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex3\Screens\Exec_time-no-opt.JPG" alt="Exec_time-no-opt" style="zoom:60%;" />

​				Figure N°1: Algorithms execution time depending on matrix size with no compiler optimizations

Figure N°1 presents the results in term of execution times of the 3 implementation of the matrix multiplication algorithm with no compiler optimizations. The command used to run these test on Dione was the following: `srun -n 1 -p gpu mm 1000 &`. Choice has been made to run on Dione's GPU because every other core was busy. Figure N°2 presents the same study but with compiler optimizations, `gcc -O3`.

<img src="D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex3\Screens\Exec_time-full-opt.JPG" alt="Exec_time-full-opt" style="zoom:60%;" />

​		Figure N°2: Algorithms execution time depending on matrix size with maximum compiler optimizations

We can easily see that the second implementation, the (I, K, J) order is by far the best as it only produces a cache miss every $4$ iterations.

##### c. Results verification

In order to verify the good computations of the 2 new implementation of the matrix multiplication algorithm we implemented a simple function comparing each coefficient of the matrices. This is not really optimized but this must not be as the algorithm only have to be verified once. Figure N°3 shows the output of the verification:

![result_verification_output](D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex3\Screens\result_verification_output.JPG)

​					Figure N°3: Screenshot of `compareMatrices()` output

### Question N°2

##### a. Implement a blocked version of matrix multiplication

The aims of this lab was to do experiment with blocked caching algorithms. To improve the ratio of cache hit, the matrixes were divided into small sets that could fits in the caches. This ensure the performance of caches. The implementation are as follow for the order ijk and the similar algorithms were implemented for other orders:

 ```
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

}```

##### b. Calculate block size

A block will contain rows and collums of two matrixes and the a cache line can contain 8 values. 
Therefore the blocksize <= sqrt(cache_size/8*2)

With above formular: the good blocksize for L1 cache is 50, and 250 for L2 cache


##### c. Test and determine best block size
The best value tested was 40, that executed in 28.56 seconds with -O3 flag. The test was done 
using Macbook Pro 14 inch 2021. The memory access was done from L1 cache. The result was different from the dione machine due to the different in L2 caches size and the designs of the memory system. The new Apple Unified Memory design has the main memory built in the same chip with the CPU. The bottle neck that happens in the data transfer are reduced significantly. 

The higher the blocksize, the slightly higher time it took to complete the tasks. The results from  the Dione cluster were slower. The blocksize that higher than 200 gave the best results. 

### Question N°3
The best performance order is IKJ, With blocksize of 40, the performance is high, it took 11.15 seconds to run the algorithm. The test was done using the same machine as in execise 2. It took 30.4 seconds for the IJK order to complete and 45.3 seconds for the JKI orders. 