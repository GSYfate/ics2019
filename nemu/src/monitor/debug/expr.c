#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, 

  /* TODO: Add more token types */
  TK_DEC,
  TK_HEX,
  TK_FLT,
  TK_REG,
  TK_NEG,
  TK_ADR,
  TK_EQU,
  TK_NEQ,
  TK_AND,
  TK_LOR
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQU},         // equal
  {"\\*",'*'},           // star
  {"/",'/'},             // div
  {"-",'-'},             // minus
  {"\\(",'('},              // left
  {"\\)",')'},               // right
  {"!=",TK_NEQ},               // neq
  {"&&",TK_AND},               // amd
  {"\\|\\|",TK_LOR},           // or 
  {"0x[0-9a-fA-F]+",TK_HEX}, //number
  {"([0-9])|([1-9][0-9]*)",TK_DEC},  // number
  {"($0)|(ra)|(sp)|(gp)|(tp)|(t0)|(t1)|(t2)|(t3)|(s0)|(s1)|(a0)|(a1)|(a2)|(a3)|(a4)|(a5)|(a6)|(a7)|(s2)|(s3)|(s4)|(s5)|(s6)|(s7)|(s8)|(s9)|(s10)|(s11)|(t3)|(t4)|(t5)|(t6)",TK_REG}
  //{"-?(([1-9]\\d*\\.\\d*)|(0\\.\\d*[1-9]\\d*))",TK_FLT}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          TK_NOTYPE:
            break;
          default:{
            Token tk;
            tk.type=rules[i].token_type;
            tk.str[0]='\0';
            strncpy(tk.str,substr_start,substr_len);
            tk.str[substr_len]=0;
            tokens[nr_token++]=tk;
          }
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


int priority(Token tk){
  // smaller prior
  switch (tk.type)
  {
  case '(':
  case ')':return 1;
  case '!':
  case TK_NEG:
  case TK_ADR:return 2;
  case '*':
  case '/':return 3;
  case '+':
  case '-':return 4;
  case TK_EQU:
  case TK_NEQ:return 7;
  case TK_AND:return 11;
  case TK_LOR:return 12;
  }
  return -1;
}

int parse(Token tk){
  char* ptr;
  switch(tk.type){
    case TK_DEC:return strtol(tk.str,&ptr,10);
    case TK_HEX:return strtol(tk.str,&ptr,16);
    case TK_REG:{
      bool success;
      int ans = isa_reg_str2val(tk.str,&success);
      if(success){
        return ans;
      }else{
        printf("reg visit fail\n");
        return 0;
      }
    }

    default:{printf("cannot parse number\n");assert(0);}
  }
  
  return  0;
}

bool check_parenthese(int p,int q){
  int nr_left = 1;
  int i;
  if(tokens[0].type=='('){
    for(i=p+1;i<=q;++i){
      if(tokens[i].type=='('){
        nr_left++;
      }else if(tokens[i].type==')'){
        nr_left--;
        if(nr_left==0){
          return (i==q);
        }
      }
    }
  }
  return false;
}


int eval(int p,  int q, bool* success){
  if(p>q){
    printf("Bad Expression\n");
    assert(0);
  }else if(p==q){
    return parse(tokens[p]);
  }else if(check_parenthese(p,q)){
    return eval(p+1,q-1,success);
  }else{
    int i,prior=-1,pos=-1,t,nr_left=0,nr_right=0;
    for(i=q;i>=p;--i){
      if(tokens[i].type==')')nr_right++;
      else if(tokens[i].type=='(')nr_right--;
      t = priority(tokens[i]);
      if(t!=-1&&t>prior&&nr_right==0){
        prior=t;
        pos=i;
      }
    }
    
    if(prior==2){
      prior=-1;
      pos=-1;
      for(i=p;i<=q;++i){
        if(tokens[i].type=='(')nr_left++;
        else if(tokens[i].type==')')nr_left--;
        t = priority(tokens[i]);
        if(t!=-1&&t>prior&&nr_left==0){
          prior=t;
          pos=i;
        }
      }
    }
    //printf("Pos:%d,priority:%d,%d\n",pos,prior,tokens[pos].type);
    int val1,val2;
    bool success1=true,success2=true;
    if(pos>p){
      val1 = eval(p,pos-1,&success1);
      val2 = eval(pos+1,q,&success2);
      if(!(success1&&success2)){
        *success=false;
        return 0;
      }
      switch (tokens[pos].type)
      {
      case '+':return val1+val2;
      case '-':return val1-val2;
      case '*':return val1*val2;
      case '/':{if(val2==0){printf("Denominator cannot be 0\n");*success=false;return 0;}return val1/val2;}
      case TK_AND:return val1&&val2;
      case TK_LOR:return val1||val2;
      default:assert(0);
      }
    }else{
      val2 = eval(pos+1,q,&success2);
      if(!success2){
        *success=false;
        return 0;
      }
      switch (tokens[pos].type)
      {
      case TK_NEG:return -val2;
      case '!':return !val2;
      case TK_ADR:return vaddr_read(val2,4);
      default:{printf("Symbol not recognized\n");assert(0);}
      }
    }
  }
  assert(0);
}



uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  int i;
  for(i=0;i<nr_token;++i){
    if(tokens[i].type=='-'||tokens[i].type=='*'){
      if(i==0||
      (tokens[i-1].type!=TK_DEC&&
      tokens[i-1].type!=TK_HEX&&
      tokens[i-1].type!=TK_REG&&
      tokens[i-1].type!=')')
      ){// neg
        if(tokens[i].type=='-')tokens[i].type=TK_NEG;
        else tokens[i].type=TK_ADR;
      }
    }
  }
  int ans = eval(0,nr_token-1,success);
  return ans;
}
