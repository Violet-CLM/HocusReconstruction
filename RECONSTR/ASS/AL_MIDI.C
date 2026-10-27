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
#include <MATH.H>
#include "COMMON.H"
#include "AL_MIDI.H"
#include "BLASTER.H"
#include "LL_MAN.H"
#include "USER.H"

typedef struct VOICE
{
	struct VOICE *next;
	struct VOICE *prev;
	
	int num;
	int key;
	int velocity;
	int channel;
	int pitchleft;
	int pitchright;
	int timbre;
	int status;
} VOICE;

typedef struct
{
	VOICE *start;
	VOICE *end;
} VOICELIST;

typedef struct
{
	VOICELIST Voices;
	int Timbre;
	int Pitchbend;
	int KeyOffset;
	int KeyDetune;
	unsigned Volume;
	int EffectiveVolume;
	int Pan;
	int Detune;
	int Pitch;
	int Velocity;
} CHANNEL;

typedef struct
{
	unsigned char SAVEK[2];
	unsigned char Level[2];
	unsigned char Env1[2];
	unsigned char Env2[2];
	unsigned char Wave[2];
	unsigned char Feedback;
} TIMBRE;

typedef struct
{
	unsigned char Timbre;
	unsigned char Key;
} DRUM_MAP;

#define AL_MAX_VOICES 9
#define AL_MAX_CHANNELS 16
#define AL_DEFAULT_PORT 0x388


static int slotVoice[9][2] = {0,3,1,4,2,5,6,9,7,10,8,11,12,15,13,16,14,17};
static char OffsetSlot[18] = {0,1,2,3,4,5,8,9,10,11,12,13,16,17,18,19,20,21};
static unsigned NotePitch[32][12] = {343,363,385,408,432,458,485,514,544,577,611,647};
static int OctavePitch[8] = {0, 0x400, 0x800, 0xC00, 0x1000, 0x1400, 0x1800, 0x1C00};
static int VoiceReserved[AL_MAX_VOICES] = {false};
static int AL_LeftPort = AL_DEFAULT_PORT;
static int AL_RightPort = AL_DEFAULT_PORT;
static int AL_Stereo = false;
static int AL_OPL3 = false;

static VOICE Voice[AL_MAX_VOICES];
static unsigned NoteMod12[96];
static unsigned NoteDiv12[96];
static VOICELIST Voice_Pool;
static CHANNEL Channel[AL_MAX_CHANNELS];
static unsigned VoiceLevel[18];
static unsigned VoiceKsl[18];
static unsigned PitchBendRange;

extern DRUM_MAP PercussionTable[];
extern TIMBRE ADLIB_TimbreBank[];

static void AL_SendOutputToPort(int port, int reg, int data)
{
	int count;
	
	asm	pushf;
	asm	cli;
	
	outp(port, reg);
	
	for (count = 6; count > 0; count--)
	{
		inp(port);
	}
	
	outp(port+1, data);
	
	asm	popf;
	
	for (count = 35; count > 0; count--)
	{
		inp(port);
	}
}

static void AL_SendOutput(int reg, int data)
{
	if (AL_Stereo)
	{
		AL_SendOutputToPort(AL_LeftPort, reg, data);
		AL_SendOutputToPort(AL_RightPort, reg, data);
	}
	else
	{
		AL_SendOutputToPort(AL_DEFAULT_PORT, reg, data);
	}
}

static void AL_SetVoiceTimbre(int voice)
{
	int off;
	int slot;
	int patch;
	int channel;
	TIMBRE *timbre;
	
	channel = Voice[voice].channel;
	if (channel == 9)
	{
		patch = PercussionTable[Voice[voice].key].Timbre;
		if (Voice[voice].timbre == patch)
		{
			return;
		}
	
		Voice[voice].timbre = patch;
	}
	else
	{
		patch = Channel[channel].Timbre;
		if (Voice[voice].timbre == patch)
		{
			return;
		}
	
		Voice[voice].timbre = patch;
	}
	
	timbre = ADLIB_TimbreBank + patch;
	slot = slotVoice[voice][0];
	off = OffsetSlot[slot];
	VoiceLevel[slot] = 0x3F - (timbre->Level[0] & 0x3F);
	VoiceKsl[slot] = timbre->Level[0] & 0xC0;
	
	AL_SendOutput(voice+0xA0, 0);
	AL_SendOutput(voice+0xB0, 0);
	AL_SendOutput(off+0x80, 0xFF);
	
	AL_SendOutput(off+0x60, timbre->Env1[0]);
	AL_SendOutput(off+0x80, timbre->Env2[0]);
	AL_SendOutput(off+0x20, timbre->SAVEK[0]);
	AL_SendOutput(off+0xE0, timbre->Wave[0]);
	AL_SendOutput(off+0x40, timbre->Level[0]);
	
	slot = slotVoice[voice][1];
	if (AL_OPL3)
	{
		AL_SendOutputToPort(AL_LeftPort, voice+0xC0, (timbre->Feedback & 0xF) | 0x20);
		AL_SendOutputToPort(AL_RightPort, voice+0xC0, (timbre->Feedback & 0xF) | 0x10);
	}
	else
	{
		AL_SendOutputToPort(AL_DEFAULT_PORT, voice+0xC0, timbre->Feedback);
	}
	
	off = OffsetSlot[slot];
	VoiceLevel[slot] = 0x3F - (timbre->Level[1] & 0x3F);
	VoiceKsl[slot] = timbre->Level[1] & 0xC0;
	
	AL_SendOutput(off+0x40, 0x3F);
	AL_SendOutput(off+0x80, 0xFF);
	
	AL_SendOutput(off+0x60, timbre->Env1[1]);
	AL_SendOutput(off+0x80, timbre->Env2[1]);
	AL_SendOutput(off+0x20, timbre->SAVEK[1]);
	AL_SendOutput(off+0xE0, timbre->Wave[1]);
}

static void AL_SetVoiceVolume(int voice)
{
	int velocity;
	int slot;
	int channel;
	unsigned long volume;
	
	channel = Voice[voice].channel;
	velocity = Voice[voice].velocity + Channel[channel].Velocity;
	velocity = max(velocity, 0x7F);	// BUG: later versions use min here
	
	slot = slotVoice[voice][1];
	volume = VoiceLevel[slot];
	volume *= velocity | 0x80;
	volume = (volume * Channel[channel].Volume) >> 15;
	volume ^= 0x3F;
	volume |= VoiceKsl[slot];
	
	AL_SendOutput(OffsetSlot[slot]+0x40, volume);
}

static int AL_AllocVoice(void)
{
	int voice;
	
	if (Voice_Pool.start != NULL)
	{
		voice = Voice_Pool.start->num;
		LL_Remove(VOICE, &Voice_Pool, Voice+voice);
		
		return voice;
	}
	return -1;
}

static int AL_GetVoice(int channel, int key)
{
	VOICE *voice;
	
	for (voice=Channel[channel].Voices.start; voice!=NULL; voice=voice->next)
	{
		if (voice->key == key)
		{
			return voice->num;
		}
	}
	
	return -1;
}

static void AL_SetVoicePitch(int voice)
{
	int key;
	int channel;
	int detune;
	int ScaleNote;
	int Octave;
	int pitch;
	
	channel = Voice[voice].channel;
	if (channel == 9)
	{
		key = PercussionTable[Voice[voice].key].Key;
	}
	else
	{
		key = Voice[voice].key;
	}
	
	key += Channel[channel].KeyOffset + Channel[channel].Pitch + -12;
	if (key > 95)
		key = 95;
	if (key < 0)
		key = 0;
	
	detune = Channel[channel].KeyDetune;
	ScaleNote = NoteMod12[key];
	Octave = NoteDiv12[key];
	
	pitch = OctavePitch[Octave] | NotePitch[detune][ScaleNote];
	Voice[voice].pitchleft = pitch;
	pitch |= Voice[voice].status;
	
	if (!AL_Stereo)
	{
		AL_SendOutputToPort(AL_DEFAULT_PORT, voice+0xA0, pitch);
		AL_SendOutputToPort(AL_DEFAULT_PORT, voice+0xB0, pitch >> 8);
	}
	else
	{
		AL_SendOutputToPort(AL_LeftPort, voice+0xA0, pitch);
		AL_SendOutputToPort(AL_LeftPort, voice+0xB0, pitch >> 8);
		
		detune += 5;
		if (detune > 31)
		{
			detune -= 32;
			if (key < 95)
			{
				key++;
				ScaleNote = NoteMod12[key];
				Octave = NoteDiv12[key];
			}
		}
		
		pitch = OctavePitch[Octave] | NotePitch[detune][ScaleNote];
		Voice[voice].pitchright = pitch;
		pitch |= Voice[voice].status;
	
		AL_SendOutputToPort(AL_RightPort, voice+0xA0, pitch);
		AL_SendOutputToPort(AL_RightPort, voice+0xB0, pitch >> 8);
	}
}

static void AL_SetChannelVolume(int channel, int volume)
{
	VOICE *voice;
	
	volume = max(0, volume);
	volume = min(volume, 127);
	
	Channel[channel].Volume = volume;
	
	for (voice=Channel[channel].Voices.start; voice!=NULL; voice=voice->next)
	{
		AL_SetVoiceVolume(voice->num);
	}
}

static void AL_SetChannelPan(int channel, int pan)
{
	Channel[channel].Pan = pan;
}

static void AL_SetChannelDetune(int channel, int detune)
{
	Channel[channel].Detune = detune;
}

static void AL_ResetVoices(void)
{
	int i;
	
	Voice_Pool.start = NULL;
	Voice_Pool.end = NULL;
	
	for (i=0; i<AL_MAX_VOICES; i++)
	{
		if (!VoiceReserved[i])
		{
			Voice[i].num = i;
			Voice[i].key = 0;
			Voice[i].velocity = 0;
			Voice[i].channel = -1;
			Voice[i].timbre = -1;
			Voice[i].status = 0;
			
			LL_AddToTail(VOICE, &Voice_Pool, Voice+i);
		}
	}
	
	for (i=0; i<AL_MAX_CHANNELS; i++)
	{
		Channel[i].Voices.start = NULL;
		Channel[i].Voices.end = NULL;
		Channel[i].Timbre = 0;
		Channel[i].Pitchbend = 0;
		Channel[i].KeyOffset = 0;
		Channel[i].KeyDetune = 0;
		Channel[i].Volume = 90;
		Channel[i].Pan = 0;
		Channel[i].Pitch = 0;
		Channel[i].Velocity = 0;
	}
}

static void AL_CalcPitchInfo(void)
{
	int note;
	int finetune;
	double detune;
	
	for (note = 0; note <= 95; note++)
	{
		NoteMod12[note] = note % 12;
		NoteDiv12[note] = note / 12;
	}
	
	for (finetune = 1; finetune <= 31; finetune++)
	{
		detune = pow(2.0, finetune/384.0);
		for (note = 0; note < 12; note++)
		{
			NotePitch[finetune][note] = (long)NotePitch[0][note] * detune;
		}
	}
}

static void AL_FlushCard(int port)
{
	int i;
	
	for (i=0; i<AL_MAX_VOICES; i++)
	{
		if (!VoiceReserved[i])
		{
			AL_SendOutputToPort(port, i+0xA0, 0);
			AL_SendOutputToPort(port, i+0xB0, 0);
			AL_SendOutputToPort(port, OffsetSlot[slotVoice[i][0]]+0xE0, 0);
			AL_SendOutputToPort(port, OffsetSlot[slotVoice[i][1]]+0xE0, 0);
		}
	}
}

void AL_Reset(void)
{
	PitchBendRange = 2;
	
	AL_SendOutputToPort(AL_DEFAULT_PORT, 0x01, 0x20);
	AL_SendOutputToPort(AL_DEFAULT_PORT, 0x08, 0x00);
	AL_SendOutputToPort(AL_DEFAULT_PORT, 0xBD, 0x00);
	
	if (AL_OPL3)
	{
		AL_SendOutputToPort(AL_RightPort, 5, 1);
	}
	if (AL_Stereo)
	{
		AL_FlushCard(AL_LeftPort);
		AL_FlushCard(AL_RightPort);
	}
	else
	{
		AL_FlushCard(AL_DEFAULT_PORT);
	}
	
	AL_ResetVoices();
}

int AL_ReserveVoice(int voice)
{
	asm	pushf;
	asm	cli;
	
	if (voice < 0 || voice >= AL_MAX_VOICES)
	{
		// BUG: should pop flags here, first!
		return -1;
	}
	
	if (VoiceReserved[voice])
	{
		asm	popf;
		return -2;
	}
	
	if (Voice[voice].status == 0x2000)
	{
		AL_NoteOff(Voice[voice].channel, Voice[voice].key, 0);
	}
	
	VoiceReserved[voice] = true;
	LL_Remove(VOICE, &Voice_Pool, Voice+voice);
	
	asm	popf;
	
	return 0;
}

int AL_ReleaseVoice(int voice)
{
	asm	pushf;
	asm	cli;
	
	if (voice < 0 || voice >= AL_MAX_VOICES)
	{
		// BUG: should pop flags here, first!
		return -1;
	}
	
	if (!VoiceReserved[voice])
	{
		asm	popf;
		return -2;
	}
	
	VoiceReserved[voice] = false;
	LL_AddToTail(VOICE, &Voice_Pool, Voice+voice);
	
	asm	popf;
	
	return 0;
}

void AL_Shutdown(void)
{
	AL_Reset();
	if (AL_OPL3)
	{
		AL_SendOutputToPort(AL_RightPort, 5, 0);
	}
}

int AL_Init(int soundcard)
{
	int status;
	BLASTER_CONFIG Blaster;
	
	AL_Stereo = false;
	AL_OPL3 = false;
	AL_LeftPort = AL_DEFAULT_PORT;
	AL_RightPort = AL_DEFAULT_PORT;
	
	switch (soundcard)
	{
	case 1:
		AL_OPL3 = true;
		AL_Stereo = true;
		AL_LeftPort = AL_DEFAULT_PORT;
		AL_RightPort = AL_DEFAULT_PORT + 2;
		break;
		
	case 0:
		status = BLASTER_GetEnv(&Blaster);
		if (status == 0)
		{
			switch (Blaster.Type)
			{
			case 4:
			case 6:
				AL_OPL3 = true;
				AL_Stereo = true;
				AL_LeftPort = Blaster.Address;
				AL_RightPort = Blaster.Address + 2;
				break;
				
			case 2:
				AL_Stereo = true;
				AL_LeftPort = Blaster.Address;
				AL_RightPort = Blaster.Address + 2;
				break;
			}
		}
		break;
	}
	
	AL_CalcPitchInfo();
	AL_Reset();
	
	return 0;
}

#pragma argsused
void AL_NoteOff(int channel, int key, int velocity)
{
	int voice;
	
	voice = AL_GetVoice(channel, key);
	if (voice == -1)
	{
		return;
	}
	
	Voice[voice].status = 0;
	if (AL_Stereo)
	{
		AL_SendOutputToPort(AL_LeftPort, voice+0xB0, hibyte(Voice[voice].pitchleft));
		AL_SendOutputToPort(AL_RightPort, voice+0xB0, hibyte(Voice[voice].pitchright));
	}
	else
	{
		AL_SendOutputToPort(AL_DEFAULT_PORT, voice+0xB0, hibyte(Voice[voice].pitchleft));
	}
	
	LL_Remove(VOICE, &Channel[channel].Voices, Voice+voice);
	
	LL_AddToTail(VOICE, &Voice_Pool, Voice+voice);
}

void AL_NoteOn(int channel, int key, int velocity)
{
	int voice;
	
	if (velocity == 0)
	{
		AL_NoteOff(channel, key, velocity);
	}
	else
	{
		voice = AL_AllocVoice();
		if (voice == -1)
		{
			if (Channel[9].Voices.start != NULL)
			{
				AL_NoteOff(9, Channel[9].Voices.start->key, 0);
				voice = AL_AllocVoice();
			}
			if (voice == -1)
			{
				return;
			}
		}
		
		Voice[voice].key = key;
		Voice[voice].channel = channel;
		Voice[voice].velocity = velocity;
		Voice[voice].status = 0x2000;
		
		LL_AddToTail(VOICE, &Channel[channel].Voices, Voice+voice);
		
		AL_SetVoiceTimbre(voice);
		AL_SetVoiceVolume(voice);
		AL_SetVoicePitch(voice);
	}
}

void AL_AllNotesOff(int channel)
{
	while (Channel[channel].Voices.start != NULL)
	{
		AL_NoteOff(channel, Channel[channel].Voices.start->key, 0);
	}
	
	AL_Reset();
}

void AL_ControlChange(int channel, int type, int data)
{
	switch (type)
	{
	case 7:
		AL_SetChannelVolume(channel, data);
		break;
		
	case 0xA:
		AL_SetChannelPan(channel, data);
		break;
		
	case 0x5E:
		AL_SetChannelDetune(channel, data);
		break;
		
	case 0x7B:
		AL_AllNotesOff(channel);
		break;
		
	case 0x79:
		AL_SetChannelVolume(channel, 90);
		AL_SetChannelPan(channel, 0);
		AL_SetChannelDetune(channel, 0);
		break;
	}
}

void AL_ProgramChange(int channel, int patch)
{
	Channel[channel].Timbre = patch;
}

void AL_SetPitchBend(int channel, int lsb, int msb)
{
	int pitch;
	unsigned long TotalBend;
	VOICE *voice;
	
	pitch = lsb + (msb << 8);
	Channel[channel].Pitchbend = pitch;
	TotalBend = pitch * PitchBendRange;
	TotalBend /= 0x200;
	Channel[channel].KeyOffset = TotalBend >> 5;
	Channel[channel].KeyOffset = Channel[channel].KeyOffset - PitchBendRange;
	Channel[channel].KeyDetune = TotalBend & 31;
	
	for (voice=Channel[channel].Voices.start; voice!=NULL; voice=voice->next)
	{
		AL_SetVoicePitch(voice->num);
	}
}

int AL_DetectFM(void)
{
	int v1, v2;
	int i;
	
	if (USER_CheckParameter("NOAL"))
	{
		return 0;
	}
	
	AL_SendOutputToPort(AL_DEFAULT_PORT, 4, 0x60);
	AL_SendOutputToPort(AL_DEFAULT_PORT, 4, 0x80);
	
	v1 = inp(AL_DEFAULT_PORT);
	
	AL_SendOutputToPort(AL_DEFAULT_PORT, 2, 0xFF);
	AL_SendOutputToPort(AL_DEFAULT_PORT, 4, 0x21);
	
	for (i = 100; i > 0; i++)	// BUG? This *should* use 'i--'
	{
		inp(AL_DEFAULT_PORT);
	}
	// Since the loop uses 'i++' instead of 'i--', this will read the port 32668
	// times instead of just 100 times when compiled into a 16-bit DOS program.
	// This could actually make the AdLib detection code more reliable on faster
	// systems, so it's not really a bug here. It just means this code might
	// take longer than it really needs to. When compiling a protected-mode
	// program, however, 'i' will be a signed 32-bit variable and the code would
	// end up reading the port over 2 billion times, which explains why this got
	// fixed in later revisions of this sound library.
	
	v2 = inp(AL_DEFAULT_PORT);
	
	AL_SendOutputToPort(AL_DEFAULT_PORT, 4, 0x60);
	AL_SendOutputToPort(AL_DEFAULT_PORT, 4, 0x80);
	
	return ((v1 & 0xE0) == 0 && (v2 & 0xE0) == 0xC0);
}
