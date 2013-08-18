
#include "process.h"
#include "framebuffer.h"
#include "textutils.h"

static volatile struct process process_list[1024];

static unsigned int process_count = 0;
static unsigned int active_process_index;

extern void main_endloop(void);

void create_main_process() {
    struct process main_process;

    main_process.id = 0;
    main_process.name = "Main";
    main_process.addr = &main_endloop;
    
    process_list[0] = main_process;
    active_process_index = 0;
    process_count++;
}

void fork(char * name, unsigned int addr) {

    struct process fork_process;

    fork_process.id = process_count;
    fork_process.name = name;
    fork_process.addr = addr;
    fork_process.parent = active_process_index;
    
    process_list[process_count] = fork_process;

    process_count++;
}

void schedule_timeout() {
    console_write("Schedule timeout. Current active pid is ");
    console_write(todec(process_list[active_process_index].id, 0));
    console_write(" with name ");
    console_write(process_list[active_process_index].name);
    console_write(". Switching to next process.\n");
    
    int next_process = (active_process_index+1 == process_count) ? 0 : active_process_index+1;
    
    active_process_index = next_process;
    
    asm volatile("movs pc, %[addr]" : :
            [addr] "r" ((unsigned int)(process_list[active_process_index].addr)) );
    
}