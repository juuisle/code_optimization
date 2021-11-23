# Code Optimization: Assignment N°1 Report

###### Cédric Léonard, 09/11/2021, student ID: 2100775

### Introduction

This first assignment intends to discover and use the Dione cluster.

### Question N°1

The first task of this assignment was about requesting a user's account for the Dione cluster. I have followed the Parallel Programming course last period, therefore I had already made a request for a Dione cluster account.

### Question N°2

The next question was about a first use of the cluster and understanding how we can execute a job.

````bash
# Default run on the cluster
srun hostname
# On one single thread
srun -n 1 hostname
# On 3 threads, all on the same node
srun -N 1 -c 3 hostname
# On 3 threads, all on separate nodes
srun –N 3 hostname
````

Bash commands above show 3 different ways of executing a job (in our case a really classic bash command: `hostname`). It's interesting to note that we can ask for a specific number of nodes and cores when executing the job, which is very useful for performance comparisons.

### Question N°3

The `lscpu ` command displays information about the processor architecture. By using this command on the login node and on a specific node of the cluster we can compare their architectures.

|                         |     Login Node     |   Cluster Node    |
| ----------------------- | :----------------: | :---------------: |
| Command line            |      `lscpu`       | `srun -n 1 lscpu` |
| Sockets                 |         2          |         2         |
| CPU Cores               |         12         |        20         |
| Threads per cores       | 2 (hyperthreading) |         1         |
| Total number of threads |     48 threads     |    40 threads     |
| Clock frequency         |      2.10 GHz      |     2.40 GHz      |

​								Table N°1: Login and Dione cluster nodes architecture

If we want to quickly summarize, we can say that both nodes have the same architecture (x86_64), CPU op-modes (32 and 64-bit) and Byte Order (Little Endian), have almost same Model (Vendor ID: GenuineIntel, CPU family: 6, Model: 85) but the main difference is in the number of cores (40 for the cluster, 48 for the login node) and the number of threads per core (1 on cluster, 2 on the login node). About the clock frequency, I expected it to be higher, in general CPU clock frequency are around 4 GHz. It may be because for this processor usage, i.e. parallel programming, a high clock frequency is not needed as memory access is slower and very frequent.

### Question N°4

Question N°4 asked us to complete 2 short programs about matrices multiplication and to run them on the Dione cluster. In this problem we only consider square matrices of size $N \times N$.

The following C code makes the trick and was implemented in C (*mm.c*) and JAVA (*mm.java*).

````c
// For each row
for(int r = 0; r < N; r++){
    // For each column
    for(int c = 0; c < N; c++){
      // Init C coefficient
      C[r][c] = 0;
      // Add each A and B coefficients multiplication
      for(int k = 0; k < N; k++)
        C[r][c] += A[r][k] * B[k][c];
    }
}
````

This code can be optimized by the used of a temporary sum in the third for loop instead of accessing the C matrix at each iteration (which will become very slow when using matrices which do not fit into the caches). This type of code is described in the solution furnished on Moodle.

<img src="D:\Zbeul\INSA\5 EII - Abo\S9\Code Optimization\Ex1\screens\Verif_screens.JPG" alt="Verif_screens" style="zoom: 67%;" />

​								Figure N°1: *mm.c* and *mm.java* output for $3 \times 3$ matrices

Figure N°1 presents the code output for $3 \times 3$ matrixes and allows us to verify that the results are correct.

| Language + Optimization | $1000 \times 1000$ | $2000 \times 2000$ | $3000 \times 3000$ | $4000 \times 4000$ |
| ----------------------- | ------------------ | ------------------ | ------------------ | ------------------ |
| C (no optimization)     | 4.74               | 45.71              | 184.18             | 796.4              |
| C (-O3)                 | 1.32               | 12.16              | 52.92              | 205.46             |
| java                    | 1.47               | 23.88              | 90.95              | 249.5              |

​								Table N°2: Execution times for different matrices size and version

Table N°2 presents the execution times on one Cluster node depending of the version (C with no compiler optimization, C with the gcc -O3 optimization or JAVA) and the size of the matrices. Intuitively, the bigger the matrices the longer the execution and the computations are faster with optimization.

Table N°2 results have been obtained with the following commands (for $1000 \times 1000$ matrices): 

```bash
# Running the C program with GCC -O3 optimization (implemented by default in the Makfile)
srun -n 1 mm 1000 &
# Running the JAVA program
srun -n 1 java mm 1000 &
# Running the JAVA program
*Change OPTFLAGS = [NOTHING] in Makefile* && srun -n 1 mm 1000 &
```

<img src="D:\Zbeul\INSA\5 EII - Abo\S9\Code Optimization\Ex1\screens\Execution-times_graph.JPG" alt="Execution-times_graph" style="zoom:67%;" />

​								Figure N°2: Graph showing execution times depending on matrix size

Figure N°2 plots runtimes depending on matrix size for the different version of our program. Adding trendlines allow us to see more easily the exploding complexity of this program, especially with un-optimized code (orange curve).

### Question N°5

Question N°5 was about calculating how many floating-point operations are executed when multiplying two matrices with the implemented algorithm. The algorithm used implies 3 nested for loops running from $0$ to index $N-1$ which means $N^3$ iterations. 

Then, an iteration consists in 2 operations, a multiplication and an addition. This produces $2N^3$ operations and $2N^3 + N^2$ if we also consider the initialization of each C matrix coefficient. If we try to go a bit further, I would add that modern processors use modern instructions, including Floating Multiply-Add (or FMADD instruction) allowing to process an addition and a multiplication in one clock cycle. Adding this with compiler optimizations for C matrix initialization means a complexity of $O(N^3)$ or cubic magnitude.

For example, running this algorithm with $1000 \times 1000$ matrices means $1000^3 = 1.000.000.000 \space FLOPS$ and represents a considerable amount of computations. For example the measured time for the C (-O3) version was $1.32s$ which gives $2.10^9 / \space 1.3 2 = 1.51 \space GFLOP/s$ and is really poor for this kind of processor.

This also means that there is room for improvement with the use of, for example, pipeline execution.

