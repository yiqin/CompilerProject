	.text
	.file	"add.ll"
	.globl	printd
	.align	16, 0x90
	.type	printd,@function
printd:                                 # @printd
	.cfi_startproc
# BB#0:
	movl	%edi, -4(%rsp)
	movl	$1, %eax
	retq
.Ltmp0:
	.size	printd, .Ltmp0-printd
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:
	subq	$24, %rsp
.Ltmp1:
	.cfi_def_cfa_offset 32
	movl	$0, 20(%rsp)
	movl	%edi, 16(%rsp)
	movl	$450, 12(%rsp)          # imm = 0x1C2
	movl	$-123, 8(%rsp)
	movl	12(%rsp), %edi
	addl	$-123, %edi
	callq	printd
	xorl	%eax, %eax
	addq	$24, %rsp
	retq
.Ltmp2:
	.size	main, .Ltmp2-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
