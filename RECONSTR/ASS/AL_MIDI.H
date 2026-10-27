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

#ifndef __AL_MIDI_H__
#define __AL_MIDI_H__

void AL_Reset(void);
int AL_ReserveVoice(int voice);
int AL_ReleaseVoice(int voice);
void AL_Shutdown(void);
int AL_Init(int soundcard);

void AL_NoteOff(int channel, int key, int velocity);
void AL_NoteOn(int channel, int key, int velocity);
void AL_AllNotesOff(int channel);
void AL_ControlChange(int channel, int type, int data);
void AL_ProgramChange(int channel, int patch);
void AL_SetPitchBend(int channel, int lsb, int msb);

int AL_DetectFM(void);

#endif