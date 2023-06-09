/*
 * Public domain.
 *
 */

#include <machine/asm.h>

RCSID("$NetBSD: s_log1p.S,v 1.7 1995/05/09 00:10:58 jtc Exp $")

	.section .rodata

	.align ALIGNARG(4)
	/* The fyl2xp1 can only be used for values in
		-1 + sqrt(2) / 2 <= x <= 1 - sqrt(2) / 2
	   0.29 is a safe value.
	*/
limit:	.quad   0x947ae147ae147ae1	/* 0.29  */
	.short	0x3ffd
	/* Please note:	 we use a double value here.  Since 1.0 has
	   an exact representation this does not effect the accuracy
	   but it helps to optimize the code.  */
one:	.double 1.0

/*
 * Use the fyl2xp1 function when the argument is in the range -0.29 to 0.29,
 * otherwise fyl2x with the needed extra computation.
 */
#ifdef PIC
#define MO(op) op##(%rip)
#else
#define MO(op) op
#endif

	.text
ENTRY(__log1pl)
	fldln2

	fldt	8(%rsp)

	fxam
	fnstsw
	fld	%st
	testb	$1, %ah
	jnz	3f		// in case x is NaN or �Inf
4:
	fabs
	fldt	MO(limit)
	fcompp
	fnstsw
	andb	$1,%ah
	jz	2f

	movzwl	8+8(%rsp), %eax
	xorb	$0x80, %ah
	cmpl	$0xc040, %eax
	jae	5f

	faddl	MO(one)
5:	fyl2x
	ret

2:	fyl2xp1
	ret

3:	testb	$4, %ah
	jnz	4b		// in case x is �Inf
	fstp	%st(1)
	fstp	%st(1)
	fadd	%st(0)
	ret

END (__log1pl)
