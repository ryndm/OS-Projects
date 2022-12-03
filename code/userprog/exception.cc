// exception.cc
//  Entry point into the Nachos kernel from user programs.
//  There are two kinds of things that can cause control to
//  transfer back to here from user code:
//
//  syscall -- The user code explicitly requests to call a procedure
//  in the Nachos kernel.  Right now, the only function we support is
//  "Halt".
//
//  exceptions -- The user code does something that the CPU can't handle.
//  For instance, accessing memory that doesn't exist, arithmetic errors,
//  etc.
//
//  Interrupts (which can also cause control to transfer from user
//  code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "unistd.h"
#include <unordered_map>
//----------------------------------------------------------------------
// ExceptionHandler
//  Entry point into the Nachos kernel.  Called when a user program
//  is executing, and either does a syscall, or generates an addressing
//  or arithmetic exception.
//
//  For system calls, the following is the calling convention:
//
//  system call code -- r2
//    arg1 -- r4
//    arg2 -- r5
//    arg3 -- r6
//    arg4 -- r7
//
//  The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//  "which" is the kind of exception.  The list of possible exceptions
//  is in machine.h.
//----------------------------------------------------------------------

// character array used to store extracted file name
char gBuf[100];
// Thread *runningThreads[10];
unordered_map<int, Thread*> runningThreads;
Thread *joiningThread;
SpaceId joinedThread;

// Program similar to RunUserProg in main.cc used to run program
void
RunUserProgTemp(void *filename) {
    AddrSpace *space = new AddrSpace;
    ASSERT(space != (AddrSpace *)NULL);
    if (space->Load((char*)filename)) {  // load the program into the space
        space->Execute();         // run the program
    }
    ASSERTNOTREACHED();
}

void extractName(int num) {
  bzero(gBuf, 100);        // set the buffer with zeros
  gBuf[99] = '\0';         // set last value as \0 just for safety
  int n = 0;
  int vAddr = kernel->machine->ReadRegister(num);
  do {
    kernel->machine->ReadMem(vAddr + n, 1, (int*)(gBuf+n));
  } while(n < 99 && gBuf[n++] != '\0');
}

void
ExceptionHandler(ExceptionType which)
{
  int type = kernel->machine->ReadRegister(2);
  DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");


  switch (which) {
    case SyscallException:
      switch(type) {
        case SC_Halt:{
          DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
          
          SysHalt();
        }break;
          
        case SC_Add:{
          DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
          
          /* Process SysAdd Systemcall*/
          int result;
          result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
                          /* int op2 */(int)kernel->machine->ReadRegister(5));
          
          DEBUG(dbgSys, "Add returning with " << result << "\n");
          /* Prepare Result */
          kernel->machine->WriteRegister(2, (int)result);
          
          
        }break;
          
        case SC_Write:{
          // printf("Write system call made by %s\n", kernel->currentThread->getName());
          // printf("in ExceptionHandler, Write() System Call is made. The first parameter is %d, the second parameter is %d, and the third parameter is %d\n\n", kernel->machine->ReadRegister(4), kernel->machine->ReadRegister(5), kernel->machine->ReadRegister(6));

          // Read the arguments passed. Not reading 3rd argument as we know its going to be ConsoleOutput
          int strAddr = kernel->machine->ReadRegister(4);
          int strLength = kernel->machine->ReadRegister(5);
          OpenFileId output = kernel->machine->ReadRegister(6);

          char strRead[strLength];        // create a temporary buffer to store the values we read from memory
          int oneChar;                    // temporary int for storing translated physical address for one char
          int n = 0;                      // counter to store the number of bytes we read
          while(n < strLength) {
            kernel->machine->ReadMem(strAddr + n,1,&oneChar);    // get the physical address
            strRead[n++] = (char)oneChar;                        // store the value present at that address into our temporary buffer
          }
          // printf("in write %s", strRead);
          // printf("output is %d", output);
          if(output == ConsoleOutput) {
            // printf("Wrote in console\n");
            printf("%s", strRead);                     // print the buffer to console
          } else {
            // printf("Wrote in file\n");
            fprintf((FILE*)output, strRead);
          }
          kernel->machine->WriteRegister(2, n);      // storing the number of bytes we read in r2
        }break;

        case SC_Create: {
          extractName(4);
          FILE *fp = fopen(gBuf, "w");
          if(fp) {
            // printf("file created\n");
            fclose(fp);
            kernel->machine->WriteRegister(2, 1);
          } else {
            // printf("file creation failed\n");
            kernel->machine->WriteRegister(2, -1);
          }
        }break;

        case SC_Open: {
          extractName(4);
          if (access(gBuf, F_OK) == 0) {
            FILE *fp = fopen(gBuf, "a");
            if(fp) {
              // printf("file opened\n");
              kernel->machine->WriteRegister(2, (OpenFileId)fp);
            } else {
              kernel->machine->WriteRegister(2, -1);
            }
          } else {
            // printf("file not present\n");
            kernel->machine->WriteRegister(2, -1);
          }
        }break;

        case SC_Read:{
          // printf("in read\n");
          char* buffer = (char *)kernel->machine->ReadRegister(4);
          int size = kernel->machine->ReadRegister(5);
          OpenFileId fileId = kernel->machine->ReadRegister(6);
          char ch;
          int n = -1;
          do {
            n++;
            ch = fgetc((FILE*)fileId);
            *(buffer + n) = ch;
            size--;
          } while(buffer[n] != '\0' && n < 99 && buffer[n] != EOF && size>=0);
          // printf("string read is\n");
          // printf("%s\n", gBuf);
          kernel->machine->WriteRegister(2, n);
        }break;

        case SC_Close:{
          int fileId = kernel->machine->ReadRegister(4);
          int status = fclose((FILE*)fileId) == 0 ? 1 : -1;   // fclose returns 0 on success
          // printf("file closed\n");
          kernel->machine->WriteRegister(2, status);
        }break;

        case SC_Exec:{
          extractName(4);
          char *fname = gBuf;
          if(fname == NULL) {
            kernel->machine->WriteRegister(2, -1);
          } else {
            Thread * t = new Thread(gBuf);
            t->Fork((VoidFunctionPtr)RunUserProgTemp, gBuf);
            runningThreads[(int)t] = t;
            kernel->machine->WriteRegister(2, (int)t);
          }
        }break;

        case SC_Exit:{
          // if(kernel->machine->ReadRegister(4) == 0)
          //   printf("Process %s exited normally\n", kernel->currentThread->getName());
          // else printf("Process %s exited abnormally\n", kernel->currentThread->getName());

          if (joiningThread != NULL && kernel->currentThread == runningThreads[joinedThread]) {
            IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff); // disable interrupts
            kernel->scheduler->ReadyToRun(joiningThread);
            kernel->interrupt->SetLevel(oldLevel);
            joiningThread = NULL;
            joinedThread = 0;
          }

          kernel->currentThread->Finish();
        }break;

        case SC_Join:{
          joinedThread = kernel->machine->ReadRegister(4);
          joiningThread = kernel->currentThread;
          // Send the thread to sleep
          IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff); // disable interrupts
          joiningThread->Sleep(FALSE);
          kernel->interrupt->SetLevel(oldLevel);
          kernel->machine->WriteRegister(2, 0);
        }break;
          
        default:
          cerr << "Unexpected system call " << type << "\n";
          break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
  }
  
  /* Modify return point */
  {
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
    
    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
    
    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
  }
}
