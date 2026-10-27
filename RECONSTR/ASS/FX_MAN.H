/* Reconstructed Hocus Pocus Source Code
 * Copyright (C) 2026 K1n9_Duk3
 *
 * The code in this file is based on the Apogee Sound System by James R. Dose.
 * Copyright (C) 1994-1995 Apogee Software, Ltd.
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

#ifndef __FX_MAN_H__
#define __FX_MAN_H__

typedef struct
{
	int MaxVoices;
	int MaxSampleBits;
	int MaxChannels;
} fx_device;

char * FX_ErrorString(int error);
int FX_SetupCard(int SoundCard, fx_device *device);
int FX_Init(int SoundCard, int numvoices, int samplebits);
int FX_Shutdown(void);
void FX_SetVolume(int volume);
int FX_GetVolume(void);
int FX_ConvertVOC(char far *data);
int FX_PlayVOC(char far *data, int foo, int bar, int priority);
int FX_SoundActive(int handle);
int FX_StopSound(int handle);
int FX_StopAllSounds(void);
int FX_VoicesPlaying(void);

#endif