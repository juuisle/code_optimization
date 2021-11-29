#include <stdio.h>
#include <math.h>

int main()
{
    const float up = 5.3E20;
    float value = 1.0f;
    int found = 0;

    while(!found)
    {
        float nextvalue = nextafterf(value, up);
        if(nextvalue-value > 1)
        {
            printf("value = %3.4f\n", value);
            printf("nextvalue = %3.4f\n", nextvalue);
            printf("The next value after %3.0f in IEEE-754 (32-bits) is %3.0f and not %d", value, nextvalue, (int)(value)+1);
            found = 1;
        }
        value = nextvalue;
    }
}
