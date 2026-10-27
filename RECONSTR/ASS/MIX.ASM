; Reconstructed Hocus Pocus Source Code
; Copyright (C) 2026 K1n9_Duk3
;
; The code in this file is based on the Apogee Sound System by James R. Dose.
; Copyright (C) 1994-1995 Apogee Software, Ltd.
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License along
; with this program; if not, write to the Free Software Foundation, Inc.,
; 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


.286

MIX_ASM		segment	byte public 'CODE' use16
		assume cs:MIX_ASM
		assume es:nothing, ss:nothing, ds:nothing



_Mix8bitAdd	proc far
public _Mix8bitAdd

arg_0		= dword	ptr  6
arg_4		= dword	ptr  0Ah
arg_8		= word ptr  0Eh

		push	bp
		mov	bp, sp
		push	si
		push	di
		push	ds
		lds	si, [bp+arg_4]
		les	di, [bp+arg_0]
		mov	dx, [bp+arg_8]
		sub	bx, bx
		cmp	dx, 10h
		jb	mix8a_remainder

mix8a_loop_16:
		mov	al, [bx+si]
		add	es:[bx+di], al
		mov	al, [bx+si+1]
		add	es:[bx+di+1], al
		mov	al, [bx+si+2]
		add	es:[bx+di+2], al
		mov	al, [bx+si+3]
		add	es:[bx+di+3], al
		mov	al, [bx+si+4]
		add	es:[bx+di+4], al
		mov	al, [bx+si+5]
		add	es:[bx+di+5], al
		mov	al, [bx+si+6]
		add	es:[bx+di+6], al
		mov	al, [bx+si+7]
		add	es:[bx+di+7], al
		mov	al, [bx+si+8]
		add	es:[bx+di+8], al
		mov	al, [bx+si+9]
		add	es:[bx+di+9], al
		mov	al, [bx+si+0Ah]
		add	es:[bx+di+0Ah],	al
		mov	al, [bx+si+0Bh]
		add	es:[bx+di+0Bh],	al
		mov	al, [bx+si+0Ch]
		add	es:[bx+di+0Ch],	al
		mov	al, [bx+si+0Dh]
		add	es:[bx+di+0Dh],	al
		mov	al, [bx+si+0Eh]
		add	es:[bx+di+0Eh],	al
		mov	al, [bx+si+0Fh]
		add	es:[bx+di+0Fh],	al
		add	bx, 10h
		sub	dx, 10h
		cmp	dx, 10h
		jnb	mix8a_loop_16

mix8a_remainder:
		or	dx, dx
		jz	mix8a_done

mix8a_loop_single:
		mov	al, [bx+si]
		add	es:[bx+di], al
		inc	bx
		dec	dx
		jnz	mix8a_loop_single

mix8a_done:
		pop	ds
		pop	di
		pop	si
		pop	bp
		retf
_Mix8bitAdd	endp




_Mix8bitSub	proc far
public _Mix8bitSub

arg_0		= dword	ptr  6
arg_4		= dword	ptr  0Ah
arg_8		= word ptr  0Eh

		push	bp
		mov	bp, sp
		push	si
		push	di
		push	ds
		lds	si, [bp+arg_4]
		les	di, [bp+arg_0]
		mov	dx, [bp+arg_8]
		sub	bx, bx
		cmp	dx, 10h
		jb	mix8s_remainder

mix8s_loop_16:
		mov	al, [bx+si]
		sub	es:[bx+di], al
		mov	al, [bx+si+1]
		sub	es:[bx+di+1], al
		mov	al, [bx+si+2]
		sub	es:[bx+di+2], al
		mov	al, [bx+si+3]
		sub	es:[bx+di+3], al
		mov	al, [bx+si+4]
		sub	es:[bx+di+4], al
		mov	al, [bx+si+5]
		sub	es:[bx+di+5], al
		mov	al, [bx+si+6]
		sub	es:[bx+di+6], al
		mov	al, [bx+si+7]
		sub	es:[bx+di+7], al
		mov	al, [bx+si+8]
		sub	es:[bx+di+8], al
		mov	al, [bx+si+9]
		sub	es:[bx+di+9], al
		mov	al, [bx+si+0Ah]
		sub	es:[bx+di+0Ah],	al
		mov	al, [bx+si+0Bh]
		sub	es:[bx+di+0Bh],	al
		mov	al, [bx+si+0Ch]
		sub	es:[bx+di+0Ch],	al
		mov	al, [bx+si+0Dh]
		sub	es:[bx+di+0Dh],	al
		mov	al, [bx+si+0Eh]
		sub	es:[bx+di+0Eh],	al
		mov	al, [bx+si+0Fh]
		sub	es:[bx+di+0Fh],	al
		add	bx, 10h
		sub	dx, 10h
		cmp	dx, 10h
		jnb	mix8s_loop_16

mix8s_remainder:
		or	dx, dx
		jz	mix8s_done

mix8s_loop_single:
		mov	al, [bx+si]
		sub	es:[bx+di], al
		inc	bx
		dec	dx
		jnz	mix8s_loop_single

mix8s_done:
		pop	ds
		pop	di
		pop	si
		pop	bp
		retf
_Mix8bitSub	endp




_Mix16bitAdd	proc far
public _Mix16bitAdd

arg_0		= dword	ptr  6
arg_4		= dword	ptr  0Ah
arg_8		= word ptr  0Eh
arg_A		= word ptr  10h

		push	bp
		mov	bp, sp
		push	si
		push	di
		push	ds
		lds	si, [bp+arg_4]
		les	di, [bp+arg_0]
		mov	dx, [bp+arg_8]
		mov	cx, [bp+arg_A]
		cmp	dx, 10h
		jnb	mix16a_loop_16
		jmp	mix16a_remainder

mix16a_loop_16:
		mov	al, [si]
		cbw
		shl	ax, cl
		add	es:[di], ax
		mov	al, [si+1]
		cbw
		shl	ax, cl
		add	es:[di+2], ax
		mov	al, [si+2]
		cbw
		shl	ax, cl
		add	es:[di+4], ax
		mov	al, [si+3]
		cbw
		shl	ax, cl
		add	es:[di+6], ax
		mov	al, [si+4]
		cbw
		shl	ax, cl
		add	es:[di+8], ax
		mov	al, [si+5]
		cbw
		shl	ax, cl
		add	es:[di+0Ah], ax
		mov	al, [si+6]
		cbw
		shl	ax, cl
		add	es:[di+0Ch], ax
		mov	al, [si+7]
		cbw
		shl	ax, cl
		add	es:[di+0Eh], ax
		mov	al, [si+8]
		cbw
		shl	ax, cl
		add	es:[di+10h], ax
		mov	al, [si+9]
		cbw
		shl	ax, cl
		add	es:[di+12h], ax
		mov	al, [si+0Ah]
		cbw
		shl	ax, cl
		add	es:[di+14h], ax
		mov	al, [si+0Bh]
		cbw
		shl	ax, cl
		add	es:[di+16h], ax
		mov	al, [si+0Ch]
		cbw
		shl	ax, cl
		add	es:[di+18h], ax
		mov	al, [si+0Dh]
		cbw
		shl	ax, cl
		add	es:[di+1Ah], ax
		mov	al, [si+0Eh]
		cbw
		shl	ax, cl
		add	es:[di+1Ch], ax
		mov	al, [si+0Fh]
		cbw
		shl	ax, cl
		add	es:[di+1Eh], ax
		add	si, 10h
		add	di, 20h
		sub	dx, 10h
		cmp	dx, 10h
		jb	mix16a_remainder
		jmp	mix16a_loop_16

mix16a_remainder:
		or	dx, dx
		jz	mix16a_done

mix16a_loop_single:
		mov	al, [si]
		cbw
		shl	ax, cl
		add	es:[di], ax
		inc	si
		add	di, 2
		dec	dx
		jnz	mix16a_loop_single

mix16a_done:
		pop	ds
		pop	di
		pop	si
		pop	bp
		retf
_Mix16bitAdd	endp




_Mix16bitSub	proc far
public _Mix16bitSub

arg_0		= dword	ptr  6
arg_4		= dword	ptr  0Ah
arg_8		= word ptr  0Eh
arg_A		= word ptr  10h

		push	bp
		mov	bp, sp
		push	si
		push	di
		push	ds
		lds	si, [bp+arg_4]
		les	di, [bp+arg_0]
		mov	dx, [bp+arg_8]
		mov	cx, [bp+arg_A]
		cmp	dx, 10h
		jnb	mix16s_loop_16
		jmp	mix16s_remainder

mix16s_loop_16:
		mov	al, [si]
		cbw
		shl	ax, cl
		sub	es:[di], ax
		mov	al, [si+1]
		cbw
		shl	ax, cl
		sub	es:[di+2], ax
		mov	al, [si+2]
		cbw
		shl	ax, cl
		sub	es:[di+4], ax
		mov	al, [si+3]
		cbw
		shl	ax, cl
		sub	es:[di+6], ax
		mov	al, [si+4]
		cbw
		shl	ax, cl
		sub	es:[di+8], ax
		mov	al, [si+5]
		cbw
		shl	ax, cl
		sub	es:[di+0Ah], ax
		mov	al, [si+6]
		cbw
		shl	ax, cl
		sub	es:[di+0Ch], ax
		mov	al, [si+7]
		cbw
		shl	ax, cl
		sub	es:[di+0Eh], ax
		mov	al, [si+8]
		cbw
		shl	ax, cl
		sub	es:[di+10h], ax
		mov	al, [si+9]
		cbw
		shl	ax, cl
		sub	es:[di+12h], ax
		mov	al, [si+0Ah]
		cbw
		shl	ax, cl
		sub	es:[di+14h], ax
		mov	al, [si+0Bh]
		cbw
		shl	ax, cl
		sub	es:[di+16h], ax
		mov	al, [si+0Ch]
		cbw
		shl	ax, cl
		sub	es:[di+18h], ax
		mov	al, [si+0Dh]
		cbw
		shl	ax, cl
		sub	es:[di+1Ah], ax
		mov	al, [si+0Eh]
		cbw
		shl	ax, cl
		sub	es:[di+1Ch], ax
		mov	al, [si+0Fh]
		cbw
		shl	ax, cl
		sub	es:[di+1Eh], ax
		add	si, 10h
		add	di, 20h
		sub	dx, 10h
		cmp	dx, 10h
		jb	mix16s_remainder
		jmp	mix16s_loop_16

mix16s_remainder:
		or	dx, dx
		jz	mix16s_done

mix16s_loop_single:
		mov	al, [si]
		cbw
		shl	ax, cl
		sub	es:[di], ax
		inc	si
		add	di, 2
		dec	dx
		jnz	mix16s_loop_single

mix16s_done:
		pop	ds
		pop	di
		pop	si
		pop	bp
		retf
_Mix16bitSub	endp


MIX_ASM		ends

end