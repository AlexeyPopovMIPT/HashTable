_ZN9HashTableD2Ev:
.LFB5552:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	push	r14
	push	r13
	push	r12
	push	rbx
	sub	rsp, 16
	.cfi_offset 14, -24
	.cfi_offset 13, -32
	.cfi_offset 12, -40
	.cfi_offset 3, -48
	mov	QWORD PTR -40[rbp], rdi
	mov	rax, QWORD PTR -40[rbp]
	mov	rbx, QWORD PTR 2097152[rax]
	mov	rax, QWORD PTR -40[rbp]
	mov	rax, QWORD PTR 2097152[rax]
	lea	r14, 2097120[rax]
.L81:
	cmp	rbx, r14
	ja	.L82
	mov	rax, QWORD PTR [rbx]
	test	rax, rax
	je	.L83
	mov	r12, QWORD PTR 8[rbx]
	mov	r13, r12
.L80:
	test	r12, r12
	je	.L78
	mov	r12, QWORD PTR 8[r12]
	test	r13, r13
	je	.L79
	mov	rdi, r13
	call	_ZN9HashTable12ChainElementD1Ev
	mov	esi, 24
	mov	rdi, r13
	call	_ZdlPvm@PLT
.L79:
	mov	r13, r12
	jmp	.L80
.L78:
	mov	rax, QWORD PTR [rbx]
	mov	rdi, rax
	call	free@PLT
	jmp	.L77
.L83:
	nop
.L77:
	add	rbx, 32
	jmp	.L81
.L82:
	nop
	add	rsp, 16
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
