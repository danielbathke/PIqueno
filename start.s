
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
