#include <unistd.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "syscall.h"

// helper macros
#define _concat(x, y) x ## y
#define concat(x, y) _concat(x, y)
#define _args(n, list) concat(_arg, n) list
#define _arg0(a0, ...) a0
#define _arg1(a0, a1, ...) a1
#define _arg2(a0, a1, a2, ...) a2
#define _arg3(a0, a1, a2, a3, ...) a3
#define _arg4(a0, a1, a2, a3, a4, ...) a4
#define _arg5(a0, a1, a2, a3, a4, a5, ...) a5

// extract an arguments from the macro array
#define SYSCALL  _args(0, ARGS_ARRAY)
#define GPR1 _args(1, ARGS_ARRAY)
#define GPR2 _args(2, ARGS_ARRAY)
#define GPR3 _args(3, ARGS_ARRAY)
#define GPR4 _args(4, ARGS_ARRAY)
#define GPRx _args(5, ARGS_ARRAY)

#ifndef __ISA_RISCV32__
#define __ISA_RISCV32__
#endif

// ISA-depedent definitions
#if defined(__ISA_X86__)
# define ARGS_ARRAY ("int $0x80", "eax", "ebx", "ecx", "edx", "eax")
#elif defined(__ISA_MIPS32__)
# define ARGS_ARRAY ("syscall", "v0", "a0", "a1", "a2", "v0")
#elif defined(__ISA_RISCV32__)
# define ARGS_ARRAY ("ecall", "a7", "a0", "a1", "a2", "a0")
#elif defined(__ISA_AM_NATIVE__)
# define ARGS_ARRAY ("call *0x100000", "rax", "rdi", "rsi", "rdx", "rax")
#else
#error syscall is not supported
#endif

intptr_t _syscall_(intptr_t type, intptr_t a0, intptr_t a1, intptr_t a2) {
  register intptr_t _gpr1 asm (GPR1) = type;
  register intptr_t _gpr2 asm (GPR2) = a0;
  register intptr_t _gpr3 asm (GPR3) = a1;
  register intptr_t _gpr4 asm (GPR4) = a2;
  register intptr_t ret asm (GPRx);
  asm volatile (SYSCALL : "=r" (ret) : "r"(_gpr1), "r"(_gpr2), "r"(_gpr3), "r"(_gpr4));
  return ret;
}

void _exit(int status) {
  _syscall_(SYS_exit, status, 0, 0);
  while (1);
}

int _open(const char *path, int flags, mode_t mode) {
  // _exit(SYS_open);
  int ret=_syscall_(SYS_open,(intptr_t)path,flags,mode);
  return ret;
}

int _write(int fd, void *buf, size_t count) {
  // _exit(SYS_write);
  // printf("fd:%d, buf:0x%x, count:%d\n",fd,(uint32_t)buf,(int)count);
  // printf("fd:%d\n",fd);
  int ret = _syscall_(SYS_write,fd,(intptr_t)buf,count);
  return ret;
}

// void _puts(const char* string){
//   int i;
//   for(i=0;string[i];++i)_putc(string[i]);
// }

// void _put_int(int num,int base){
//   int stack[128],ptr=0;
//   for(;num;stack[ptr++]=num%base,num/=base);
//   for(--ptr;num>=0;stack[--ptr]){
//     if(stack[ptr]<10)_putc(stack[ptr]+'0');
//     else if(stack[ptr]<16)_putc(stack[ptr]-10+'a');
//     else  assert(0);
//   }
// }
void *_sbrk(intptr_t increment) {
  // static void* program_break = (uintptr_t)&end;
  // void* old = program_break;
  // if(_syscall_(SYS_brk,program_break+increment,0,0)==0){
  //   program_break += increment;
  //   return old;
  // }
  // return (void*)-1;
  extern intptr_t end;
  static intptr_t program_break = (intptr_t)&end;
  int t = program_break;
  
  int res = _syscall_(SYS_brk, t, increment, 0);
  if (res == 0) {
    program_break += increment;
    return (void *)t;
  } else {
    return (void *)-1;
  }
}

int _read(int fd, void *buf, size_t count) {
  // _exit(SYS_read);
  int ret = _syscall_(SYS_read,fd,(intptr_t)buf,count);
  return ret;
}

int _close(int fd) {
  // _exit(SYS_close);
  int ret = _syscall_(SYS_close,fd,0,0);
  return ret;
}

off_t _lseek(int fd, off_t offset, int whence) {
  // _exit(SYS_lseek);
  off_t ret = _syscall_(SYS_lseek,fd,offset,whence);
  return ret;
}

int _execve(const char *fname, char * const argv[], char *const envp[]) {
  // _exit(SYS_execve);
  int ret = _syscall_(SYS_execve,(intptr_t)fname,(intptr_t)argv,(intptr_t)envp);
  return ret;
}

// The code below is not used by Nanos-lite.
// But to pass linking, they are defined as dummy functions

int _fstat(int fd, struct stat *buf) {
  int ret = _syscall_(SYS_fstat,fd,(intptr_t)buf,0);
  return ret;
}

int _kill(int pid, int sig) {
  // _exit(-SYS_kill);
  int ret = _syscall_(SYS_kill,pid,sig,0);
  return ret;
}

pid_t _getpid() {
  // _exit(-SYS_getpid);
  pid_t ret = _syscall_(SYS_getpid,0,0,0);
  return ret;
}

pid_t _fork() {
  pid_t ret = _syscall_(SYS_fork,0,0,0);
  return -1;
}

int _link(const char *d, const char *n) {
  int ret = _syscall_(SYS_link,(intptr_t)d,(intptr_t)n,0);
  return ret;
}

int _unlink(const char *n) {
  int ret = _syscall_(SYS_unlink,(intptr_t)n,0,0);
  return ret;
}

pid_t _wait(int *status) {
  pid_t ret = _syscall_(SYS_wait,(intptr_t)status,0,0);
  return -1;
}

clock_t _times(void *buf) {
  clock_t ret = _syscall_(SYS_times,(intptr_t)buf,0,0);
  return 0;
}

int _gettimeofday(struct timeval *tv) {
  int ret = _syscall_(SYS_gettimeofday,0,0,0);
  tv->tv_sec = ret;
  tv->tv_usec = 0;
  return 0;
}
