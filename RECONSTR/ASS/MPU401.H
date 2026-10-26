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

#ifndef __MPU401_H__
#define __MPU401_H__

int MPU_Init(int addr);

void MPU_NoteOff(int channel, int key, int velocity);
void MPU_NoteOn(int channel, int key, int velocity);
void MPU_PolyAftertouch(int channel, int key, int pressure);
void MPU_ControlChange(int channel, int number, int value);
void MPU_ProgramChange(int channel, int program);
void MPU_ChannelAftertouch(int channel, int pressure);
void MPU_PitchBend(int channel, int lsb, int msb);


#endif