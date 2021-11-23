# Code Optimization: Assignment N°3 Report

###### Duy Le - Cédric Léonard, 23/11/2021

### Introduction

This third assignment intends to dive deeper into the Memory system and optimization. The goal of this exercise is to manipulate and test 3 version of the basic matrix multiplication algorithm and then to write a new algorithm with cache blocking.

### Question N°1

##### a. Analyze the memory access patterns

In this part we will consider that a cache line is made of 8 consecutive values and when talking about accesses we will talk about accesses inside the inner-most for loop.

###### (i, j, k) or (j, i, k) orders

In this most straight forward implementation, matrix $A$ accesses are done row-wise and matrix $B$ accesses are done column-wise. This leads to a cache miss for matrix $A$ every 8 accesses and a miss each iteration for matrix $B$ (except if the matrix size is $< 8$ which is unlikely).

As explained in the subject, this leads to a $1.125$ cache misses per iteration.

###### (k, i, j) or (i, k, j) orders

In this second implementation: access to elements in matrix $B$ are done row-wise, as similar to an example. Hence, for every 8 iteration, there will be a cache miss. Access to elements in matrix $C$ has the same cache miss as matrix $B$ since it's a row-wise operation. 
In total, there is $0.25$ cache miss in the innermost loop after each iteration.

###### (j, k, i) or (k, j, i) orders

In this last implementation access to the elements in matrix $A$ and $C$ are done column-wise, which will lead to a cache miss on every iteration. The total cache miss of each iteration in the innermost loop is $2$.

##### b. Implement and test the algorithms

##### c. Results verification

### Question N°2

##### a. Implement a blocked version of matrix multiplication

##### b. Calculate block size

A block size should has 4 values for L1 (4x8=32) and 128 for L2(128x8=1024)

##### c. Test and determine best block size

### Question N°3

