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

#ifndef __ASS_H__
#define __ASS_H__

enum {
	CARD_NONE = -1,
	CARD_SOUNDBLASTER,
	CARD_PROAUDIO,
	CARD_ADLIB,
	CARD_GENERALMIDI,
	CARD_WAVEBLASTER,
	CARD_GRAVIS,
	CARD_SOUNDSOURCE
};

typedef struct
{
	int stuff[3];	// never used by the game code anyway
} fx_device;

int FX_PlayVOC(void *data, int b, int c, int priority);
int FX_SetupCard(int card, fx_device *device);
char *FX_ErrorString(int err);
int FX_Init(int SoundCard, int numvoices, int samplebits);
int FX_SetVolume(int vol);
int FX_Shutdown(void);
int FX_ConvertVOC(void *snd);

int MUSIC_Init(int card, int port);
char *MUSIC_ErrorString(int err);
int MUSIC_SetVolume(int vol);
int MUSIC_PlaySong(void *buffer, int loopflag);
int MUSIC_StopSong(void);
int MUSIC_Shutdown(void);

void * TS_ScheduleTask(void *func, int rate, int priority, void *dataptr);
int TS_Dispatch(void);
int TS_Terminate(void *NodeToRemove);
int TS_Shutdown(void);

#endif