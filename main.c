#include "led.h"
#include "framebuffer.h"
#include "interrupts.h"
#include "mailbox.h"
#include "memory.h"
#include "memutils.h"
#include "textutils.h"
#include "process.h"
#include "scheduler.h"

// Location of the initial page table in RAM
static unsigned int *initpagetable = (unsigned int *) mem_p2v(0x4000);

void main(unsigned int r0, unsigned int machtype, unsigned int atagsaddr)
{
	// Initialise basic memory management
	mem_init();
	
	// Initialize led management
	led_init();
	
	// Initialize framebuffer (video reserved memory location)
	fb_init();
	
	// First boot message
	console_write("Welcome to PIqueno\n\n");
        
	// Creating my process in the process table
	create_main_process();
    
    // Create three sample processes with name and address of function
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
