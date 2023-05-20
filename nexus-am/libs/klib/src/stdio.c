#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define MAX_SIZE 4096


void puts_helper(const char* string,int ptr){
  int i;
  if(ptr<0){
    for(i=0;string[i];++i)_putc(string[i]);
  }else{
    for(i=0;i<ptr;++i)_putc(string[i]);
  }
}

int my_printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  char buffer[MAX_SIZE];
  int ptr = vsprintf(buffer,fmt,ap);
  puts_helper(buffer,ptr);
  va_end(ap);
  return 0;
}

int itoa_helper(char *out, int ptr, uint32_t valint, int base){
  assert(valint>=0);
  int stack[128],sptr=0;
  if(valint==0)out[ptr++]='0';
  else{
    for(;valint;stack[sptr++]=valint%base,valint=valint/base);
    for(--sptr;sptr>=0;--sptr){
      if(stack[sptr]<10)out[ptr++]=stack[sptr]+'0';
      else if(stack[sptr]<16)out[ptr++]=stack[sptr]-10+'a';
      else{
        assert(0);
      }
    }
  }
  return ptr;
}

int my_vsprintf(char *out, const char *fmt, va_list ap) {
  int ptr=0;
  for(;*fmt;++fmt){
    if(*fmt!='%')out[ptr++]=*fmt;
    else{
      switch(*(++fmt)){
        case 'c':{
          char c = va_arg(ap,int);
          out[ptr++]=c;
          break;
        }
        case 'd':{
          int valint=va_arg(ap,int);
          if(valint<0){
            out[ptr++]='-';
            valint=-valint;
          }
          ptr = itoa_helper(out,ptr,valint,10);
          break;
        }
        case 'x':{
          uint32_t valint=va_arg(ap,uint32_t);
          ptr = itoa_helper(out,ptr,valint,16);
          break;
        }
        case 's':{
          const char* valstr=va_arg(ap,char*);
          for(;*valstr;out[ptr++]=*valstr++);
          break;
        }
        case 'f':{
          float valflt=va_arg(ap,float);
          if(valflt<0){
            out[ptr++]='-';
            valflt=-valflt;
          }
          int tmpint=(int)valflt;
          int tmpflt=(int)(10000*(valflt-tmpint));
          ptr=itoa_helper(out,ptr,tmpint,10);
          out[ptr++]='.';
          ptr=itoa_helper(out,ptr,tmpflt,10);
          break;
        }
        default:{
          out[ptr++]='%';
          out[ptr++]=*fmt;
          break;
        }
      }
    }
  }
  out[ptr]=0;
  return ptr;
}

int my_sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);

  int ptr=my_vsprintf(out,fmt,ap);

  va_end(ap);
  return ptr;
}

int my_snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list arg;
  va_start(arg,fmt);
  vsprintf(out,fmt,arg);
  out[n]=0;
  va_end(arg);
  return n;
}

#endif
