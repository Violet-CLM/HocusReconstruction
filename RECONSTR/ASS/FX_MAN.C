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

#include <STDLIB.H>
#include "COMMON.H"
#include "FX_MAN.H"
#include "BLASTER.H"
#include "PAS16.H"
#include "SNDSRC.H"
#include "MULTIVOC.H"

typedef struct
{
	char tag;	// would be 'C' for unconverted VOC files, 0 for converted ones
	char far *samples;
	long length;
	long samplerate;
} fx_sound;

static int FX_NumVoices = 4;
static int FX_SampleBits = 8;
static int FX_SoundDevice = 0;
static int FX_ErrorCode = 0;

char * FX_ErrorString(int error)
{
	char *msg;
	
	switch (error)
	{
	case -1:
		msg = FX_ErrorString(FX_ErrorCode);
		break;
		
	case 0:
		msg = "Fx ok.";
		break;
		
	case 1:
		switch (FX_SoundDevice)
		{
		case CARD_SOUNDBLASTER:
			msg = BLASTER_ErrorString(-1);
			break;
			
		case CARD_PROAUDIO:
			msg = PAS_ErrorString(-1);
			break;
			
		case CARD_SOUNDSOURCE:
			msg = SS_ErrorString(-1);
			break;
			
		default:
			msg = FX_ErrorString(2);
		}
		break;
		
	case 2:
		msg = "Invalid Sound Fx device.";
		break;
		
	case 3:
		msg = MV_ErrorString(-1);
		break;
		
	case 4:
		msg = "Invalid VOC file.";
		break;
		
	default:
		msg = "Unknown Fx error code.";
		break;
	}
	
	return msg;
}

int FX_SetupCard(int SoundCard, fx_device *device)
{
	int status;
	int DeviceStatus;
	BLASTER_CONFIG Blaster;
	
	FX_SoundDevice = SoundCard;
	status = 0;
	
	switch (SoundCard)
	{
	case CARD_SOUNDBLASTER:
		DeviceStatus = BLASTER_GetEnv(&Blaster);
		if (DeviceStatus != 0)
		{
			FX_ErrorCode = 1;
			status = -1;
			break;
		}
		BLASTER_SetCardSettings(&Blaster);
		DeviceStatus = BLASTER_Init();
		if (DeviceStatus != 0)
		{
			FX_ErrorCode = 1;
			status = -1;
			break;
		}
		device->MaxVoices = 8;
		BLASTER_GetCardInfo(&device->MaxSampleBits, &device->MaxChannels);
		break;
		
	case CARD_PROAUDIO:
		DeviceStatus = PAS_Init();
		if (DeviceStatus != 0)
		{
			FX_ErrorCode = 1;
			status = -1;
			break;
		}
		device->MaxVoices = 8;
		PAS_GetCardInfo(&device->MaxSampleBits, &device->MaxChannels);
		break;
		
	case CARD_SOUNDSOURCE:
		DeviceStatus = SS_SetPort(0x378);
		if (DeviceStatus != 0)
		{
			FX_ErrorCode = 1;
			status = -1;
			break;
		}
		device->MaxVoices = 4;
		device->MaxSampleBits = 8;
		device->MaxChannels = 1;
		break;
		
	default:
		FX_ErrorCode = 2;
		status = -1;
	}
	
	return status;
}

int FX_Init(int SoundCard, int numvoices, int samplebits)
{
	int status;
	int DeviceStatus;
	int MixMode;
	fx_device device;
	
	status = FX_SetupCard(SoundCard, &device);
	if (status != 0)
	{
		return status;
	}
	
	FX_SoundDevice = SoundCard;	// redundant, FX_SetupCard already did that
	
	switch (SoundCard)
	{
	case CARD_SOUNDBLASTER:
	case CARD_PROAUDIO:
	case CARD_SOUNDSOURCE:
		MixMode = 0;
		FX_SampleBits = min(samplebits, device.MaxSampleBits);
		if (samplebits == 16)
		{
			MixMode |= 2;
		}
		FX_NumVoices = numvoices;
		DeviceStatus = MV_Init(SoundCard, 10000, numvoices, MixMode);
		if (DeviceStatus != 0)
		{
			FX_ErrorCode = 3;
			status = -1;
		}
		break;
		
	default:
		FX_ErrorCode = 2;
		status = -1;
	}
	
	return status;
}

int FX_Shutdown(void)
{
	int status;
	
	status = 0;
	
	switch (FX_SoundDevice)
	{
	case CARD_SOUNDBLASTER:
	case CARD_PROAUDIO:
	case CARD_SOUNDSOURCE:
		status = MV_Shutdown();
		if (status != 0)
		{
			FX_ErrorCode = 3;
			status = -1;
		}
		break;
		
	default:
		FX_ErrorCode = 2;
		status = -1;
		break;
	}
	
	return status;
}

void FX_SetVolume(int volume)
{
	switch (FX_SoundDevice)
	{
	case CARD_SOUNDBLASTER:
		if (BLASTER_CardHasMixer())
		{
			BLASTER_SetVoiceVolume(volume);
		}
		break;
		
	case CARD_PROAUDIO:
		PAS_SetPCMVolume(volume);
		break;
	}
}

int FX_GetVolume(void)
{
	int volume;
	
	volume = 0;
	
	switch (FX_SoundDevice)
	{
	case CARD_SOUNDBLASTER:
		if (BLASTER_CardHasMixer())
		{
			volume = BLASTER_GetVoiceVolume();
		}
		break;
		
	case CARD_PROAUDIO:
		volume = PAS_GetPCMVolume();
		break;
	}
	
	return volume;
}

static unsigned long FX_ConvertSampleRate(char far *data, unsigned long length, unsigned oldrate, unsigned newrate)
{
	unsigned long factor;
	unsigned long index;
	char far *ptr;
	
	factor = (oldrate * 0x10000) / newrate;
	if (factor <= 0x10000)
	{
		// The conversion would increase the length of the sound, which means it
		// wouldn't fit into the allocated buffer anymore. Converting the sample
		// data is not possible.
		return length;
	}
	
	index = 0;
	ptr = data;
	length = length * 0x10000;
	
	while (index < length)
	{
		*ptr = data[(index / 0x10000)];
		ptr++;
		index += factor;
	}
	
	return ptr - data;
}

int FX_ConvertVOC(char far *data)
{
	char huge *datablock;
	unsigned long blocklength;
	unsigned long samplerate;
	unsigned ratebyte;
	unsigned i;
	
	if (*data == 'C')	// VOC files start with "Creative Voice File"
	{
		//
		// The 16-bit value at offset 20 in the VOC file stores the offset of the
		// first data block in the VOC file
		//
		datablock = data + *((unsigned far *)(data+20));
		
		//
		// VOC files can store all kinds of things, but since this code was
		// designed to play many sounds simultaneously, most of the VOC data
		// cannot be processed and only a small subset of VOC files are fully
		// supported. Only the pure sound data is used, everything else is
		// ignored
		//
		while (*datablock > 1)
		{
			datablock += 1;
			blocklength = *((unsigned long far*)datablock) & 0xFFFFFF;
			datablock += blocklength + 3;
		}
		if (*datablock == 0)
		{
			//
			// We reached the Terminator block without encountering any type 1
			// blocks (sound data), which means we can't play this VOC file.
			//
			FX_ErrorCode = 4;
			return -1;
		}
		
		//
		// we found a sound data block (type 1)
		//
		datablock += 1;	// skip the type value
		blocklength = (*((unsigned long far*)datablock) & 0xFFFFFF) - 2;
		ratebyte = *(datablock + 3);
		datablock += 5;	// skip to start of PCM data
		samplerate = 256000000 / (0x10000 - (ratebyte << 8));	// convert ratebyte to Hz
		
		// Note: The code above skips the compression type byte (directly after
		// the rate byte) and assumes that the compression type is always 0.
		// VOC data that uses Adaptive Depth Pulse Code Modulation (ADPCM) will
		// be played back incorrectly (treated as 8-bit PCM). This code will not
		// be able to play certain VOC sound effects from Duke Nukem II or Major
		// Stryker.
		
		if (FX_SoundDevice == CARD_SOUNDSOURCE)
		{
			blocklength= FX_ConvertSampleRate((char far *)datablock, blocklength, 10000, 7000);
		}
		
		((fx_sound far *)data)->tag = 0;	// erase the 'C' of the VOC header
		((fx_sound far *)data)->samples = (char far *)datablock;
		((fx_sound far *)data)->length = blocklength;
		((fx_sound far *)data)->samplerate = samplerate;
		
		if (FX_SampleBits == 16)
		{
			// convert samples into signed 8-bit PCM data (for faster mixing):
			for (i=0; i<blocklength; i++)
			{
				datablock[i] = datablock[i] + 0x80;
			}
		}
		else
		{
			// convert samples into signed 8-bit PCM data AND divide each sample
			// by the number of voices used by the mixer (for faster mixing):
			for (i=0; i<blocklength; i++)
			{
				datablock[i] = datablock[i] + 0x80;
				datablock[i] = datablock[i] / FX_NumVoices;
			}
			
			// Note: The sounds will still get played as unsigned 8-bit PCM data.
			// The mixer code initializes the mix buffer to the correct "silence"
			// value and then simply adds the sound's signed PCM sample data into
			// that buffer, resulting in proper unsigned PCM data.
		}
	}
	
	FX_ErrorCode = 0;
	return 0;
}

#pragma argsused
int FX_PlayVOC(char far *data, int foo, int bar, int priority)
{
#define sound ((fx_sound far *)data)
	int status;
	int handle;
	
	//
	// convert the VOC sound if it hasn't been converted yet:
	//
	if (*data == 'C')	// VOC files start with "Creative Voice File"
	{
		status = FX_ConvertVOC(data);
		if (status != 0)
		{
			return status;
		}
	}
	
	switch (FX_SoundDevice)
	{
	case CARD_SOUNDBLASTER:
	case CARD_PROAUDIO:
	case CARD_SOUNDSOURCE:
		handle = MV_Play(sound->samples, sound->length, priority);
		// Note that the sound's samplerate is ignored. This version of the Apogee
		// sound system plays every sound at 10000 Hz ( SoundSource: 7000 Hz ).
		if (handle == -1)
		{
			FX_ErrorCode = 3;
			handle = -1;
		}
		break;
		
	default:
		FX_ErrorCode = 2;
		handle = -1;
		break;
	}
	
	return handle;
#undef sound
}

int FX_SoundActive(int handle)
{
	return MV_VoicePlaying(handle);
}

int FX_StopSound(int handle)
{
	int status;
	
	status = MV_Kill(handle);
	if (status != 0)
	{
		FX_ErrorCode = 3;
		return -2;
	}
	
	return 0;
}

int FX_StopAllSounds(void)
{
	int status;
	
	status = MV_KillAllVoices();
	if (status != 0)
	{
		FX_ErrorCode = 3;
		return -2;
	}
	
	return 0;
}

int FX_VoicesPlaying(void)
{
	return MV_VoicesPlaying();
}
