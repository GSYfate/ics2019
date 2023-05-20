#include "cpu/exec.h"

const char x[5] = {0,'b','h',0,'w'};

// #define DISPLAY
// #undef DISPLAY

make_EHelper(ld) {
#ifdef DISPLAY
  printf("l%cu r%d 0x%x\n",x[decinfo.width],id_dest->reg,id_src->addr);
#endif
  rtl_lm(&s0, &id_src->addr, decinfo.width);
  rtl_sr(id_dest->reg, &s0, 4);

  switch (decinfo.width) {
    case 4: print_asm_template2(lw); break;
    case 2: print_asm_template2(lhu); break;
    case 1: print_asm_template2(lbu); break;
    default: assert(0);
  }
}

make_EHelper(lds){
#ifdef DISPLAY
  printf("l%c r%d 0x%x\n",x[decinfo.width],id_dest->reg,id_src->addr);
#endif
  rtl_lm(&s0, &id_src->addr, decinfo.width);
  rtl_sext(&s0, &s0, decinfo.width);
  rtl_sr(id_dest->reg,&s0,4);
  switch (decinfo.width) {
    case 4: print_asm_template2(lw); assert(0);
    case 2: print_asm_template2(lh); break;
    case 1: print_asm_template2(lb); break;
    default: assert(0);
  }
}

make_EHelper(st) {
#ifdef DISPLAY
  printf("s%c r%d 0x%x\n",x[decinfo.width],id_dest->reg,id_src->addr);
#endif
  rtl_sm(&id_src->addr, &id_dest->val, decinfo.width);

  switch (decinfo.width) {
    case 4: print_asm_template2(sw); break;
    case 2: print_asm_template2(sh); break;
    case 1: print_asm_template2(sb); break;
    default: assert(0);
  }
}
