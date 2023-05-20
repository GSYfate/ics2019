#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <sys/time.h>

// static struct timeval boot_time={};
static int boot_time;

void puts(const char* string){
  char*  p = string;
  for(p=string;*p;++p)_putc(*p);
}

size_t __am_timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _DEV_TIMER_UPTIME_t *uptime = (_DEV_TIMER_UPTIME_t *)buf;
      int cur_time = *(int*)0xa1000048-boot_time;
      uptime->hi = 0;
      uptime->lo =  cur_time;
      //_puts("after __am_timer_read\n");
      return sizeof(_DEV_TIMER_UPTIME_t);
    }
    case _DEVREG_TIMER_DATE: {
      _DEV_TIMER_DATE_t *rtc = (_DEV_TIMER_DATE_t *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2000;
      return sizeof(_DEV_TIMER_DATE_t);
    }
  }
  return 0;
}

void __am_timer_init() {
  // init_timer();
  //gettimeofday(&boot_time,NULL);
  boot_time=*(int*)0xa1000048;
}
