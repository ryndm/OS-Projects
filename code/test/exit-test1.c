#include "syscall.h"

int
main()
{
  OpenFileId output = ConsoleOutput;

  char* str = "Hello from exit-test1\n";

  Write(str, 23, output);

  Exit(1);

}
