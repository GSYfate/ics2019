#include "cpu/exec.h"
// #define DISPLAY
// #undef DISPLAY

make_EHelper(mul){
#ifdef DISPLAY
    printf("mul r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
    rtl_imul_lo(&id_dest->val,&id_src->val,&id_src2->val);
    rtl_sr(id_dest->reg,&id_dest->val,4);
}

make_EHelper(mulh){
#ifdef DISPLAY
    printf("mulh r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
    rtl_imul_hi(&id_dest->val,&id_src->val,&id_src2->val);
    rtl_sr(id_dest->reg,&id_dest->val,4);
}

make_EHelper(mulhsu){
#ifdef DISPLAY
    printf("mulhsu r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
    rtl_imul_hi(&id_dest->val,&id_src->val,&id_src2->val);
    rtl_sr(id_dest->reg,&id_dest->val,4);
}

make_EHelper(mulhu){
#ifdef DISPLAY
    printf("mulhu r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
    rtl_mul_hi(&id_dest->val,&id_src->val,&id_src2->val);
    rtl_sr(id_dest->reg,&id_dest->val,4);
}


make_EHelper(div){
#ifdef DISPLAY
    printf("div r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
    rtl_idiv_q(&id_dest->val,&id_src->val,&id_src2->val);
    rtl_sr(id_dest->reg,&id_dest->val,4);
}

make_EHelper(divu){
#ifdef DISPLAY
    printf("divu r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
    rtl_div_q(&id_dest->val,&id_src->val,&id_src2->val);
    rtl_sr(id_dest->reg,&id_dest->val,4);
}

make_EHelper(rem){
#ifdef DISPLAY
    printf("rem r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
    rtl_idiv_r(&id_dest->val,&id_src->val,&id_src2->val);
    rtl_sr(id_dest->reg,&id_dest->val,4);
}

make_EHelper(remu){
#ifdef DISPLAY
    printf("rem r%d r%d r%d\n",id_dest->reg,id_src->reg,id_src2->reg);
#endif
    rtl_div_r(&id_dest->val,&id_src->val,&id_src2->val);
    rtl_sr(id_dest->reg,&id_dest->val,4);
}