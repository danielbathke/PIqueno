
#include "process.h"
#include "framebuffer.h"
#include "textutils.h"

extern unsigned int _physdatastart, _physbssstart, _physbssend;
extern unsigned int _datastart, _bssstart, _bssend;
extern unsigned int _kstart, _krodata, _kend;

static process process_list[1024];

static unsigned int process_count = 0;
static unsigned int active_process_index;

extern void main_endloop(void);

void create_main_process() {
    process main_process;

    main_process.id = 0;
    main_process.name = "Main";
    main_process.addr = &main_endloop;
    main_process.times_loaded = 1;
    
    process_list[0] = main_process;
    active_process_index = 0;
    process_count++;
}

void fork(char * name, unsigned int addr) {

    process fork_process;

    fork_process.id = process_count;
    fork_process.name = name;
    fork_process.addr = addr;
    fork_process.parent = active_process_index;
    fork_process.times_loaded = 0;
    
    process_list[process_count] = fork_process;

    process_count++;
}

void schedule_timeout() {
	
	unsigned int lr;
	unsigned int * stack_pointer;
	
	asm volatile ("cps #0x1f\n\t");
    
	asm volatile ("push {lr}\n\t");
//    asm volatile ("push {r0-r12,lr}\n\t");
    asm volatile ("push {r0}\n\t");
    asm volatile ("push {r1}\n\t");
    asm volatile ("push {r2}\n\t");
    asm volatile ("push {r3}\n\t");
    asm volatile ("push {r4}\n\t");
    asm volatile ("push {r5}\n\t");
    asm volatile ("push {r6}\n\t");
    asm volatile ("push {r7}\n\t");
    asm volatile ("push {r8}\n\t");
    asm volatile ("push {r9}\n\t");
    asm volatile ("push {r10}\n\t");
    asm volatile ("push {r11}\n\t");
    asm volatile ("push {r12}\n\t");
    
    asm volatile ("push {lr}\n\t");
    
    //asm volatile ("mrs  r2,cpsr");
    //asm volatile ("push {r2}");
    
    asm volatile ("MOV %0, SP\n\t" : "=r" (stack_pointer) );
	
    console_write("\n");
    console_write("\n");
    console_write("Schedule timeout. Current active pid is ");
    console_write(todec(process_list[active_process_index].id, 0));
    console_write(" with name ");
    console_write(process_list[active_process_index].name);
    console_write(". Switching to next process.\n");      
    
	// asm volatile ("cps #0x1f\n\t");
	
	
    
    // salvar lr
    //asm volatile("mov %[addr], lr" : [addr] "=r" (lr) );
    //asm volatile ("MOV %0, LR\n\t" : "=r" (lr) );

	console_write("Saving lr...");
    
    // salvar lr
    // asm volatile("mov %[addr], lr" : [addr] "=r" (process_list[active_process_index].addr) );
    //process_list[active_process_index].addr = lr;
    
    //console_write(" lr saved, was ");
    //console_write(todec(process_list[active_process_index].addr, 0));
    //console_write("\n");
    
    
    //console_write("Saving stack...");
    
    // salvar stack
    // asm volatile ("MOV %0, SP\n\t" : "=r" (process_list[active_process_index].stack_pointer) );
    
    process_list[active_process_index].stack_pointer = stack_pointer;
    
    console_write(" stack saved, was ");
    console_write(todec(process_list[active_process_index].stack_pointer, 0));
    console_write("\n");
    
    //console_write("----------------------------------------------------- LR WAS: ");
    //console_write(todec(process_list[active_process_index].addr, 0));
    
    //console_write("\n");
    //console_write("\n");
    
    /*
    asm volatile ("MOV %0, LR\n\t" : "=r" (process_list[active_process_index].addr) );

    asm volatile ("MOV %0, r0\n\t" : "=r" (process_list[active_process_index].r0) );
    asm volatile ("MOV %0, r1\n\t" : "=r" (process_list[active_process_index].r1) );
    asm volatile ("MOV %0, r2\n\t" : "=r" (process_list[active_process_index].r2) );
    asm volatile ("MOV %0, r3\n\t" : "=r" (process_list[active_process_index].r3) );
    asm volatile ("MOV %0, r4\n\t" : "=r" (process_list[active_process_index].r4) );
    asm volatile ("MOV %0, r5\n\t" : "=r" (process_list[active_process_index].r5) );
    asm volatile ("MOV %0, r6\n\t" : "=r" (process_list[active_process_index].r6) );
    asm volatile ("MOV %0, r7\n\t" : "=r" (process_list[active_process_index].r7) );
    asm volatile ("MOV %0, r8\n\t" : "=r" (process_list[active_process_index].r8) );
    asm volatile ("MOV %0, r9\n\t" : "=r" (process_list[active_process_index].r9) );
    asm volatile ("MOV %0, r10\n\t" : "=r" (process_list[active_process_index].r10) );
    asm volatile ("MOV %0, r11\n\t" : "=r" (process_list[active_process_index].r11) );
    asm volatile ("MOV %0, r12\n\t" : "=r" (process_list[active_process_index].r12) );
    asm volatile ("MOV %0, r13\n\t" : "=r" (process_list[active_process_index].r13) );
    asm volatile ("MOV %0, r14\n\t" : "=r" (process_list[active_process_index].r14) );
    */
    
    //process_list[active_process_index].stack_pointer = (unsigned int *)read_stack_pointer();
    
    int next_process = (active_process_index+1 == process_count) ? 1 : active_process_index+1;
    
    active_process_index = next_process;
    
    if (process_list[active_process_index].times_loaded == 5) { return; }
    
    process_list[active_process_index].times_loaded++;
    
    if (process_list[active_process_index].times_loaded > 1) {    
		asm volatile("mov sp, %[addr]" : : [addr] "r" ((unsigned int *)(process_list[active_process_index].stack_pointer)) );
		//asm volatile ("pop {r0}\n\t");
		//asm volatile ("bic r0,r0,#0x80");
		//asm volatile ("msr spsr,r0");
		
		asm volatile("cpsie i");
		
		//asm volatile ("ldmfd  sp!,{r0-r12,lr,pc}^");
		
		asm volatile ("pop {lr}\n\t");
		
		asm volatile ("pop {r12}\n\t");
		asm volatile ("pop {r11}\n\t");
		asm volatile ("pop {r10}\n\t");
		asm volatile ("pop {r9}\n\t");
		asm volatile ("pop {r8}\n\t");
		asm volatile ("pop {r7}\n\t");
		asm volatile ("pop {r6}\n\t");
		asm volatile ("pop {r5}\n\t");
		asm volatile ("pop {r4}\n\t");
		asm volatile ("pop {r3}\n\t");
		asm volatile ("pop {r2}\n\t");
		asm volatile ("pop {r1}\n\t");
		asm volatile ("pop {r0}\n\t");
		
		asm volatile ("pop {pc}\n\t");
		
		//asm volatile ("pop {r0-r12,lr}\n\t");
		//asm volatile ("pop {pc}\n\t");
		asm volatile ("bx lr\n\t");
		
		//asm volatile("ldmfd  sp!,{r0-r12,lr,pc}^");
        /*
        asm volatile("mov r0, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r0)) );
        asm volatile("mov r1, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r1)) );
        asm volatile("mov r2, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r2)) );
        asm volatile("mov r3, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r3)) );
        asm volatile("mov r4, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r4)) );
        asm volatile("mov r5, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r5)) );
        asm volatile("mov r6, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r6)) );
        asm volatile("mov r7, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r7)) );
        asm volatile("mov r8, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r8)) );
        asm volatile("mov r9, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r9)) );
        asm volatile("mov r10, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r10)) );
        asm volatile("mov r11, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r11)) );
        asm volatile("mov r12, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r12)) );
        asm volatile("mov r13, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r13)) );
        asm volatile("mov r14, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].r14)) );
        */
    } else {
		//process_list[active_process_index].stack_pointer = &stack_pointer + 10240000;
		//asm volatile("mov sp, %[addr]" : : [addr] "r" ((unsigned int)(process_list[active_process_index].stack_pointer)) );
	}
    
    asm volatile("cpsie i");
    
    asm volatile("movs pc, %[addr]" : :
            [addr] "r" ((unsigned int)(process_list[active_process_index].addr)) );
    
}
