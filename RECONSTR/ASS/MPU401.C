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

#include <CONIO.H>
#include <DOS.H>
#include <STDLIB.H>
#include "COMMON.H"
#include "MPU401.H"
#include "USER.H"

static int MPU_BaseAddr = 0x330;
static int MPU_Delay = 1500;

static void MPU_SendCommand(int data)
{
	int port = MPU_BaseAddr + 1;
	unsigned count;
	
	for (count = 0xFFFF; count > 0; count--)
	{
		if (!(inp(port) & 0x40))
			break;
	}
	
	outp(port, data);
}

static void MPU_SendMidi(int data)
{
	int port = MPU_BaseAddr + 1;
	unsigned count;
	
	for (count = MPU_Delay; count != 0; count--)
	{
		if (!(inp(port) & 0x40))
			break;
	}
	
	port--;
	outp(port, data);
}

static int MPU_Reset(void)
{
	int port = MPU_BaseAddr + 1;
	unsigned count;
	
	MPU_SendCommand(0xFF);
	
	for (count = 0xFFFF; count > 0; count--)
	{
		if (!(inp(port) & 0x80))
		{
			port--;
			if (inp(port) == 0xFE)
			{
				return 0;
			}
			port++;
		}
	}
	
	return -1;
}

static int MPU_EnterUART(void)
{
	int port = MPU_BaseAddr + 1;
	unsigned count;
	
	MPU_SendCommand(0x3F);
	
	for (count = 0xFFFF; count > 0; count--)
	{
		if (!(inp(port) & 0x80))
		{
			port--;
			if (inp(port) == 0xFE)
			{
				return 0;
			}
			port++;
		}
	}
	
	return -2;
}

int MPU_Init(int addr)
{
	int status;
	int count;
	char *ptr;
	
	ptr = USER_GetText("MPUDELAY");
	if (ptr != NULL)
	{
		MPU_Delay = atol(ptr);
	}
	
	MPU_BaseAddr = addr;
	
	for (count = 4; count > 0; count--)
	{
		status = MPU_Reset();
		if (status == 0)
		{
			return MPU_EnterUART();
		}
	}
	
	return status;
}

void MPU_NoteOff(int channel, int key, int velocity)
{
	MPU_SendMidi(0x80 | channel);
	MPU_SendMidi(key);
	MPU_SendMidi(velocity);
}

void MPU_NoteOn(int channel, int key, int velocity)
{
	MPU_SendMidi(0x90 | channel);
	MPU_SendMidi(key);
	MPU_SendMidi(velocity);
}

void MPU_PolyAftertouch(int channel, int key, int pressure)
{
	MPU_SendMidi(0xA0 | channel);
	MPU_SendMidi(key);
	MPU_SendMidi(pressure);
}

void MPU_ControlChange(int channel, int number, int value)
{
	MPU_SendMidi(0xB0 | channel);
	MPU_SendMidi(number);
	MPU_SendMidi(value);
}

void MPU_ProgramChange(int channel, int program)
{
	MPU_SendMidi(0xC0 | channel);
	MPU_SendMidi(program);
}

void MPU_ChannelAftertouch(int channel, int pressure)
{
	MPU_SendMidi(0xD0 | channel);
	MPU_SendMidi(pressure);
}

void MPU_PitchBend(int channel, int lsb, int msb)
{
	MPU_SendMidi(0xE0 | channel);
	MPU_SendMidi(lsb);
	MPU_SendMidi(msb);
}
