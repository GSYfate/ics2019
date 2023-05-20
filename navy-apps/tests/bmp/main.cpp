#include <assert.h>
#include <stdlib.h>
#include <ndl.h>
#include <stdio.h>

int main() {
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  //printf("here\n");
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  //printf("fuckyou:0x%x\n",bmp->pixels);
  assert(bmp->pixels);
  //printf("before open display:[%d,%d]\n",bmp->w,bmp->h);
  NDL_OpenDisplay(bmp->w, bmp->h);
  //printf("before draw:[%d,%d]\n",bmp->w,bmp->h);
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  //printf("after draw\n");
  NDL_Render();
  printf("after render\n");
  NDL_CloseDisplay();
  while (1);
  return 0;
}
