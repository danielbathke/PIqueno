
#ifndef PROCESS_H
#define	PROCESS_H

typedef struct {
    unsigned int id;
    char* name;
    unsigned int parent;
    unsigned int addr;
    unsigned int stack_pointer;
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r12;
    unsigned int r13;
    unsigned int r14;
    unsigned int r15;
    unsigned int SPSR;
    unsigned int times_loaded;
} process;

extern void sample_process_1();

extern void sample_process_2();

#endif	/* PROCESS_H */

