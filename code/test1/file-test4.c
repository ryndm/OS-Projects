#include<stdio.h>
#define READ_BYTES 33

// convert an integer to a string. The code is available over the Internet
// int tostring(char str[], int num)
// {
//     int i, rem, len = 0, n;
 
//     n = num;
//     while (n != 0)
//     {
//         len++;
//         n /= 10;
//     }
//     for (i = 0; i < len; i++)
//     {
//         rem = num % 10;
//         num = num / 10;
//         str[len - (i + 1)] = rem + '0';
//     }
//     str[len] = '\0';
//   return len+1;
// }

int main() {
//   char* buf;
//   OpenFileId output, failTest = 0;
//   char num2stringBuffer[100];

//   output = Open('test-file.txt');
  
//   if(output == -1)
//     Exit(1);

//   int rd = Read(buf, READ_BYTES, output);

//   Write(buf, READ_BYTES, ConsoleOutput);

//   Write("Read returned ", 14, ConsoleOutput);
//   int length = tostring(&num2stringBuffer, rd);
//   Write(&num2stringBuffer, length, ConsoleOutput);
//   Write("\n", 2, ConsoleOutput);

//   if(Close(output) == -1)
//     Write("Failed to close file\n", 22, ConsoleOutput);
//   else
//     Write("Successfully closed file\n", 26, ConsoleOutput);

  // This condition is strictly written to test Close returns -1 for non-existent file
//   if(Close(failTest) == -1)
//     Write("Failed to close file\n", 22, ConsoleOutput);
//   else
//     Write("Successfully closed file\n", 26, ConsoleOutput);

  Exit(0);  
}