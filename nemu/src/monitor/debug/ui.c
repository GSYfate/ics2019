#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "monitor/snapshot.h"
#include "nemu.h"

#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char* args);

static int cmd_p(char* args);

static int cmd_x(char* args);

static int cmd_w(char* args);

static int cmd_d(char*  args);

static int cmd_save(char* args);

static int cmd_load(char* args);

static int cmd_pc(char* args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "si [N] : execute N steps, default 1",cmd_si},
  { "info", "info r/w : display register/monitor status",cmd_info},
  { "p", "p EXPR : return the value of EXPR",cmd_p},
  { "x", "x N EXPR : return the 4 bytes of mem[EXPR]",cmd_x},
  { "w", "w EXPR : set a watchpoint at EXPR",cmd_w},
  { "d", "d N : delete the NO.N watchpoint",cmd_d},
  { "save","save PATH : save snapshot to PATH",cmd_save},
  { "load","load PATH : load snapshot from PATH",cmd_load},
  { "pc", "pc : return the pc",cmd_pc}
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_si(char *args){
  char* arg=strtok(args," ");
  int N=1;
  if(arg!=NULL){
    N=atoi(arg);
  }
  cpu_exec(N);
  return 0;
}

static int cmd_help(char *args){
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_info(char* args){
  if(args==NULL){
    printf("too few arguments!\n");
    return 0;
  }
  char* arg=strtok(args," ");
  if(arg==NULL){
    printf("too few arguments!\n");
    return 0;
  }
  if(arg[0]=='r')isa_reg_display();
  else if(arg[0]=='w')print_wp();
  else printf("no such command : %s\n",arg);
  return 0;
}

static int cmd_p(char *args){
  if(args == NULL){
    printf("too few arguemnts!\n");
    return 0;
  }
  char* arg=strtok(args," ");
  if(arg==NULL){
    printf("too few arguments!\n");
    return 0;
  }
  bool success=true;
  int ans = expr(arg, &success);
  if(success){
    printf("%d\n",ans);
  }else{
    printf("Expression Error\n");
  }
  return  0;
}

static int cmd_x(char *args){
  if(args == NULL){
    printf("too few arguemnts!\n");
    return 0;
  }
  char* arg=strtok(args," ");
  if(arg==NULL){
    printf("too few arguments!\n");
    return 0;
  }
  char* s;
  int n = strtol(arg,s,10);
  char *exp =  strtok(NULL," ");
  if(exp==NULL){
    printf("too few arguments!\n");
    return 0;
  }
  // char* str;
  // vaddr_t addr=strtol(exp,&str,16);
  bool success;
  vaddr_t addr=expr(exp,&success);
  for(int i=0;i<n;++i){
    uint32_t data=vaddr_read(addr+i*4,4);
    printf("0x%08x ",addr+i*4);
    for(int j=0;j<4;++j){
      printf("0x%02x ",data&0xff);
      data = data>>8;
    }
    printf("\n");
  }
  return 0;
}

static int cmd_w(char* args){
  if(args == NULL){
    printf("too few arguemnts!\n");
    return 0;
  }
  char* exp=strtok(args," ");
  if(exp==NULL){
    printf("too few arguments!\n");
    return 0;
  }
  new_wp(exp);
  return 0;
}

static int cmd_d(char* args){
 if(args == NULL){
    printf("too few arguemnts!\n");
    return 0;
  }
  char* N =strtok(args," ");
  if(N==NULL){
    printf("too few arguments!\n");
    return 0;
  }
  int no=atoi(N);
  delete_wp(no);
  return 0;
}

static int cmd_save(char* args){
  if(args == NULL){
    printf("too few arguemnts!\n");
    return 0;
  }
  char* path=strtok(args," ");
  if(path==NULL){
    printf("too few arguments!\n");
    return 0;
  }
  save_snapshot(path);
  return 0;
}

static int cmd_load(char*  args){
  if(args == NULL){
    printf("too few arguemnts!\n");
    return 0;
  }
  char* path=strtok(args," ");
  if(path==NULL){
    printf("too few arguments!\n");
    return 0;
  }
  load_snapshot(path);
  return 0;
}

static int cmd_pc(char* args){
  int pc = get_pc();
  printf("0x%x\n",pc);
  return 0;
}


void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
