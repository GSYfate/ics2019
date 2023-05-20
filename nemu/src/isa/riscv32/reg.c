#include "nemu.h"

const char *regsl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const char *csr_regl[] = {"sstatus","stvec","sepc","scause","satp"};

rtlreg_t* csr_name2reg(int r){
  switch (r)
  {
  case 0x100:return &reg_csrl(0);//sstatus
  case 0x105:return &reg_csrl(1);//stvec
  case 0x141:return &reg_csrl(2);//sepc
  case 0x142:return &reg_csrl(3);//scause
  case 0x180:return &reg_csrl(4);//satp
  default:
    assert(0);
  }
  return NULL;
}


void isa_reg_display() {
  int i;
  for(i=0;i<32;++i){
    if(i%4==0)
      printf("\n");
    printf("[%2d]%4s:0x%-12x\t",i,regsl[i],(unsigned int)cpu.gpr[i]._32);
  }
  printf("\n");
}

void isa_csr_reg_display(){
  int i;
  for(i=0;i<5;++i){
    printf("%6s:0x%-12x\t",csr_regl[i],(uint32_t)cpu.csr[i]._32);
  }
  printf("\n");
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  int i;
  for(i=0;i<32;++i){
    if(!strcmp(s,regsl[i])){
      success=true;
      return cpu.gpr[i]._32;
    }
  }
  success=false;
  return 0;
}
