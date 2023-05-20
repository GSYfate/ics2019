#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
#ifdef MULTI_PROC
  _yield();
#endif
  int i;
  const char* s = (char*) buf;
  for(i=0;i<len;++i)_putc(s[i]);
  return 0;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
#ifdef MULTI_PROC
  _yield();
#endif
  _DEV_INPUT_KBD_t key;
  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &key, sizeof(_DEV_INPUT_KBD_t));
  if (key.keycode){
    if(key.keydown){
      sprintf(buf,"kd %s\n",keyname[0x7fff&key.keycode]);
    }else{
      sprintf(buf,"ku %s\n",keyname[0x7fff&key.keycode]);
    }
  } else {
    _DEV_TIMER_UPTIME_t rtc;
    _io_read(_DEV_TIMER, _DEVREG_TIMER_UPTIME, &rtc, sizeof(_DEV_TIMER_UPTIME_t));
    sprintf(buf,"t %d\n",rtc.lo);
  }
  len = strlen(buf);
  return len;
}

static char dispinfo[128] __attribute__((used)) = {};
static int W,H;
#define max(a,b) ((a<b)?b:a)
#define min(a,b) ((a<b)?a:b)

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  int i;
  char* s = (char*)buf;
  int _len = max(0,min(strlen(dispinfo)-offset,len));
  for(i=0;i<_len;++i)s[i]=dispinfo[i+offset];
  return _len;
}

size_t fb_write(void *buf, size_t offset, size_t len) {
#ifdef MULTIPROC
  _yield();
#endif
  offset /=4;
  //printf("fb write buf:0x%x offset:%d len:%d\n",buf,offset,len);
  uint32_t * p=buf;
  len = min(H*W-offset,len);
  int ret = len;
  _DEV_VIDEO_FBCTL_t ctl ;
  ctl.pixels = p;
  ctl.x = offset%W;
  ctl.y =offset/W;
  ctl.w = W-offset%W;
  ctl.h = 1,
  ctl.sync = 0,
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &ctl, sizeof(ctl));
  
  ctl.x=0;
  ctl.y+=ctl.h;
  len-=ctl.w*ctl.h;
  p+=ctl.w*ctl.h;
  if(len/W){
    ctl.h=len/W;
    ctl.w =W;
    ctl.pixels=p;
    _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &ctl, sizeof(ctl));
    ctl.y+=ctl.h;
    len-=ctl.h*ctl.w;
    p+=ctl.h*ctl.w;
  }

  if(len){
    ctl.pixels = p;
    ctl.w = len;
    ctl.h = 1;
    _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &ctl, sizeof(ctl));
  }
  return ret;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  //printf("fbsync write buf:0x%x offset:%d len:%d\n",buf,offset,len);
  _DEV_VIDEO_FBCTL_t ctl;
  ctl.pixels = NULL;
  ctl.x = ctl.y = ctl.w = ctl.h = 0;
  ctl.sync = 1;
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &ctl, sizeof(ctl));
  return 0;
}


void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  _DEV_VIDEO_INFO_t info;
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  H = info.height;
  W = info.width;
  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d\n",info.width,info.height);
}
