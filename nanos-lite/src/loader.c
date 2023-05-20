#include "memory.h"
#include "loader.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
#define USER_STACK_END 0x48000000
#define USER_STACK_SIZE 0x8000
// static uintptr_t loader(PCB *pcb, const char *filename) {
//   int fd = fs_open(filename, 0, 0);
//   Elf_Ehdr elf_header;
//   fs_read(fd, (void *)&elf_header, sizeof(Elf_Ehdr));

//   uint32_t phoff = elf_header.e_phoff;
//   uint32_t phsize = elf_header.e_phentsize;
//   uint32_t phcount = elf_header.e_phnum;
//   Elf_Phdr elf_ph_header;
//   for (int i = 0; i < phcount; i++) {
//     fs_lseek(fd, phoff + i * phsize, SEEK_SET);
//     fs_read(fd, (void *)&elf_ph_header, phsize);
//     if (elf_ph_header.p_type == PT_LOAD) {
//       // 需要加载
//       // 从文件fd的p_offset处开始的p_filesz个字节的数据
//       // 加载到内存中地址p_vaddr处开始的p_memsz字节的范围内
//       int block_count = elf_ph_header.p_memsz / PGSIZE;
//       if (elf_ph_header.p_memsz % PGSIZE) block_count++;
//       int j = 0;
//       uint32_t *paddr;
//       for(j = 0; j < block_count - 1; j++) {
//         paddr = _map(&pcb->as, (void *)(elf_ph_header.p_vaddr + j * PGSIZE), 0, 0);
//         printf("paddr:0x%x vaddr:0x%x\n",paddr,(elf_ph_header.p_vaddr + j * PGSIZE));
//         fs_lseek(fd, elf_ph_header.p_offset + j * PGSIZE, SEEK_SET);
//         fs_read(fd, (void *)paddr, PGSIZE);
//       }

//       paddr = _map(&pcb->as, (void *)(elf_ph_header.p_vaddr + j * PGSIZE), 0, 0);
//       printf("paddr:0x%x vaddr:0x%x\n",paddr,(elf_ph_header.p_vaddr + j * PGSIZE));
//       fs_lseek(fd, elf_ph_header.p_offset + j * PGSIZE, SEEK_SET);
//       fs_read(fd, (void *)paddr, elf_ph_header.p_filesz - j * PGSIZE);
//     }
//   }
//   return elf_header.e_entry;
// }

#define PGSIZE 4096
#define PGMASK          (PGSIZE - 1)    // Mask for bit ops
#define PGROUNDDOWN(a)  (((a)) & ~PGMASK)
#define NR_PGROUNDUP(x)  ((x>>12)+(int)((x&0xfff)!=0))
#define PGSHFT    12      // log2(PGSIZE)
#define PTXSHFT   12      // Offset of PTX in a linear address
#define PDXSHFT   22      // Offset of PDX in a linear address
#define PDX(va)     (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)     (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)     ((uint32_t)(va) & 0xfff)

#define USE_MAP
#define PGSIZE 4096
static uintptr_t loader(PCB *pcb, const char *filename) {
  // TODO();
  if(filename == NULL){
    Elf_Ehdr ehdr;
    ramdisk_read(&ehdr,0,sizeof(Elf_Ehdr));
    Elf_Phdr phdr[ehdr.e_phnum];
    ramdisk_read(phdr,ehdr.e_ehsize,sizeof(Elf_Phdr)*ehdr.e_phnum);
    for(size_t i=0;i<ehdr.e_phnum;++i){
      if(phdr[i].p_type==PT_LOAD){
        ramdisk_read((void*)phdr[i].p_vaddr,phdr[i].p_offset,phdr[i].p_memsz);
        memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz),0,phdr[i].p_memsz-phdr[i].p_filesz);
      }
    }
    return ehdr.e_entry;
  }else{
    int fd = fs_open(filename, 0,0);
    Elf_Ehdr ehdr;
    int ret = fs_read(fd, &ehdr,sizeof(Elf_Ehdr));
    assert(ret!=-1);
    //printf("ehdr entry:0x%x\n",ehdr.e_entry);
    ret = fs_lseek(fd,ehdr.e_ehsize,SEEK_SET);
    //  printf("ehdr size:%d   offset:%d\n",ehdr.e_ehsize,_fs_offset(fd));
    assert(ret!=-1);
    Elf_Phdr phdr[ehdr.e_phnum];
    ret = fs_read(fd,phdr,sizeof(Elf_Phdr)*ehdr.e_phnum);
    assert(ret!=-1);
    printf("n_phdr:%d\n",ehdr.e_phnum);
    for(size_t i=0;i<ehdr.e_phnum;++i){
      //printf("offset:%d p_type:%d PT_LOAD:%d\n",phdr[i].p_offset,phdr[i].p_type,PT_LOAD);
      if(phdr[i].p_type==PT_LOAD){
        // printf("offset:%d\n",phdr[i].p_offset);
#ifdef USE_MAP
        /*
        |...**|***..|
        |.***.|
        |*****|
        */
        uint32_t start_vaddr = phdr[i].p_vaddr;
        uint32_t end_vaddr   = phdr[i].p_vaddr+phdr[i].p_memsz;
        uint32_t start_v_pg  = start_vaddr&~0xfff;
        uint32_t end_v_pg    = end_vaddr&~0xfff;
        uint32_t offset = 0;
        uint32_t v_pg,p_pg,vaddr;
        printf("vaddr[0x%x:0x%x]\n",start_vaddr,end_vaddr);
        for(v_pg=start_v_pg;v_pg<=end_v_pg;v_pg+=(1<<12)){
          p_pg = (uint32_t)new_page(1);
          printf("vaddr:0x%x v_pg:0x%x->p_pg:0x%x\n",start_vaddr+offset,v_pg,p_pg);
          ret = fs_lseek(fd, phdr[i].p_offset+offset, SEEK_SET);
          assert(ret!=-1);
          if(v_pg==start_v_pg&&v_pg==end_v_pg){
            uint32_t start_pos = start_vaddr-start_v_pg;
            uint32_t end_pos   = end_vaddr  -end_v_pg;
            ret =  fs_read(fd,p_pg+start_pos,end_pos-start_pos);
            offset+=(end_pos-start_pos);
          }else if(v_pg == start_v_pg){
            uint32_t start_pos = start_vaddr-start_v_pg;
            ret = fs_read(fd,p_pg+start_pos,PGSIZE-start_pos);
            offset += (PGSIZE-start_pos);
          }else if(v_pg == end_v_pg){
            uint32_t end_pos = end_vaddr-end_v_pg;
            ret = fs_read(fd,p_pg,end_pos);
            offset += end_pos;
          }else{
            ret = fs_read(fd,p_pg,PGSIZE);
            offset += PGSIZE;
          }
          if(ret==-1){
            printf("fs read fail: v_pg:0x%x p_pg:0x%x",v_pg,p_pg);
            assert(0);
          }
          ret = _map(&pcb->as,v_pg,p_pg,0);
          if(ret==-1){
            printf("_map fail: v_pg:0x%x p_pg:0x%x\n",v_pg,p_pg);
            assert(0);
          }
          
        }
#else
        ret = fs_lseek(fd,phdr[i].p_offset,SEEK_SET);
        assert(ret!=-1);
        ret = fs_read(fd,(void*)phdr[i].p_vaddr,phdr[i].p_memsz);
        assert(ret!=-1);
        //memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz),0,phdr[i].p_memsz-phdr[i].p_filesz);
        printf("vaddr:[0x%x:0x%x]\n",phdr[i].p_vaddr,phdr[i].p_vaddr+phdr[i].p_memsz);
#endif 
      }
    }
    fs_close(fd);
    // uint32_t va = 0x400017e8;
    // uint32_t* pdir = pcb->as.ptr;
    // uint32_t pde = pdir[PDX(va)];
    // uint32_t* ptab = (pde&~0x3ff)<<2;
    // uint32_t pte = ptab[PTX(va)];
    // uint32_t pa = ((pte<<2)&~0xfff)|(va&0xfff);
    // printf("load finish\n");
    // printf("loader vaddr 0x%x =pde:0x%x(0x%x[%d])=pte:0x%x(0x%x[%d])= paddr:0x%x\n",va,pde,pdir,PDX(va),pte,ptab,PTX(va),pa);
    return ehdr.e_entry;
  }

}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry, void*arg) {
  _protect(&pcb->as);
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, arg);
}

void context_uload(PCB *pcb, const char *filename, int argc,  char** argv) {
  printf("uload filename:%s argc:%d\n",filename,argc);
  _protect(&pcb->as);
  //pcb->as.ptr += pcb->index;
  //__am_switch(pcb->cp);
  uintptr_t entry = loader(pcb, filename);
  _Area stack; 
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);
  uint32_t args[] = {argc,(uint32_t)argv};
  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, (void*)args);
}
