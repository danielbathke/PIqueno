
#ifndef PROCESS_H
#define	PROCESS_H

typedef struct {
    unsigned int id;
    char* name;
    unsigned int parent;
    unsigned long stack_pointer;
	unsigned long pc;
    unsigned int times_loaded;
} process;

extern void sample_process_1();

extern void sample_process_2();

#endif	/* PROCESS_H */

