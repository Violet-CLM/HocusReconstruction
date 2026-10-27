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

#ifndef __HP_SOUND_H__
#define __HP_SOUND_H__

#include "HP_DEFS.H"

enum {
	SND_PLAYERSHOT=0,
	SND_HEAL,
	SND_BONUS1,
	SND_BONUS2,
	SND_CRYSTAL,
	SND_TELEPORT,
	SND_FLIPSWITCH,
	SND_SWOOSH,
	SND_OUCH,
	SND_YAHOO,
	SND_EXPLODE,
	SND_LAUGH,
	SND_DRAGONFIRE,
	SND_ENEMYSHOT,
	SND_ENEMYSPAWN,
	SND_HITFLOOR
};

void PlaySound(Sint16 number);
void PlayVocData(void *data);
void StartupSound(void);
void PlayVocChunk(Uint16 number);
void PlayMusicChunk(Uint16 number, Uint16 looping);
void StopMusic(void);
void ShutdownSound(void);

#endif