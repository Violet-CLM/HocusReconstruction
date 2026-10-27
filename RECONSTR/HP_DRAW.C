/* Reconstructed Hocus Pocus Source Code
 * Copyright (C) 2026 K1n9_Duk3
 *
 * Original Code by Mike Voss, Copyright (c) 1994 Moonlite Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "HP_DEFS.H"
#include "HP_MAIN.H"
#include "HP_GRAPH.H"
#include "HP_GAME.H"

/*
=============================================================================

	EXTERN DECLARATIONS (see note in HP_GAME.H)

=============================================================================
*/

extern Sint16 camX;
extern Sint16 camY;
extern Sint16 oddCamX;
extern Sint16 camX_t8;


/*
=============================================================================

	FUNCTIONS

=============================================================================
*/

void DrawBackdrop_Even(Uint8 *drawsrc, Uint8 *drawdest)
{
	Sint16 tx, ty, y1, tileindex, srcoff, destoff, x1;
	
	SetWriteMode(1);
	SetPlaneMask(0xF);
	tileindex = camY*240 + camX;
	x1 = camX_t8 % 80;
	destoff = 0;
	srcoff = x1;
	for (ty=0; ty<10; ty++)
	{
		for (tx=0; tx<20; tx++)
		{
			if (mapBackLayer[tileindex] == 255 && mapMainLayer[tileindex] == 255)
			{
				if (x1 <= 76)	//can we just copy a 16x16 pixel rect from source to dest?
				{
					asm {
						push	ds;
						lds	si, drawsrc;
						add	si, srcoff;
						les	di, drawdest;
						add	di, destoff;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						pop	ds;
					}
					destoff += 4;
					srcoff += 4;
					x1 += 4;
					if (x1 >= 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
				}
				else	//backdrop wraps around here, so draw each column individually
				{
					//column 1:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 2:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 3:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 4:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
				}
			}
			else	//no backdrop visible here, just adjust the offsets
			{
				destoff += 4;
				srcoff += 4;
				x1 += 4;
				if (x1 >= 80)
				{
					x1 -= 80;
					srcoff -= 80;
				}
			}
			tileindex++;
		}
		destoff += 1200;
		srcoff += 1280;
		tileindex += 220;
	}
}

void DrawBackdrop_Odd(Uint8 *drawsrc, Uint8 *drawdest)
{
	Sint16 tx, ty, y1, tileindex, destoff, srcoff, x1;
	
	SetWriteMode(1);
	SetPlaneMask(0xF);
	tileindex = camY*240 + camX;
	x1 = camX_t8 % 80;
	destoff = 0;
	srcoff = x1;
	
	for (ty=0; ty<10; ty++)
	{
		//
		// leftmost half-tile:
		//
		if (mapBackLayer[tileindex] == 255 && mapMainLayer[tileindex] == 255)
		{
			if (x1 <= 78)
			{
				asm {
					push	ds;
					lds	si, drawsrc;
					add	si, srcoff;
					les	di, drawdest;
					add	di, destoff;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					pop	ds;
				}
				destoff += 2;
				srcoff += 2;
				x1 += 2;
				if (x1 >= 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
			}
			else
			{
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff += 80;
					srcoff += 80;
				}
				destoff -= 1279;
				srcoff -= 1279;
				x1++;
				if (x1 == 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff += 80;
					srcoff += 80;
				}
				destoff -= 1279;
				srcoff -= 1279;
				x1++;
				if (x1 == 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
			}
		}
		else
		{
			destoff += 2;
			srcoff += 2;
			x1++;
			if (x1 == 80)
			{
				x1 = 0;
				srcoff -= 80;
			}
			x1++;
			if (x1 == 80)
			{
				x1 = 0;
				srcoff -= 80;
			}
		}
		tileindex++;
		
		//
		// center area:
		//
		for (tx=0; tx<19; tx++)
		{
			if (mapBackLayer[tileindex] == 255 && mapMainLayer[tileindex] == 255)
			{
				if (x1 <= 76)
				{
					asm {
						push	ds;
						lds	si, drawsrc;
						add	si, srcoff;
						les	di, drawdest;
						add	di, destoff;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						add	si, 76;
						add	di, 76;
						movsb;
						movsb;
						movsb;
						movsb;
						pop	ds;
					}
					destoff += 4;
					srcoff += 4;
					x1 += 4;
					if (x1 >= 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
				}
				else
				{
					//column 1:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 2:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 3:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 4:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
				}
			}
			else
			{
				destoff += 4;
				srcoff += 4;
				x1 += 4;
				if (x1 >= 80)
				{
					x1 -= 80;
					srcoff -= 80;
				}
			}
			tileindex++;
		}
		
		//
		// rightmost half-tile:
		//
		if (mapBackLayer[tileindex] == 255 && mapMainLayer[tileindex] == 255)
		{
			if (x1 <= 78)
			{
				asm {
					push	ds;
					lds	si, drawsrc;
					add	si, srcoff;
					les	di, drawdest;
					add	di, destoff;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					add	si, 78;
					add	di, 78;
					movsb;
					movsb;
					pop	ds;
				}
				destoff += 2;
				srcoff += 2;
				x1 += 2;
				if (x1 >= 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
			}
			else
			{
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff += 80;
					srcoff += 80;
				}
				destoff -= 1279;
				srcoff -= 1279;
				x1++;
				if (x1 == 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff += 80;
					srcoff += 80;
				}
				destoff -= 1279;
				srcoff -= 1279;
				x1++;
				if (x1 == 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
			}
		}
		else
		{
			destoff += 2;
			srcoff += 2;
			x1 += 2;
			// Note: Since this function only gets executed when the camera is at
			// an "odd" half-tile position, it means x1 ALWAYS starts at an odd
			// value (1, 3, ..., 79) in this function. And since this code only
			// increases x1 by even numbers (or increases it an even number of
			// times in increments of 1), this means x1 will ALWAYS be odd here.
			// That means it cannot be 80 here, it can only be 79 or 81, which
			// also means that the following check works correctly even though it
			// should have used the condition (x1 >= 80) to be 100% correct.
			if (x1 > 80)
			{
				x1 -= 80;
				srcoff -= 80;
			}
		}
		destoff += 1200;
		srcoff += 1280;
		tileindex += 220;
	}
}

void DrawBackdrop_Even_Slow(void)	//never used!
{
	Sint16 tx, ty, y1, tileindex, srcoff, destoff, x1;
	
	SetWriteMode(1);
	SetPlaneMask(0xF);
	tileindex = camY*240 + camX;
	x1 = camX_t8 % 80;
	destoff = 0;
	srcoff = x1;
	for (ty=0; ty<10; ty++)
	{
		for (tx=0; tx<20; tx++)
		{
			if (mapBackLayer[tileindex] == 255 && mapMainLayer[tileindex] == 255)
			{
				if (x1 <= 76)
				{
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff++;
						srcoff++;
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff++;
						srcoff++;
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff++;
						srcoff++;
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff++;
						srcoff++;
						
						destoff += 76;
						srcoff += 76;
					}
					destoff -= 1276;
					srcoff -= 1276;
					x1 += 4;
					if (x1 >= 80)
					{
						x1 -= 80;
						srcoff -= 80;
					}
				}
				else
				{
					//column 1:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 2:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 3:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 4:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
				}
			}
			else
			{
				destoff += 4;
				srcoff += 4;
				x1 += 4;
				if (x1 >= 80)
				{
					x1 -= 80;
					srcoff -= 80;
				}
			}
			
			tileindex++;
		}
		
		destoff += 1200;
		srcoff += 1280;
		tileindex += 220;
	}
}

void DrawBackdrop_Odd_Slow(void)	//never used!
{
	Sint16 tx, ty, y1, tileindex, srcoff, destoff, x1;
	
	SetWriteMode(1);
	SetPlaneMask(0xF);
	tileindex = camY*240 + camX;
	x1 = camX_t8 % 80;
	destoff = 0;
	srcoff = x1;
	
	for (ty=0; ty<10; ty++)
	{
		//
		// leftmost half-tile:
		//
		if (mapBackLayer[tileindex] == 255 && mapMainLayer[tileindex] == 255)
		{
			if (x1 <= 78)
			{
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff++;
					srcoff++;
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff++;
					srcoff++;
					
					destoff += 78;
					srcoff += 78;
				}
				destoff -= 1278;
				srcoff -= 1278;
				x1 += 2;
				if (x1 >= 80)
				{
					x1 -= 80;
					srcoff -= 80;
				}
			}
			else
			{
				//column 1:
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff += 80;
					srcoff += 80;
				}
				destoff -= 1279;
				srcoff -= 1279;
				x1++;
				if (x1 == 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
				//column 2:
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff += 80;
					srcoff += 80;
				}
				destoff -= 1279;
				srcoff -= 1279;
				x1++;
				if (x1 == 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
			}
		}
		else
		{
			destoff += 2;
			srcoff += 2;
			x1++;
			if (x1 == 80)
			{
				x1 = 0;
				srcoff -= 80;
			}
			x1++;
			if (x1 == 80)
			{
				x1 = 0;
				srcoff -= 80;
			}
		}
		tileindex++;
		
		//
		// center area:
		//
		for (tx=0; tx<19; tx++)
		{
			if (mapBackLayer[tileindex] == 255 && mapMainLayer[tileindex] == 255)
			{
				if (x1 <= 76)
				{
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff++;
						srcoff++;
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff++;
						srcoff++;
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff++;
						srcoff++;
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff++;
						srcoff++;
						
						destoff += 76;
						srcoff += 76;
					}
					destoff -= 1276;
					srcoff -= 1276;
					x1 += 4;
					if (x1 >= 80)
					{
						x1 -= 80;
						srcoff -= 80;
					}
				}
				else
				{
					//column 1:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 2:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 3:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
					//column 4:
					for (y1=0; y1<16; y1++)
					{
						drawPtr[destoff] = screenpagePtr[3][srcoff];
						destoff += 80;
						srcoff += 80;
					}
					destoff -= 1279;
					srcoff -= 1279;
					x1++;
					if (x1 == 80)
					{
						x1 = 0;
						srcoff -= 80;
					}
				}
			}
			else
			{
				destoff += 4;
				srcoff += 4;
				x1 += 4;
				if (x1 >= 80)
				{
					x1 -= 80;
					srcoff -= 80;
				}
			}
			
			tileindex++;
		}
		
		//
		// rightmost half-tile:
		//
		if (mapBackLayer[tileindex] == 255 && mapMainLayer[tileindex] == 255)
		{
			if (x1 <= 78)
			{
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff++;
					srcoff++;
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff++;
					srcoff++;
					
					destoff += 78;
					srcoff += 78;
				}
				destoff -= 1278;
				srcoff -= 1278;
				x1 += 2;
				if (x1 >= 80)
				{
					x1 -= 80;
					srcoff -= 80;
				}
			}
			else
			{
				//column 1:
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff += 80;
					srcoff += 80;
				}
				destoff -= 1279;
				srcoff -= 1279;
				x1++;
				if (x1 == 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
				//column 2:
				for (y1=0; y1<16; y1++)
				{
					drawPtr[destoff] = screenpagePtr[3][srcoff];
					destoff += 80;
					srcoff += 80;
				}
				destoff -= 1279;
				srcoff -= 1279;
				x1++;
				if (x1 == 80)
				{
					x1 = 0;
					srcoff -= 80;
				}
			}
		}
		else
		{
			destoff += 2;
			srcoff += 2;
			x1 +=2 ;
			if (x1 > 80)	// should be 'x1 >= 80' but x1 shouldn't be 80 here anyway
			{
				x1 -= 80;
				srcoff -= 80;
			}
		}
		
		destoff += 1200;
		srcoff += 1280;
		tileindex += 220;
	}
}

/*
Note:

The DrawLevel_... functions are optimized for speed, which is why they are in
pure assembly. However, you could make them a little faster and reduce the code
size a bit if you do the following:

Change

	lds	si, (mainlayer/backlayer);
	add	si, bx;
	mov	ax, 0;
	mov	al, [si]
	cmp	al, 255;
	jz		(label1);
	mov	cl, 6;
	shl	ax, cl;
	jmp	(draw);
(label1):
	[...]
(draw):
	lds	si, drawsrc;
	[...]

into

	lds	si, (mainlayer/backlayer);
	add	si, bx;
	mov	al, [si]
	cmp	al, 255;
	jnz	(draw);
(label1):
	[...]
(draw):
	mov	ah, 0;
	mov	cl, 6;
	shl	ax, cl;
	lds	si, drawsrc;
	[...]

This avoids clearing ah as long as we are not sure we actually need to and
also eliminates some unconditional jumps.
*/
void DrawLevel_Even(Uint8 *drawdest, Uint8 *drawsrc, Uint8 *mainlayer, Uint8 *backlayer, Uint16 tileindex)
{
	asm {
		cld;
		push	ds;
		mov	bx, tileindex;
		les	di, drawdest;
		mov	dh, 10;
	}
loop_y:
	asm {
		mov	dl, 20;
	}
loop_x:
	asm {
		lds	si, mainlayer;
		add	si, bx;
		mov	ax, 0;
		mov	al, [si];
		cmp	al, 255;
		jz		check_back;
		mov	cl, 6;
		shl	ax, cl;
		jmp	do_draw;
	}
check_back:
	asm {
		lds	si, backlayer;
		add	si, bx;
		mov	ax, 0;
		mov	al, [si];
		cmp	al, 255;
		jz		no_draw;
		mov	cl, 6;
		shl	ax, cl;
		jmp	do_draw;
	}
no_draw:
	asm {
		add	di, 4;
		jmp	check_x;
	}
do_draw:
	asm {
		lds	si, drawsrc;
		add	si, ax;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		sub	di, 1200;
	}
check_x:
	asm {
		inc	bx;
		dec	dl;
		jz		check_y;
		jmp	loop_x;
	}
check_y:
	asm {
		add	di, 1200;
		add	bx, 220;
		dec	dh;
		jz		done;
		jmp	loop_y;
	}
done:
	asm {
		pop	ds;
	}
}

void DrawLevel_Odd(Uint8 *drawdest, Uint8 *drawsrc, Uint8 *mainlayer, Uint8 *backlayer, Uint16 tileindex)
{
	asm {
		cld;
		push	ds;
		mov	bx, tileindex;
		les	di, drawdest;
		mov	dh, 10;
	}
loop_y:
	asm {
		lds	si, mainlayer;
		add	si, bx;
		mov	ax, 0;
		mov	al, [si];
		cmp	al, 255;
		jz		left_check_back;
		mov	cl, 6;
		shl	ax, cl;
		jmp	left_do_draw;
	}
left_check_back:
	asm {
		lds	si, backlayer;
		add	si, bx;
		mov	ax, 0;
		mov	al, [si];
		cmp	al, 255;
		jz		left_no_draw;
		mov	cl, 6;
		shl	ax, cl;
		jmp	left_do_draw;
	}
left_no_draw:
	asm {
		add	di, 2;
		jmp	mid;
	}
left_do_draw:
	asm {
		lds	si, drawsrc;
		add	si, ax;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		sub	di, 1200;
	}
mid:
	asm {
		inc	bx;
		mov	dl, 19;
	}
loop_x:
	asm {
		lds	si, mainlayer;
		add	si, bx;
		mov	ax, 0;
		mov	al, [si];
		cmp	al, 255;
		jz		mid_check_back;
		mov	cl, 6;
		shl	ax, cl;
		jmp	mid_do_draw;
	}
mid_check_back:
	asm {
		lds	si, backlayer;
		add	si, bx;
		mov	ax, 0;
		mov	al, [si];
		cmp	al, 255;
		jz		mid_no_draw;
		mov	cl, 6;
		shl	ax, cl;
		jmp	mid_do_draw;
	}
mid_no_draw:
	asm {
		add	di, 4;
		jmp	check_x;
	}
mid_do_draw:
	asm {
		lds	si, drawsrc;
		add	si, ax;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		add	di, 76;
		movsb;
		movsb;
		movsb;
		movsb;
		sub	di, 1200;
	}
check_x:
	asm {
		inc	bx;
		dec	dl;
		jz		right;
		jmp	loop_x;
	}
right:
	asm {
		lds	si, mainlayer;
		add	si, bx;
		mov	ax, 0;
		mov	al, [si];
		cmp	al, 255;
		jz		right_check_back;
		mov	cl, 6;
		shl	ax, cl;
		jmp	right_do_draw;
	}
right_check_back:
	asm {
		lds	si, backlayer;
		add	si, bx;
		mov	ax, 0;
		mov	al, [si];
		cmp	al, 255;
		jz		right_no_draw;
		mov	cl, 6;
		shl	ax, cl;
		jmp	right_do_draw;
	}
right_no_draw:
	asm {
		add	di, 2;
		jmp	check_y;
	}
right_do_draw:
	asm {
		lds	si, drawsrc;
		add	si, ax;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		add	di, 78;
		add	si, 2;
		movsb;
		movsb;
		sub	di, 1200;
	}
check_y:
	asm {
		add	di, 1200;
		add	bx, 220;
		dec	dh;
		jz		done;
		jmp	loop_y;
	}
done:
	asm {
		pop	ds;
	}
}
