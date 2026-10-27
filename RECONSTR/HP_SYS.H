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

#ifndef __HP_SYS_H__
#define __HP_SYS_H__

#include "HP_DEFS.H"

#ifndef FIX_JOY
#define FIX_JOY 0
#endif

enum {
	ERROR_OUT_OF_MEMORY=0,
	ERROR_CANNOT_OPEN=1
};

void PC_Service(void);
void PC_Startup(void);
void PC_Shutdown(void);
void SleepMilliSecs(Sint16 millis);
Sint16 WaitMilliSecs(Sint16 millis);
void ClearKeys(void);
void ClearKeyboardBuffer(void);
void KB_Startup(void);
void KB_Shutdown(void);
void interrupt KB_Service(void);
void KB_TestKeycodes(void);
Sint16 Random(Sint16 val);
void Quit(void);
void OutOfMemory(void);
void QuitMessage(char *message);
void QuitError(Sint16 errortype, char *info);
void AllocBufferB(Uint8 **buffptr, Uint32 size, char *str);
void AllocBufferW(Sint16 **buffptr, Uint32 size, char *str);
void AllocBufferD(Uint32 **buffptr, Uint32 size, char *str);
void GetFatEntry(Uint16 number, Uint32 *offset, Uint32 *size);
void GetFatOffset(Uint16 number, Uint32 *offset);
void ReadJoy(void);
void JoyCalibrateCentered(void);

#endif