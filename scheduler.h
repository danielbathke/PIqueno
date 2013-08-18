
#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include "process.h"

extern void create_main_process();
extern void fork(char * name, unsigned int addr);
extern void schedule_timeout();

#endif	/* SCHEDULER_H */

