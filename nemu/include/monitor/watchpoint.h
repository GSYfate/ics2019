#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char* expression;
  uint32_t value;
  uint32_t old_value;
  bool enable;

} WP;

WP* new_wp();
void free_wp(WP*);
bool delete_wp(int);
bool change_wp();
void print_wp();

#endif
