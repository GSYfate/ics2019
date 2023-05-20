#include "nemu.h"
// #define ORIGIN

#define PGSIZE 4096
#define NR_PDE    1024    // # directory entries per page directory
#define NR_PTE    1024    // # PTEs per page table
#define PGSHFT    12      // log2(PGSIZE)
#define PTXSHFT   12      // Offset of PTX in a linear address
#define PDXSHFT   22      // Offset of PDX in a linear address
// Page table/directory entry flags
#define PTE_V 0x01
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10
typedef uint32_t PTE;
typedef uint32_t PDE;
#define PDX(va)     (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)     (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)     ((uint32_t)(va) & 0xfff)
#define PTE_ADDR(pte)   (((uint32_t)(pte) & ~0x3ff) << 2)
#define ROUNDUP(a, sz)   ((((uintptr_t)a)+(sz)-1) & ~((sz)-1))
#define ROUNDDOWN(a, sz) ((((uintptr_t)a)) & ~((sz)-1))
// construct virtual address from indexes and offset
#define PGADDR(d, t, o) ((uint32_t)((d) << PDXSHFT | (t) << PTXSHFT | (o)))


uint32_t read_satp(uint32_t *mode){
  uint32_t satp = cpu.csr[4]._32;
  *mode = ((satp>>31)&0x1);
  uint32_t pgdir = (satp&0x3fffff)<<12;
  return pgdir;
}
uint32_t read_mode(){
  uint32_t mode;
  read_satp(&mode);
  return mode;
}
uint32_t read_ppn(){
  uint32_t mode;
  return read_satp(&mode);
}

static int error=0;
// #define FLAG 
paddr_t page_translate(vaddr_t vaddr){
#ifdef FLAG
  int flag = (vaddr>>30)&0x1;
  if(flag){
    printf("page_translate 0x%x ",vaddr);
  }
#endif
  uint32_t* pdir = (uint32_t*)read_ppn();
#ifdef FLAG
  if(flag){
    printf("pdir:0x%x ",pdir);
  }
#endif
  uint32_t pde = (uint32_t)paddr_read(pdir+PDX(vaddr),4);
#ifdef FLAG
  if(flag){
    printf("pde:0x%x ",pde);
  }
#endif
  if(!(((uint32_t)pde)&PTE_V)){
    printf("page_translate pde:0x%x\n", pde);
    error = 1;
    return 0;
    //assert(0);
  }
  uint32_t* ptab =PTE_ADDR(pde);
#ifdef FLAG
  if(flag){
    printf("ptab:0x%x ",ptab);
  }
#endif
  uint32_t pte = paddr_read(ptab+PTX(vaddr),4);
#ifdef FLAG
  if(flag){
    printf("pte:0x%x ",pte);
  }
#endif
  if(!(((uint32_t)pte)&PTE_V)){
      printf("page_translate pde:0x%x 0x%x[%d] pte:0x%x 0x%x[%d]\n", pde,pdir,PDX(vaddr),pte,ptab,PTX(vaddr));
      error = 1;
      return 0;
      //assert(0);
  }
  uint32_t paddr = ((pte&~0x3ff)<<2)|OFF(vaddr);
#ifdef FLAG
  if(flag){
    printf("paddr:0x%x\n",paddr);
  }
#endif
  return paddr;
}


int32_t is_cross_page(vaddr_t addr,int len){
  return (addr&(~0xfff))!=((addr+len-1)&(~0xfff));
}



uint32_t isa_vaddr_read(vaddr_t addr, int len) {
#ifdef FLAG
    int flag = (addr>>30)&0x1;
#endif
    //printf("isa_vaddr_read(0x%x, %d)\n",addr,len);
    if(len<1||len>4){
      printf("isa_vaddr_read(0x%x, %d)\n",addr,len);
    }
#ifdef ORIGIN
    int mode = 0;
#else
    int mode = read_mode();
#endif
#ifdef FLAG
    if(flag){
      printf("isa_vaddr_read 0x%x mode:%x\n",addr,mode);
    }
#endif
    if(mode==0){
      return paddr_read(addr, len);
    }else{
      int cross_page = is_cross_page(addr,len);
      if(cross_page){
        //printf("isa_vaddr_read cross_page (0x%x,%d)\n",addr,len);
        uint32_t data = 0;
        int left_len = ROUNDUP(addr, PGSIZE) - addr;
        int right_len = len - left_len;
        paddr_t paddr1 = page_translate(addr);
        data = paddr_read(paddr1, left_len);
        paddr1 = page_translate(ROUNDUP(addr, PGSIZE));
        data |= (paddr_read(paddr1, right_len) << (left_len * 8));
        return data;
      }else{
        //printf("isa_vaddr_read one_page\n");
        paddr_t paddr = page_translate(addr);
        if(error==1){
          printf("isa_vaddr_read(0x%x, %d)\n",addr,len);
          assert(0);
        }
        return paddr_read(paddr,len);
      }
    }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
#ifdef FLAG
    int flag = (addr>>30)&0x1;
#endif
    if(len<1||len>4){
      printf("isa_vaddr_write(0x%x, 0x%x, %d)\n",addr,data,len);
    }
#ifdef ORIGIN
    int mode = 0;
#else
    int mode  = read_mode();
#endif
#ifdef FLAG
    if(flag){
      printf("isa_vaddr_write 0x%x mode:%x ",addr,mode);
    }
#endif
    if(mode==0){  
      paddr_write(addr, data, len);
    }else{
      int cross_page = is_cross_page(addr,len);
#ifdef FLAG
      if(flag){
        printf("cross page:%d\n",cross_page);
      }
#endif
      if(cross_page){
         int left_len = ROUNDUP(addr, PGSIZE) - addr;
        int right_len = len - left_len;
        paddr_t paddr1 = page_translate(addr);
        if(left_len<1||left_len>4){
          printf("isa_vaddr_write(0x%x, 0x%x, %d) left_len:%d\n",addr,data,len,left_len);
        }
        if(right_len<1||right_len>4){
          printf("isa_vaddr_write(0x%x, 0x%x, %d) right_len:%d\n",addr,data,len,right_len);
        }
        paddr_write(paddr1, data & ((1 << (left_len * 8)) - 1), left_len);
        paddr1 = page_translate(ROUNDUP(addr, PGSIZE));
        paddr_write(paddr1, (data >> (left_len * 8)) & ((1 << (right_len * 8)) - 1), right_len);
        return;
      }else{
        paddr_t paddr = page_translate(addr);
        if(error==1){
          printf("isa_vaddr_write(0x%x, %d)\n",addr,len);
          assert(0);
        }
        return paddr_write(paddr,data,len);
      }
    }
}
    

