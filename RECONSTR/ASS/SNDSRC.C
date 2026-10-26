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
#include "SNDSRC.H"
#include "USER.H"
#include "TASK_MAN.H"


static int SS_Installed = false;
static int SS_Port = 0x378;
static int SS_OffCommand = 0xC;
static int SS_BufferNum = 0;
static int SS_NumBuffers = 0;
static int SS_TotalBufferSize = 0;
static unsigned SS_TransferLength = 0;
static unsigned SS_CurrentLength = 0;
static int SS_ErrorCode = 0;

static int SS_SoundPlaying;
static char far *SS_CurrentBuffer;
static char far *SS_BufferStart;
static char far *SS_BufferEnd;
static char far *SS_SoundPtr;
static void (*SS_CallBack)(void);
static task *SS_Timer;

char * SS_ErrorString(int error)
{
	char *msg;
	
	switch (error)
	{
		case -1:
		msg = SS_ErrorString(SS_ErrorCode);
		break;
		
	case 0:
		msg = "Sound Source ok.";
		break;
		
	case 1:
		msg = "Could not detect Sound Source.";
		break;
		
	case 2:
		msg = "No sound playing.";
		break;
		
	default:
		msg = "Unknown Sound Source error code.";
		break;
	}
	
	return msg;
}

#pragma argsused
static void SS_ServiceInterrupt(task *Task)
{
	int port;
	int done;
	
	port = SS_Port;
	done = false;
	
	while ((inp(port+1) & 0x40) == 0)
	{
		outp(port, *SS_SoundPtr++);
		outp(port+2, SS_OffCommand);
		outp(port+2, 4);
		
		SS_CurrentLength--;
		if (SS_CurrentLength == 0)
		{
			SS_CurrentBuffer += SS_TransferLength;
			if (++SS_BufferNum >= SS_NumBuffers)
			{
				SS_BufferNum = 0;
				SS_CurrentBuffer = SS_BufferStart;
			}
			SS_CurrentLength = SS_TransferLength;
			SS_SoundPtr = SS_CurrentBuffer;
			done = true;
		}
	}
	
	if (done == true && SS_CallBack)
	{
		SS_CallBack();
	}
}

void SS_StopPlayback(void)
{
	if (SS_SoundPlaying)
	{
		TS_Terminate(SS_Timer);
		
		outp(SS_Port, 0x80);
		outp(SS_Port+2, SS_OffCommand);
		outp(SS_Port+2, 4);
		
		SS_SoundPlaying = false;
		SS_BufferStart = NULL;
	}
}

int SS_GetCurrentPos(void)
{
	int position;
	
	if (!SS_SoundPlaying)
	{
		SS_ErrorCode = 2;
		return -2;
	}
	
	position = SS_SoundPtr - SS_CurrentBuffer;
	
	return position;
}

int SS_BeginBufferedPlayback(char far *BufferPtr, int BufferSize, int NumDivisions, void (*CallBackFunc)(void))
{
	if (SS_SoundPlaying)
	{
		SS_StopPlayback();
	}
	
	SS_SetCallBack(CallBackFunc);
	SS_BufferStart = BufferPtr;
	SS_CurrentBuffer = BufferPtr;
	SS_SoundPtr = BufferPtr;
	SS_TotalBufferSize = BufferSize;
	SS_BufferEnd = BufferPtr + BufferSize;
	SS_CurrentLength = SS_TransferLength = BufferSize / NumDivisions;
	SS_BufferNum = 0;
	SS_NumBuffers = NumDivisions;
	
	SS_SoundPlaying = true;
	SS_Timer = TS_ScheduleTask(SS_ServiceInterrupt, 500, 1, NULL);
	TS_Dispatch();
	
	return 0;
}

unsigned SS_GetPlaybackRate(void)
{
	return 7000;
}

int SS_SetMixMode(int Mode)
{
	Mode = 0;
	return Mode;
}

int SS_SetPort(int port)
{
	if (SS_Installed)
	{
		SS_Shutdown();
	}
	
	SS_Port = port;
	
	return 0;
}

void SS_SetCallBack(void (CallBackFunc)(void))
{
	// BUG: interrupts should be disabled while changing SS_CallBack
	SS_CallBack = CallBackFunc;
}

static void SS_TestTimer(task *Task)
{
	(*((int *)Task->data))++;
}

static int SS_TestSoundSource(int port)
{
	int present;
	task *timer;
	volatile int ticks;
	int i;
	
	present = false;
	
	timer = TS_ScheduleTask(SS_TestTimer, 140, 1, &ticks);
	TS_Dispatch();
	
	outp(port+2, 4);
	ticks = 0;
	do {} while (ticks < 4);
	
	TS_Terminate(timer);
	
	if ((inp(port+1) & 0x40) == 0)
	{
		for (i=32; i>0; i++)	// BUG: This should use i-- instead of i++ 
		{
			outp(port, 0x80);
			outp(port+2, SS_OffCommand);
			outp(port+2, 4);
		}
		
		if ((inp(port+1) & 0x40) != 0)
		{
			present = true;
		}
	}
	
	outp(port+2, SS_OffCommand);
	
	return present;
}

static int SS_DetectSoundSource(void)
{
	if (USER_CheckParameter("ss1"))
	{
		SS_Port = 0x3BC;
		return true;
	}
	if (USER_CheckParameter("ss2"))
	{
		SS_Port = 0x378;
		return true;
	}
	if (USER_CheckParameter("ss3"))
	{
		SS_Port = 0x278;
		return true;
	}
	
	if (SS_TestSoundSource(0x3BC))
	{
		SS_Port = 0x3BC;
		return true;
	}
	if (SS_TestSoundSource(0x378))
	{
		SS_Port = 0x378;
		return true;
	}
	if (SS_TestSoundSource(0x278))
	{
		SS_Port = 0x278;
		return true;
	}
	
	return false;
}

int SS_Init(void)
{
	int status;
	
	if (SS_Installed)
	{
		SS_Shutdown();
	}
	
	if (USER_CheckParameter("sst"))
	{
		SS_OffCommand = 0x0E;
	}
	else
	{
		SS_OffCommand = 0x0C;
	}
	
	status = SS_DetectSoundSource();
	if (!status)
	{
		SS_ErrorCode = 1;
		return -2;
	}
	
	status = 0;
	outp(SS_Port+2, 4);
	
	SS_SoundPlaying = false;
	SS_SetCallBack(NULL);
	SS_BufferStart = NULL;
	SS_Installed = true;
	SS_ErrorCode = status;
	
	return status;
}

void SS_Shutdown(void)
{
	SS_StopPlayback();
	outp(SS_Port+2, SS_OffCommand);
	SS_SoundPlaying = false;
	SS_BufferStart = NULL;
	SS_SetCallBack(NULL);
	SS_Installed = false;
}