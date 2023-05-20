#include "cpu/exec.h"
// #define DISPLAY
// #undef DISPLAY
#define SSTATUS 0x100
#define STVEC    0x105
#define SEPC   0x141
#define SCAUSE  0x142
#define SATP    0x180

#define STATUS_U 2
#define STATUS_S 1
#define STATUS_M 0

make_EHelper(csrrw){
#ifdef DISPLAY
    printf("csrrw[%x] r%d r%d\n",id_src2->imm,id_dest->reg,id_src->reg);
#endif
    rtl_csr_lr(&s0,id_src2->imm);
    rtl_sr(id_dest->reg,&s0,4);
    rtl_csr_sr(id_src2->imm,&id_src->val);
}

make_EHelper(csrrs){
#ifdef DISPLAY
    printf("csrrs[%x] r%d r%d\n",id_src2->imm,id_dest->reg,id_src->reg);
#endif 
    rtl_csr_lr(&s0,id_src2->imm);
    rtl_sr(id_dest->reg,&s0,4);
    rtl_li(&s0,s0|id_src->val);
    rtl_csr_sr(id_src2->imm,&s0);
}

make_EHelper(csrrc){
#ifdef DISPLAY
    printf("csrrc[%x] r%d r%d\n",id_src2->imm,id_dest->reg,id_src->reg);
#endif 
    rtl_csr_lr(&s0,id_src2->imm);
    rtl_sr(id_dest->reg,&s0,4);
    rtl_li(&s0,s0&~id_src->val);
    rtl_csr_sr(id_src2->imm,&s0);
}

make_EHelper(csrrwi){
#ifdef DISPLAY
    printf("csrwi[%x] r%d 0x%x\n",id_src2->imm,id_dest->reg,id_src->imm);
#endif 
    rtl_csr_lr(&s0,id_src2->imm);
    rtl_sr(id_dest->reg,&s0,4);
    rtl_li(&s0,id_src->imm);
    rtl_csr_sr(id_src2->imm,&s0);
}

make_EHelper(csrrsi){
#ifdef DISPLAY
    printf("csrsi[%x] r%d 0x%x\n",id_src2->imm,id_dest->reg,id_src->imm);
#endif 
    rtl_csr_lr(&s0,id_src2->imm);
    rtl_sr(id_dest->reg,&s0,4);
    rtl_li(&s0,id_src->imm|s0);
    rtl_csr_sr(id_src2->imm,&s0);   
}

make_EHelper(csrrci){
#ifdef DISPLAY
    printf("csrci[%x] r%d 0x%x\n",id_src2->imm,id_dest->reg,id_src->imm);
#endif 
    rtl_csr_lr(&s0,id_src2->imm);
    rtl_sr(id_dest->reg,&s0,4);
    rtl_li(&s0,id_src->imm|~s0);
    rtl_csr_sr(id_src2->imm,&s0);   
}


make_EHelper(ecall){
#ifdef DISPLAY
    printf("ecall\n");
#endif
    rtl_li(&s0,cpu.pc+4);
    rtl_csr_sr(SEPC,&s0);
    rtl_li(&s0,isa_reg_str2val("a7"));
    rtl_csr_sr(SCAUSE,&s0);
    rtl_csr_lr(&s0,STVEC);
    rtl_jr(&s0);
}

make_EHelper(sret){
#ifdef DISPLAY
    printf("sret\n");
#endif
    rtl_csr_lr(&s0,SEPC);
    rtl_jr(&s0);
}