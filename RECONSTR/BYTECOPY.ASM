ideal

; cannot use the model directive in this file (would cause word alignment)

segment BYTECOPY byte private 'CODE'

proc	_CopyBytes
public	_CopyBytes

	push	bp
	mov	bp, sp

	push	ds
	push	es
	push	si
	push	di

	les	di, [bp+6]
	lds	si, [bp+10]
	mov	cx, [bp+14]
	rep movsb
	
	pop	di
	pop	si
	pop	es
	pop	ds
	
	mov	sp, bp
	pop	bp
	retf

endp

ends

end