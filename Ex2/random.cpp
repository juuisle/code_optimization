/* Rand number example */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>

#define N 1000

int main()
{
  int r, small = 0, large = 0;
  
  /* Initialize random seed from current time */
  srand(time(NULL));

  for(int i = 0; i < N; i++)
  {
    /* Generate number between 0 and 9: */
    r = rand() % 10;
    if(r < 5)
      small++;
    else
      large++;
  }

  std::cout << "Nr of small values = " << small << std::endl;
  std::cout << "Nr of large values = " << large << std::endl;
  
  return 0;
}
