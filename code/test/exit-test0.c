#include "syscall.h"

int
main()
{
  int i,j;
  for (i = 0; i < 5; i++)
  {
    for (j = 0; j < 10000; j++);
  }

  Exit(0);
}

