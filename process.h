
#ifndef PROCESS_H
#define	PROCESS_H

typedef struct {
    unsigned int id;
    char* name;
    unsigned int parent;
    unsigned int addr;
    unsigned int * stack_pointer;
    unsigned int times_loaded;
} process;

extern void sample_process_1();

extern void sample_process_2();

#endif	/* PROCESS_H */

