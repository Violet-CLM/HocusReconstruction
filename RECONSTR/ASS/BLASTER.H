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

#ifndef __BLASTER_H__
#define __BLASTER_H__

typedef struct
{
	int Address;
	int Type;
	int Interrupt;
	int Dma8;
	int Dma16;
	int Midi;
} BLASTER_CONFIG;

char *BLASTER_ErrorString(int error);

void BLASTER_SetPlaybackRate(unsigned rate);
unsigned BLASTER_GetPlaybackRate(void);
int BLASTER_SetMixMode(int mode);
void BLASTER_StopPlayback(void);
int BLASTER_GetCurrentPos(void);
int BLASTER_BeginBufferedPlayback(char far *BufferStart, int BufferSize, int NumDivisions, unsigned SampleRate, int MixMode, void (*CallBackFunc)(void));
int BLASTER_GetVoiceVolume(void);
int BLASTER_SetVoiceVolume(int volume);
int BLASTER_GetMidiVolume(void);
int BLASTER_SetMidiVolume(int volume);
int BLASTER_CardHasMixer(void);
int BLASTER_GetEnv(BLASTER_CONFIG *Config);
int BLASTER_SetCardSettings(BLASTER_CONFIG *Config);
int BLASTER_GetCardInfo(int *MaxSampleBits, int *MaxChannels);
void BLASTER_SetCallBack(void (*CallBackFunc)(void));
void BLASTER_SaveVoiceVolume(void);
void BLASTER_RestoreVoiceVolume(void);
void BLASTER_SaveMidiVolume(void);
void BLASTER_RestoreMidiVolume(void);
int BLASTER_Init(void);
void BLASTER_Shutdown(void);

#endif