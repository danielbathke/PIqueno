
#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include "process.h"


//This defines the stack frame that is saved  by the hardware
typedef struct {
  unsigned int r0;
  unsigned int r1;
  unsigned int r2;
  unsigned int r3;
  unsigned int r12;
  unsigned int lr;
  unsigned int pc;
  unsigned int psr;
} hw_stack_frame_t;
 
//This defines the stack frame that must be saved by the software
typedef struct {
  unsigned int r4;
  unsigned int r5;
  unsigned int r6;
  unsigned int r7;
  unsigned int r8;
  unsigned int r9;
  unsigned int r10;
  unsigned int r11;
} sw_stack_frame_t;

extern void create_main_process();
extern void fork(char * name, unsigned int addr);
extern void schedule_timeout();

#endif	/* SCHEDULER_H */

