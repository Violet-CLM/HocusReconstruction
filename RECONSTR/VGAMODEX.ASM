ideal

model large, c

codeseg

proc	StartPlanarVGA _dummy:WORD
public	StartPlanarVGA
uses	si, di

	; change video mode:
	mov	ax, 13h		; mode change to VGA 320x200
	int	10h

	; enable sequential memory mode:
	mov	dx, 3C4h
	mov	al, 4
	out	dx, al
	inc	dx
	in 	al, dx
	and	al, not 8	; clear bit 3 (chain 4 linear graphics mode)
	or 	al, 4		; set bit 2 (sequential memory mode)
	out	dx, al

	; disable odd/even RAM addressing:
	mov	dx, 3CEh
	mov	al, 5
	out	dx, al
	inc	dx
	in 	al, dx
	and	al, not 10h	; clear bit 4 (odd/even RAM addressing)
	out	dx, al
	dec	dx

	; do not chain odd maps after even maps:
	mov	al, 6
	out	dx, al
	inc	dx
	in 	al, dx
	and	al, not 2	; clear bit 1 (chain odd maps after even maps)
	out	dx, al

	; according to the WOLFENSTEIN 3-D source code,
	; the mode change above only clears 16k per plane,
	; so make sure all the video memory gets cleared:
	mov	dx, 3C4h
	mov	ax, 0F02h
	out	dx, ax
	mov	ax, 0A000h
	mov	es, ax
	sub	di, di
	mov	ax, di
	mov	cx, 8000h
	cld
	rep stosw

	; set maximum scan line to 1:
	mov	dx, 3D4h
	mov	al, 9		; maximum scan lines register
	out	dx, al
	inc	dx
	in 	al, dx
	mov	al, 1		; disable double scan, etc., max. scan line = 1
	out	dx, al
	dec	dx

	; make "underline" use word mode:
	mov	al, 14h		; underline location register
	out	dx, al
	inc	dx
	in 	al, dx
	and	al, not 40h	; clear bit 6 (disable dword mode)
	out	dx, al
	dec	dx

	; enable byte mode:
	mov	al, 17h		; mode control register
	out	dx, al
	inc	dx
	in 	al, dx
	or 	al, 40h		; set bit 6 (byte mode)
	out	dx, al
	
	ret
endp

ends

end
