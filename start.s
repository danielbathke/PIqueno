
.global	_start

_start:

	mov r4, #0x80000000

	cps #0x13		/* Change to supervisor (SVC) mode */
	add sp, r4, #0x2400

	cps #0x17		/* Change to Abort mode */
	add sp, r4, #0x2800

	cps #0x12		/* Change to IRQ mode */
	add sp, r4, #0x2c00

	cps #0x1f		/* Change to system mode */
	add sp, r4, #0x3c00

	/* Stay in system mode from now on */

	/* Turn on unaligned memory access */
	mrc p15, #0, r4, c1, c0, #0
	orr r4, #0x400000	/* 1<22 */
	mcr p15, #0, r4, c1, c0, #0

	/* Jump to memory map initialisation code */
	b initsys
	
/*
 * \b archContextSwitch
 *
 * Architecture-specific context switch routine.
 *
 * Note that interrupts are always locked out when this routine is
 * called. For cooperative switches, the scheduler will have entered
 * a critical region. For preemptions (called from an ISR), the
 * ISR will have disabled interrupts on entry.
 *
 * @param[in] old_tcb_ptr Pointer to the thread being scheduled out
 * @param[in] new_tcb_ptr Pointer to the thread being scheduled in
 *
 * @return None
 *
 * void archContextSwitch (unsigned int *old_tcb_ptr, unsigned int *new_tcb_ptr)
 */
archContextSwitch:
    STMFD       sp!, {r4 - r11, lr}             /* Save registers */

    STR         sp, [r0]                        /* Save old SP in old_tcb_ptr->sp_save_ptr (first TCB element) */
    LDR         r1, [r1]                        /* Load new SP from new_tcb_ptr->sp_save_ptr (first TCB element) */
    MOV         sp, r1                           

    LDMFD       sp!, {r4 - r11, pc}             /* Load new registers */
