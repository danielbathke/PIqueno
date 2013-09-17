
#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include "process.h"

extern void create_main_process();
extern void fork(char * name, unsigned long addr);
extern void schedule_timeout(unsigned long stack_pointer, unsigned long pc);

#endif	/* SCHEDULER_H */

