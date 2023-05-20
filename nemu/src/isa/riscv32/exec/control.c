#include "cpu/exec.h"

// #define DISPLAY 
// #undef DISPLAY

make_EHelper(jal) {
#ifdef DISPLAY
    printf("jal r%d 0x%x\n",id_dest->reg,id_src->imm+cpu.pc);
#endif    
    rtl_li(&s0,id_src->imm);
    rtl_addi(&s0,&s0,cpu.pc);
    rtl_li(&id_dest->val,cpu.pc+4);
    rtl_jr(&s0);
    rtl_sr(id_dest->reg,&id_dest->val,4);
    print_asm_template2(jal);
}

make_EHelper(jalr){
#ifdef DISPLAY
    printf("jalr r%d r%d 0x%x\n",id_dest->reg,id_src->reg,id_src2->imm);
#endif   
    rtl_addi(&s0,&id_src->val,id_src2->imm);
    rtl_li(&id_dest->val,cpu.pc+4);
    rtl_jr(&s0);
    rtl_sr(id_dest->reg,&id_dest->val,4);
    print_asm_template2(jalr);
}

make_EHelper(beq){
#ifdef DISPLAY
    printf("beq r%d r%d 0x%x\n",id_src->reg,id_src2->reg,id_dest->imm+cpu.pc);
#endif
    rtl_jrelop(RELOP_EQ,&id_src->val,&id_src2->val,id_dest->imm+cpu.pc);
    print_asm_template2(beq);
}

make_EHelper(bne){
#ifdef DISPLAY
    printf("bne r%d r%d 0x%x\n",id_src->reg,id_src2->reg,id_dest->imm+cpu.pc);
#endif   
    rtl_jrelop(RELOP_NE,&id_src->val,&id_src2->val,id_dest->imm+cpu.pc);
    print_asm_template2(bne);
}

make_EHelper(blt){
#ifdef DISPLAY
    printf("blt r%d r%d 0x%x\n",id_src->reg,id_src2->reg,id_dest->imm+cpu.pc);
#endif
    rtl_jrelop(RELOP_LT,&id_src->val,&id_src2->val,id_dest->imm+cpu.pc);
    print_asm_template2(blt);
}

make_EHelper(bge){
#ifdef DISPLAY
    printf("bge r%d r%d 0x%x\n",id_src->reg,id_src2->reg,id_dest->imm+cpu.pc);
#endif    
    rtl_jrelop(RELOP_GE,&id_src->val,&id_src2->val,id_dest->imm+cpu.pc);
    print_asm_template2(bge);
}

make_EHelper(bltu){
#ifdef DISPLAY
    printf("bltu r%d r%d 0x%x\n",id_src->reg,id_src2->reg,id_dest->imm+cpu.pc);
#endif
    rtl_jrelop(RELOP_LTU,&id_src->val,&id_src2->val,id_dest->imm+cpu.pc);
    print_asm_template2(bltu);
}

make_EHelper(bgeu){
#ifdef DISPLAY
    printf("bgeu r%d r%d 0x%x\n",id_src->reg,id_src2->reg,id_dest->imm+cpu.pc);
#endif    
    rtl_jrelop(RELOP_GEU,&id_src->val,&id_src2->val,id_dest->imm+cpu.pc);
    print_asm_template2(bgeu);
}