	.file	"sha1.c"
	.text
	.globl	SHA1Reset
	.type	SHA1Reset, @function
SHA1Reset:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L2
	movl	$1, %eax
	jmp	.L3
.L2:
	movq	-8(%rbp), %rax
	movl	$0, 20(%rax)
	movq	-8(%rbp), %rax
	movl	$0, 24(%rax)
	movq	-8(%rbp), %rax
	movw	$0, 28(%rax)
	movq	-8(%rbp), %rax
	movl	$1732584193, (%rax)
	movq	-8(%rbp), %rax
	movl	$-271733879, 4(%rax)
	movq	-8(%rbp), %rax
	movl	$-1732584194, 8(%rax)
	movq	-8(%rbp), %rax
	movl	$271733878, 12(%rax)
	movq	-8(%rbp), %rax
	movl	$-1009589776, 16(%rax)
	movq	-8(%rbp), %rax
	movl	$0, 96(%rax)
	movq	-8(%rbp), %rax
	movl	$0, 100(%rax)
	movl	$0, %eax
.L3:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	SHA1Reset, .-SHA1Reset
	.globl	SHA1Result
	.type	SHA1Result, @function
SHA1Result:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	cmpq	$0, -24(%rbp)
	je	.L5
	cmpq	$0, -32(%rbp)
	jne	.L6
.L5:
	movl	$1, %eax
	jmp	.L7
.L6:
	movq	-24(%rbp), %rax
	movl	100(%rax), %eax
	testl	%eax, %eax
	je	.L8
	movq	-24(%rbp), %rax
	movl	100(%rax), %eax
	jmp	.L7
.L8:
	movq	-24(%rbp), %rax
	movl	96(%rax), %eax
	testl	%eax, %eax
	jne	.L9
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	SHA1PadMessage
	movl	$0, -4(%rbp)
	jmp	.L10
.L11:
	movq	-24(%rbp), %rdx
	movl	-4(%rbp), %eax
	cltq
	movb	$0, 30(%rdx,%rax)
	addl	$1, -4(%rbp)
.L10:
	cmpl	$63, -4(%rbp)
	jle	.L11
	movq	-24(%rbp), %rax
	movl	$0, 20(%rax)
	movq	-24(%rbp), %rax
	movl	$0, 24(%rax)
	movq	-24(%rbp), %rax
	movl	$1, 96(%rax)
.L9:
	movl	$0, -4(%rbp)
	jmp	.L12
.L13:
	movl	-4(%rbp), %eax
	sarl	$2, %eax
	movl	%eax, %edx
	movq	-24(%rbp), %rax
	movslq	%edx, %rdx
	movl	(%rax,%rdx,4), %edx
	movl	-4(%rbp), %eax
	notl	%eax
	andl	$3, %eax
	sall	$3, %eax
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %ecx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movl	%ecx, %edx
	movb	%dl, (%rax)
	addl	$1, -4(%rbp)
.L12:
	cmpl	$19, -4(%rbp)
	jle	.L13
	movl	$0, %eax
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	SHA1Result, .-SHA1Result
	.globl	SHA1Input
	.type	SHA1Input, @function
SHA1Input:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	cmpl	$0, -20(%rbp)
	jne	.L15
	movl	$0, %eax
	jmp	.L16
.L15:
	cmpq	$0, -8(%rbp)
	je	.L17
	cmpq	$0, -16(%rbp)
	jne	.L18
.L17:
	movl	$1, %eax
	jmp	.L16
.L18:
	movq	-8(%rbp), %rax
	movl	96(%rax), %eax
	testl	%eax, %eax
	je	.L19
	movq	-8(%rbp), %rax
	movl	$3, 100(%rax)
	movl	$3, %eax
	jmp	.L16
.L19:
	movq	-8(%rbp), %rax
	movl	100(%rax), %eax
	testl	%eax, %eax
	je	.L21
	movq	-8(%rbp), %rax
	movl	100(%rax), %eax
	jmp	.L16
.L25:
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	movl	%eax, %edx
	addl	$1, %edx
	movl	%edx, %ecx
	movq	-8(%rbp), %rdx
	movw	%cx, 28(%rdx)
	movswl	%ax, %esi
	movq	-16(%rbp), %rax
	movzbl	(%rax), %ecx
	movq	-8(%rbp), %rdx
	movslq	%esi, %rax
	movb	%cl, 30(%rdx,%rax)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	leal	8(%rax), %edx
	movq	-8(%rbp), %rax
	movl	%edx, 20(%rax)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	testl	%eax, %eax
	jne	.L22
	movq	-8(%rbp), %rax
	movl	24(%rax), %eax
	leal	1(%rax), %edx
	movq	-8(%rbp), %rax
	movl	%edx, 24(%rax)
	movq	-8(%rbp), %rax
	movl	24(%rax), %eax
	testl	%eax, %eax
	jne	.L22
	movq	-8(%rbp), %rax
	movl	$1, 100(%rax)
.L22:
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	cmpw	$64, %ax
	jne	.L23
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	SHA1ProcessMessageBlock
.L23:
	addq	$1, -16(%rbp)
.L21:
	movl	-20(%rbp), %eax
	leal	-1(%rax), %edx
	movl	%edx, -20(%rbp)
	testl	%eax, %eax
	je	.L24
	movq	-8(%rbp), %rax
	movl	100(%rax), %eax
	testl	%eax, %eax
	je	.L25
.L24:
	movl	$0, %eax
.L16:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	SHA1Input, .-SHA1Input
	.globl	SHA1ProcessMessageBlock
	.type	SHA1ProcessMessageBlock, @function
SHA1ProcessMessageBlock:
.LFB3:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$384, %rsp
	movq	%rdi, -376(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, -364(%rbp)
	jmp	.L27
.L28:
	movl	-364(%rbp), %eax
	sall	$2, %eax
	movq	-376(%rbp), %rdx
	cltq
	movzbl	30(%rdx,%rax), %eax
	movzbl	%al, %eax
	sall	$24, %eax
	movl	%eax, %edx
	movl	-364(%rbp), %eax
	cltq
	movl	%edx, -336(%rbp,%rax,4)
	movl	-364(%rbp), %eax
	cltq
	movl	-336(%rbp,%rax,4), %ecx
	movl	-364(%rbp), %eax
	sall	$2, %eax
	addl	$1, %eax
	movq	-376(%rbp), %rdx
	cltq
	movzbl	30(%rdx,%rax), %eax
	movzbl	%al, %eax
	sall	$16, %eax
	orl	%eax, %ecx
	movl	%ecx, %edx
	movl	-364(%rbp), %eax
	cltq
	movl	%edx, -336(%rbp,%rax,4)
	movl	-364(%rbp), %eax
	cltq
	movl	-336(%rbp,%rax,4), %ecx
	movl	-364(%rbp), %eax
	sall	$2, %eax
	addl	$2, %eax
	movq	-376(%rbp), %rdx
	cltq
	movzbl	30(%rdx,%rax), %eax
	movzbl	%al, %eax
	sall	$8, %eax
	orl	%eax, %ecx
	movl	%ecx, %edx
	movl	-364(%rbp), %eax
	cltq
	movl	%edx, -336(%rbp,%rax,4)
	movl	-364(%rbp), %eax
	cltq
	movl	-336(%rbp,%rax,4), %ecx
	movl	-364(%rbp), %eax
	sall	$2, %eax
	addl	$3, %eax
	movq	-376(%rbp), %rdx
	cltq
	movzbl	30(%rdx,%rax), %eax
	movzbl	%al, %eax
	orl	%eax, %ecx
	movl	%ecx, %edx
	movl	-364(%rbp), %eax
	cltq
	movl	%edx, -336(%rbp,%rax,4)
	addl	$1, -364(%rbp)
.L27:
	cmpl	$15, -364(%rbp)
	jle	.L28
	movl	$16, -364(%rbp)
	jmp	.L29
.L30:
	movl	-364(%rbp), %eax
	subl	$3, %eax
	cltq
	movl	-336(%rbp,%rax,4), %edx
	movl	-364(%rbp), %eax
	subl	$8, %eax
	cltq
	movl	-336(%rbp,%rax,4), %eax
	xorl	%eax, %edx
	movl	-364(%rbp), %eax
	subl	$14, %eax
	cltq
	movl	-336(%rbp,%rax,4), %eax
	xorl	%eax, %edx
	movl	-364(%rbp), %eax
	subl	$16, %eax
	cltq
	movl	-336(%rbp,%rax,4), %eax
	xorl	%edx, %eax
	roll	%eax
	movl	%eax, %edx
	movl	-364(%rbp), %eax
	cltq
	movl	%edx, -336(%rbp,%rax,4)
	addl	$1, -364(%rbp)
.L29:
	cmpl	$79, -364(%rbp)
	jle	.L30
	movq	-376(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -360(%rbp)
	movq	-376(%rbp), %rax
	movl	4(%rax), %eax
	movl	%eax, -356(%rbp)
	movq	-376(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, -352(%rbp)
	movq	-376(%rbp), %rax
	movl	12(%rax), %eax
	movl	%eax, -348(%rbp)
	movq	-376(%rbp), %rax
	movl	16(%rax), %eax
	movl	%eax, -344(%rbp)
	movl	$0, -364(%rbp)
	jmp	.L31
.L32:
	movl	-360(%rbp), %eax
	roll	$5, %eax
	movl	%eax, %ecx
	movl	-356(%rbp), %eax
	andl	-352(%rbp), %eax
	movl	%eax, %edx
	movl	-356(%rbp), %eax
	notl	%eax
	andl	-348(%rbp), %eax
	orl	%edx, %eax
	leal	(%rcx,%rax), %edx
	movl	-344(%rbp), %eax
	addl	%eax, %edx
	movl	-364(%rbp), %eax
	cltq
	movl	-336(%rbp,%rax,4), %eax
	addl	%eax, %edx
	movl	K.2054(%rip), %eax
	addl	%edx, %eax
	movl	%eax, -340(%rbp)
	movl	-348(%rbp), %eax
	movl	%eax, -344(%rbp)
	movl	-352(%rbp), %eax
	movl	%eax, -348(%rbp)
	movl	-356(%rbp), %eax
	rorl	$2, %eax
	movl	%eax, -352(%rbp)
	movl	-360(%rbp), %eax
	movl	%eax, -356(%rbp)
	movl	-340(%rbp), %eax
	movl	%eax, -360(%rbp)
	addl	$1, -364(%rbp)
.L31:
	cmpl	$19, -364(%rbp)
	jle	.L32
	movl	$20, -364(%rbp)
	jmp	.L33
.L34:
	movl	-360(%rbp), %eax
	roll	$5, %eax
	movl	%eax, %edx
	movl	-356(%rbp), %eax
	xorl	-352(%rbp), %eax
	xorl	-348(%rbp), %eax
	addl	%eax, %edx
	movl	-344(%rbp), %eax
	addl	%eax, %edx
	movl	-364(%rbp), %eax
	cltq
	movl	-336(%rbp,%rax,4), %eax
	addl	%eax, %edx
	movl	4+K.2054(%rip), %eax
	addl	%edx, %eax
	movl	%eax, -340(%rbp)
	movl	-348(%rbp), %eax
	movl	%eax, -344(%rbp)
	movl	-352(%rbp), %eax
	movl	%eax, -348(%rbp)
	movl	-356(%rbp), %eax
	rorl	$2, %eax
	movl	%eax, -352(%rbp)
	movl	-360(%rbp), %eax
	movl	%eax, -356(%rbp)
	movl	-340(%rbp), %eax
	movl	%eax, -360(%rbp)
	addl	$1, -364(%rbp)
.L33:
	cmpl	$39, -364(%rbp)
	jle	.L34
	movl	$40, -364(%rbp)
	jmp	.L35
.L36:
	movl	-360(%rbp), %eax
	roll	$5, %eax
	movl	%eax, %ecx
	movl	-352(%rbp), %eax
	orl	-348(%rbp), %eax
	andl	-356(%rbp), %eax
	movl	%eax, %edx
	movl	-352(%rbp), %eax
	andl	-348(%rbp), %eax
	orl	%edx, %eax
	leal	(%rcx,%rax), %edx
	movl	-344(%rbp), %eax
	addl	%eax, %edx
	movl	-364(%rbp), %eax
	cltq
	movl	-336(%rbp,%rax,4), %eax
	addl	%eax, %edx
	movl	8+K.2054(%rip), %eax
	addl	%edx, %eax
	movl	%eax, -340(%rbp)
	movl	-348(%rbp), %eax
	movl	%eax, -344(%rbp)
	movl	-352(%rbp), %eax
	movl	%eax, -348(%rbp)
	movl	-356(%rbp), %eax
	rorl	$2, %eax
	movl	%eax, -352(%rbp)
	movl	-360(%rbp), %eax
	movl	%eax, -356(%rbp)
	movl	-340(%rbp), %eax
	movl	%eax, -360(%rbp)
	addl	$1, -364(%rbp)
.L35:
	cmpl	$59, -364(%rbp)
	jle	.L36
	movl	$60, -364(%rbp)
	jmp	.L37
.L38:
	movl	-360(%rbp), %eax
	roll	$5, %eax
	movl	%eax, %edx
	movl	-356(%rbp), %eax
	xorl	-352(%rbp), %eax
	xorl	-348(%rbp), %eax
	addl	%eax, %edx
	movl	-344(%rbp), %eax
	addl	%eax, %edx
	movl	-364(%rbp), %eax
	cltq
	movl	-336(%rbp,%rax,4), %eax
	addl	%eax, %edx
	movl	12+K.2054(%rip), %eax
	addl	%edx, %eax
	movl	%eax, -340(%rbp)
	movl	-348(%rbp), %eax
	movl	%eax, -344(%rbp)
	movl	-352(%rbp), %eax
	movl	%eax, -348(%rbp)
	movl	-356(%rbp), %eax
	rorl	$2, %eax
	movl	%eax, -352(%rbp)
	movl	-360(%rbp), %eax
	movl	%eax, -356(%rbp)
	movl	-340(%rbp), %eax
	movl	%eax, -360(%rbp)
	addl	$1, -364(%rbp)
.L37:
	cmpl	$79, -364(%rbp)
	jle	.L38
	movq	-376(%rbp), %rax
	movl	(%rax), %edx
	movl	-360(%rbp), %eax
	addl	%eax, %edx
	movq	-376(%rbp), %rax
	movl	%edx, (%rax)
	movq	-376(%rbp), %rax
	movl	4(%rax), %edx
	movl	-356(%rbp), %eax
	addl	%eax, %edx
	movq	-376(%rbp), %rax
	movl	%edx, 4(%rax)
	movq	-376(%rbp), %rax
	movl	8(%rax), %edx
	movl	-352(%rbp), %eax
	addl	%eax, %edx
	movq	-376(%rbp), %rax
	movl	%edx, 8(%rax)
	movq	-376(%rbp), %rax
	movl	12(%rax), %edx
	movl	-348(%rbp), %eax
	addl	%eax, %edx
	movq	-376(%rbp), %rax
	movl	%edx, 12(%rax)
	movq	-376(%rbp), %rax
	movl	16(%rax), %edx
	movl	-344(%rbp), %eax
	addl	%eax, %edx
	movq	-376(%rbp), %rax
	movl	%edx, 16(%rax)
	movq	-376(%rbp), %rax
	movw	$0, 28(%rax)
	nop
	movq	-8(%rbp), %rax
	xorq	%fs:40, %rax
	je	.L39
	call	__stack_chk_fail@PLT
.L39:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	SHA1ProcessMessageBlock, .-SHA1ProcessMessageBlock
	.globl	SHA1PadMessage
	.type	SHA1PadMessage, @function
SHA1PadMessage:
.LFB4:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	cmpw	$55, %ax
	jle	.L41
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	movl	%eax, %edx
	addl	$1, %edx
	movl	%edx, %ecx
	movq	-8(%rbp), %rdx
	movw	%cx, 28(%rdx)
	cwtl
	movq	-8(%rbp), %rdx
	cltq
	movb	$-128, 30(%rdx,%rax)
	jmp	.L42
.L43:
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	movl	%eax, %edx
	addl	$1, %edx
	movl	%edx, %ecx
	movq	-8(%rbp), %rdx
	movw	%cx, 28(%rdx)
	cwtl
	movq	-8(%rbp), %rdx
	cltq
	movb	$0, 30(%rdx,%rax)
.L42:
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	cmpw	$63, %ax
	jle	.L43
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	SHA1ProcessMessageBlock
	jmp	.L44
.L45:
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	movl	%eax, %edx
	addl	$1, %edx
	movl	%edx, %ecx
	movq	-8(%rbp), %rdx
	movw	%cx, 28(%rdx)
	cwtl
	movq	-8(%rbp), %rdx
	cltq
	movb	$0, 30(%rdx,%rax)
.L44:
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	cmpw	$55, %ax
	jle	.L45
	jmp	.L46
.L41:
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	movl	%eax, %edx
	addl	$1, %edx
	movl	%edx, %ecx
	movq	-8(%rbp), %rdx
	movw	%cx, 28(%rdx)
	cwtl
	movq	-8(%rbp), %rdx
	cltq
	movb	$-128, 30(%rdx,%rax)
	jmp	.L47
.L48:
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	movl	%eax, %edx
	addl	$1, %edx
	movl	%edx, %ecx
	movq	-8(%rbp), %rdx
	movw	%cx, 28(%rdx)
	cwtl
	movq	-8(%rbp), %rdx
	cltq
	movb	$0, 30(%rdx,%rax)
.L47:
	movq	-8(%rbp), %rax
	movzwl	28(%rax), %eax
	cmpw	$55, %ax
	jle	.L48
.L46:
	movq	-8(%rbp), %rax
	movl	24(%rax), %eax
	shrl	$24, %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movb	%dl, 86(%rax)
	movq	-8(%rbp), %rax
	movl	24(%rax), %eax
	shrl	$16, %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movb	%dl, 87(%rax)
	movq	-8(%rbp), %rax
	movl	24(%rax), %eax
	shrl	$8, %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movb	%dl, 88(%rax)
	movq	-8(%rbp), %rax
	movl	24(%rax), %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movb	%dl, 89(%rax)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	shrl	$24, %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movb	%dl, 90(%rax)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	shrl	$16, %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movb	%dl, 91(%rax)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	shrl	$8, %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movb	%dl, 92(%rax)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movb	%dl, 93(%rax)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	SHA1ProcessMessageBlock
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	SHA1PadMessage, .-SHA1PadMessage
	.section	.rodata
	.align 16
	.type	K.2054, @object
	.size	K.2054, 16
K.2054:
	.long	1518500249
	.long	1859775393
	.long	-1894007588
	.long	-899497514
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
