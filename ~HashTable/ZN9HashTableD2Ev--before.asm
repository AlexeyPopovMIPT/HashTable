_ZN9HashTableD2Ev:
.LFB5552:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	push	rbx
	sub	rsp, 56
	.cfi_offset 3, -24
	mov	r8, rdi
	mov	rax, r8
	mov	rax, QWORD PTR 2097152[rax]
	mov	r9, rax
	mov	rax, r8
	mov	rax, QWORD PTR 2097152[rax]
	add	rax, 2097120
	mov	r12, rax
.L81:
	cmp	r9, r12
	ja	.L82
	mov	rcx, QWORD PTR [r9]
	jrcxz	.L77
	mov	rax, QWORD PTR 8[r9]
	mov	r10, rax
	mov	r11, rax
.L80:
	cmp	r10, 0               
	je	.L78                
	mov	rax, QWORD PTR 8[r10]
	mov	r10, rax             
	;;;mov	rbx, r11
	;;;test	rbx, rbx
	;;;je	.L79
	mov	rdi, r11
	call	_ZN9HashTable12ChainElementD1Ev
	mov	esi, 24
	mov	rdi, r11
	call	_ZdlPvm@PLT
.L79:
	;;;mov	rax, r10
	mov	r11, r10           
	jmp	.L80 
.L78:
	mov	rax, QWORD PTR [r9]
	mov	rdi, rcx
	call	free@PLT
.L77:
	add	r9, 32
	jmp	.L81
.L82:
	nop
	add	rsp, 56
	pop	rbx
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
