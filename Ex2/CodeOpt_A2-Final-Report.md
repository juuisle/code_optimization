# Code Optimization: Assignment N°2 Report

###### Duy Le - Cédric Léonard, 18/11/2021

### Introduction

This second assignment intends to discover **Gcov** and to understand the branch predictions mechanism.

### Question N°1

The first question was about compiling *mm.c* from Assignment N°1 with **Gcov** to profile our program behavior and understand its branches. The following code shows how to generate **Gcov** profile on the Dione cluster. It's important to make sure that the `gcc `module is still loaded when calling the `gcov` command or it will result with a version error which doesn't make much sense.

````bash
# Load gcc on the login node
module load gcc
# Compile it with the code coverage utility
g++ --coverage mm.c -o mm
# Run it (On GPU because the queue was full)
srun -n 1 -p gpu mm 1000
# Profile the program (-b: branch probabilities, -c branch counts)
gcov -b -c mm.c
# Check the file created
more mm.c.gcov
````

<img src="D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex2\screens\gcov_behavior_3for_loops.JPG" alt="gcov_behavior_3for_loops" style="zoom:75%;" />

​         		Figure N°1: Screenshot of *mm.c.gcov* Matrices multiplication algorithm analysis

If we look at **Gcov** output and focus more specifically on our matrices multiplication function, like presented in Figure N°1, we can state the following:

- In our case the program was runned with $1000\times 1000$ matrices, with 3 nested for loops. This implies $1000^3 = 10^9$ iterations,  we can verify that in the line N°98: `1000000000: 98: sum += A[r][k] * B[k][c];`
- Looking more precisely on the first for loop we can see it has been called $1001$ times, taken $1000$ times and jumped out $1$ time. Same statement can be made by looking at the second and third for loops but respectively multiplied by $10^3$ and $10^{3^2}$ because our for loops are nested;
- This type of prediction corresponds to a static prediction: "Predicted as taken" all the time. This is really effective when used on for branch with a huge number of iterations;
- We can also easily calculate the number of mispredicted branches:  $1000000 (3^{rd}) + 1000 (2^{nd}) + 1 (1^{st}) = 1001001$ false predictions. Otherwise, it means that $0.1\%$ of the branches are mispredicted.

#### Question N°2

<img src="D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex2\screens\Form-a_branch_prediction.JPG" alt="Form-a_branch_prediction" style="zoom: 60%;" />

Figure N°2: *Form.docx* filled with the branch predictions for array $A = \{-1, 2, -3, 4, -5, 6, -7, 8, -9, 10\}$

Figure N°2 presents the branch predictions for the first array A, with 7 right predictions and 3 mispredicted branches. Branch pattern history and Branch history register are updated each step and the modified index is shown in **red**. We can see that for such a "constant" pattern (a condition alternating true / false), this mechanism with 4 2-bit Branch History Register and a 2-bit Branch History Register takes 6 steps (From the beginning until a **2** appears for the taken condition) to initialize, adapt to this pattern.

$30\%$ may seem a consequent number of mispredictions, but if the array was bigger and "constant", this kind of prediction system would have made no more error subsequently.

As you can see I miss-read the checked condition and filled the doc for the condition $X[i] > 0$ and not $\leq$, I only realized that when reading the Model solution on Moodle. Fortunately, it doesn't change too much for the array $A$ as it is a "1 on 2" pattern so I choose to not modify my first answer.

<img src="D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex2\screens\Form-b_branch_prediction.JPG" alt="Form-b_branch_prediction" style="zoom:60%;" />

Figure N°3: *Form.docx* filled with the branch predictions for array $B = \{1, 2, 3, 4, -5, -6, -7, -8, -9, -10\}$

This type of system really is robust but a bit slow to setup, Figure N°3 is a good example. For the second array B, results for the predicted branches are 4 false predictions and 6 right. In this case, the system starts with 4 right predictions which is lucky because it depends on its initialization, if the initialization was "Strongly taken" or 3 it would have failed the 4 first predictions. Then, the pattern becomes "always taken" and it takes 4 steps to our system to adapt and nicely predict the jump at index 8. As this pattern keeps going we can imagine that if the array was bigger our system would continue predicting correctly.

We can also imagine how it will perform if suddenly the pattern becomes "never taken" again. After only 1 mispredictions the selected pattern will be the number 2, uninitialized, and after 1 more prediction the selected pattern will be he number 0, leading to right predictions: "not taken". Therefore, after some initialization steps and for such a "simple" pattern ("always taken" or "never taken") the adaptive time of our system is 1 step. This means than whenever the pattern switch,  the system will only do 1 mistake before adapting. 

#### Question 3

After implementing the studied algorithm we looked for different solutions in order to modified the `if()then{}` statement into a non-branching construct.

The first tested solution was the following piece of code:

````c
void minmax_ternary(int a[], int b[], int n)
{
    for (int i = 0; i < n; i++)
    {
        int min = a[i] < b[i] ? a[i] : b[i];
        int max = a[i] < b[i] ? b[i] : a[i];
        a[i] = min;
        b[i] = max;
    }
}
````

This solution was presented in the *Ex2ModelSolution.pdf* on Moodle. The purpose of this code is to replace the `if()then{}` statement with ternary conditions which can be executed as the assembler `cmov` instruction which is a non-branching instruction.

![Non-branching_constructs](D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex2\screens\Non-branching_constructs_ternary.JPG)

​         			Figure N°4: Screenshot of the minmax algorithm using ternary conditions

Unfortunately, as presented in Figure N°4, this does not work for us as presented in the solution. This can be related to compiler optimizations but we made sure to NOT precise any `-O` option in the building command line. Therefore we tried another solution, using masks, presented in the next piece of code:

````c
void minmax_mask(int a[], int b[], int n)
{
    for (int i = 0; i < n; i++)
    {
        int mask = (a[i] < b[i]);
        int min = (a[i] * mask) + (b[i] * !mask);
        int max = (b[i] * mask) + (a[i] * !mask);
        a[i] = min;
        b[i] = max;
    }
}
````

![Non-branching_constructs_mask](D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex2\screens\Non-branching_constructs_mask.JPG)

​         			Figure N°5: Screenshot of the minmax algorithm using mask

This second solution works very nicely, as shown on Figure N°5. It is a bit different from the usual mask solution as we do not use the `bool` or `bytes` type for the mask but the `int` type in order to not include any other library. This leads us to implement the **AND** condition as a multiplication and the **OR** condition as an addition.

#### Question 4

The last question of this assignment was a bit different and ask us to familiarize ourselves with the documentation of a modern CPU: **Intel Skylake**.

- How many stages does the pipeline have?

Adaptive pipeline system: minimum stages 14 and maximum 19.

- How many physical registers does the register renaming mechanism use?

Reorder Buffer: 224 entries / physical registers

- How many reservation stations does it have?

97 entries in the scheduler (Unified Reservation Station)

- How large are the load- and store-buffers?

56 entries for the store buffer and 72 for the load buffer

- How many execution units are there (i.e. hardware units that can execute microoperations)?

The scheduler owns 8 ports, from 0 to 7, which means 8 different functional units. But each functional unit is made of different Execution Units (EUs), for example Port 6 owns an ALU capable of interger arithmetic and Logic, a Shift EU and a Branch. In total there is 22 EUs, including 4 ALUs, 3 VEC ALUs, etc.
