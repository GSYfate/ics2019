#ifndef __SNAPSHOT_H__
#define __SNAPSHOT_H__
#include "common.h"
int get_pc();
int save_snapshot(const char* fpath);
int load_snapshot(const char* fpath);

#endif