#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include "common.h"

#define PC_START (0x80000000u + IMAGE_START)
#define NR_CSR 5

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  struct {
    rtlreg_t _32;
  } csr[NR_CSR];

  vaddr_t pc;

} CPU_state;



static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 32);
  return index;
}
static inline int check_csr_reg_index(int index){
  assert(index >=0 && index<NR_CSR);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_csrl(index) (cpu.csr[check_csr_reg_index(index)]._32)

static inline const char* reg_name(int index, int width) {
  extern const char* regsl[];
  assert(index >= 0 && index < 32);
  return regsl[index];
}

rtlreg_t* csr_name2reg(int r);






#endif
