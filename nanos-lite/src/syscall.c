#include "common.h"
#include "syscall.h"
#include "loader.h"


uint32_t va2pa(_Context* c,uint32_t va){
  uint32_t pdx = (va>>22)&0x3ff;
  uint32_t ptx = (va>>12)&0x3ff;
  uint32_t off = va&0xfff;
  uint32_t* pdir  = c->as->ptr;
  uint32_t pde = pdir[ptx];
  if((pde&0x1)==0){
    printf("pde invalide:0x%x  va:0x%x\n",pde,va);
    printf("syscall vaddr 0x%x =pde:0x%x(0x%x,[%d])\n",va,pde,pdir,pdx);
    assert(0);
  }
  uint32_t* ptab = (pde&0x3ff)<<2;
  uint32_t pte = ptab[ptx];
  if((pte&0x1)==0){
    printf("pte invalid:0x%x va:0x%x\n",pte,va);
    assert(0);
  }
  uint32_t pa = ((pte&0x3ff)<<2)|off;
  return pa;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  
  switch (a[0]) {
    case SYS_exit:{
      printf("sys exit\n");
      _halt(0);
      break;
    }
    case SYS_yield:{
      printf("sys yield\n");
      //schedule();
      _yield();
      break;
    }
    case SYS_open:{
      //printf("sys open pathname:%s flags:%d mode:%d ",(char*)c->GPR2,c->GPR3,c->GPR4);
      c->GPRx =fs_open((char*)c->GPR2,c->GPR3,c->GPR4);
      //printf("fd:0x%x\n",c->GPRx);
      break;
    }
    case SYS_read:{
      //printf("sys read fd:0x%x buf:0x%x len:%d\n",c->GPR2,c->GPR3,c->GPR4);
      c->GPRx =fs_read(c->GPR2,(char*)c->GPR3,c->GPR4);
      break;
    }
    case SYS_write:{
      //printf("sys write fd:0x%x buf:0x%x len:%d\n",c->GPR2,c->GPR3,c->GPR4);
      switch (c->GPR2)
      {
        case 1:
        case 2:{
          char buf[128];
          memcpy(buf,(char*)c->GPR3,c->GPR4);
          buf[c->GPR4]=0;
          c->GPRx=c->GPR4;
          printf("%s",buf);
          break;
        }
        default:{
          c->GPRx=(uintptr_t)fs_write(c->GPR2,(void*)c->GPR3,c->GPR4);
        }
      }
      break;
    }
    case SYS_kill:{
      printf("sys kill\n");
      // assert(0);
      break;
    }
    case SYS_getpid:{
      printf("sys getpid\n");
      break;
    }
    case SYS_close:{
      printf("sys close\n");
      break;
    }
    case SYS_lseek:{
      //printf("sys lseek fd:0x%x offset:%d whencde:%d",c->GPR2,c->GPR3,c->GPR4);
      c->GPRx=fs_lseek(c->GPR2,c->GPR3,c->GPR4);
      //printf(" ret:%d\n",c->GPRx);
      break;
    }
    case SYS_brk:{
      printf("sys brk ");
      // uint32_t pa = va2pa(c,(uint32_t)c->GPR2);
      // if(pa<=_heap.start||pa>=_heap.end){
      //   c->GPRx = -1;
      //   printf("fail\n");
      //   break;
      // }
      // c->GPRx=0;
      // printf("success\n");
      c->GPRx = mm_brk(c->GPR2,c->GPR3);
      break;
    }
    case SYS_fstat:{
      printf("sys fstat\n");
      break;
    }
    case SYS_time:{
      printf("sys time\n");
      break;
    }
    case SYS_signal:{
      printf("sys signal\n");
      break;
    }
    case SYS_execve:{
      //const char *fname, char * const argv[], char *const envp[]
      printf("sys execve:%s\n",(char*)c->GPR2);
      // char** tmp = (char**)c->GPR3;
      // int i;
      // for(i=0;tmp[i]!=NULL;++i);
      // printf("GPR2:%s\n",(char*)c->GPR2);
      PCB* proc = new_proc();
      context_uload(proc,(char*)c->GPR2,0,NULL);
      c->GPRx = proc->index;
      // printf("sys execve over\n");
      // naive_uload(NULL,(char*)c->GPR2);
      _yield();
      break;
    }
    case SYS_fork:{
      printf("sys fork\n");
      break;
    }
    case SYS_link:{
      printf("sys link\n");
      break;
    }
    case SYS_unlink:{
      printf("sys unlink\n");
      break;
    }
    case SYS_wait:{
      printf("sys wait\n");
      break;
    }
    case SYS_times:{
      printf("sys times\n");
      break;
    }
    case SYS_gettimeofday:{
      printf("sys getitmeofday\n");
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
