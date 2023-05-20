
#ifndef __PROC_H__
#define __PROC_H__


#include "common.h"
#include "memory.h"

#define STACK_SIZE (8 * PGSIZE)

typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN;
  struct {
    _Context *cp;
    _AddressSpace as;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
  };
  uint32_t index;
} PCB;
PCB* new_proc();
extern PCB *current;
_Context* schedule(_Context *prev);
#endif
