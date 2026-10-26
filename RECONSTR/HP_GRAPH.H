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

#ifndef __HP_GRAPH_H__
#define __HP_GRAPH_H__

#include "HP_DEFS.H"

void SetTextMode(void);
void SetDisplayPageAtVBL(Uint16 page);
void SetDisplayPage(Uint16 page);
void SetDrawPage(Uint16 page);
void WaitVBL(void);
void SetReadMap(Uint8 mask);
void SetCursorVisible(Uint8 on);
void SetPlaneMask(Uint8 mask);
void SetWriteMode(Uint16 on);
void InitGraphics(void);
void CopyScreenPage(Uint16 destpage, Uint16 sourcepage);
void CopyScreenRect(Sint16 sourcepage, Sint16 minX, Sint16 minY, Sint16 maxX, Sint16 maxY, Sint16 destpage, Sint16 x, Sint16 y);
void ClearScreen(void);
void ClearRectFast(Sint16 minX, Sint16 minY, Sint16 maxX, Sint16 maxY);
Sint16 ReadPixel(Sint16 x, Sint16 y);
void DrawPixel(Sint16 x, Sint16 y, Uint16 color);
void ClearRect(Sint16 minX, Sint16 minY, Sint16 maxX, Sint16 maxY);
void SetPalette(Uint8 *data, Sint16 firstColor, Sint16 numColors);
void GetPalette(Uint8 *data, Sint16 firstColor, Sint16 numColors);
void FadeToBlack(Sint16 steps);
void FadeFromBlack(Sint16 steps);
void FadeToWhite(void);
void SetToBlack(void);
void SetCurrentPalette(void);
void DecompressPCX_Chunk(void);
void DrawPCX_Chunk(Sint16 number, Sint16 applyPalette);
void DecompressPCX_File(void);
Uint16 DrawPCX_File(char *filename);
void SaveScreenshot(void);
void DrawScreenChunk(Uint16 number);
void DrawImageChunk(Uint16 number, Uint16 x, Uint16 y);
void LoadPaletteChunk(Uint16 number, Sint16 firstColor, Sint16 applyPalette);

#endif