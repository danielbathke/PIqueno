
#include "scheduler.h"
#include "syscall.h"
#include "framebuffer.h"
#include "textutils.h"

void syscall(unsigned int swi) {
	
	console_write("Handling syscall: ");
	console_write(todec(swi, 0));
	console_write("\n");
	
	switch (swi) {
		
		case SYSCALL_TERMINATE_PROCESS:
			console_write("Invoking syscall terminate_process()");
			console_write("\n");
			terminate_process();
			break;
	}

	console_write("Turning interrupt on again");
	console_write("\n");
	
	asm volatile("cpsie i");
	
	halt();
	
}
