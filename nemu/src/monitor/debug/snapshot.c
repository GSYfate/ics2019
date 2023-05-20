#include "monitor/snapshot.h"
// #include "memory/memory.h"
#include "nemu.h"
#include <stdio.h>
int get_pc(){
    return cpu.pc;
}

int save_snapshot(const char* fpath){
    FILE* fp = fopen(fpath,"w");
    assert(fp);
    int i;
    fprintf(fp,"0x%x\n",cpu.pc);
    for(i=0;i<32;++i){
        fprintf(fp,"0x%x ",(uint32_t)cpu.gpr[i]._32);
    }
    fprintf(fp,"\n");
    for(i=0;i<5;++i){
        fprintf(fp,"0x%x ",(uint32_t)cpu.csr[i]._32);
    }
    fprintf(fp,"\n");
    for(i=0;i<PMEM_SIZE/4;++i){
        fprintf(fp,"0x%x ",paddr_read(0x80000000u+i*4, 4));
    }
    fclose(fp);
    return 1;
}
int load_snapshot(const char* fpath){
    FILE* fp = fopen(fpath,"r");
    assert(fp);
    int i;
    fscanf(fp,"%x",&(cpu.pc));
    for(i=0;i<32;++i){
        fscanf(fp,"%x",&(cpu.gpr[i]._32));
    }
    for(i=0;i<5;++i){
        fscanf(fp,"%x",&(cpu.csr[i]._32));
    }
    uint32_t data;
    for(i=0;i<PMEM_SIZE/4;++i){
        fscanf(fp,"%x",&data);
        //printf("addr:0x%x value:0x%x\n",0x80000000u+i*4, data, 4);
        paddr_write(0x80000000u+i*4, data, 4);
    }
    fclose(fp);
    return 1;
}