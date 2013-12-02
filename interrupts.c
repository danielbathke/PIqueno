#include "interrupts.h"

#include "framebuffer.h"
#include "led.h"
#include "memory.h"
#include "textutils.h"
#include "scheduler.h"
#include "syscall.h"

static volatile unsigned int *irqEnable1 = (unsigned int *) mem_p2v(0x2000b210);
static volatile unsigned int *irqEnable2 = (unsigned int *) mem_p2v(0x2000b214);
static volatile unsigned int *irqEnableBasic = (unsigned int *) mem_p2v(0x2000b218);

static volatile unsigned int *armTimerLoad = (unsigned int *) mem_p2v(0x2000b400);
static volatile unsigned int *armTimerValue = (unsigned int *) mem_p2v(0x2000b404);
static volatile unsigned int *armTimerControl = (unsigned int *) mem_p2v(0x2000b408);
static volatile unsigned int *armTimerIRQClear = (unsigned int *) mem_p2v(0x2000b40c);

/* Interrupt vectors called by the CPU. Needs to be aligned to 32 bits as the
 * bottom 5 bits of the vector address as set in the control coprocessor must
 * be zero
 *
 * The RESET vector is set to bad_exception. On CPU reset the interrupt vectors
 * are set back to 0x00000000, so it won't be used. Any attempt to call this
 * vector is clearly an error. Also, resetting the Pi will reset VideoCore,
 * and reboot.
 */
__attribute__ ((naked, aligned(32))) static void interrupt_vectors(void)
{
	asm volatile("b bad_exception\n"		// Reset
		"b bad_exception\n"					// Undefined
		"b interrupt_swi\n"					// Software interrupt
		"b interrupt_prefetch_abort \n"		// Prefech abort interrupt
		"b interrupt_data_abort \n"			// Data abort interrupt
		"b bad_exception;\n"				// Unused vector
		"b interrupt_irq \n"				// IRQ interrupt
		"b bad_exception\n"					// Fast Interrupt (FIQ), not used
	);
}

// Unhandled exceptions, halt
__attribute__ ((naked)) void bad_exception(void)
{
	console_write("Bad exception. System halted.");
	while(1);
}

// Software interrupt
__attribute__ ((interrupt ("SWI"))) void interrupt_swi(void)
{
	unsigned int addr;
	unsigned int swi;
	
	// Read link register into addr - contains the address of the instruction after the SWI
	asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );

	addr -= 4;
	
	// Bottom 24 bits of the SWI instruction are the SWI number
	swi = *((unsigned int *)addr) & 0x00ffffff;

	// Debug message
	console_write("\n");
	console_write(COLOUR_PUSH FG_GREEN "SWI call. Address: 0x");
	console_write(tohex(addr, 4));
	console_write("  SWI number ");
	console_write(todec(swi, 0));
	console_write(COLOUR_POP "\n");
	
	// Changing processor mode
	asm volatile("cps #0x1f");
	
	// Handle syscall
	syscall(swi);
}

// IRQ
__attribute__ ((interrupt ("IRQ"))) void interrupt_irq(void)
{   
	// This function starts on IRQ mode
	
	// Push all registers into the IRQ mode stack (R13)
	asm volatile("push {R0-R12}");
	
	// Put LR register of IRQ mode (PC of interrupted process) on R0
	asm volatile("MOV R0, LR");
	
	
	// Change to system mode
	asm volatile("cps #0x1f");
	
	// Push R0 (interrupted PC) to the system mode stack
	asm volatile("push {R0}");
	
	
	// Return to IRQ mode
	asm volatile("cps #0x12");
	
	// Pop all registers again
	asm volatile("pop {R0-R12}");
	
	
	// Return to system mode
	asm volatile("cps #0x1f");
	
	// Push all registers into the system mode stack
	asm volatile("push {R0-R12}");
	
	// Push the interrupted LR to system mode stack
	asm volatile("push {LR}");
	
	// Copy the processor status to R0
    asm volatile("MRS R0, SPSR");
    
    // Push the processor status to system mode stack
    asm volatile("push {R0}");
    
    
    // Return to IRQ mode
    asm volatile("cps #0x12");
	
	// Copy LR to R0
	asm volatile("MOV R0, LR");
	
	// Back to system mode
	asm volatile("cps #0x1f");
	
	unsigned long pc;

    unsigned long stack_pointer;
	
	// Getting pc and stack just to debug
	asm volatile ("MOV %0, R0\n\t" : "=r" (pc) );
    asm volatile ("MOV %0, SP\n\t" : "=r" (stack_pointer) );
    
    // Invert led to inform context switch activity
	led_invert();
	
	// Jump to scheduler to do the context switch
	schedule_timeout(stack_pointer, pc);
}

__attribute__ ((interrupt ("ABORT"))) void interrupt_data_abort(void)
{		
	register unsigned int addr, far;
	asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );
	/* Read fault address register */
	asm volatile("mrc p15, 0, %[addr], c6, c0, 0": [addr] "=r" (far) );


	console_write("Data abort!\n");
	console_write("Instruction address: 0x");
	/* addr = lr, but the very start of the abort routine does
	 * sub lr, lr, #4
	 * lr = address of aborted instruction, plus 8
	 */
	console_write(tohex(addr-4, 4));

	console_write("  fault address: 0x");
	console_write(tohex(far, 4));
	console_write("\n");

	/* Routine terminates by returning to LR-4, which is the instruction
	 * after the aborted one
	 * GCC doesn't properly deal with data aborts in its interrupt
	 * handling - no option to return to the failed instruction
	 */
	 
	asm volatile("cps #0x1f");	 
	asm volatile("cpsid aif");
}

// Return to this function after a prefetch abort
extern void main_endloop(void);

__attribute__ ((interrupt ("ABORT"))) void interrupt_prefetch_abort(void)
{
	register unsigned int addr;
	asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );

	console_write("Prefetch abort!\n");
	console_write("Instruction address: 0x");
	/* lr = address of aborted instruction, plus 4
	 * addr = lr, but the very start of the abort routine does
	 * sub lr, lr, #4
	 */
	console_write(tohex(addr, 4));
	console_write("\n");

	/* Set the return address to be the function main_endloop(), by
	 * putting its address into the program counter
	 *
	 * THIS IS JUST A TEST - you can't normally do this as it doesn't
	 * restore the registers or put the stack pointer back where it was,
	 * so repeated aborts will overflow the stack.
	 */
	asm volatile("movs pc, %[addr]" : :
		[addr] "r" ((unsigned int)(&main_endloop)) );

	/* Doesn't reach this point */
}

/* Initialise the interrupts
 *
 * Enable the ARM timer interrupt
 */
void interrupts_init(void)
{
	// Set interrupt base register
	asm volatile("mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" (&interrupt_vectors));
        
	// Turn on interrupts
	asm volatile("cpsie i");

	// Enable ARM timer IRQ
	*irqEnableBasic = 0x00000001;

	// Interrupt every x * 256 (prescaler) timer ticks
	*armTimerLoad = 0x00000400;

	// Timer enabled
	*armTimerControl = 0x000000aa;
}

void timer_reset(void)
{
	*armTimerIRQClear = 0;
}
