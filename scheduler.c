
#include "process.h"
#include "framebuffer.h"
#include "textutils.h"
#include "interrupts.h"

extern unsigned int _physdatastart, _physbssstart, _physbssend;
extern unsigned int _datastart, _bssstart, _bssend;
extern unsigned int _kstart, _krodata, _kend;

static process process_list[1024];

static unsigned int stack_base;

static unsigned int process_count = 0;
static unsigned int active_process_index;

extern void main_endloop(void);

void create_main_process() {
    process main_process;

    main_process.id = 0;
    main_process.name = "Main";
    main_process.pc = &main_endloop;
    main_process.times_loaded = 1;
    
    unsigned int stack_pointer;
    
	asm volatile ("MOV %0, SP\n\t" : "=r" (stack_pointer) );
	
	stack_base = stack_pointer;
	
	console_write("Main stack is 0x");
	console_write(tohex(stack_pointer, 4));
	console_write("\n");
    
    
    process_list[0] = main_process;
    active_process_index = 0;
    process_count++;
}

void fork(char * name, unsigned int * pc) {

	asm volatile("cps #0x1f");

    process fork_process;
	
	unsigned int * forked_stack_pointer = stack_base + (process_count * 1024);
	
	console_write("Forked stack is 0x");
	console_write(tohex(forked_stack_pointer, 4));
	console_write("\n");

    fork_process.id = process_count;
    fork_process.name = name;
    fork_process.pc = pc;
    fork_process.parent = active_process_index;
    fork_process.times_loaded = 0;
    fork_process.stack_pointer = forked_stack_pointer;
    
    process_list[process_count] = fork_process;

    process_count++;
}

void schedule_timeout(unsigned long stack_pointer, unsigned long pc) {
	
    process_list[active_process_index].stack_pointer = stack_pointer;
    process_list[active_process_index].pc = pc;
    
    console_write("\n");
    console_write("\n");
    console_write("Schedule timeout. Current active pid is ");
    console_write(todec(process_list[active_process_index].id, 0));
    console_write(" with name ");
    console_write(process_list[active_process_index].name);
    console_write(". Switching to next process.\n");      

	console_write("Saving stack...");
    console_write(" stack saved, was 0x");
    console_write(tohex(process_list[active_process_index].stack_pointer, 4));
    console_write("\n");

	console_write("Saving pc...");
    console_write(" pc saved, was 0x");
    console_write(tohex(process_list[active_process_index].pc, 4));
    console_write("\n");
    
    int next_process = (active_process_index+1 == process_count) ? 1 : active_process_index+1;
    
    active_process_index = next_process;
    
//    if (process_list[active_process_index].times_loaded == 5) { return; }
    
    process_list[active_process_index].times_loaded++;
    
    console_write("Restoring stack 0x");
    console_write(tohex(process_list[active_process_index].stack_pointer, 4));
    console_write("\n");
	
    console_write("Restoring pc 0x");
    console_write(tohex(process_list[active_process_index].pc, 4));
    console_write("\n");
    
	asm volatile("MOV SP, %[addr]" : : [addr] "r" ((unsigned long )(process_list[active_process_index].stack_pointer)) );
	
	if (process_list[active_process_index].times_loaded > 1) {
		
		timer_reset();
		
		asm volatile("pop {R0}");
		asm volatile("MSR   SPSR_cxsf, R0");
		
		asm volatile("pop {LR}");
		
		asm volatile("pop {R0 - R12}");
		
		asm volatile("cpsie i");
		
		asm volatile("pop {PC}");
		
	} else {
	
		timer_reset();
	
		asm volatile("cpsie i");
		
		asm volatile("MOV PC, %[addr]" : : [addr] "r" ((unsigned long )(process_list[active_process_index].pc)) );
	
	}
	
}
