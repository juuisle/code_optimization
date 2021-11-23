# CODE OPTIMIZATION - EXERCISE 3

   1. Matrix multipication algorithm
    a. Analyze the memory acccess patterns 
    **for (i,k,j) orders:**
  - Access to elements in matrix b are done row-wise as similar to an example. Hence, for every 8 iteration, there will be a cache miss 
- Access to elements in matrix c has the same cache miss as matrix b since it's a row-wise operation. 
In total, there's 0.25 cache miss in the innermost loop after each iteration
    
    **for (j,k,i) orders**
    Access to the elements in matrix a and c are done collumn-wise, which will lead to a cache miss on every iteration. The total cache miss of each iteration in the innermost loop is 2

    
   b. 
   c. ?


2. Matrix multiplation with cache blocking
   a. 
   b. A block size should has 4 values for L1 (4x8=32) and 128 for L2(128x8=1024)

3. 