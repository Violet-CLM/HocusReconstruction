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
#include "PAS16.H"
#include "DMA.H"

typedef struct
{
	unsigned char sysspkrtmr;
	unsigned char systmrctlr;
	unsigned char sysspkrreg;
	unsigned char joystick;
	unsigned char lfmaddr;
	unsigned char lfmdata;
	unsigned char rfmaddr;
	unsigned char rfmdata;
	unsigned char dfmaddr;
	unsigned char dfmdata;
	unsigned char RESRVD1;
	unsigned char paudiomixr;
	unsigned char audiomixr;
	unsigned char intrctlrst;
	unsigned char audiofilt;
	unsigned char intrctlr;
	unsigned char pcmdata;
	unsigned char RESRVD2;
	unsigned char crosschannel;
	unsigned char RESRVD3;
	unsigned short samplerate;
	unsigned short samplecnt;
	unsigned short spkrtmr;
	unsigned char tmrctlr;
	unsigned char mdirqvect;
	unsigned char mdsysctlr;
	unsigned char mdsysstat;
	unsigned char mdirqclr;
	unsigned char mdgroup1;
	unsigned char mdgroup2;
	unsigned char mdgroup3;
	unsigned char mdgroup4;
} MVState;

typedef struct
{
	void far (*SetMixer)(void);
	void far (*SetVolume)(void);
	void far (*SetFilter)(void);
	void far (*SetCrossChannel)(void);
	void far (*GetMixer)(void);
	void far (*GetVolume)(void);
	void far (*GetFilter)(void);
	void far (*GetCrossChannel)(void);
	void far (*ReadSound)(void);
	void far (*FMSplit)(void);
} MVFunc;


// initialized variables:
static int PAS_Interrupts[] = {0,0,10,11,0,13,0,15,0,0,114,115,116,0,0,119};
static int PAS_Installed = false;
static int PAS_TranslateCode = 0;
static unsigned PAS_OriginalPCMLeftVolume = 75;
static unsigned PAS_OriginalPCMRightVolume = 75;
static unsigned PAS_OriginalFMLeftVolume = 75;
static unsigned PAS_OriginalFMRightVolume = 75;
static unsigned PAS_DMAChannel = 1;
static unsigned PAS_Irq = 7;
static MVState far *PAS_State = NULL;
static MVFunc far *PAS_Func = NULL;
static unsigned PAS_TransferLength = 0;
static unsigned PAS_MixMode = 0;
static unsigned PAS_SampleRate = 11000;
static int PAS_TimeInterval = 0;
static int PAS_VolumeLookup[] = {0,28,36,42,49,56,62,68,74,80,85,89,92,95,98,100};
static int PAS_ErrorCode = 0;


// un-initialized variables:
static char far *PAS_DMABuffer;
static void (*PAS_CallBack)(void);
static void interrupt (*PAS_OldInt)(void);
static unsigned PAS_TotalDMABufferSize;
static unsigned PAS_IntController1Mask;
static unsigned PAS_IntController2Mask;
static char far *PAS_CurrentDMABuffer;
static char far *PAS_DMABufferEnd;
static int PAS_SoundPlaying;

// prototypes:
static void PAS_Write(int Register, int Data);
static int PAS_Read(int Register);


char * PAS_ErrorString(int error)
{
	char *msg;
	
	switch (error)
	{
	case -1:
	case -2:
		msg = PAS_ErrorString(PAS_ErrorCode);
		break;
		
	case 0:
		msg = "Pro AudioSpectrum ok.";
		break;
		
	case 1:
		msg = "MVSOUND.SYS not loaded.";
		break;
		
	case 2:
		msg = DMA_ErrorString(-1);
		break;
		
	case 3:
		msg = "Invalid Pro AudioSpectrum Irq.";
		break;
		
	case 5:
		msg = "No sound playing.";
		break;
		
	case 6:
		msg = "Could not find Pro AudioSpectrum.";
		break;
		
	default:
		msg = "Unknown Pro AudioSpectrum error code.";
		//break;
	}
	
	return msg;
}

static int PAS_CheckForDriver(void)
{
	union REGS regs;
	int result;
	
	regs.x.ax = 0xBC00;
	regs.x.bx = 0x3F3F;
	int86(0x2F, &regs, &regs);
	
	if (regs.x.ax != 0xBC00)
	{
		PAS_ErrorCode = 1;
		return -1;
	}
	result = regs.x.bx ^ regs.x.cx ^ regs.x.dx;
	if (result != 0x4D56)
	{
		PAS_ErrorCode = 1;
		return -1;
	}
	
	return 0;
}

static MVState far *PAS_GetStateTable(void)
{
	union REGS regs;
	MVState far *result;
	
	regs.x.ax = 0xBC02;
	int86(0x2F, &regs, &regs);
	if (regs.x.ax != 0x4D56)
	{
		PAS_ErrorCode = 1;
		return NULL;
	}
	
	result = MK_FP(regs.x.dx, regs.x.bx);
	return result;
}

static MVFunc far *PAS_GetFunctionTable(void)
{
	union REGS regs;
	MVFunc far *result;
	
	regs.x.ax = 0xBC03;
	int86(0x2F, &regs, &regs);
	if (regs.x.ax != 0x4D56)
	{
		PAS_ErrorCode = 1;
		return NULL;
	}
	
	result = MK_FP(regs.x.dx, regs.x.bx);
	return result;
}

static int PAS_GetCardSettings(void)
{
	union REGS regs;
	int status;
	
	regs.x.ax = 0xBC04;
	int86(0x2F, &regs, &regs);
	if (regs.x.ax != 0x4D56)
	{
		PAS_ErrorCode = 1;
		return -1;
	}
	
	PAS_DMAChannel = regs.x.bx;
	PAS_Irq = regs.x.cx;
	if (PAS_Irq > 15)
	{
		PAS_ErrorCode = 3;
		return -1;
	}
	if (PAS_Interrupts[PAS_Irq] == 0)
	{
		PAS_ErrorCode = 3;
		return -1;
	}
	
	status = DMA_VerifyChannel(PAS_DMAChannel);
	if (status == -1)
	{
		PAS_ErrorCode = 2;
		return -1;
	}
	
	return 0;
}

static void PAS_EnableInterrupt(void)
{
	int mask;
	int data;
	
	asm	pushf;
	asm	cli;
	
	if (PAS_Irq < 8)
	{
		mask = inp(0x21) & ~(1 << PAS_Irq);
		outp(0x21, mask);
	}
	else
	{
		mask = inp(0xA1) & ~(1 << (PAS_Irq-8));
		outp(0xA1, mask);
		
		mask = inp(0x21) & ~(1 << 2);
		outp(0x21, mask);
	}
	
	PAS_Write(0xB89, PAS_Read(0xB89) & 0x40);
	data = PAS_State->intrctlr;
	data |= 8;
	PAS_Write(0xB8B, data);
	PAS_State->intrctlr = data;
	
	asm	popf;
}

static void PAS_DisableInterrupt(void)
{
	int mask;
	int data;
	
	asm	pushf;
	asm	cli;
	
	data = PAS_State->intrctlr;
	data &= ~( 8 | 4 );
	PAS_Write(0xB8B, data);
	PAS_State->intrctlr = data;
	
	if (PAS_Irq < 8)
	{
		mask = inp(0x21) & ~(1 << PAS_Irq);
		mask |= PAS_IntController1Mask & (1 << PAS_Irq);
		outp(0x21, mask);
	}
	else
	{
		mask = inp(0x21) & ~(1 << 2);
		mask |= PAS_IntController1Mask & (1 << 2);
		outp(0x21, mask);
		
		mask = inp(0xA1) & ~(1 << (PAS_Irq-8));
		mask |= PAS_IntController2Mask & (1 << (PAS_Irq-8));
		outp(0xA1, mask);
	}
	
	asm	popf;
}

static void interrupt PAS_ServiceInterrupt(void)
{
	int irqstatus;
	
	irqstatus = PAS_Read(0xB89);
	if (!(irqstatus & 8))
	{
		PAS_OldInt();
		return;
	}
	
	irqstatus &= ~8;
	PAS_Write(0xB89, irqstatus);
	
	if (PAS_Irq > 7)
	{
		outp(0xA0, 0x20);
	}
	outp(0x20, 0x20);
	
	PAS_CurrentDMABuffer += PAS_TransferLength;
	if (PAS_CurrentDMABuffer >= PAS_DMABufferEnd)
	{
		PAS_CurrentDMABuffer = PAS_DMABuffer;
	}
	
	if (PAS_CallBack != NULL)
	{
		PAS_CallBack();
	}
}

static void PAS_Write(int Register, int Data)
{
	int port;

	port = Register ^ PAS_TranslateCode;
	outp(port, Data);
}

static int PAS_Read(int Register)
{
	int port;
	int data;

	port = Register ^ PAS_TranslateCode;
	data = inp(port);
	
	return data;
}

static void PAS_SetSampleRateTimer(void)
{
	int LoByte;
	int HiByte;
	int data;
	
	asm	pushf;
	asm	cli;
	
	data = PAS_State->audiofilt;
	data &= ~0x40;
	PAS_Write(0xB8A, data);
	PAS_State->audiofilt = data;
	
	data = 0x36;
	PAS_Write(0x138B, data);
	PAS_State->tmrctlr = data;
	
	LoByte = lobyte(PAS_TimeInterval);
	HiByte = hibyte(PAS_TimeInterval);
	PAS_Write(0x1388, LoByte);
	PAS_Write(0x1388, HiByte);
	PAS_State->samplerate = PAS_TimeInterval;
	
	asm	popf;
}

static void PAS_SetSampleBufferCount(void)
{
	int LoByte;
	int HiByte;
	int length;
	int data;
	
	asm	pushf;
	asm	cli;
	
	data = PAS_State->audiofilt;
	data &= ~0x80;
	PAS_Write(0xB8A, data);
	PAS_State->audiofilt = data;
	
	data = 0x74;
	PAS_Write(0x138B, data);
	PAS_State->tmrctlr = data;
	
	length = PAS_TransferLength;
	if (PAS_DMAChannel > 3)
	{
		length >>= 1;
	}
	LoByte = lobyte(length);
	HiByte = hibyte(length);
	PAS_Write(0x1389, LoByte);
	PAS_Write(0x1389, HiByte);
	PAS_State->samplecnt = length;
	
	asm	popf;
}

static void PAS_SetPlaybackRate(unsigned rate)
{
	long frequency;
	
	if (rate < 4000)
	{
		rate = 4000;
	}
	if (rate > 44000)
	{
		rate = 44000;
	}
	
	PAS_TimeInterval = 0x1234DClu / rate;
	if (PAS_MixMode & 1)
	{
		PAS_TimeInterval /= 2;
	}
	
	// convert it back to frequency in Hz, so that PAS_GetPlaybackRate will be
	// able to return the actual playback rate (a rate of 44000 Hz will play at
	// 44191 Hz in 8-bit mode and 45891 Hz in 16-bit mode, for example)
	frequency = 0x1234DClu / PAS_TimeInterval;
	if (PAS_MixMode & 1)
	{
		frequency /= 2;
	}
	PAS_SampleRate = frequency;
}

unsigned PAS_GetPlaybackRate(void)
{
	return PAS_SampleRate;
}

int PAS_SetMixMode(int MixMode)
{
	MixMode &= 3;
	if ((PAS_State->intrctlr & 0xE0) == 0)
	{
		MixMode &= ~2;
	}
	
	PAS_MixMode = MixMode;
	PAS_SetPlaybackRate(PAS_SampleRate);
	
	return MixMode;
}

void PAS_StopPlayback(void)
{
	int data;
	
	PAS_DisableInterrupt();
	data = PAS_State->crosschannel;
	data &= 0x3F;
	PAS_Write(0xF8A, data);
	PAS_State->crosschannel = data;
	PAS_SoundPlaying = false;
	PAS_DMABuffer = NULL;
}

static int PAS_SetupDMABuffer(char far *buffer, unsigned length)
{
	int status;
	int data;
	
	data = PAS_State->crosschannel;
	data |= 0x80;
	PAS_Write(0xF8A, data);
	PAS_State->crosschannel = data;
	
	status = DMA_SetupTransfer(PAS_DMAChannel, buffer, length);
	if (status == -1)
	{
		PAS_ErrorCode = 2;
		return -1;
	}
	
	PAS_DMABuffer = buffer;
	PAS_CurrentDMABuffer = buffer;
	PAS_TotalDMABufferSize = length;
	PAS_DMABufferEnd = buffer + length;
	
	return 0;
}

int PAS_GetCurrentPos(void)
{
	int pos;
	char far * ptr;
	
	if (!PAS_SoundPlaying)
	{
		PAS_ErrorCode = 5;
		return -1;
	}
	
	ptr = DMA_GetCurrentPos(PAS_DMAChannel);
	if (!ptr)
	{
		PAS_ErrorCode = 2;
		return -1;
	}
	
	pos = (char huge *)ptr - (char huge *)PAS_CurrentDMABuffer;
	if (PAS_MixMode & 2)
	{
		pos >>= 1;
	}
	if (PAS_MixMode & 1)
	{
		pos >>= 1;
	}
	
	return pos;
}

static int PAS_GetFilterSetting(int rate)
{
	if ((unsigned long)rate > 35794ul)
	{
		return 1;
	}
	if ((unsigned long)rate > 31818ul)
	{
		return 2;
	}
	if ((unsigned long)rate > 23862ul)
	{
		return 9;
	}
	if ((unsigned long)rate > 17896ul)
	{
		return 0x11;
	}
	if ((unsigned long)rate > 11930ul)
	{
		return 0x19;
	}
	return 4;
}

static void PAS_BeginTransfer(void)
{
	int data;
	
	PAS_SetSampleRateTimer();
	PAS_SetSampleBufferCount();
	PAS_EnableInterrupt();
	
	if (PAS_State->intrctlr & 0xE0)
	{
		data = PAS_Read(0x8389);
		data &= 0xF3;
		if (PAS_MixMode & 2)
		{
			data |= 4;
		}
		PAS_Write(0x8389, data);
	}
	
	data = PAS_State->crosschannel;
	data &= 0xF;
	data |= 0xD0;
	data |= 3;
	if (!(PAS_MixMode & 1))
	{
		data |= 0x20;
	}
	PAS_Write(0xF8A, data);
	PAS_State->crosschannel = data;
	
	data = PAS_GetFilterSetting(PAS_SampleRate);
	data |= 0xC0;
	data |= 0x20;
	PAS_Write(0xB8A, data);
	PAS_State->audiofilt = data;
}

int PAS_BeginBufferedPlayback(char far *buffer, int length, int division, unsigned samplerate, int MixMode, void (*CallBackFunc)(void))
{
	int status;
	
	PAS_StopPlayback();
	PAS_SetMixMode(MixMode);
	PAS_SetPlaybackRate(samplerate);
	PAS_TransferLength = length / division;
	PAS_SetCallBack(CallBackFunc);
	
	status = PAS_SetupDMABuffer(buffer, length);
	if (status == -1)
	{
		return -1;
	}
	
	PAS_BeginTransfer();
	return 0;
}

static int PAS_CallMVFunction(void far (*func)(void), int arg1, int arg2, int arg3)
{
	_BX = arg1;
	_CX = arg2;
	_DX = arg3;
	func();
	return _BX;
}

void PAS_SetPCMVolume(int volume)
{
	volume = max(0, volume);
	volume = min(volume, 255);
	
	volume = PAS_VolumeLookup[volume >> 4];
	
	PAS_CallMVFunction(PAS_Func->SetMixer, volume, 0, 6);
	PAS_CallMVFunction(PAS_Func->SetMixer, volume, 0, 13);
}

int PAS_GetPCMVolume(void)
{
	int left;
	int right;
	int volume;
	
	if (!PAS_Func)
	{
		return 255;
	}
	
	left = PAS_CallMVFunction(PAS_Func->GetMixer, 0, 0, 6);
	right = PAS_CallMVFunction(PAS_Func->GetMixer, 0, 0, 13);
	
	volume = (right + left) / 2;
	volume *= 255;
	volume /= 100;
	return volume;
}

void PAS_SetFMVolume(int volume)
{
	volume = max(0, volume);
	volume = min(volume, 255);
	
	volume = PAS_VolumeLookup[volume >> 4];
	
	if (PAS_Func)
	{
		PAS_CallMVFunction(PAS_Func->SetMixer, volume, 0, 1);
		PAS_CallMVFunction(PAS_Func->SetMixer, volume, 0, 8);
	}
}

int PAS_GetFMVolume(void)
{
	int left;
	int right;
	int volume;
	
	if (!PAS_Func)
	{
		return 255;
	}
	
	left = PAS_CallMVFunction(PAS_Func->GetMixer, 0, 0, 1);
	right = PAS_CallMVFunction(PAS_Func->GetMixer, 0, 0, 8);
	
	volume = (right + left) / 2;
	volume *= 255;
	volume /= 100;
	volume = min(255, volume);	// in case the mixer retuned a value > 100, I guess
	return volume;
}

int PAS_GetCardInfo(int *MaxSampleBits, int *MaxChannels)
{
	int status;
	
	if (!PAS_State)
	{
		status = PAS_CheckForDriver();
		if (status != 0)
		{
			return status;
		}
		PAS_State = PAS_GetStateTable();
		if (!PAS_State)
		{
			return -1;
		}
	}
	
	*MaxChannels = 2;
	if ((PAS_State->intrctlr & 0xE0) == 0)
	{
		*MaxSampleBits = 8;
	}
	else
	{
		*MaxSampleBits = 16;
	}
	
	return 0;
}

void PAS_SetCallBack(void (*CallBackFunc)(void))
{
	PAS_CallBack = CallBackFunc;
}

static int PAS_TestAddress(int address)
{
	asm {
		mov	ax, address;
		mov	dx, 0B8Bh;
		xor	dx, ax;
		in 	al, dx;
		cmp	al, 0FFh;
		je 	done;
		mov	ah, al;
		xor	al, 0E0h;
		out	dx, al;
		jmp	l1;
	}
l1:
	asm	jmp	l2;
l2:
	asm {
		in 	al, dx;
		xchg	al, ah;
		out	dx, al;
		sub	al, ah;
	}
done:
	return _AX & 0xFF;
}

static int PAS_FindCard(void)
{
	int status;
	
	status = PAS_TestAddress(0);
	if (status == 0)
	{
		PAS_TranslateCode = 0;
		return 0;
	}
	
	status = PAS_TestAddress(0xC);
	if (status == 0)
	{
		PAS_TranslateCode = 0xC;
		return 0;
	}
	
	status = PAS_TestAddress(4);
	if (status == 0)
	{
		PAS_TranslateCode = 4;
		return 0;
	}
	
	status = PAS_TestAddress(0x100);
	if (status == 0)
	{
		PAS_TranslateCode = 0x100;
		return 0;
	}
	
	PAS_ErrorCode = 6;
	return -1;
}

int PAS_SaveMusicVolume(void)
{
	int status;
	int data;
	
	if (!PAS_Installed)
	{
		status = PAS_CheckForDriver();
		if (status != 0)
		{
			return status;
		}
		PAS_State = PAS_GetStateTable();
		if (!PAS_State)
		{
			return -1;
		}
		PAS_Func = PAS_GetFunctionTable();
		if (!PAS_Func)
		{
			return -1;
		}
		status = PAS_GetCardSettings();
		if (status != 0)
		{
			return status;
		}
		status = PAS_FindCard();
		if (status != 0)
		{
			return status;
		}
		
		data = PAS_State->audiofilt;
		data |= 0x20;
		PAS_Write(0xB8A, data);
		PAS_State->audiofilt = data;
	}
	
	PAS_OriginalFMLeftVolume = PAS_CallMVFunction(PAS_Func->GetMixer, 0, 0, 1);
	PAS_OriginalFMRightVolume = PAS_CallMVFunction(PAS_Func->GetMixer, 0, 0, 8);
	
	return 0;
}

void PAS_RestoreMusicVolume(void)
{
	if (!PAS_Func)
	{
		return;
	}
	
	PAS_CallMVFunction(PAS_Func->SetMixer, PAS_OriginalFMLeftVolume, 0, 1);
	PAS_CallMVFunction(PAS_Func->SetMixer, PAS_OriginalFMRightVolume, 0, 8);
}

int PAS_Init(void)
{
	int status;
	int Interrupt;
	int data;
	
	if (PAS_Installed)
	{
		PAS_Shutdown();
	}
	
	PAS_IntController1Mask = inp(0x21);
	PAS_IntController2Mask = inp(0xA1);
	
	status = PAS_CheckForDriver();
	if (status != 0)
	{
		return status;
	}
	PAS_State = PAS_GetStateTable();
	if (!PAS_State)
	{
		return -1;
	}
	PAS_Func = PAS_GetFunctionTable();
	if (!PAS_Func)
	{
		return -1;
	}
	status = PAS_GetCardSettings();
	if (status != 0)
	{
		return status;
	}
	status = PAS_FindCard();
	if (status != 0)
	{
		return status;
	}
	
	PAS_OriginalPCMLeftVolume = PAS_CallMVFunction(PAS_Func->GetMixer, 0, 0, 6);
	PAS_OriginalPCMRightVolume = PAS_CallMVFunction(PAS_Func->GetMixer, 0, 0, 13);
	
	PAS_SoundPlaying = false;
	PAS_SetCallBack(NULL);
	PAS_DMABuffer = NULL;
	
	data = PAS_State->audiofilt;
	data |= 0x20;
	PAS_Write(0xB8A, data);
	PAS_State->audiofilt = data;
	
	PAS_SetPlaybackRate(11000);
	PAS_SetMixMode(0);
	
	Interrupt = PAS_Interrupts[PAS_Irq];
	PAS_OldInt = getvect(Interrupt);
	setvect(Interrupt, PAS_ServiceInterrupt);
	
	PAS_Installed = true;
	PAS_ErrorCode = 0;
	return 0;
}

void PAS_Shutdown(void)
{
	int Interrupt;
	
	if (!PAS_Installed)
	{
		return;
	}
	
	PAS_StopPlayback();
	Interrupt = PAS_Interrupts[PAS_Irq];
	setvect(Interrupt, PAS_OldInt);
	
	PAS_SoundPlaying = false;
	PAS_DMABuffer = NULL;
	PAS_SetCallBack(NULL);
	PAS_CallMVFunction(PAS_Func->SetMixer, PAS_OriginalPCMLeftVolume, 0, 6);
	PAS_CallMVFunction(PAS_Func->SetMixer, PAS_OriginalPCMRightVolume, 0, 13);
	
	PAS_Installed = false;
}
