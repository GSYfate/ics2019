#ifndef __COMMON_H__
#define __COMMON_H__

/* Uncomment these macros to enable corresponding functionality. */
#define HAS_CTE
#define HAS_VME
#define MULTI_PROC

#include <am.h>
#include <klib.h>
#include "debug.h"
#include "ramdisk.h"
#include "device.h"
#include "fs.h"

typedef char bool;
#define true 1
#define false 0
_Context* do_syscall(_Context *c);
#endif
