
#include "textutils.h"
#include "framebuffer.h"

void sample_process_1() {
    
    int to = 1000;
    
    int i;
    for (i=0; i<to; i++) {
        console_write("sample_process_1: ");
        console_write(todec(i, 0));
    }
    
}

void sample_process_2() {

    int to = 1000;
    
    int i;
    for (i=0; i<to; i++) {
        console_write("sample_process_2: ");
        console_write(todec(i, 0));
    }
    
}
