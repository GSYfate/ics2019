#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void _puts(const char* string){
  char*  p = string;
  for(p=string;*p;++p)_putc(*p);
}

size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
      int k = *(int*)0xa1000060;
      kbd->keydown = k?1:0;
      kbd->keycode = k;
      //_puts("after __am_input_read\n");
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}
