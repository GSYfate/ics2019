#ifndef __LOADER_H__
#define __LOADER_H__
#include "proc.h"
void context_kload(PCB *pcb, void *entry, void* arg);
void naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB *pcb, const char *filename, int argc, char** argv);
#endif