#include "cpu/exec.h"

// #define DISPLAY
// #undef DISPLAY

make_EHelper(lui) {
#ifdef DISPLAY
  printf("lui r%d 0x%x\n",id_dest->reg,id_src->imm);
#endif
  rtl_sr(id_dest->reg, &id_src->imm, 4);
  print_asm_template2(lui);
}

make_EHelper(auipc){
#ifdef DISPLAY
  printf("auipc r%d 0x%x\n",id_dest->reg,id_src->imm);
#endif 
  rtl_li(&s0,id_src->imm);
  rtl_addi(&id_dest->val,&s0,cpu.pc);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(auipc);
}

make_EHelper(addi){
#ifdef DISPLAY
  printf("addi r%d r%d %d\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif
  rtl_addi(&id_dest->val,&id_src->val,id_src2->imm);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(addi);
}

make_EHelper(slti){
#ifdef DISPLAY
  printf("stli r%d r%d 0x%x\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif
  rtl_setrelopi(RELOP_LT,&id_dest->val,&id_src->val,id_src2->imm);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(slti);
}

make_EHelper(sltiu){
#ifdef DISPLAY
  printf("sltiu r%d r%d 0x%x\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif
  rtl_setrelopi(RELOP_LTU,&id_dest->val,&id_src->val,id_src2->imm);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(sltiu);
}

make_EHelper(xori){
#ifdef DISPLAY
  printf("xori r%d r%d 0x%x\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif  
  rtl_xori(&id_dest->val,&id_src->val,id_src2->imm);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(xori);
}

make_EHelper(ori){
#ifdef DISPLAY
  printf("ori r%d r%d 0x%x\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif  
  rtl_ori(&id_dest->val,&id_src->val,id_src2->imm);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(ori);
}

make_EHelper(andi){
#ifdef DISPLAY
  printf("andi r%d r%d 0x%x\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif
  rtl_andi(&id_dest->val,&id_src->val,id_src2->imm);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(andi);
}

make_EHelper(slli){
#ifdef DISPLAY
  printf("slli r%d r%d 0x%x\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif
  rtl_shli(&id_dest->val,&id_src->val,id_src2->imm);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(slli);
}

make_EHelper(srli){
#ifdef DISPLAY
  printf("srli r%d r%d 0x%x\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif  
  rtl_shri(&id_dest->val,&id_src->val,id_src2->imm);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(srli);
}

make_EHelper(srai){
#ifdef DISPLAY
  printf("srai r%d r%d 0x%x\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif  
  rtl_sari(&id_dest->val,&id_src->val,id_src2->imm);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(srai);
}




make_EHelper(add){
#ifdef DISPLAY
  printf("add r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_add(&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(add);
}
make_EHelper(sub){
#ifdef DISPLAY
  printf("sub r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_sub(&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(sub);
}
make_EHelper(sll){
#ifdef DISPLAY
  printf("sll r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_shl(&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(sll);
}
make_EHelper(slt){
#ifdef DISPLAY
  printf("slt r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_setrelop(RELOP_LT,&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(slt);
}
make_EHelper(sltu){
#ifdef DISPLAY
  printf("sltu r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_setrelop(RELOP_LTU,&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(sltu);
}
make_EHelper(xor){
#ifdef DISPLAY
  printf("xor r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_xor(&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(xor);
}
make_EHelper(srl){
#ifdef DISPLAY
  printf("srl r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_shr(&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(srl);
}
make_EHelper(sra){
#ifdef DISPLAY
  printf("sra r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_sar(&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(sra);
}
make_EHelper(or){
#ifdef DISPLAY
  printf("or r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_or(&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(or);
}
make_EHelper(and){
#ifdef DISPLAY
  printf("and r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
  rtl_and(&id_dest->val,&id_src->val,&id_src2->val);
  rtl_sr(id_dest->reg,&id_dest->val,4);
  print_asm_template2(and);
}