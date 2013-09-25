#include "led.h"
#include "atags.h"
#include "barrier.h"
#include "framebuffer.h"
#include "interrupts.h"
#include "mailbox.h"
#include "memory.h"
#include "memutils.h"
#include "textutils.h"
#include "process.h"
#include "scheduler.h"

/* Location of the initial page table in RAM */
static unsigned int *initpagetable = (unsigned int *) mem_p2v(0x4000);

/* Data/bss locations in physical RAM */
extern unsigned int _physdatastart, _physbssstart, _physbssend;
extern unsigned int _datastart, _bssstart, _bssend;
extern unsigned int _kstart, _krodata, _kend;

/* Main high memory kernel routine - called directly from initsys.c
 * Begins by completing the memory management work of initsys; removing the
 * virtual memory page that initsys was running in.
 *
 * ARM procedure call standard says the first 3 parameters of a function
 * are r0, r1, r2. These registers are untouched by _start/initsys, so will be
 * exactly as the bootloader set them
 * r0 should be 0
 * r1 should be the machine type - 0x0c42 = Raspberry Pi
 * r2 should be the ATAGs structure address (probably 0x100)
 */
void main(unsigned int r0, unsigned int machtype, unsigned int atagsaddr)
{
	/* No further need to access kernel code at 0x00000000 - 0x000fffff */
	initpagetable[0] = 0;
        
	/* Flush it out of the TLB */
	asm volatile("mcr p15, 0, %[data], c8, c7, 1" : : [data] "r" (0x00000000));

	/* Initialise stuff */
	mem_init();
	led_init();
	fb_init();
	
	// First boot message
	console_write("Welcome to PIqueno\n\n");
        
	// Creating my process in the process table
	create_main_process();
    
    // Create two sample processes with name and address of function
	fork("Sample process 1", &sample_process_1);
	fork("Sample process 2", &sample_process_2);
	fork("Sample process 3", &sample_process_2);
		
	// Configuring iterrupts	
	interrupts_init();
        
	// Do nothing and wait for scheduler to stop executing me
	main_endloop();
}

void main_endloop(void)
{
	// Repeatedly halt the CPU and wait for interrupt
	halt();
}
