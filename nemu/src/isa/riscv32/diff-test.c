#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // if(decinfo.isa.instr.opcode6_2==0x19){
  //   difftest_skip_dut(1, 2);
  // }
  int i;
  for(i=0;i<32;++i){
    if(ref_r->gpr[i]._32!=cpu.gpr[i]._32)
      return false;
  }
  if(ref_r->pc!=cpu.pc)
    return false;
  return true;
}

void isa_difftest_attach(void) {
}
