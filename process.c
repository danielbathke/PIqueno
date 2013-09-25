
#include "textutils.h"
#include "framebuffer.h"
#include "scheduler.h"

void sample_process_1() {
    
    console_write("Starting process 1 ");
    
    int to = 1000;
    
    int i, j;
    for (i=0; i<to; i++) {
        console_write(todec(i,0));
        console_write(" ");
        for (j=0; j<to*to; j++) {
			asm volatile("NOP");
		}
    }
    
    asm volatile("SWI #0");
}

void sample_process_2() {

    console_write("Starting process 2 ");
    
    int to = 1000;
    
    int i, j;
    for (i=0; i<to; i++) {
        console_write(todec(i,0));
        console_write(" ");
        for (j=0; j<to*to; j++) {
			asm volatile("NOP");
		}
    }
    
    asm volatile("SWI #0");
}
