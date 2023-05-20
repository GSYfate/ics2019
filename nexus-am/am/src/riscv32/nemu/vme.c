#include <am.h>
#include <riscv32.h>
#include <nemu.h>
#include <klib.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

#define PGSIZE 4096
#define PGMASK          (PGSIZE - 1)    // Mask for bit ops
#define PGROUNDDOWN(a)  (((a)) & ~PGMASK)
#define PGROUNDUP_GT(sz)  (PGROUNDDOWN(sz)+PGSIZE)

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0x80000000u, .end = (void*)(0x80000000u + PMEM_SIZE)},
  {.start = (void*)MMIO_BASE,   .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

static inline void set_satp(void *pdir) {
  printf("set satp 0x%x\n",pdir);
  asm volatile("csrw satp, %0" : : "r"(0x80000000 | ((uintptr_t)pdir >> 12)));
}

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;
  //printf("vme_init pgalloc_usr:0x%x\n",pgalloc_usr);
  // make all PDEs invalid
  int i;
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      //  虚拟地址 4(useless)|10(dir)|8(tab)|10(status)
      kpdirs[pdir_idx] = ((uintptr_t)ptab >> PGSHFT << 10) | PTE_V;

      // fill PTE
      //  2(useless)|10(dir)|10(tab)|10(status)
      PTE pte = (PGADDR(pdir_idx, 0, 0) >> PGSHFT << 10) | PTE_V | PTE_R | PTE_W | PTE_X;
      PTE pte_end = (PGADDR(pdir_idx + 1, 0, 0) >> PGSHFT << 10) | PTE_V | PTE_R | PTE_W | PTE_X;
      for (; pte < pte_end; pte += (1 << 10)) { // NR_PTE
        *ptab = pte;
        ptab ++;
      }
    }
  }
  
  set_satp(kpdirs);
  vme_enable = 1;
  return 0;
}

int _protect(_AddressSpace *as) {
  printf("_protect\n");
  PDE *updir = (PDE*)(pgalloc_usr(1));
  //printf("here1\n");
  as->ptr = updir;
  //printf("here2\n");
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as) {
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c) {
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  printf("__am_switch\n");
  if (vme_enable) {
    set_satp(c->as->ptr);
    cur_as = c->as;
  }
}

/*
  va 2 pa 
  pdir

  uint32_t pdx = PDX(vaddr);
  uint32_t ptx = PTX(vaddr);
  uint32_t off = OFF(vaddr)
  PDE pde = pdir[pdx];
  PTE* ptab = PTE_ADDR(pde)
  PTE pte = ptab[ptx]
  paddr = PGADDR(PDX(pte), PTX(pte), off);
*/
/*
typedef struct _AddressSpace {
  size_t pgsize;
  _Area area;  //vaddr
  void *ptr;   //pdir
} _AddressSpace;
*/
int _map(_AddressSpace *as, void *va, void *pa, int prot) {
  //printf("_map vaddr:0x%x paddr:0x%x\n",va,pa);
  if(((uint32_t)pa>>12<<12)!=(uint32_t)pa){
    printf("_map pa error:0x%x\n",pa);
    assert(0);
  }
  uint32_t* pdir = (uint32_t*)(as->ptr);
  uint32_t pde = pdir[PDX(va)];
  //printf("PDX[0x%x] PTX[0x%x] OFF[0x%x] \n",PDX(va),PTX(va),OFF(va));
  
  if((pde&PTE_V)==0){
    uint32_t *new_pg = pgalloc_usr(1);
    memset(new_pg,0,PGSIZE);
    pdir[PDX(va)] = ((uint32_t)new_pg>>12<<10) | PTE_V;
    pde=pdir[PDX(va)];
  }
  //printf("pde:%x\n",pde);
  uint32_t *ptab = PTE_ADDR(pde);
  uint32_t pte = ptab[PTX(va)];
  //printf("pte:0x%x\n",pte);
  if((pte&PTE_V)==0){
    ptab[PTX(va)]=((((uint32_t)pa)&(~0xfff))>>2)|PTE_V;
    pte = ptab[PTX(va)];
    printf("_map vaddr 0x%x =pde:0x%x(0x%x,[%d])=pte:0x%x(0x%x[%d])= paddr:0x%x\n",va,pde,pdir,PDX(va),pte,ptab,PTX(va),pa);
    return 0;
  }else{
    printf("pte already valid: pde:0x%x pte:0x%x vaddr:0x%x paddr:0x%x\n",pde,pte,va,pa);
    return -1;
  }

  //uint32_t *pte = ((uint32_t*)(as->ptr))[PDX(va)];
  // if(!(((uint32_t)pte)&PTE_V)){
  //   uint32_t *new_pg = pgalloc_usr(1);
  //   memset(new_pg,0,PGSIZE);
  //   ((uint32_t*)(as->ptr))[PDX(va)] = ((uint32_t)new_pg) | PTE_V;
  //   pte = ((uint32_t*)(as->ptr))[PDX(va)];
  // }
  // uint32_t* pgentry = ((uint32_t*)(PTE_ADDR(pte)))[PTX(va)];
  // if((uint32_t)pgentry&PTE_V){
  //   return PTE_ADDR(pgentry);
  // }
  // pa=pgalloc_usr(1);
  // ((uint32_t*)(PTE_ADDR(pte)))[PTX(va)]=((uint32_t)pa)|PTE_V;
  // return pa;
}


_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
  printf("_ucontext as->ptr:0x%x\n",as->ptr);
  _Context* cp = ((_Context*)ustack.end)-1;
  memset(cp,0,sizeof(_Context));
  //_Context* cp = (_Context*)kstack.start+sizeof(_Context);
  cp->as = as;
  cp->epc = entry;
  int32_t* arg = args;
  cp->gpr[10]=(uintptr_t)arg[0];
  if(arg[0]>0){
    cp->gpr[11]=(uintptr_t)arg[1];
  }
  return cp;
}
