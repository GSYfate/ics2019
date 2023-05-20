#include "loader.h"
#define MAX_NR_PROC 4
static int allocated_proc = 0;
static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB* current = NULL;
static int cur_proc = 0;

void switch_boot_pcb(){
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  char* s = (char*)arg;
  while (1) {
    Log("Parameter is %s",s);
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

PCB* new_proc(){
  PCB* proc = &pcb[allocated_proc++];
  //printf("Proc:%x  at index:%d\n",proc,allocated_proc);
  proc->index = allocated_proc-1;
  return proc;
}

void init_proc() {
  Log("init proc");
  #ifdef MULTI_PROC
    char** args = NULL;
    context_uload(new_proc(), "/bin/hello",0,NULL);
    //char* args[] = {"--skip"};
    context_uload(new_proc(), "/bin/bmptest",0,NULL);
  #else
    char** args = NULL;
    context_uload(new_proc(), "/bin/bmptest",0,args);
  #endif
  pcb_boot = pcb[cur_proc];
  switch_boot_pcb();
  // _yield();

  Log("Initializing processes...");

  // // load program here
  //naive_uload(current,"/bin/init");
}

_Context* schedule(_Context *prev) {
  // save the context pointer
  printf("in schedule\n");
  current->cp = prev;

  // always select pcb[0] as the new process
  printf("allocated_proc:%d cur_proc:%d ",allocated_proc,cur_proc);
  cur_proc = (cur_proc+1)%allocated_proc;
  printf("now_proc:%d\n",cur_proc);
  current = &pcb[cur_proc];
  //__am_switch(current->cp);
  // then return the new context
  return current->cp;
}
