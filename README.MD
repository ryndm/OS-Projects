#### code/threads -> Heart of Kernel Scheduler, synch primitives, etc

#### code/filesys -> Filesystem

#### code/lib -> Library routines

#### code/machine -> MIPS simulator & simulated hardware

#### code/network -> Networking

#### code/test -> test user programs in C. Need a cross-compiler

#### code/userprog -> Support for user-level processes

<br>

## Build Nachos

#### cd nachos/code/build.linux
#### make clean, make depend, make
#### ./nachos -K (only run ThreadTest())
#### ./nachos -x compiled_userprogram_path (run user programs)