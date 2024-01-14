# The Abstract Machine (AM)

Abstract Machine: Essential abstraction to help you write a complete computer system!

## Directory Organization

- `am/`: AM header files and AM code implemented for each architecture.
- `libs/`: Architecture-independent runtime libraries built on AM, such as software-simulated floating-point numbers, basic libc functions, etc.
- `apps/`: Examples of applications running on AM.
- `tests/`: Simple test programs to test AM implementations.

## Creating an AM Application

You can create applications running on AM in any directory. Programs are written in C/C++ and cannot call other library functions except AM (but can refer to header files containing architecture-related data definitions such as `stdarg.h`, `limits.h`, etc.).

For this, you need to add a Makefile in the root directory of the application project:

```plaintext
NAME = app-name
SRCS = src/main.c src/help.c src/lib.cpp
include $(AM_HOME)/Makefile.app