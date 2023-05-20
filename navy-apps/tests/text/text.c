#include <stdio.h>
#include <assert.h>

int main() {
  FILE *fp = fopen("/share/texts/num", "r+");
  assert(fp);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  //assert(size == 5000);
  assert(size == 6000);

  //fseek(fp, 500 * 5, SEEK_SET);
  fseek(fp, 500 * 6, SEEK_SET);
  int i, n;
  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    // printf("%d\n",n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    //fprintf(fp, "%4d\n", i + 1 + 1000);
    fprintf(fp,"%5d\n",i+1000+1);
  }

  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    //printf("%d\n",n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
