#include <am.h>
#include <riscv32.h>
#include <klib.h>

static _Context* (*user_handler)(_Event, _Context*) = NULL;



_Context* __am_irq_handle(_Context *c) {
  // printf("cause:%d sepc:0x%x status:%d\n",c->cause,c->epc,c->status);
  // printf("Context:0x%x\n",c);
  // printf("Context AreaSpace:0x%x\n",c->as);
  __am_get_cur_as(c);
  _Context *next = c;
  if (user_handler) {
    _Event ev = {0};
    switch (c->cause) {
      case -1:ev.event = _EVENT_YIELD;break;
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:ev.event = _EVENT_SYSCALL;break;
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }
  __am_switch(next);
  // uint32_t va = 0x400017e8;
  // uint32_t* pdir = next->as->ptr;
  // uint32_t pde = pdir[PDX(va)];
  // uint32_t* ptab = (pde&~0x3ff)<<2;
  // uint32_t pte = ptab[PTX(va)];
  // uint32_t pa = ((pte<<2)&~0xfff)|(va&0xfff);
  // printf("irq vaddr 0x%x =pde:0x%x(0x%x[%d])=pte:0x%x(0x%x[%d])= paddr:0x%x\n",va,pde,pdir,PDX(va),pte,ptab,PTX(va),pa);
  return next;
}

extern void __am_asm_trap(void);

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize exception entry
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  _Context *cp = (_Context *)(stack.end) - 1;
  memset(cp, 0, sizeof(_Context));
  cp->epc = (uintptr_t)entry;
  cp->gpr[10]=(uintptr_t)arg;
  return cp;
} 

void _yield() {
  asm volatile("li a7, -1; ecall");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
