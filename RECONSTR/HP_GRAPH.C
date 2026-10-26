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

#include <DOS.H>
#include <MEM.H>
#include <STDIO.H>

#include "HP_DEFS.H"
#include "HP_MAIN.H"
#include "HP_GRAPH.H"
#include "HP_FILES.H"
#include "HP_SYS.H"
#include "HP_COPY.H"
#include "HP_VGA.H"

//////////
// Types:
//////////

typedef struct {	//based on ZSoft PCX File Format Technical Reference Manual
	Uint8 Manufacturer;
	Uint8 Version;
	Uint8 Encoding;
	Uint8 BitsPerPixel;
	Uint16 Xmin;
	Uint16 Ymin;
	Uint16 Xmax;
	Uint16 Ymax;
	Uint16 HDpi, VDpi;
	Uint8 Colormap[48];
	Uint8 Reserved;
	Uint8 NPlanes;
	Uint16 BytesPerLine;
	Uint16 PaletteInfo;
	Uint16 HscreenSize, VscreenSize;
	Uint8 Filler[54];
} TPcxHeader;

#if sizeof(TPcxHeader) != 128
#error PCX header size incorrect
#endif

////////////////////////////
// Uninitialized Variables:
////////////////////////////

#ifdef VERSION_11
FILE *pcx_fp;
#endif
TPcxHeader pcxHeader;
Uint16 pcxWidth;
Uint16 pcxHeight;
Uint16 pcxBytesPerLine;

//////////////
// Functions:
//////////////

void SetTextMode(void)
{
	register Sint16 i;
	union REGS regs;
	regs.h.ah = 0;
	regs.h.al = 3;
	int86(0x10, &regs, &regs);
	SetCursorVisible(0);
	for (i=0; i<55; i++)
		WaitVBL();
	SetCursorVisible(1);
}

void SetDisplayPageAtVBL(Uint16 page)
{
	register Uint8 val;
	Uint16 off;
	
	off = screenpageOff[page];
	
	// The display page is updated by writing a new CRTC start address. The start
	// address gets "latched" at the beginning of the vertical retrace, which
	// means the start address needs to be written before the vertical retrace
	// to be in effect during the next refresh.
	
	// There are some problems with this code. For one, an interrupt could fire
	// after the first part of the start address was updated and the CRT could be
	// in the vertical retrace after the interrupt has been taken care of. That
	// would mean only the first half of the new starting address value will be
	// in effect during the next refresh.
	//
	// The other problem is that the CRT might have been just about to enter the
	// vertical retrace, meaning there is still a chance that the start address
	// for the next refresh gets "latched" before we are done updating it here.
	// That means it's not always safe to draw to the old page immediately after
	// this function returns.
	//
	// The code should really wait until the display is active ((inp(0x3DA) & 9)
	// == 0). There should always be a sufficiently long delay between the end of
	// the last "display active" phase and the start of the vertical retrace,
	// giving you enough time to update the CRTC start address. You should still
	// disable interrupts as soon as you see the "display active" signal.
	
	// wait until NOT in a vertical retrace:
	while (inp(0x3DA) & 8);
	
	// set CRTC start address:
	outp(0x3D4, 0xC);
	val = (off >> 8);
	outp(0x3D5, val);
	outp(0x3D4, 0xD);
	val = off;
	outp(0x3D5, val);
	
	// wait until in a vertical retrace:
	while (!(inp(0x3DA) & 8));
	
	// This waiting for a vertical retrace means the display is done displaying
	// the "old" screen page and it *should* be safe to draw to the old page
	// immediately after this function returns. But that's not always the case
	// with this particular implemenatation.
	
	currDisplayPage = page;
}

void SetDisplayPage(Uint16 page)
{
	register Uint8 val;
	Uint16 off;
	
	off = screenpageOff[page];
	
	// This version doesn't bother to do any synchronization and just writes the
	// new start address. Since low byte of the address is always 0 anyway in
	// Hocus Pocus, this won't cause the CRT to display a completely wrong area
	// if the vertical retrace starts while this code is updating the CRTC start
	// address. But it willl become a problem if you want to optimize the VGA
	// memory usage.
	
	// set CRTC start address:
	outp(0x3D4, 0xC);
	val = (off >> 8);
	outp(0x3D5, val);
	outp(0x3D4, 0xD);
	val = off;
	outp(0x3D5, val);
	
	currDisplayPage = page;
}

void SetDrawPage(Uint16 page)
{
	drawPtr = screenpagePtr[page];
	currDrawPage = page;
}

void WaitVBL(void)
{
	while ((inp(0x3DA) & 8));
	while (!(inp(0x3DA) & 8));
}

void SetReadMap(Uint8 mask)
{
	outp(0x3CE, 4);
	outp(0x3CF, mask);
}

void SetCursorVisible(Uint8 on)
{
	outp(0x3C4, 1);
	if (on != 0)
		outp(0x3C5, inp(0x3C5) & ~0x20);
	else
		outp(0x3C5, inp(0x3C5) | 0x20);
}

void SetPlaneMask(Uint8 mask)
{
	outp(0x3C4, 2);
	outp(0x3C5, mask);
}

void SetWriteMode(Uint16 on)
{
	// Write mode 1 allows fast VGA-to-VGA data copying across all 4 planes
	// (used in combination with SetPlaneMask(0xF) to write to all 4 planes).
	// Write mode 0 is for "normal" Memory-to-VGA transfer.
	
	// The *real* EGA/VGA "write mode" would normally be set like this:
	//		outp(0x3CE, 5);
	// 	outp(0x3CF, on);
	// But a part of the planar VGA "mode X" startup code involved a write to
	// this register and writing to it again will cause display problems.
	//
	// Thankfully, the behavior of EGA/VGA write mode 1 can be emulated by
	// setting the bitmask register to 0. This means the EGA/VGA ignores all bits
	// in the byte value sent by the CPU and instead writes the bits of the last
	// value that was read from video memory. Each of the 4 "planes" has its own
	// 1-byte buffer (called a "latch") that remembers the last value read from
	// that plane. Write mode 1 is just an alternative way to make the card
	// ignore the entire CPU byte and write the "latched" bytes instead.
	
	outp(0x3CE, 8);
	outp(0x3CF, ((!on) * 0xFF));
}

void InitGraphics(void)
{
	register Sint16 i;
	
	StartPlanarVGA();
	for (i=0; i<4; i++)
	{
		//Note: using 16000 instead of 16128 would leave no unused padding between pages
		screenpageOff[i] = i*16128;
		screenpagePtr[i] = screenptrVGA + i*16128;
		setmem(screenpagePtr[i], 16000, 0);	//not required -- StartPlanarVGA already cleared the entire VGA memory
	}
/*
To get the maximum out of the VGA memory, use 16000 instead of 16128 as the
page size in the loop above, then do this:

	screenpagePtr[3] = screenptrVGA + 49536;
	screenpageOff[3] = 49536;

Note: The second part (adjusting screenpageOff[3]) is not required unless you
want to actually show page 3 (the original game only shows pages 0 and 1).

This basically leaves 17536 bytes for page 2, which is used to store tile and
sprite graphics in VGA memory. This gives you an additional 1408 bytes to use
for sprite or tile graphics (1408 bytes are exactly 22 tiles).

You could set aside even more than that by storing the image data for the menu
cursor, the volume sliders and the HUD numbers and key icons as linear pixel
data at the beginning of page 2, and change the offset for drawing the backdrop
from 2000 to 3200. This is a fair amount of work as you will need to change
many functions, and considering the fact that it will only give you 374 bytes
(which is less than 6 tiles) it is probably not worth the effort.

The reason why the code uses a page size of 16128 bytes here is that this is a
multiple of 256, meaning the low byte of all screen page offsets will be 0 and
an incomplete CRTC start address update (see SetDisplayPageAtVBL) will not have
any bad side-effects. That means you will need to fix the problems with the
SetDisplayPage functions if you want to have more VGA memory available for tiles
and sprite images.
*/
	SetDrawPage(0);
	SetDisplayPage(0);
}

void CopyScreenPage(Uint16 destpage, Uint16 sourcepage)
{
	SetWriteMode(1);
	SetPlaneMask(0xF);
	CopyBytes(screenpagePtr[destpage], screenpagePtr[sourcepage], 16000);
}

void CopyScreenRect(Sint16 sourcepage, Sint16 minX, Sint16 minY, Sint16 maxX, Sint16 maxY, Sint16 destpage, Sint16 x, Sint16 y)
{
	//Note: x coordinates must be 0-79
	register Sint16 iy;
	Sint16 width = (maxX-minX)+1;
	
	SetWriteMode(1);
	SetPlaneMask(0xF);
	for (iy=minY; iy<=maxY; iy++)
	{
		CopyBytes(screenpagePtr[destpage]+y*80+x, screenpagePtr[sourcepage]+iy*80+minX, width);
	}
}

void ClearScreen(void)
{
	SetWriteMode(0);
	SetPlaneMask(0xF);
	setmem(drawPtr, 16000, 0);
}

void ClearRectFast(Sint16 minX, Sint16 minY, Sint16 maxX, Sint16 maxY)	//never used!
{
	//Note: x coordinates must be 0-79
	SetWriteMode(0);
	SetPlaneMask(0xF);
	for (; minY <= maxY; minY++)
	{
		setmem(drawPtr+minY*80+minX, maxX-minX+1, 0);
	}
}

Sint16 ReadPixel(Sint16 x, Sint16 y)
{
	SetReadMap(x % 4);	//x & 3 would be faster
	return (drawPtr+y*80)[x/4];
}

void DrawPixel(Sint16 x, Sint16 y, Uint16 color)
{
	SetPlaneMask(1 << (x % 4));	//x & 3 would be faster
	(drawPtr+y*80)[x >> 2] = color;
}

void ClearRect(Sint16 minX, Sint16 minY, Sint16 maxX, Sint16 maxY)
{
	//Note: x coordinates must be 0-319
	register Sint16 y, x;
	
	for (y=minY; y<=maxY; y++)
	{
		for (x=minX; x<=maxX; x++)
		{
			DrawPixel(x, y, 0);
		}
	}
}

void SetPalette(Uint8 *data, Sint16 firstColor, Sint16 numColors)
{
	outp(0x3C8, firstColor);
	while (numColors-- > 0)
	{
		outp(0x3C9, *(data++));
		outp(0x3C9, *(data++));
		outp(0x3C9, *(data++));
	}
}

void GetPalette(Uint8 *data, Sint16 firstColor, Sint16 numColors)
{
	outp(0x3C7, firstColor);
	while (numColors-- > 0)
	{
		*(data++) = inp (0x3C9);
		*(data++) = inp (0x3C9);
		*(data++) = inp (0x3C9);
	}
}

void FadeToBlack(Sint16 steps)
{
	register Sint16 i, step;
	
	for (i=0; i<0x300; i++)	//useless loop
	{
		workPalette[i] = currPalette[i];	//useless assignment
	}
	
	for (step=steps; step >= 0; step--)
	{
		for (i=0; i<0x300; i++)
		{
			workPalette[i] = ((currPalette[i]*step)/steps);
		}
		WaitVBL();
		SetPalette(workPalette, 0, 0x100);
	}
}

void FadeFromBlack(Sint16 steps)
{
	register Sint16 i, step;
	
	for (step=0; step <= steps; step++)
	{
		for (i=0; i<0x300; i++)
		{
			workPalette[i] = ((currPalette[i]*step)/steps);
		}
		WaitVBL();
		SetPalette(workPalette, 0, 0x100);
	}
}

void FadeToWhite(void)
{
	register Sint16 i, step;
	
	for (step=0; step<70; step++)
	{
		for (i=0; i<0x300; i++)
		{
			if (currPalette[i] < 0x3F)
			{
				currPalette[i] = currPalette[i]+1;
			}
		}
		WaitVBL();
		SetPalette(currPalette, 0, 0x100);
	}
}

void SetToBlack(void)
{
	register Sint16 i;
	
	for (i=0; i<0x300; i++)	//Note: setmem(workPalette, 0x300, 0) would be faster than this loop
	{
		workPalette[i] = 0;
	}
	SetPalette(workPalette, 0, 0x100);
}

void SetCurrentPalette(void)
{
	SetPalette(currPalette, 0, 0x100);
}

/*
Note: You could merge the two DecompressPCX functions into one by passing the
respective file pointer (dat_fp or pcx_fp) as an argument to DecompressPCX.
*/
void DecompressPCX_Chunk(void)
{
	register Sint16 x, plane;
	Uint8 pixels[4*80];
	Uint16 pix, y, count;
	
	for (y=0; y<pcxHeight; y++)
	{
		//un-RLE one scanline into 4 planes in buffer:
		x=plane=0;
		do {
			pix = ReadByteFromDat() & 0xFF;
			if ((pix & 0xC0) == 0xC0)
			{
				count = pix & 0x3F;
				pix = ReadByteFromDat();
				while (count--)
				{
					pixels[plane*80+(x >> 2)] = pix;
					x++;
					plane++;
					if (plane > 3)
					{
						plane = 0;
					}
				}
			}
			else
			{
				pixels[plane*80+(x >> 2)] = pix;
				x++;
				plane++;
				if (plane > 3)
				{
					plane = 0;
				}
			}
		} while (x < pcxBytesPerLine);
		
		//write planes to screen memory:
		SetWriteMode(0);
		outp(0x3C4, 2);
		count = y*80;
		outp(0x3C5, 1);
		CopyBytes(drawPtr+count, pixels+80*0, 80);
		outp(0x3C5, 2);
		CopyBytes(drawPtr+count, pixels+80*1, 80);
		outp(0x3C5, 4);
		CopyBytes(drawPtr+count, pixels+80*2, 80);
		outp(0x3C5, 8);
		CopyBytes(drawPtr+count, pixels+80*3, 80);
	}
}

void DrawPCX_Chunk(Sint16 number, Sint16 applyPalette)
{
	register Sint16 i;
	Uint32 offset, size;
	
	OpenDatFile();
	GetFatEntry(number, &offset, &size);
	ReadFromDat(offset, 128, &pcxHeader);
	ReadFromDat(offset+size+-0x300, 0x300, currPalette);
	for (i=0; i<0x300; i++)
	{
		currPalette[i] >>= 2;
	}
	if (applyPalette)
	{
		SetPalette(currPalette, 0, 0x100);
	}
	SeekDat(offset+128);
	pcxWidth = (pcxHeader.Xmax-pcxHeader.Xmin)+1;	//useless, variable is never used
	pcxHeight = (pcxHeader.Ymax-pcxHeader.Ymin)+1;
	pcxBytesPerLine = pcxHeader.BytesPerLine;
	DecompressPCX_Chunk();
	CloseDatFile();
}

#ifdef VERSION_11

void DecompressPCX_File(void)
{
	register Sint16 x, plane;
	Uint8 pixels[4*80];
	Uint16 pix, y, count;
	
	for (y=0; y<pcxHeight; y++)
	{
		//un-RLE one scanline into 4 planes in buffer:
		x=plane=0;
		do {
			pix = fgetc(pcx_fp) & 0xFF;
			if ((pix & 0xC0) == 0xC0)
			{
				count = pix & 0x3F;
				pix = fgetc(pcx_fp);
				while (count--)
				{
					pixels[plane*80+(x >> 2)] = pix;
					x++;
					plane++;
					if (plane > 3)
					{
						plane = 0;
					}
				}
			}
			else
			{
				pixels[plane*80+(x >> 2)] = pix;
				x++;
				plane++;
				if (plane > 3)
				{
					plane = 0;
				}
			}
		} while (x < pcxBytesPerLine);
		
		//write planes to screen memory:
		SetWriteMode(0);
		outp(0x3C4, 2);
		count = y*80;
		outp(0x3C5, 1);
		CopyBytes(drawPtr+count, pixels+80*0, 80);
		outp(0x3C5, 2);
		CopyBytes(drawPtr+count, pixels+80*1, 80);
		outp(0x3C5, 4);
		CopyBytes(drawPtr+count, pixels+80*2, 80);
		outp(0x3C5, 8);
		CopyBytes(drawPtr+count, pixels+80*3, 80);
	}
}

Uint16 DrawPCX_File(char *filename)
{
	register Sint16 i;
	
	pcx_fp = fopen(filename, "rb");
	if (!pcx_fp)
	{
		return 0;
	}
	
	fread(&pcxHeader, 128, 1, pcx_fp);
	fseek(pcx_fp, -0x300, SEEK_END);
	fread(currPalette, 0x300, 1, pcx_fp);
	for (i=0; i<0x300; i++)
	{
		currPalette[i] >>= 2;
	}
	fseek(pcx_fp, 128, SEEK_SET);
	pcxWidth = (pcxHeader.Xmax-pcxHeader.Xmin)+1;	//useless, variable is never used
	pcxHeight = (pcxHeader.Ymax-pcxHeader.Ymin)+1;
	pcxBytesPerLine = pcxHeader.BytesPerLine;
	DecompressPCX_File();
	fclose(pcx_fp);
	return 1;
}

#endif	// ifdef VERSION_11

void SaveScreenshot(void)	//never used!
{
	register Sint16 i;
	Sint16 plane;
	FILE *fp;

	fp = fopen("screen1.bin", "w+b");
	fwrite(currPalette, 0x300, 1, fp);
	for (plane=0; plane<4; plane++)
	{
		SetReadMap(plane);
		for (i=0; i<16000; i++)
		{
			fputc(drawPtr[i], fp);
		}
	}
	fclose(fp);
}

void DrawScreenChunk(Uint16 number)	//never used!
{
	register Sint16 i;
	Sint16 plane;
	Uint32 offset;
	
	OpenDatFile();
	GetFatOffset(number, &offset);
	ReadFromDat(offset, 0x300, currPalette);
	SeekDat(offset+0x300);	//probably useless (file position should already be at offset+0x300)
	for (plane=0; plane<4; plane++)
	{
		SetPlaneMask(1 << plane);
		for (i=0; i<16000; i++)
		{
			drawPtr[i] = fgetc(dat_fp);
		}
	}
	CloseDatFile();
}

void DrawImageChunk(Uint16 number, Uint16 x, Uint16 y)
{
	register Sint16 ix, iy;
	Sint16 plane;
	Uint32 offset;
	
	OpenDatFile();
	GetFatOffset(number, &offset);
	ReadFromDat(offset, sizeof(imageInfo), &imageInfo);
	SetWriteMode(0);
	for (plane=0; plane<4; plane++)
	{
		SetPlaneMask(1 << plane);
		for (iy=0; iy<imageInfo.height; iy++)
		{
			for (ix=0; ix<imageInfo.width; ix++)
			{
				*(drawPtr+(y+iy)*80+x+ix) = ReadByteFromDat();
			}
		}
	}
	CloseDatFile();
}

void LoadPaletteChunk(Uint16 number, Sint16 firstColor, Sint16 applyPalette)
{
	OpenDatFile();
	firstColor *= 3;
	ReadDatChunk(number, &currPalette[firstColor]);
	if (applyPalette)
	{
		SetPalette(currPalette, 0, 0x100);
	}
	CloseDatFile();
}