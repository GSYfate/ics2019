#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t my_strlen(const char *s) {
    int i;
    for(i=0;s[i];++i);
    return i;
}

char *my_strcpy(char* dst,const char* src) {
    char *p1;
    for(p1=dst;*p1=*src;++p1,++src);
    return dst;
}

char* my_strncpy(char* dst, const char* src, size_t n) {
    char *p1 = dst;
    if (n) {
        while((*p1++ = *src++) && --n > 0);
        if ((*--src == '\0') && --n > 0) {
            do {
                *p1++ = '\0';
            } while(--n > 0);
        }
    }
    return dst;
}

char* my_strcat(char* dst, const char* src) {
  char*  p1 = dst;
  for(;*p1;p1++);
  for(;*src;*p1++=*src++);
  return dst;
}

int my_strcmp(const char* s1, const char* s2) {
  for(;*s1&&*s2;++s1,++s2){
    if(*s1==*s2)continue;
    else return ((int32_t)*s1-(int32_t)*s2);
  }
  if(*s1)return 1;
  else if(*s2)return -1;
  return 0;
}

int my_strncmp(const char* s1, const char* s2, size_t n) {
    if (n) {
        do {
            if (*s1 != *s2++)
                break;
            if (*s1++ == '\0')
                return 0;
        } while (--n > 0);
        if (n > 0) {
            if (*s1 == '\0') return -1;
            if (*--s2 == '\0') return 1;
            return (unsigned char) *s1 - (unsigned char) *s2;
        }
    }
    return 0;
}

void* my_memset(void* v,int c,size_t n) {
  assert(n>=0);
  // printf("memset(0x%x,%d,%d)\n",v,c,n);
  for(size_t i = 0; i < n; i++) {
    ((uint8_t *)v)[i] = (uint8_t)c;
  }
  return v;

  // c = c&0xff;
  // uint32_t c_4 = (c<<24)|(c<<16)|(c<<8)|c;
  // uint32_t* v_4 = (uint32_t*)v;
  // char* v_1 = (char*)v;
  // size_t n_4 = n/4;
  // size_t n_4_ = n%4;
  // size_t n_16 = n_4/4;
  // int i,j;
  // j = n_16*4;
  // for(i=0;i<j;i+=4){
  //   v_4[i]=c_4;
  //   v_4[i+1]=c_4;
  //   v_4[i+2]=c_4;
  //   v_4[i+3]=c_4;
  // }
  // j = n_4;
  // for(;i<j;++i){
  //   v_4[i]=c_4;
  // }
  // i*=4;
  // for(;i<n;++i){
  //   v_1[i]=c;
  // }
  return v;
}

void* my_memcpy(void* out, const void* in, size_t n) {
    size_t n_4 = n/4;
    size_t n_4_ = n%4;
    size_t n_16 = n_4/4;
    const uint32_t* in_4 = (uint32_t*)in;
    uint32_t * out_4 = (uint32_t*)out;
    const char* in_1 = (char*)in;
    char*  out_1 = (char*)out;
    int i,j;
    j = n_16*4;
    for(i=0;i<j;i+=4){
      out_4[i]=in_4[i];
      out_4[i+1]=in_4[i+1];
      out_4[i+2]=in_4[i+2];
      out_4[i+3]=in_4[i+3];
    }
    j = n_4;
    for(;i<j;++i){
      out_4[i]=in_4[i];
    }
    i = i*4;
    for(j=0;j<n_4_;++j){
      out_1[i+j]=in_1[i+j];
    }
    return out;
}

int my_memcmp(const void* s1, const void* s2, size_t n){
  const char* p1=s1, *p2=s2;
  if(n){
    n++;
    while(--n>0){
      if(*p1++==*p2++)continue;
      return *--p1 - *--p2;
    }
  }
  return 0;
}

#endif
