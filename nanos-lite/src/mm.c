#include "memory.h"
#include "proc.h"
#include "common.h"

static void *pf = NULL;
#define PGSIZE  4096

void* new_page(size_t nr_page) {
  printf("new_page:%d\n",nr_page);
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}


/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  printf("mm_brk\n");
  if (current->max_brk < brk) {
    current->max_brk = brk / PGSIZE * PGSIZE;
    if (brk % PGSIZE) current->max_brk++;
  }
  if (current->max_brk < brk + increment) {
    int block_count = (brk + increment - current->max_brk) / PGSIZE;
    if ((brk + increment - current->max_brk) % PGSIZE) block_count++;
    for (int i = 0; i < block_count; i++) {
      uint32_t p_pg = new_page(1);
      uint32_t v_pg = current->max_brk;
      _map(&current->as, v_pg, p_pg, 0);
      printf("brk_map 0x%x->0x%x\n",v_pg,p_pg);
      current->max_brk += PGSIZE;
    }
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from 0x%x", pf);

  _vme_init(new_page, free_page);
}
