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
#include "HP_FILES.H"
#include "HP_GRAPH.H"
#include "HP_SOUND.H"
#include "HP_SYS.H"

void IntroScreens(void)
{
	FadeToBlack(40);
	SetDrawPage(0);
	SetDisplayPage(0);
#if !SHAREWARE
	DrawPCX_Chunk(PIRACY_PCX, 0);	//piracy.pcx
	FadeFromBlack(20);
#ifdef VERSION_11
	WaitMilliSecs(15000);
	FadeToBlack(20);
#else
	MenuWait();	// does Starfield animation, waits for input (no timeout)
#endif
	SetDrawPage(0);
	SetDisplayPage(0);
#endif
	DrawPCX_Chunk(APOGEE_PCX, 0);	//apogee.pcx
	PlayMusicChunk(FANFARE_MID, false);	//fanfare.mid, not looped
	FadeFromBlack(20);
	WaitMilliSecs(9000);	//should match duration of fanfare.mid
	FadeToBlack(30);
	PlayMusicChunk(MUSIC03_MID, true);
	SetDrawPage(0);
	SetDisplayPage(0);
	DrawPCX_Chunk(TITLE_PCX, 0);	//title.pcx
#if !SHAREWARE
	if (isRegisteredVersion)
	{
		DrawImageChunk(REGIST_IMG, 0, 188);
	}
#endif
	FadeFromBlack(40);
	PlayVocChunk(SOUND01_VOC);
	WaitMilliSecs(4000);
	FadeToBlack(30);
}