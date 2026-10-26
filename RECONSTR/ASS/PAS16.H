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

#ifndef __PAS16_H__
#define __PAS16_H__

char * PAS_ErrorString(int error);

unsigned PAS_GetPlaybackRate(void);
int PAS_SetMixMode(int MixMode);
void PAS_StopPlayback(void);

int PAS_GetCurrentPos(void);

int PAS_BeginBufferedPlayback(char far *buffer, int length, int division, unsigned samplerate, int MixMode, void (*CallBackFunc)(void));

void PAS_SetPCMVolume(int volume);
int PAS_GetPCMVolume(void);
void PAS_SetFMVolume(int volume);
int PAS_GetFMVolume(void);
int PAS_GetCardInfo(int *MaxSampleBits, int *MaxChannels);
void PAS_SetCallBack(void (*CallBackFunc)(void));

int PAS_SaveMusicVolume(void);
void PAS_RestoreMusicVolume(void);
int PAS_Init(void);
void PAS_Shutdown(void);

#endif