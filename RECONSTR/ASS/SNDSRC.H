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

#ifndef __SNDSRC_H__
#define __SNDSRC_H__

char * SS_ErrorString(int error);

void SS_StopPlayback(void);
int SS_GetCurrentPos(void);
int SS_BeginBufferedPlayback(char far *BufferPtr, int BufferSize, int NumDivisions, void (*CallBackFunc)(void));
unsigned SS_GetPlaybackRate(void);
int SS_SetMixMode(int Mode);
int SS_SetPort(int port);
void SS_SetCallBack(void (CallBackFunc)(void));

int SS_Init(void);
void SS_Shutdown(void);

#endif