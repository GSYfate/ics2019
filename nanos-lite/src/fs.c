#include "common.h"
#include "fs.h"


#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a<b)?a:b)
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;



enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}


/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0,invalid_read, invalid_write},
  {"stdout", 0, 0, 0,invalid_read, invalid_write},
  {"stderr", 0, 0, 0,invalid_read, invalid_write},
  {"/dev/events",0,0,0},
  {"/proc/dispinfo",0,0,0},
  {"/dev/fb",0,0,0},
  {"/dev/fbsync",0,0,0},
  {"/dev/tty",0,0,0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char* pathname, int flags, int mode){

  int i;
  for(i=0;i<NR_FILES;++i){
    if(!strcmp(pathname,file_table[i].name))break;
  }
  if(i<NR_FILES){
    file_table[i].open_offset=0;
    printf("fopen index:%d filename:%s\n",i,file_table[i].name);
    return i;
  }else{
    printf("File %s no found\n",pathname);
    assert(0);
  }
  return -1;
}

const char* writeonly[]={"stdout","stderr","/dev/fb","/dev/fbsync","/dev/tty"};
const char* readonly[]=  {"/proc/dispinfo","/dev/events"};
const char* forbidden[]= {"stdin"};


#define NR_WRITEONLY  sizeof(writeonly)/sizeof(char*)
#define NR_READONLY  sizeof(readonly)/sizeof(char*)
#define NR_FORBIDDEN  sizeof(forbidden)/sizeof(char*)
size_t fs_read(int fd, void *buf, size_t len){
  Finfo fp= file_table[fd];
  // printf("name:%s ",fp.name);
  int i;
  for(i=0;i<NR_WRITEONLY;++i){
    if(!strcmp(writeonly[i],fp.name)){
      printf("%s is write only\n",fp.name);
      assert(0);
      return -1;
    }
  }
  for(i=0;i<NR_FORBIDDEN;++i){
    if(!strcmp(forbidden[i],fp.name)){
      printf("%s is forbidden\n",fp.name);
      assert(0);
      return -1;
    }
  }
  
  if(!strcmp(fp.name,"/dev/events")){
    return events_read(buf,0,len);
  }else if(!strcmp(fp.name,"/proc/dispinfo")){
    int inc= dispinfo_read(buf,fp.open_offset,len);
    file_table[fd].open_offset+=inc;
    return inc;
  }else{
    //printf("len:%d size:%d offset:%d\n",len,fp.size,fp.open_offset);
    //assert(len<=fp.size-fp.open_offset);
    //printf("here disk_offset:0x%x\n",fp.disk_offset);
    len = min(len,fp.size-fp.open_offset);
    ramdisk_read(buf, fp.disk_offset+fp.open_offset, len);
    file_table[fd].open_offset+=len;
    return len;
  }
}


size_t fs_write(int fd, void *buf, size_t len){
  Finfo fp=file_table[fd];
  int i;
  for(i=0;i<NR_READONLY;++i){
    if(!strcmp(readonly[i],fp.name)){
      printf("%s is write only\n",fp.name);
      assert(0);
      return -1;
    }
  }
  for(i=0;i<NR_FORBIDDEN;++i){
    if(!strcmp(forbidden[i],fp.name)){
      printf("%s is forbidden\n",fp.name);
      assert(0);
      return -1;
    }
  }
  if(!strcmp(fp.name,"stdout")){
    const char* p = (char*)buf;
    for(int i=0;i<len;++i)_putc(p[i]);
  }else if(!strcmp(fp.name,"stderr")){
    _putc('e');_putc('r');_putc('r');_putc(':');
    const char* p = (char*)buf;
    for(int i=0;i<len;++i)_putc(p[i]);
  }else if(!strcmp(fp.name,"/dev/fb")){
    int inc= fb_write(buf, fp.open_offset, len);
    file_table[fd].open_offset+=inc;
    return  inc;
  }else if(!strcmp(fp.name,"/dev/fbsync")){
    int inc= fbsync_write(buf, fp.open_offset, len);
    file_table[fd].open_offset+=inc;
    return inc;
  }else if(!strcmp(fp.name,"/dev/tty")){
    serial_write(buf, 0, len);
  }else{
    //assert(len<=fp.size-fp.open_offset);
    len = min(len,fp.size-fp.open_offset);
    ramdisk_write(buf, fp.disk_offset+fp.open_offset,len);
    file_table[fd].open_offset+=len;
    return len;
  }
  return -1;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  switch (whence)
  {
  case SEEK_SET:{
    file_table[fd].open_offset=offset;
    break;
  }
  case SEEK_CUR:{
    file_table[fd].open_offset+=offset;
    break;
  }
  case SEEK_END:{
    //printf("seek end, size:%d\n",file_table[fd].size);
    file_table[fd].open_offset=file_table[fd].size;
    break;
  }
  default:
    break;
  }
  return file_table[fd].open_offset;
}
int fs_close(int fd){
  file_table[fd].open_offset=0;
  return 0;
}

int _fs_start(int fd){
  Finfo fp=file_table[fd];
  return fp.disk_offset;
}
int _fs_end(int fd){
  Finfo fp=file_table[fd];
  return fp.size+fp.disk_offset;
}
int _fs_size(int fd){
  Finfo fp=file_table[fd];
  return fp.size;
}
int _fs_offset(int fd){
  Finfo fp=file_table[fd];
  return fp.open_offset;
}
