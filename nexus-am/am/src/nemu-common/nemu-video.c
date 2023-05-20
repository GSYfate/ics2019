#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>

#define W 400
#define H 300
#define min(a,b) ((a<b)?a:b)
uint32_t* fb = 0xa0000000;

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      info->width = W;
      info->height = H;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      int x=ctl->x;
      int y=ctl->y;
      int w=ctl->w;
      int h=ctl->h;
      uint32_t* pixels = ctl->pixels;
      int cp_bytes=sizeof(uint32_t)*min(w,W-x);
      for(int j=0;j<h&&y+j<H;++j){
        memcpy(&fb[(y+j)*W+x],pixels,cp_bytes);
        pixels+=w;
      }
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
    //printf("__am_vga_init\n");
    memset(fb,0,W*H*sizeof(uint32_t));
    //printf("__am_vag_init\n");
}
