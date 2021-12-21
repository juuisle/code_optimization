# Code Optimization: Assignment N°5 Report

###### Duy Le - Cédric Léonard, 6/12/2021

### Introduction
The assignment is about understanding the complier-generated assembly language. We will study the classic SAXPY procedure which stands for Single-precision $A \times X \space Plus \space Y$

### Question N°1

The first question concerns the code analyze of the straight-forward implementation of the SAXPY procedure in *saxpy.c*. 

- How are the parameters passed to the procedure
The parameters are passed to the procedure as follow: n to register EDI and RDI, alpha to XMN0, *X to RSI, *Y to RDX

- How is the loop implemented?

```assembly
  for (int i=0; i<n; i++)
   0:   31 c0                   xor    %eax,%eax                # Set i = 0
   2:   85 ff                   test   %edi,%edi                # if n <= 0:
   4:   7e 25                   jle    2b <saxpy+0x2b>          #   then jump to 2b: terminate the program
    # Execute other code.
   23:   48 83 c0 01             add    $0x1,%rax               # Increase i

  for (int i=0; i<n; i++)
   27:   39 c7                   cmp    %eax,%edi               # Compare i and n
   29:   7f e5                   jg     10 <saxpy+0x10>         # Jump to the arithmetic operation if the the result is n>i
   2b:   f3 c3                   repz retq                      # Otherwise, return
```

- How are the arithmetic operations done?

```assembly
Y[i] = alpha*X[i] + Y[i];
  10:   f3 0f 10 0c 86          movss  (%rsi,%rax,4),%xmm1      `# xmm1 = X[i]
  15:   f3 0f 59 c8             mulss  %xmm0,%xmm1               # xmm1 = x[i] x alpha
  19:   f3 0f 58 0c 82          addss  (%rdx,%rax,4),%xmm1       # xmm1 += Y[i]
  1e:   f3 0f 11 0c 82          movss  %xmm1,(%rdx,%rax,4)       # Y[i] = xmm1
```

- Instructions that have no effect, i.e. that are inserted by the compiler to align branch targets?
```6:   66 2e 0f 1f 84 00 00    nopw   %cs:0x0(%rax,%rax,1) # Used to align```

b. Compile the saxpy procedure with full optimizations (-O3) and look at the generated assembly language code.


```console
dione:~$ gcc -O3 -c -fopt-info saxpy.c
saxpy.c:7:3: optimized: loop vectorized using 16 byte vectors
saxpy.c:7:3: optimized:  loop versioned for vectorization because of possible aliasing
saxpy.c:7:3: optimized: loop with 2 iterations completely unrolled (header execution count 49959011)
```


The O3 optimization level, which is the highest one, performs more optimizing operation. In the above assembly code, the loop has been vectorized and unrolled as below. 

```assembly
40:   0f 10 0c 06             movups (%rsi,%rax,1),%xmm1
  44:   0f 10 1c 02             movups (%rdx,%rax,1),%xmm3
  48:   0f 59 ca                mulps  %xmm2,%xmm1
  4b:   0f 58 cb                addps  %xmm3,%xmm1
  4e:   0f 11 0c 02             movups %xmm1,(%rdx,%rax,1)
```

```assembly
  77:   f3 0f 59 c8             mulss  %xmm0,%xmm1
  7b:   f3 0f 58 09             addss  (%rcx),%xmm1
  7f:   f3 0f 11 09             movss  %xmm1,(%rcx)
  83:   8d 48 01                lea    0x1(%rax),%ecx
  86:   39 cf                   cmp    %ecx,%edi
  88:   7e 5d                   jle    e7 <saxpy+0xe7>
  8a:   48 63 c9                movslq %ecx,%rcx
  8d:   83 c0 02                add    $0x2,%eax
  90:   f3 0f 10 0c 8e          movss  (%rsi,%rcx,4),%xmm1
  95:   4c 8d 04 8a             lea    (%rdx,%rcx,4),%r8
  99:   f3 0f 59 c8             mulss  %xmm0,%xmm1
  9d:   f3 41 0f 58 08          addss  (%r8),%xmm1
  a2:   f3 41 0f 11 08          movss  %xmm1,(%r8)
```

c. Compile the saxpy procedure with -O2 optimization and the additional flag -mfpmath=387

How are the parameters passed to the procedure?
The first parameter (a) is passed into register EDI and the second (b) in ESI, the third parameter (c) is passed into register EDX


```assembly
int med3(int a, int b, int c) {
  int mid;
  if ( a > b )   {
   0:   39 f7                   cmp    %esi,%edi                # compare b with a
int med3(int a, int b, int c) {
   2:   89 f0                   mov    %esi,%eax                # move esi (b) to eax
  if ( a > b )   {
   4:   7e 12                   jle    18 <med3+0x18>           # if less than or equal, jump to 18
    if ( c > b) {
   6:   39 d6                   cmp    %edx,%esi                # compare c with b
   8:   7d 19                   jge    23 <med3+0x23>           # if greater than or equal
   a:   39 d7                   cmp    %edx,%edi                # compare c with a
   c:   89 d0                   mov    %edx,%eax                # move edx (c) to eax
   e:   0f 4e c7                cmovle %edi,%eax                # move edi (a) to eax if less than or equal
  11:   c3                      retq                            # return
  12:   66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)         # has no effect, insert by the compiler to align branch targets
      mid = b;
    }
  }
  else {
    if ( b > c ) {
  18:   39 d6                   cmp    %edx,%esi                # compare c to b
  1a:   7e 07                   jle    23 <med3+0x23>           # if less than or equal, jump to 23 (return)
  1c:   39 d7                   cmp    %edx,%edi                # compare c to a
  1e:   89 d0                   mov    %edx,%eax                # move c to eax
  20:   0f 4d c7                cmovge %edi,%eax                # move a to eax if greater than or equal
    else {
      mid = b;
    }
  }
  return mid ;
}
  23:   f3 c3                   repz retq                       # return
```


### Question N°3
The procedure in the attached file call_max.c illustrates how procedure calls are implemented in the generated assembly language

a.

In funtion max(int a, int b), the first parameter (a) is passed into register EDI and the second (b) in ESI. Afterthat, a and b are compared and stored to the eax register.

```assembly
int max(int a, int b) {
   0: 39 f7                 cmp    %esi,%edi                         # compare b to a
   2: 89 f0                 mov    %esi,%eax                         # move b to eax
   4: 0f 4d c7              cmovge %edi,%eax                         # move a to eax if greater than or equal
  if (a>b) return a;
  else return b;
}
   7: c3                    retq                                     # return
   8: 0f 1f 84 00 00 00 00  nopl   0x0(%rax,%rax,1)                  # has no effect
   f: 00
```

In the second procedure, the first parameter (*a) is passed into register EDI and the second (*b) in ESI, the third parameter (len) is passed into register EDX

```assembly
0000000000000010 <select>:

void select (int *a, int *b, int len) {

  for (int i=0; i<len; i++) {
  10: 85 d2                 test   %edx,%edx                         # if len = 0
  12: 7e 3c                 jle    50 <select+0x40>                  # then return by jumping into 50
void select (int *a, int *b, int len) {
  14: 41 55                 push   %r13                              # push r13
  16: 8d 42 ff              lea    -0x1(%rdx),%eax                   # eax = len - 1
  19: 41 54                 push   %r12                              # push r12
  1b: 4c 8d 2c 85 04 00 00  lea    0x4(,%rax,4),%r13                 # r13 = r12 + 4
  22: 00
  23: 49 89 f4              mov    %rsi,%r12                         # move rsi to r12
  26: 55                    push   %rbp                              # push register base pointer
  27: 48 89 fd              mov    %rdi,%rbp                         # move rdi to rbp
  2a: 53                    push   %rbx                              # push rbx
  for (int i=0; i<len; i++) {
  2b: 31 db                 xor    %ebx,%ebx                         # i = 0
  2d: 0f 1f 00              nopl   (%rax)                            # no effect
    a[i] = max(a[i], b[i]);
  30: 41 8b 34 1c           mov    (%r12,%rbx,1),%esi                # move r12 to esi
  34: 8b 7c 1d 00           mov    0x0(%rbp,%rbx,1),%edi             # move rbp to edi
  38: e8 00 00 00 00        callq  3d <select+0x2d>                  # calq 3d
  3d: 89 44 1d 00           mov    %eax,0x0(%rbp,%rbx,1)             # move eax to rbp
  41: 48 83 c3 04           add    $0x4,%rbx                         # rbx += 4
  for (int i=0; i<len; i++) {
  45: 4c 39 eb              cmp    %r13,%rbx                         # compare r13 with rbx
  48: 75 e6                 jne    30 <select+0x20>                  # if not equal, jump to 30
  }


}
  4a: 5b                    pop    %rbx                              # pop rbx
  4b: 5d                    pop    %rbp                              # pop rbp
  4c: 41 5c                 pop    %r12                              # pop r12
  4e: 41 5d                 pop    %r13                              # pop r13
  50: f3 c3                 repz retq                                # return
```

b.

```assembly
int max(int a, int b) {

   0:   39 f7                   cmp    %esi,%edi                # compare b to a
   2:   89 f0                   mov    %esi,%eax                # move b to eax
   4:   0f 4d c7                cmovge %edi,%eax                # move a to eax if greater than or equal
  if (a>b) return a;
  else return b;
}
   7:   c3                      retq                            # return
   8:   0f 1f 84 00 00 00 00    nopl   0x0(%rax,%rax,1)         # no effect
   f:   00

0000000000000010 <select>:

void select (int *a, int *b, int len) {
  for (int i=0; i<len; i++) {
  10:   31 c0                   xor    %eax,%eax                # set i = 0
  12:   85 d2                   test   %edx,%edx                # if len = 0
  14:   7e 1f                   jle    35 <select+0x25>         # then return by jumping to 35
  16:   66 2e 0f 1f 84 00 00    nopw   %cs:0x0(%rax,%rax,1)     # no effect, used to align
  1d:   00 00 00
  20:   8b 0c 86                mov    (%rsi,%rax,4),%ecx       # move rsi to ecx
  23:   39 0c 87                cmp    %ecx,(%rdi,%rax,4)       # compare ecx to rdi
  26:   0f 4d 0c 87             cmovge (%rdi,%rax,4),%ecx       # move rdi to ecx if greater than or equal
    a[i] = max(a[i], b[i]);
  2a:   89 0c 87                mov    %ecx,(%rdi,%rax,4)       # move ecx to rdi
  2d:   48 83 c0 01             add    $0x1,%rax                # i++
  for (int i=0; i<len; i++) {
  31:   39 c2                   cmp    %eax,%edx                # compare i to len
  33:   7f eb                   jg     20 <select+0x10>         # jump to 20 if greater than
  35:   f3 c3                   repz retq                       # return
```

With function inlining enabled, the function is inlined, the structure of the code is similar to the previous case.
