#include "cpu/exec.h"
#include "all-instr.h"

static OpcodeEntry load_table [8] = {
  EXW(lds,1), EXW(lds,2), EXW(ld, 4), EMPTY, EXW(ld,1), EXW(ld, 2), EMPTY, EMPTY
};

static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry store_table [8] = {
  EXW(st,1), EXW(st,2), EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}


/*
    Workplace
*/

static OpcodeEntry computei_table_0 [8] = {
  EX(addi), EX(slli), EX(slti), EX(sltiu), EX(xori), EX(srli), EX(ori), EX(andi)
};
static OpcodeEntry computei_table_1 [8] = {
  EX(addi), EX(slli), EX(slti),EX(sltiu), EX(xori), EX(srai), EX(ori), EX(andi)
};
static make_EHelper(computei){
  if(decinfo.isa.instr.funct7==0x20){
    idex(pc,&computei_table_1[decinfo.isa.instr.funct3]);
  }else{
    idex(pc,&computei_table_0[decinfo.isa.instr.funct3]);
  }
}

static OpcodeEntry compute_table_0 [8]={
  EX(add),EX(sll),EX(slt),EX(sltu),EX(xor),EX(srl),EX(or),EX(and)
};
static OpcodeEntry compute_table_1 [8]={
  EX(sub),EMPTY,EMPTY,EMPTY,EMPTY,EX(sra),EMPTY,EMPTY,EMPTY
};
static OpcodeEntry compute_table_2 [8]={
  EX(mul),EX(mulh),EX(mulhsu),EX(mulhu),EX(div),EX(divu),EX(rem),EX(remu)
};

static make_EHelper(compute){
  if(decinfo.isa.instr.funct7==0){
    idex(pc,&compute_table_0[decinfo.isa.instr.funct3]);
  }else if(decinfo.isa.instr.funct7==0x1){
    idex(pc,&compute_table_2[decinfo.isa.instr.funct3]);
  }else if(decinfo.isa.instr.funct7==0x20){
    idex(pc,&compute_table_1[decinfo.isa.instr.funct3]);
  }else{
    printf("instr error\n");
    assert(0);
  }
}

static OpcodeEntry branch_table [8] = {
  EX(beq), EX(bne), EMPTY,EMPTY,EX(blt), EX(bge), EX(bltu), EX(bgeu)
};
static make_EHelper(branch){
  idex(pc, &branch_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry system_table_0 [8] = {
  EX(ecall),EX(csrrw),EX(csrrs),EX(csrrc),EMPTY,EX(csrrwi),EX(csrrsi),EX(csrrci)
};
static OpcodeEntry system_table_1 [8] = {
  EX(sret),EX(csrrw),EX(csrrs),EX(csrrc),EMPTY,EX(csrrwi),EX(csrrsi),EX(csrrci)
};
static make_EHelper(system){

  if(decinfo.isa.instr.funct7==0x8&&decinfo.isa.instr.rs1==0){
    idex(pc,&system_table_1[decinfo.isa.instr.funct3]);
  }else{
    idex(pc,&system_table_0[decinfo.isa.instr.funct3]);
  }

}

/*
end of work place
*/

static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY,EMPTY,IDEX(computei,computei), IDEX(U,auipc), EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R,compute), IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  
  /* b11 */ IDEX(B,branch), IDEX(I,jalr), EX(nemu_trap), IDEX(J,jal), IDEX(system,system), EMPTY, EMPTY, EMPTY,
};

// #ifndef DISPLAY
// #define DISPLAY
// #endif
// #undef DISPLAY
void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  // printf("inside isa_exec\n");
  // printf("opcode6_2:%d\n",decinfo.isa.instr.opcode6_2);
#ifdef DISPLAY 
  printf("pc:%x  instr:0x%x\n",cpu.pc,decinfo.isa.instr.val);
#endif
  if(decinfo.isa.instr.opcode1_0 != 0x3){
    printf("Error pc:%x  instr:0x%x\n",cpu.pc,decinfo.isa.instr.val);
  }
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
#ifdef DISPLAY
#ifdef DISPLAY_REG
  isa_reg_display();
  isa_csr_reg_display();
#endif
#endif
  //if(cpu.pc==0x801000c4)assert(0);
  // if(decinfo.isa.instr.val==0xfeb612e3)assert(0);
}
