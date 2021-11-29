# Code Optimization: Assignment N°4 Report

###### Duy Le - Cédric Léonard, 29/11/2021

### Introduction

### Question N°1

![Filled_form](D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex4\Screens\Filled_form.JPG)

​			Figure N°1: Screenshot of *Form.docx* filled with 6-bit floating-point format based on the IEEE standard

#### Question N°2

- Original number: -275.875

- Convert its positive version in classic binary:  $275 = 256 + 16 + 2 + 1$   and $0.875 = 0.5 + 0.25 + 0.125$ => $275.875 = 100010011.111_2 \times 2^0$
- Normalize it: $275.875 = 1.00010011111_2 \times 2^8$
- Compute the exponent: $E = 8+127 = 135 = 128 + 4 + 2 + 1 = 10000111_2$ 
- Compute the mantissa (drop the leading 1-bit and fill with insignificant 0-bits): $f = 0001001111 1000000000 000$ 
- Don't forget the sign bit: $1$ for negative numbers

Result: $1 \space 10000111 \space 0001001111 1000000000 000$

If we verify our result with the formula $(-1)^S \times 1.f \times 2^{E-127}$ we get:

$(-1)^1 \times 1. \times 2^{8} = -275.875$

-4 -7 -8 -9 -10 -11 

#### Question 3

The smallest single-precision floating-point value $X.0$ for which $X+1.0f = X$ is  $X.0 = 16777216.0 = 2^{24}$. This makes sense because in IEEE-754 the mantissa is only made of 23 bits, so whenever we hit $16777216$ we don't have any more digits to encode this $1.0f$ increase. Therefore, in IEEE-754 the next number after $16777216$ is $16777218$.

It's possible to deduce that with a pen and some paper but I was lazy and wrote a program, Cf. *smallest_float.c*.

![smallest_float_output](D:\Zbeul\INSA\5 EII - Abo\S9\code_optimization\Ex4\Screens\smallest_float_output.JPG)

​																Figure N°2: *smallest_float.c* output

#### Question 4

````C
int main()
{
    float f = 0.1f;
    float sum1 = 0.0f;

    // Calculate the result by adding
    for (int i = 0; i < 10; ++i)
        sum1 += f;
    // Calculate the result by multiplying
    float sum2 = f * 10.0f;
    // Print results, with a third calculation of the value
    printf("sum1 = %1.15f, sum2 = %1.15f, sum3 = %1.15f\n",
           sum1, sum2, f*10.0);
    return 0;
}
````

The output of this program is:

`sum1 = 1.000000119209290, sum2 = 1.000000000000000, sum3 = 1.000000014901161`

We can see that the only result corresponding to the "answer" is `sum2`. This is because we explicitly precise that we are doing a multiplication with float operands, therefore the compiler is able to use the proper instruction.

`sum1` is easy to explain why it is that different from `sum2`, we proceed via 10 additions leading to 9 temporary results, each rounded. For example, the second computation should give us $0.2$, but in the binary  representation $0.2$ is encoded as  $0.00110011[0011]_2$  ... if we had an infinite numbers of bits which we do not. Therefore,  $0.2$ is rounded and introduces a first error in our result.

`sum3` is a bit more tricky, I would say that the slight difference in the result comes from the fact that we didn't specify that $10.0$ was a float constant, so it has been encoded as with a `double` precision and then converted in a float before being multiplied which leads to the "error".
