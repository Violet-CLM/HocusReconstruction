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

char * MUSIC_ErrorString(int error);
int MUSIC_Init(int SoundCard, int MidiPort);
int MUSIC_Shutdown(void);
int MUSIC_SetVolume(int volume);
int MUSIC_GetVolume(void);
void MUSIC_SetLoopFlag(int loop);
int MUSIC_SongPlaying(void);
void MUSIC_Continue(void);
void MUSIC_Pause(void);
int MUSIC_StopSong(void);
int MUSIC_PlaySong(char far *MusicBuffer, int looping);


#endif