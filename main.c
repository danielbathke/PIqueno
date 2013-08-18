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

/* Pull various bits of information from the VideoCore and display it on
 * screen
 */
void mailboxtest(void)
{
	/* 1kb buffer on the stack for passing data to/from VideoCore */
	volatile unsigned int buffer[256] __attribute__((aligned (16)));
	unsigned int count, var;
	unsigned int mem, size;

	console_write(BG_GREEN BG_HALF "Reading from tag mailbox\n\n" BG_BLACK);

	buffer[0] = 8 * 4;	// Total size
	buffer[1] = 0;		// Request

	buffer[2] = 0x40003;	// Display size
	buffer[3] = 8;		// Buffer size
	buffer[4] = 0;		// Request size
	buffer[5] = 0;
	buffer[6] = 0;

	buffer[7] = 0;

	writemailbox(8, mem_v2p((unsigned int)buffer));

	var = readmailbox(8);

	console_write(COLOUR_PUSH FG_CYAN "Display resolution: " BG_WHITE BG_HALF BG_HALF);
	console_write(todec(buffer[5], 0));
	console_write("x");
	console_write(todec(buffer[6], 0));
	console_write(COLOUR_POP "\n");

	buffer[0] = 8 * 4;	// Total size
	buffer[1] = 0;		// Request

	buffer[2] = 0x40008;	// Display size
	buffer[3] = 8;		// Buffer size
	buffer[4] = 0;		// Request size
	buffer[5] = 0;
	buffer[6] = 0;

	buffer[7] = 0;

	writemailbox(8, mem_v2p((unsigned int)buffer));

	var = readmailbox(8);

	console_write(COLOUR_PUSH FG_CYAN "Pitch: " BG_WHITE BG_HALF BG_HALF);
	console_write(todec(buffer[5], 0));
	console_write(" bytes" COLOUR_POP "\n");

	buffer[0] = 200 * 4;	// Total size
	buffer[1] = 0;		// Request

	buffer[2] = 0x50001;	// Command line
	buffer[3] = 195*4;	// Buffer size
	buffer[4] = 0;		// Request size
	
	for(count=5; count<200; count++)
		buffer[count] = 0;

	writemailbox(8, mem_v2p((unsigned int)buffer));

	var = readmailbox(8);

	console_write("\n" COLOUR_PUSH FG_RED "Kernel command line: " COLOUR_PUSH BG_RED BG_HALF BG_HALF);
	console_write((char *)(&buffer[5]));
	console_write(COLOUR_POP COLOUR_POP "\n\n");


	buffer[0] = 13 * 4;	// Total size
	buffer[1] = 0;		// Request

	buffer[2] = 0x10005;	// ARM memory
	buffer[3] = 8;		// Buffer size
	buffer[4] = 0;		// Request size
	buffer[5] = 0;
	buffer[6] = 0;

	buffer[7] = 0x10006;	// VideoCore memory
	buffer[8] = 8;		// Buffer size
	buffer[9] = 0;		// Request size
	buffer[10] = 0;
	buffer[11] = 0;

	buffer[12] = 0;

	writemailbox(8, mem_v2p((unsigned int)buffer));

	var = readmailbox(8);

	mem = buffer[5];
	size = buffer[6];
	var = size / (1024*1024);

	console_write(COLOUR_PUSH FG_YELLOW "ARM memory: " BG_YELLOW BG_HALF BG_HALF "0x");
	console_write(tohex(mem, 4));
	console_write(" - 0x");
	console_write(tohex(mem+size-1, 4));
	console_write(" (");
	console_write(todec(size, 0));
	/* ] appears as an arrow in the SAA5050 character set */
	console_write(" bytes ] ");
	console_write(todec(var, 0));
	console_write(" megabytes)" COLOUR_POP "\n");

	mem = buffer[10];
	size = buffer[11];
	var = size / (1024*1024);
	console_write(COLOUR_PUSH FG_YELLOW "VC memory:  " BG_YELLOW BG_HALF BG_HALF "0x");
	console_write(tohex(mem, 4));
	console_write(" - 0x");
	console_write(tohex(mem+size-1, 4));
	console_write(" (");
	console_write(todec(size, 0));
	console_write(" bytes ] ");
	console_write(todec(var, 0));
	console_write(" megabytes)" COLOUR_POP "\n");
}

/* Call non-existent code at 33MB - should cause a prefetch abort */
//static void(*deliberate_prefetch_abort)(void) = (void(*)(void))0x02100000;

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
	interrupts_init();

	/* First boot message */
	console_write("Welcome to PIqueno\n\n");
        
	create_main_process();
        
        fork("Sample process 1", &sample_process_1);
        fork("Sample process 2", &sample_process_2);
        

	while(1);
}

void main_endloop(void)
{

	/* Repeatedly halt the CPU and wait for interrupt */
	while(1)
		asm volatile("mcr p15,0,r0,c7,c0,4" : : : "r0");

}
