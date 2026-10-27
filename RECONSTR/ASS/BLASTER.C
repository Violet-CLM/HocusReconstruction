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
#include <CTYPE.H>
#include <DOS.H>
#include <STDIO.H>
#include <STDLIB.H>
#include "COMMON.H"
#include "BLASTER.H"
#include "DMA.H"
#include "USER.H"

typedef struct
{
	int IsSupported;
	int HasMixer;
	int MaxMixMode;
	unsigned MinSamplingRate;
	unsigned MaxSamplingRate;
} CARD_CAPABILITY;


// initialized variables:
static int BLASTER_Interrupts[] = {0,0,10,11,0,13,0,15,0,0,114,115,116,0,0,119};
static int BLASTER_SampleSize[] = {1, 2, 2, 4};

static CARD_CAPABILITY BLASTER_CardConfig[] =
{
	{0, 0, 0, 0, 0},
	{1, 0, 0, 4000, 23000},
	{1, 1, 1, 4000, 44100},
	{1, 0, 0, 4000, 23000},
	{1, 1, 1, 4000, 44100},
	{0, 0, 0, 0, 0},
	{1, 1, 3, 5000, 44100}
};

static BLASTER_CONFIG BLASTER_Config = {-1,-1,-1,-1,-1,-1};

static int BLASTER_Installed = false;
static unsigned BLASTER_TransferLength = 0;
static int BLASTER_MixMode = 0;
static int BLASTER_SamplePacketSize = 2;
static unsigned BLASTER_SampleRate = 11000;
static int BLASTER_HaltTransferCommand = 0xD0;
static int BLASTER_MixerAddress = -1;
static int BLASTER_MixerType = 0;
static int BLASTER_OriginalMidiVolumeLeft = 255;
static int BLASTER_OriginalMidiVolumeRight = 255;
static int BLASTER_OriginalVoiceVolumeLeft = 255;
static int BLASTER_OriginalVoiceVolumeRight = 255;
static int BLASTER_ErrorCode = 0;

// uninitialized variables:
static char far *BLASTER_DMABuffer;
static int BLASTER_Version;
static void (*BLASTER_CallBack)(void);
static void interrupt (*BLASTER_OldInt)(void);
static unsigned BLASTER_TotalDMABufferSize;
static int BLASTER_IntController1Mask;
static int BLASTER_IntController2Mask;
static char far *BLASTER_CurrentDMABuffer;
static char far *BLASTER_DMABufferEnd;
static int BLASTER_SoundPlaying;

// prototypes:
static int BLASTER_DSP1xx_BeginPlayback(int length);

char *BLASTER_ErrorString(int error)
{
	char *msg;
	
	switch (error)
	{
	case -1:
		msg = BLASTER_ErrorString(BLASTER_ErrorCode);
		break;
		
	case 0:
		msg = "Sound Blaster ok.";
		break;
		
	case 1:
		msg = "BLASTER evironment variable not set.";
		break;
		
	case 2:
		msg = "Missing Sound Blaster address in BLASTER evironment variable.";
		break;
		
	case 3:
		msg = "Missing Sound Blaster interrupt in BLASTER evironment variable.";
		break;
		
	case 4:
		msg = "Missing Sound Blaster DMA channel in BLASTER evironment variable.";
		break;
		
	case 5:
		msg = "Sound Blaster card type not set.";
		break;
		
	case 6:
		msg = "Invalid parameter in BLASTER evironment variable.";
		break;
		
	case 7:
		msg = "Sound Blaster card not ready.";
		break;
		
	case 8:
		msg = "No sound playing.";
		break;
		
	case 9:
		msg = "Invalid Sound Blaster Irq.";
		break;
		
	case 10:
		msg = DMA_ErrorString(-1);
		break;
		
	case 11:
		msg = "Mixer not available on selected Sound Blaster card.";
		break;
		
	default:
		msg = "Unknown Sound Blaster error code.";
		break;
	}
	
	return msg;
}

static void BLASTER_EnableInterrupt(void)
{
	int Irq;
	int mask;
	
	asm	pushf;
	asm	cli;
	
	Irq = BLASTER_Config.Interrupt;
	if (Irq < 8)
	{
		mask = inp(0x21) & ~(1 << Irq);
		outp(0x21, mask);
	}
	else
	{
		mask = inp(0xA1) & ~(1 << (Irq-8));
		outp(0xA1, mask);
		
		mask = inp(0x21) & ~(1 << 2);
		outp(0x21, mask);
	}
	
	asm	popf;
}

static void BLASTER_DisableInterrupt(void)
{
	int Irq;
	int mask;
	
	asm	pushf;
	asm	cli;
	
	Irq = BLASTER_Config.Interrupt;
	if (Irq < 8)
	{
		mask =  inp(0x21) & ~(1 << Irq);
		mask |= BLASTER_IntController1Mask & (1 << Irq);
		outp(0x21, mask);
	}
	else
	{
		mask =  inp(0x21) & ~(1 << 2);
		mask |= BLASTER_IntController1Mask & (1 << 2);
		outp(0x21, mask);
		
		mask =  inp(0xA1) & ~(1 << (Irq-8));
		mask |= BLASTER_IntController2Mask & (1 << (Irq-8));
		outp(0xA1, mask);
	}

	asm	popf;
}

static void interrupt BLASTER_ServiceInterrupt(void)
{
	int status;
	
	if (BLASTER_Version >= 0x400)
	{
		outp(BLASTER_Config.Address+4, 0x82);
		status = inp(BLASTER_Config.Address+5);
		if (status & 2)
		{
			inp(BLASTER_Config.Address+15);
		}
		else if (status & 1)
		{
			inp(BLASTER_Config.Address+14);
		}
		else
		{
			BLASTER_OldInt();
			return;
		}
	}
	else
	{
		inp(BLASTER_Config.Address+14);
	}
	
	BLASTER_CurrentDMABuffer += BLASTER_TransferLength;
	if (BLASTER_CurrentDMABuffer >= BLASTER_DMABufferEnd)
	{
		BLASTER_CurrentDMABuffer = BLASTER_DMABuffer;
	}
	
	if (BLASTER_Version < 0x200)
	{
		BLASTER_DSP1xx_BeginPlayback(BLASTER_TransferLength);
	}
	
	if (BLASTER_CallBack)
	{
		BLASTER_CallBack();
	}
	
	if (BLASTER_Config.Interrupt > 7)
	{
		outp(0xA0, 0x20);
	}
	outp(0x20, 0x20);
}

static int BLASTER_WriteDSP(int data)
{
	int port;
	unsigned count;
	int status;
	
	port = BLASTER_Config.Address+12;
	status = 7;
	
	count = 0xFFFF;
	do
	{
		if ((inp(port) & 0x80) == 0)
		{
			outp(port, data);
			status = 0;
			break;
		}
		count--;
	} while (count > 0);
	
	BLASTER_ErrorCode = status;
	return status;
}

static int BLASTER_ReadDSP(void)
{
	int port;
	unsigned count;
	int status;
	
	port = BLASTER_Config.Address + 14;
	status = -1;
	
	count = 0xFFFF;
	do
	{
		if ((inp(port) & 0x80) != 0)
		{
			status = inp(BLASTER_Config.Address + 10);
			break;
		}
		count--;
	} while (count > 0);
	
	if (status == -1)
	{
		BLASTER_ErrorCode = 7;
	}
	return status;
}

static int BLASTER_ResetDSP(void)
{
	int port;
	int count;
	int status;
	
	port = BLASTER_Config.Address + 6;
	status = 7;
	
	outp(port, 1);
	
	count = 0x100;
	do
	{
		count--;
	} while (count > 0);
	
	outp(port, 0);
	
	count = 100;
	do
	{
		if (BLASTER_ReadDSP() == 0xAA)
		{
			status = 0;
			break;
		}
		count--;
	} while (count > 0);
	
	BLASTER_ErrorCode = status;
	return status;
}

static int BLASTER_GetDSPVersion(void)
{
	int MajorVersion;
	int MinorVersion;
	int version;
	
	BLASTER_WriteDSP(0xE1);
	MajorVersion = BLASTER_ReadDSP();
	MinorVersion = BLASTER_ReadDSP();
	
	if (MajorVersion == -1 || MinorVersion == -1)
	{
		return -1;
	}
	
	version = (MajorVersion << 8) + MinorVersion;
	return version;
}

static void BLASTER_SpeakerOn(void)
{
	BLASTER_WriteDSP(0xD1);
}

static void BLASTER_SpeakerOff(void)
{
	BLASTER_WriteDSP(0xD3);
}

void BLASTER_SetPlaybackRate(unsigned rate)
{
	int LoByte;
	int HiByte;
	CARD_CAPABILITY *card;
	
	
	card = BLASTER_CardConfig + BLASTER_Config.Type;
	if (BLASTER_Version < 0x400)
	{
		int timeconstant;
		long ActualRate;
		
		// Note: The calculations below won't work correctly for large 'rate'
		// values. You would need to cast 'rate' to long to get more reliable
		// results.
		
		ActualRate = rate * BLASTER_SamplePacketSize;
		if (card->MinSamplingRate > ActualRate)
		{
			rate = card->MinSamplingRate / BLASTER_SamplePacketSize;
		}
		if (card->MaxSamplingRate < ActualRate)
		{
			rate = card->MaxSamplingRate / BLASTER_SamplePacketSize;
		}
		
		timeconstant = (0x10000 - (256000000 / (BLASTER_SamplePacketSize * rate))) >> 8;
		BLASTER_SampleRate = 256000000 / (0x10000 - (timeconstant << 8));
		
		BLASTER_SampleRate /= BLASTER_SamplePacketSize;
		
		BLASTER_WriteDSP(0x40);
		BLASTER_WriteDSP(timeconstant);
	}
	else
	{
		BLASTER_SampleRate = rate;
		if (BLASTER_SampleRate < card->MinSamplingRate)
			BLASTER_SampleRate = card->MinSamplingRate;
		if (BLASTER_SampleRate > card->MaxSamplingRate)
			BLASTER_SampleRate = card->MaxSamplingRate;
		
		HiByte = hibyte(BLASTER_SampleRate);
		LoByte = lobyte(BLASTER_SampleRate);

		BLASTER_WriteDSP(0x41);
		BLASTER_WriteDSP(HiByte);
		BLASTER_WriteDSP(LoByte);
	}
}

unsigned BLASTER_GetPlaybackRate(void)
{
	return BLASTER_SampleRate;
}

int BLASTER_SetMixMode(int mode)
{
	int port;
	int data;
	int CardType;
	
	CardType = BLASTER_Config.Type;
	mode &= 3;
	
	if (!(BLASTER_CardConfig[CardType].MaxMixMode & 1))
	{
		mode &= ~1;
	}
	if (!(BLASTER_CardConfig[CardType].MaxMixMode & 2))
	{
		mode &= ~2;
	}
	
	BLASTER_MixMode = mode;
	BLASTER_SamplePacketSize = BLASTER_SampleSize[mode];
	
	if (CardType == 2 || CardType == 4)
	{
		port = BLASTER_Config.Address + 4;
		outp(port, 0xE);

		port = BLASTER_Config.Address + 5;
		data = inp(port);
		if (mode & 1)
		{
			data |= 2;
		}
		else
		{
			data &= ~2;
		}
		outp(port, data);
	
		BLASTER_SetPlaybackRate(BLASTER_SampleRate);
	}
	
	return mode;
}

void BLASTER_StopPlayback(void)
{
	BLASTER_DisableInterrupt();
	if (BLASTER_HaltTransferCommand == -1)
	{
		BLASTER_ResetDSP();
	}
	else
	{
		BLASTER_WriteDSP(BLASTER_HaltTransferCommand);
	}
	BLASTER_SpeakerOff();
	BLASTER_SoundPlaying = false;
	BLASTER_DMABuffer = NULL;
}

static int BLASTER_SetupDMABuffer(char far *BufferPtr, unsigned BufferSize)
{
	int DmaChannel;
	int DmaStatus;
	
	if (BLASTER_MixMode & 2)
	{
		DmaChannel = BLASTER_Config.Dma16;
	}
	else
	{
		DmaChannel = BLASTER_Config.Dma8;
	}
	if (DmaChannel == -1)
	{
		BLASTER_ErrorCode = 4;
		return -1;
	}
	
	DmaStatus = DMA_SetupTransfer(DmaChannel, BufferPtr, BufferSize);
	if (DmaStatus == -1)
	{
		BLASTER_ErrorCode = 10;
		return -1;
	}
	BLASTER_DMABuffer = BufferPtr;
	BLASTER_CurrentDMABuffer = BufferPtr;
	BLASTER_TotalDMABufferSize = BufferSize;
	BLASTER_DMABufferEnd = BufferPtr + BufferSize;
	
	return 0;
}

int BLASTER_GetCurrentPos(void)
{
	int DmaChannel;
	int offset;	// BUG? should probably be unsigned to avoid problems
	char far *CurrentAddr;
	
	if (!BLASTER_SoundPlaying)
	{
		BLASTER_ErrorCode = 8;
		return -1;
	}
	
	if (BLASTER_MixMode & 2)
	{
		DmaChannel = BLASTER_Config.Dma16;
	}
	else
	{
		DmaChannel = BLASTER_Config.Dma8;
	}
	if (DmaChannel == -1)
	{
		BLASTER_ErrorCode = 4;
		return -1;
	}
	
	CurrentAddr = DMA_GetCurrentPos(DmaChannel);
	offset = (char huge *)CurrentAddr - (char huge *)BLASTER_CurrentDMABuffer;
	
	if (BLASTER_MixMode & 2)
	{
		offset >>= 1;
	}
	if (BLASTER_MixMode & 1)
	{
		offset >>= 1;
	}
	
	return offset;
}

static int BLASTER_DSP1xx_BeginPlayback(int length)
{
	int SampleLength;
	int LoByte;
	int HiByte;
	
	SampleLength = length - 1;
	
	HiByte = hibyte(SampleLength);
	LoByte = lobyte(SampleLength);
	
	BLASTER_WriteDSP(0x14);
	BLASTER_WriteDSP(LoByte);
	BLASTER_WriteDSP(HiByte);
	
	BLASTER_HaltTransferCommand = 0xD0;
	BLASTER_SoundPlaying = true;
	
	return 0;
}

static int BLASTER_DSP2xx_BeginPlayback(int length)
{
	int SampleLength;
	int LoByte;
	int HiByte;
	
	SampleLength = length - 1;
	
	HiByte = hibyte(SampleLength);
	LoByte = lobyte(SampleLength);
	
	BLASTER_WriteDSP(0x48);
	BLASTER_WriteDSP(LoByte);
	BLASTER_WriteDSP(HiByte);
	
	if (BLASTER_Version >= 0x201 && BLASTER_SampleRate*BLASTER_SamplePacketSize > 22000)
	{
		BLASTER_WriteDSP(0x90);
		BLASTER_HaltTransferCommand = -1;
	}
	else
	{
		BLASTER_WriteDSP(0x1C);
		BLASTER_HaltTransferCommand = 0xD0;
	}

	BLASTER_SoundPlaying = true;
	
	return 0;
}

static int BLASTER_DSP4xx_BeginPlayback(int length)
{
	int TransferCommand;
	int TransferMode;
	int SampleLength;
	int LoByte;
	int HiByte;
	
	if (BLASTER_MixMode & 2)
	{
		TransferCommand = 0xB6;
		SampleLength = length/2 - 1;
		BLASTER_HaltTransferCommand = 0xD5;
		
		if (BLASTER_MixMode & 1)
		{
			TransferMode = 0x30;
		}
		else
		{
			TransferMode = 0x10;
		}
	}
	else
	{
		TransferCommand = 0xC6;
		SampleLength = length - 1;
		BLASTER_HaltTransferCommand = 0xD0;
		
		if (BLASTER_MixMode & 1)
		{
			TransferMode = 0x20;
		}
		else
		{
			TransferMode = 0x00;
		}
	}
	
	HiByte = hibyte(SampleLength);
	LoByte = lobyte(SampleLength);
	
	BLASTER_WriteDSP(TransferCommand);
	BLASTER_WriteDSP(TransferMode);
	BLASTER_WriteDSP(LoByte);
	BLASTER_WriteDSP(HiByte);
	
	BLASTER_SoundPlaying = true;
	
	return 0;
}

int BLASTER_BeginBufferedPlayback(char far *BufferStart, int BufferSize, int NumDivisions, unsigned SampleRate, int MixMode, void (*CallBackFunc)(void))
{
	int DmaStatus;
	int TransferLength;
	
	if (BLASTER_SoundPlaying)
	{
		BLASTER_StopPlayback();
	}
	
	DmaStatus = BLASTER_SetupDMABuffer(BufferStart, BufferSize);
	if (DmaStatus == -1)
	{
		return -1;
	}
	
	BLASTER_SetMixMode(MixMode);
	BLASTER_SetPlaybackRate(SampleRate);
	BLASTER_SetCallBack(CallBackFunc);
	BLASTER_EnableInterrupt();
	BLASTER_SpeakerOn();
	
	TransferLength = BufferSize / NumDivisions;
	BLASTER_TransferLength = TransferLength;
	
	if (BLASTER_Version < 0x200)
	{
		BLASTER_DSP1xx_BeginPlayback(TransferLength);
	}
	else if (BLASTER_Version < 0x400)
	{
		BLASTER_DSP2xx_BeginPlayback(TransferLength);
	}
	else
	{
		BLASTER_DSP4xx_BeginPlayback(TransferLength);
	}
	
	return 0;
}

static void BLASTER_WriteMixer(int reg, int data)
{
	outp(BLASTER_MixerAddress+4, reg);
	outp(BLASTER_MixerAddress+5, data);
}

static int BLASTER_ReadMixer(int reg)
{
	int data;
	
	outp(BLASTER_MixerAddress+4, reg);
	data = inp(BLASTER_MixerAddress+5);
	
	return data;
}

int BLASTER_GetVoiceVolume(void)
{
	int volume;
	int left;
	int right;
	
	switch (BLASTER_MixerType)
	{
	case 2:
	case 4:
		left = BLASTER_ReadMixer(4);
		right = (left & 0xF) << 4;
		left &= 0xF0;
		volume = (left + right) / 2;
		break;
		
	case 6:
		left = BLASTER_ReadMixer(0x32);
		right = BLASTER_ReadMixer(0x33);
		volume = (left + right) / 2;
		break;
		
	default:
		BLASTER_ErrorCode = 11;
		volume = -1;
		break;
	}
	
	return volume;
}

int BLASTER_SetVoiceVolume(int volume)
{
	int data;
	int status;
	
	volume = min(255, volume);
	volume = max(0, volume);
	
	status = 0;
	
	switch (BLASTER_MixerType)
	{
	case 2:
	case 4:
		data = (volume & 0xF0) + (volume >> 4);
		BLASTER_WriteMixer(4, data);
		break;
		
	case 6:
		BLASTER_WriteMixer(0x32, volume & 0xF8);
		BLASTER_WriteMixer(0x33, volume & 0xF8);
		break;
		
	default:
		BLASTER_ErrorCode = 11;
		status = -1;
	}
	
	return status;
}

int BLASTER_GetMidiVolume(void)
{
	int volume;
	int left;
	int right;
	
	switch (BLASTER_MixerType)
	{
	case 2:
	case 4:
		left = BLASTER_ReadMixer(0x26);
		right = (left & 0xF) << 4;
		left &= 0xF0;
		volume = (left + right) / 2;
		break;
		
	case 6:
		left = BLASTER_ReadMixer(0x34);
		right = BLASTER_ReadMixer(0x35);
		volume = (left + right) / 2;
		break;
		
	default:
		BLASTER_ErrorCode = 11;
		volume = -1;
	}
	
	return volume;
}

int BLASTER_SetMidiVolume(int volume)
{
	int data;
	int status;
	
	volume = min(255, volume);
	volume = max(0, volume);
	
	status = 0;
	
	switch (BLASTER_MixerType)
	{
	case 2:
	case 4:
		data = (volume & 0xF0) + (volume >> 4);
		BLASTER_WriteMixer(0x26, data);
		break;
		
	case 6:
		BLASTER_WriteMixer(0x34, volume & 0xF8);
		BLASTER_WriteMixer(0x35, volume & 0xF8);
		break;
		
	default:
		BLASTER_ErrorCode = 11;
		status = -1;
	}
	
	return status;
}

int BLASTER_CardHasMixer(void)
{
	int status;
	BLASTER_CONFIG Config;
	
	if (BLASTER_MixerAddress == -1)
	{
		status = BLASTER_GetEnv(&Config);
		if (status == 0)
		{
			BLASTER_MixerAddress = Config.Address;
			BLASTER_MixerType = 0;
			if (Config.Type >= 1 || Config.Type <= 6)	// BUG: should be '&&', not '||'!
			{
				BLASTER_MixerType = Config.Type;
			}
		}
	}
	if (BLASTER_MixerAddress != -1)
	{
		return BLASTER_CardConfig[BLASTER_MixerType].HasMixer;
	}
	else
	{
		return 0;
	}
}

int BLASTER_GetEnv(BLASTER_CONFIG *Config)
{
	char *Blaster;
	char parameter;
	int error;
	int status;
	
	Config->Address = -1;
	Config->Type = -1;
	Config->Interrupt = -1;
	Config->Dma8 = -1;
	Config->Dma16 = -1;
	Config->Midi = -1;
	
	Blaster = getenv("BLASTER");
	if (Blaster == NULL)
	{
		BLASTER_ErrorCode = 1;
		return -1;
	}
	
	while (*Blaster)
	{
		if (*Blaster == ' ')
		{
			Blaster++;
		}
		else
		{
			parameter = toupper(*Blaster);
			Blaster++;
			if (!isxdigit(*Blaster))
			{
				BLASTER_ErrorCode = 6;
				return -1;
			}
			switch (parameter)
			{
			case 'A':
				sscanf(Blaster, "%x", &Config->Address);
				break;
				
			case 'I':
				sscanf(Blaster, "%d", &Config->Interrupt);
				break;
				
			case 'D':
				sscanf(Blaster, "%d", &Config->Dma8);
				break;
				
			case 'T':
				sscanf(Blaster, "%d", &Config->Type);
				break;
				
			case 'H':
				sscanf(Blaster, "%d", &Config->Dma16);
				break;
				
			case 'P':
				sscanf(Blaster, "%x", &Config->Midi);
				break;
				
			default:
				BLASTER_ErrorCode = 6;
				return -1;
			}
			
			while (isxdigit(*Blaster))
			{
				Blaster++;
			}
		}
	}
	
	status = 0;
	error = 0;
	
	if (Config->Type == -1)
		error = 5;
	if (Config->Dma8 == -1)
		error = 4;
	if (Config->Interrupt == -1)
		error = 3;
	if (Config->Address == -1)
		error = 2;
	
	if (error)
	{
		status = -1;
		BLASTER_ErrorCode = error;
	}
	
	return status;
}

int BLASTER_SetCardSettings(BLASTER_CONFIG *Config)
{
	if (BLASTER_Installed)
	{
		BLASTER_Shutdown();
	}
	
	BLASTER_Config.Address = Config->Address;
	BLASTER_Config.Type = Config->Type;
	BLASTER_Config.Interrupt = Config->Interrupt;
	BLASTER_Config.Dma8 = Config->Dma8;
	BLASTER_Config.Dma16 = Config->Dma16;
	BLASTER_Config.Midi = Config->Midi;
	
	return 0;
}

int BLASTER_GetCardInfo(int *MaxSampleBits, int *MaxChannels)
{
	int CardType;
	
	CardType = BLASTER_Config.Type;
	if (CardType == -1)
	{
		BLASTER_ErrorCode = 5;
		return -1;
	}
	
	if (BLASTER_CardConfig[CardType].MaxMixMode & 1)
	{
		*MaxChannels = 2;
	}
	else
	{
		*MaxChannels = 1;
	}
	
	if (BLASTER_CardConfig[CardType].MaxMixMode & 2)
	{
		*MaxSampleBits = 16;
	}
	else
	{
		*MaxSampleBits = 8;
	}
	
	return 0;
}

void BLASTER_SetCallBack(void (*CallBackFunc)(void))
{
	// BUG: interrupts should be disabled while changing BLASTER_CallBack
	BLASTER_CallBack = CallBackFunc;
}

void BLASTER_SaveVoiceVolume(void)
{
	if (BLASTER_CardHasMixer())
	{
		switch (BLASTER_MixerType)
		{
		case 2:
		case 4:
			BLASTER_OriginalVoiceVolumeLeft = BLASTER_ReadMixer(4);
			break;
			
		case 6:
			BLASTER_OriginalVoiceVolumeLeft = BLASTER_ReadMixer(0x32);
			BLASTER_OriginalVoiceVolumeRight = BLASTER_ReadMixer(0x33);
			break;
		}
	}
}

void BLASTER_RestoreVoiceVolume(void)
{
	if (BLASTER_CardHasMixer())
	{
		switch (BLASTER_MixerType)
		{
		case 2:
		case 4:
			BLASTER_WriteMixer(4, BLASTER_OriginalVoiceVolumeLeft);
			break;
			
		case 6:
			BLASTER_WriteMixer(0x32, BLASTER_OriginalVoiceVolumeLeft);
			BLASTER_WriteMixer(0x33, BLASTER_OriginalVoiceVolumeRight);
			break;
		}
	}
}

void BLASTER_SaveMidiVolume(void)
{
	if (BLASTER_CardHasMixer())
	{
		switch (BLASTER_MixerType)
		{
		case 2:
		case 4:
			BLASTER_OriginalMidiVolumeLeft = BLASTER_ReadMixer(0x26);
			break;
			
		case 6:
			BLASTER_OriginalMidiVolumeLeft = BLASTER_ReadMixer(0x34);
			BLASTER_OriginalMidiVolumeRight = BLASTER_ReadMixer(0x35);
			break;
		}
	}
}

void BLASTER_RestoreMidiVolume(void)
{
	if (BLASTER_CardHasMixer())
	{
		switch (BLASTER_MixerType)
		{
		case 2:
		case 4:
			BLASTER_WriteMixer(0x26, BLASTER_OriginalMidiVolumeLeft);
			break;
			
		case 6:
			BLASTER_WriteMixer(0x34, BLASTER_OriginalMidiVolumeLeft);
			BLASTER_WriteMixer(0x35, BLASTER_OriginalMidiVolumeRight);
			break;
		}
	}
}

int BLASTER_Init(void)
{
	int Irq;
	int Interrupt;
	int status;
	
	if (BLASTER_Installed)
	{
		BLASTER_Shutdown();
	}
	
	BLASTER_IntController1Mask = inp(0x21);
	BLASTER_IntController2Mask = inp(0xA1);
	
	status = BLASTER_ResetDSP();
	if (status == 0)
	{
		BLASTER_SaveVoiceVolume();
		BLASTER_SoundPlaying = false;
		BLASTER_SetCallBack(NULL);
		BLASTER_DMABuffer = NULL;
		BLASTER_Version = BLASTER_GetDSPVersion();
		BLASTER_SetPlaybackRate(11000);
		BLASTER_SetMixMode(0);
		
		if (BLASTER_Config.Dma16 != -1)
		{
			status = DMA_VerifyChannel(BLASTER_Config.Dma16);
			if (status == -1)
			{
				BLASTER_ErrorCode = 10;
				return -1;
			}
		}
		if (BLASTER_Config.Dma8 != -1)
		{
			status = DMA_VerifyChannel(BLASTER_Config.Dma8);
			if (status == -1)
			{
				BLASTER_ErrorCode = 10;
				return -1;
			}
		}
		
		Irq = BLASTER_Config.Interrupt;
		Interrupt = BLASTER_Interrupts[Irq];
		
		if (Interrupt == 0)
		{
			BLASTER_ErrorCode = 9;
			return -1;
		}
		
		BLASTER_OldInt = getvect(Interrupt);
		setvect(Interrupt, BLASTER_ServiceInterrupt);
		
		BLASTER_Installed = true;
		status = 0;
	}
	
	BLASTER_ErrorCode = status;
	return status;
}

void BLASTER_Shutdown(void)
{
	int Irq;
	int Interrupt;
	
	BLASTER_StopPlayback();
	BLASTER_RestoreVoiceVolume();
	BLASTER_ResetDSP();
	
	Irq = BLASTER_Config.Interrupt;
	Interrupt = BLASTER_Interrupts[Irq];
	setvect(Interrupt, BLASTER_OldInt);
	
	BLASTER_SoundPlaying = false;
	BLASTER_DMABuffer = NULL;
	BLASTER_SetCallBack(NULL);
	
	BLASTER_Installed = false;
}
