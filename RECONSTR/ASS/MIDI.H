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

#ifndef __MIDI_H__
#define __MIDI_H__

typedef struct
{
	void (*NoteOff)(int, int, int);
	void (*NoteOn)(int, int, int);
	void (*PolyAftertouch)(int, int, int);
	void (*ControlChange)(int, int, int);
	void (*ProgramChange)(int, int);
	void (*ChannelAftertouch)(int, int);
	void (*PitchBend)(int, int, int);
	void (*ReleasePatches)(void);
	void (*LoadPatch)(int);
	int (*SetVolume)(int);	// result is never used
	int (*GetVolume)(void);
} midifuncs;

int MIDI_AllNotesOff(void);	// might have been public, but is never used outside MIDI.C
int MIDI_Reset(void);	// might have been public, but is never used outside MIDI.C

int MIDI_SetVolume(int volume);
int MIDI_GetVolume(void);
void MIDI_SetLoopFlag(int looping);
void MIDI_ContinueSong(void);
void MIDI_PauseSong(void);
int MIDI_SongPlaying(void);
int MIDI_SetMidiFuncs(midifuncs *funcs);
void MIDI_StopSong(void);
int MIDI_PlaySong(unsigned char far *song, int looping);

void MIDI_LoadTimbres(void);	// might have been public, but is never used outside MIDI.C

#endif