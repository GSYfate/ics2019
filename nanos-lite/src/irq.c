#include "common.h"
#include "proc.h"

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case _EVENT_NULL: {
      printf("event null\n");
      break;
    }
    case _EVENT_ERROR:{
      printf("event error\n");
      break;
    }
    case _EVENT_IRQ_TIMER:{
      printf("event irq timer\n");
      break;
    }
    case _EVENT_IRQ_IODEV:{
      printf("evnet irq iodev\n");
      break;
    }
    case _EVENT_PAGEFAULT:{
      printf("event page fault\n");
      break;
    }
    case _EVENT_YIELD:{
      printf("event yield\n");
      c=schedule(c);
      //printf("after schedule epc:0x%x\n",c->epc);
      break;
    }
    case _EVENT_SYSCALL:{
      do_syscall(c);
      break;
    }
    default: panic("Unhandled event ID = %d", e.event);
  }
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
