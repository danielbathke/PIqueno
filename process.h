
#ifndef PROCESS_H
#define	PROCESS_H

    struct process {
        unsigned int id;
        char* name;
        unsigned int addr;
        unsigned int parent;
    };
    
    extern void sample_process_1();
    
    extern void sample_process_2();

#endif	/* PROCESS_H */

