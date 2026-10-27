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

#include <ALLOC.H>
#include <DOS.H>
#include <STDLIB.H>
#include "COMMON.H"
#include "MULTIVOC.H"
#include "BLASTER.H"
#include "PAS16.H"
#include "SNDSRC.H"
#include "LL_MAN.H"

#define MIX_PAGES 4
#define MAX_VOICES 9

#define MIX_PAGE_SIZE 128
#define MIX_BUFFER_SIZE (MIX_PAGES * MIX_PAGE_SIZE)


typedef struct VoiceNode
{
	struct VoiceNode *next;
	struct VoiceNode *prev;
	char far *sampledata;
	unsigned length;
	unsigned position;
	unsigned oldposition[MIX_PAGES];
	unsigned oldlength[MIX_PAGES];
	int Playing[MIX_PAGES];
	int handle;
	int priority;
} VoiceNode;

typedef struct
{
	VoiceNode *start;
	VoiceNode *end;
} TVoiceList;

static int MV_Installed = 0;
static int MV_SoundCard = 0;
static int MV_MaxVoices = 1;
static int MV_BitShifts = 0;
static unsigned MV_BufferSize = MIX_PAGE_SIZE;
static unsigned MV_MixMode = 0;
static unsigned MV_Silence = 0x8080;
static void far *MV_MixBufferAlloc = NULL;
static int MV_MixPage = 0;
static int MV_PrevMixPage = 0;
static int MV_VoiceHandle = 1;
static int MV_IsPlaying = false;
static int SoundsOn = 0;	// number of sounds currently playing
static int MV_ErrorCode = 0;

static VoiceNode MixVoices[MAX_VOICES];
static unsigned MV_RequestedMixRate;
static TVoiceList VoiceList;
static TVoiceList VoicePool;
static void far *MV_MixBuffer[MIX_PAGES];
static unsigned MV_MixRate;

void Mix8bitAdd(char far *buffer, char far *samples, unsigned length);
void Mix8bitSub(char far *buffer, char far *samples, unsigned length);
void Mix16bitAdd(char far *buffer, char far *samples, unsigned length, int shifts);
void Mix16bitSub(char far *buffer, char far *samples, unsigned length, int shifts);

char * MV_ErrorString(int error)
{
	char *msg;
	
	switch (error)
	{
	case -2:
	case -1:
		msg = MV_ErrorString(MV_ErrorCode);
		break;
		
	case 0:
		msg = "Multivoc ok.";
		break;
		
	case 1:
		msg = "Selected sound card is not supported by Multivoc.";
		break;
		
	case 2:
		msg = "Multivoc not installed.";
		break;
		
	case 3:
		msg = "No free voices available to Multivoc.";
		break;
		
	case 4:
		msg = "Out of memory in Multivoc.";
		break;
		
	case 5:
		msg = "No voice with matching handle found.";
		break;
		
	case 6:
		msg = BLASTER_ErrorString(-1);
		break;
		
	case 7:
		msg = PAS_ErrorString(-1);
		break;
		
	case 8:
		msg = SS_ErrorString(-1);
		break;
		
	default:
		msg = "Unknown Multivoc error code.";
		break;
	}
	
	return msg;
}

static void MV_Mix8Bit(VoiceNode *voice, int page, int add)
{
	unsigned length;
	char far *buffer;
	char far *sampleptr;
	
	buffer = MV_MixBuffer[page];
	length = min(MV_BufferSize, voice->length);
	sampleptr = voice->sampledata + voice->oldposition[page];
	
	if (add == true)
	{
		Mix8bitAdd(buffer, sampleptr, length);
	}
	else
	{
		Mix8bitSub(buffer, sampleptr, length);
	}
	
	voice->length -= length;
	voice->position += length;
	voice->oldlength[page] = length;
}

static void MV_Mix16Bit(VoiceNode *voice, int page, int add)
{
	unsigned length;
	char far *buffer;
	char far *sampleptr;
	
	buffer = MV_MixBuffer[page];
	length = min(MV_BufferSize, voice->length);
	sampleptr = voice->sampledata + voice->oldposition[page];
	
	if (add == true)
	{
		Mix16bitAdd(buffer, sampleptr, length, MV_BitShifts);
	}
	else
	{
		Mix16bitSub(buffer, sampleptr, length, MV_BitShifts);
	}
	
	voice->position += length;
	voice->length -= length;
	voice->oldlength[page] = length;
}

static void MV_MixAdd(VoiceNode *voice, int page)
{
	if (voice->length == 0)
	{
		voice->Playing[page] = false;
		return;
	}
	
	voice->Playing[page] = true;
	voice->oldposition[page] = voice->position;
	
	switch (MV_MixMode)
	{
	case 0:
		MV_Mix8Bit(voice, page, 1);
		break;
		
	case 2:
		MV_Mix16Bit(voice, page, 1);
		break;
	}
}

static void MV_MixSub(VoiceNode *voice, int page)
{
	voice->position = voice->oldposition[page];
	voice->length = voice->oldlength[page];
	
	switch (MV_MixMode)
	{
	case 0:
		MV_Mix8Bit(voice, page, 0);
		break;
		
	case 2:
		MV_Mix16Bit(voice, page, 0);
		break;
	}
}

static void MV_MixToBuffer(int page)
{
	VoiceNode *voice;
	
	//
	// set the entire buffer to the silence sample value:
	//
	asm	push	es;
	_CX = MIX_PAGE_SIZE / 2;
	_AX = MV_Silence;
	_DI = FP_OFF(MV_MixBuffer[page]);
	_ES = FP_SEG(MV_MixBuffer[page]);
	asm	cld;
	asm	rep stosw;
	asm	pop es;
	
	//
	// add samples for all currently playing voices onto that silence value:
	//
	for (voice = VoiceList.start; voice != NULL; voice = voice->next)
	{
		MV_MixAdd(voice, page);
	}
}

static void MV_PurgeFinishedVoices(int page)
{
	VoiceNode *voice;
	VoiceNode *next;
	
	asm	pushf;
	asm	cli;
	
	for (voice = VoiceList.start; voice != NULL; voice = next)
	{
		next = voice->next;
		
		if (!voice->Playing[page])
		{
			LL_Remove(VoiceNode, &VoiceList, voice);
			LL_AddToTail(VoiceNode, &VoicePool, voice);
			SoundsOn--;
		}
	}
	
	asm	popf;
}

static void MV_ServiceVoc(void)
{
	MV_PrevMixPage = MV_MixPage;
	MV_MixPage++;
	if (MV_MixPage >= MIX_PAGES)
	{
		MV_MixPage = 0;
	}
	
	MV_MixToBuffer(MV_MixPage);
	MV_PurgeFinishedVoices(MV_MixPage);
}

static VoiceNode * MV_GetVoice(int handle)
{
	VoiceNode *voice;
	
	asm	pushf;
	asm	cli;
	
	for (voice = VoiceList.start; voice != NULL; voice = voice->next)
	{
		if (voice->handle == handle)
		{
			break;
		}
	}
	
	asm	popf;
	
	if (!voice)
	{
		MV_ErrorCode = 5;
	}
	
	return voice;
}

int MV_VoicePlaying(int handle)
{
	VoiceNode *voice;
	
	if (!MV_Installed)
	{
		MV_ErrorCode = 2;
		return false;
	}
	
	voice = MV_GetVoice(handle);
	if (!voice)
	{
		return false;
	}
	return true;
}

int MV_KillAllVoices(void)
{
	VoiceNode *voice;
	
	if (!MV_Installed)
	{
		MV_ErrorCode = 2;
		return -1;
	}
	
	switch (MV_SoundCard)
	{
	case CARD_SOUNDBLASTER:
		BLASTER_StopPlayback();
		break;
		
	case CARD_PROAUDIO:
		PAS_StopPlayback();
		break;
		
	case CARD_SOUNDSOURCE:
		SS_StopPlayback();
		break;
	}
	
	MV_IsPlaying = false;
	
	while (VoiceList.start)
	{
		voice = VoiceList.start;
		LL_Remove(VoiceNode, &VoiceList, voice);
		LL_AddToTail(VoiceNode, &VoicePool, voice);
		SoundsOn--;
	}
	
	return 0;
}

int MV_Kill(int handle)
{
	int page;
	VoiceNode *voice;
	
	if (!MV_Installed)
	{
		MV_ErrorCode = 2;
		return -1;
	}
	
	voice = MV_GetVoice(handle);
	if (!voice)
	{
		MV_ErrorCode = 5;
		return -1;
	}
	
	asm	pushf;
	asm	cli;
	
	LL_Remove(VoiceNode, &VoiceList, voice);
	
	SoundsOn--;
	
	page = MV_PrevMixPage;
	MV_MixSub(voice, page);
	page++;
	if (page >= MIX_PAGES)
	{
		page = 0;
	}
	MV_MixSub(voice, page);
	
	LL_AddToTail(VoiceNode, &VoicePool, voice);
	
	asm	popf;
	
	MV_ErrorCode = 0;
	return 0;
}

int MV_VoicesPlaying(void)
{
	int playing;
	int page;
	VoiceNode *voice;
	
	playing = 0;
	
	if (!MV_Installed)
	{
		MV_ErrorCode = 2;
		return 0;
	}
	
	asm	pushf;
	asm	cli;
	
	page = MV_PrevMixPage;
	for (voice = VoiceList.start; voice != NULL; voice = voice->next)
	{
		if (voice->Playing[page])
		{
			playing++;
		}
	}
	
	asm	popf;
	
	return playing;
}

static VoiceNode * MV_AllocVoice(int priority)
{
	VoiceNode *voice;
	
	if (SoundsOn >= MV_MaxVoices)
	{
		//
		// no voices available, try to kill a voice with lower priority
		//
		asm	pushf;
		asm	cli;
		
		for (voice = VoiceList.start; voice != NULL; voice = voice->next)
		{
			if (voice->priority <= priority)
			{
				MV_Kill(voice->handle);
				break;
			}
		}
		
		asm	popf;
		
		if (!voice)
		{
			return NULL;	// didn't find a voice to kill
		}
	}
	
	asm	pushf;
	asm	cli;
	
	// Note: The voice pool *should* never be empty here. The code uses 9 voice
	// nodes, but the Hocus Pocus setup program doesn't allow the user to select
	// more than 8 voices. This also means the 'voice' variable *should* never
	// be in an undefined state after the if-block.
	
	if (VoicePool.start)
	{
		voice = VoicePool.start;
		LL_Remove(VoiceNode, &VoicePool, voice);
	}
	
	asm	popf;
	
	if (voice)
	{
		do
		{
			MV_VoiceHandle++;
			if (MV_VoiceHandle < 1)
			{
				MV_VoiceHandle = 1;
			}
		} while (MV_VoicePlaying(MV_VoiceHandle));
		
		voice->handle = MV_VoiceHandle;
	}
	
	return voice;
}

static int MV_SetMixMode(int MixMode)
{
	if (!MV_Installed)
	{
		MV_ErrorCode = 2;
		return -1;
	}
	
	MV_KillAllVoices();
	
	switch (MV_SoundCard)
	{
	case CARD_SOUNDBLASTER:
		MV_MixMode = BLASTER_SetMixMode(MixMode);
		break;
		
	case CARD_PROAUDIO:
		MV_MixMode = PAS_SetMixMode(MixMode);
		break;
		
	case CARD_SOUNDSOURCE:
		MV_MixMode = SS_SetMixMode(MixMode);
		break;
	}
	
	switch (MV_MixMode)
	{
	case 0:
		MV_BufferSize = MIX_PAGE_SIZE;
		MV_Silence = 0x8080;
		break;
		
	case 2:
		MV_BufferSize = MIX_PAGE_SIZE/2;
		MV_Silence = 0;
		break;
	}
	
	return 0;
}

static void MV_StartPlayback(void)
{
	MV_MixPage = 0;
	MV_MixToBuffer(MV_MixPage);
	MV_PrevMixPage = MV_MixPage;
	
	switch (MV_SoundCard)
	{
	case CARD_SOUNDBLASTER:
		BLASTER_BeginBufferedPlayback(MV_MixBuffer[0], MIX_BUFFER_SIZE, MIX_PAGES, MV_RequestedMixRate, MV_MixMode, MV_ServiceVoc);
		MV_MixRate = BLASTER_GetPlaybackRate();
		break;
		
	case CARD_PROAUDIO:
		PAS_BeginBufferedPlayback(MV_MixBuffer[0], MIX_BUFFER_SIZE, MIX_PAGES, MV_RequestedMixRate, MV_MixMode, MV_ServiceVoc);
		MV_MixRate = PAS_GetPlaybackRate();
		break;
		
	case CARD_SOUNDSOURCE:
		SS_BeginBufferedPlayback(MV_MixBuffer[0], MIX_BUFFER_SIZE, MIX_PAGES, MV_ServiceVoc);
		MV_MixRate = 7000;
		break;
	}
	
	MV_MixPage++;
	if (MV_MixPage >= MIX_PAGES)
	{
		MV_MixPage = 0;
	}
	MV_MixToBuffer(MV_MixPage);
	
	MV_IsPlaying = true;
}

static void MV_PlayVoice(VoiceNode *voice)
{
	int page;
	
	asm	pushf;
	asm	cli;
	
	page = MV_PrevMixPage + 1;
	if (page >= MIX_PAGES)
	{
		page = 0;
	}
	
	MV_MixAdd(voice, page);
	LL_AddToTail(VoiceNode, &VoiceList, voice);
	SoundsOn++;
	
	asm	popf;
}

int MV_Play(char far *data, unsigned length, int priority)
{
	int status;
	int i;
	VoiceNode *voice;
	
	if (!MV_Installed)
	{
		MV_ErrorCode = 2;
		return -1;
	}
	
	voice = MV_AllocVoice(priority);
	if (!voice)
	{
		MV_ErrorCode = 3;
		return -1;
	}
	
	if (!MV_IsPlaying)
	{
		MV_StartPlayback();
	}
	
	voice->sampledata = data;
	voice->length = length;
	voice->next = NULL;
	voice->prev = NULL;
	voice->position = 0;
	for (i=0; i<MIX_PAGES; i++)
	{
		voice->oldposition[i] = 0;
		voice->oldlength[i] = 0;
		voice->Playing[i] = false;
	}
	voice->priority = priority;
	
	MV_PlayVoice(voice);
	MV_ErrorCode = 0;
	
	return voice->handle;
}

int MV_Init(int SoundCard, int MixRate, int Voices, int MixMode)
{
	char huge *ptr;
	int status;
	int i;
	int voice;
	
	if (MV_Installed)
	{
		MV_Shutdown();
	}
	
	MV_ErrorCode = 0;
	
	if (SoundCard == CARD_SOUNDSOURCE)
	{
		// allocate just enough memory (SoundSource doesn't use DMA!)
		ptr = farmalloc(MIX_BUFFER_SIZE);
	}
	else
	{
		// allocate twice as much memory as needed to avoid DMA page overflows
		// during playback (see below)
		ptr = farmalloc(MIX_BUFFER_SIZE*2);
	}
	
	if (ptr == NULL)
	{
		MV_ErrorCode = 4;
		return -1;
	}
	
	switch (SoundCard)
	{
	case CARD_SOUNDBLASTER:
		status = BLASTER_Init();
		if (status != 0)
		{
			MV_ErrorCode = 6;
		}
		break;
		
	case CARD_PROAUDIO:
		status = PAS_Init();
		if (status != 0)
		{
			MV_ErrorCode = 7;
		}
		break;
		
	case CARD_SOUNDSOURCE:
		status = SS_Init();
		if (status != 0)
		{
			MV_ErrorCode = 8;
		}
		break;
		
	default:
		MV_ErrorCode = 1;
	}
	
	if (MV_ErrorCode != 0)
	{
		// BUG: This should free 'ptr', not 'MV_MixBufferAlloc'!
		farfree(MV_MixBufferAlloc);
		MV_MixBufferAlloc = NULL;
		return -1;
	}
	
	MV_MixBufferAlloc = (void far *)ptr;
	MV_SoundCard = SoundCard;
	if (SoundCard != CARD_SOUNDSOURCE)	// SoundSource doesn't use DMA!
	{
		//
		// DMA transfers cannot cross a DMA "page" boundary, which means the low
		// 16 bits of the linear ("flat") memory address MUST NOT overflow during
		// the transfer.
		//
		// BUG: The following code will NOT be able to detect and avoid this
		// problem, because it only operates on the offset part of the pointer,
		// which *should* always be < 16 for a pointer allocated via farmalloc
		// in Borland C++ (and any other compiler for 16-bit DOS code, really).
		//
#if 1
		// the original code:
		if ((long)FP_OFF(ptr) + MIX_BUFFER_SIZE > 0x10000)
		{
			ptr += MIX_BUFFER_SIZE;
		}
#else
		// K1n9_Duk3's bugfix:
		if ((unsigned)( (FP_SEG(ptr) << 4) + FP_OFF(ptr) ) > (0x10000 - MIX_BUFFER_SIZE))
		{
			ptr += MIX_BUFFER_SIZE;
		}
#endif
	}
	
	for (i=0; i<MIX_PAGES; i++)
	{
		MV_MixBuffer[i] = (void far *)ptr;
		ptr += MIX_PAGE_SIZE;
	}
	
	MV_Installed = true;
	
	MV_RequestedMixRate = MixRate;
	MV_SetMixMode(MixMode);
	MV_MaxVoices = Voices;
	
	//
	// The 16 bit mixing converts the signed 8-bit samples into signed 16-bit
	// samples by shifting the (sign-extended) sample a few bits to the left.
	// The shifted samples of each voice are then added together. When more
	// voices are to be mixed, the number of bit shifts must be reduced to avoid
	// an integer overflow during the mixing process.
	//
	switch (Voices)
	{
	case 1:
		MV_BitShifts = 8;
		break;
		
	case 2:
		MV_BitShifts = 7;
		break;
		
	case 3:
	case 4:
		MV_BitShifts = 6;
		break;
		
	case 5:
	case 6:
	case 7:
	case 8:
		MV_BitShifts = 5;
		break;
		
	default:
		MV_BitShifts = 0;
		break;
	}
	
	VoiceList.start = NULL;
	VoiceList.end = NULL;
	VoicePool.start = NULL;
	VoicePool.end = NULL;
	
	for (voice = 0; voice < MAX_VOICES; voice++)
	{
		LL_AddToTail(VoiceNode, &VoicePool, MixVoices+voice);
	}
	
	return 0;
}

int MV_Shutdown(void)
{
	int status;
	int i;
	
	if (!MV_Installed)
	{
		MV_ErrorCode = 2;
		return -1;
	}
	
	switch (MV_SoundCard)
	{
	case CARD_SOUNDBLASTER:
		BLASTER_Shutdown();
		break;
		
	case CARD_PROAUDIO:
		PAS_Shutdown();
		break;
		
	case CARD_SOUNDSOURCE:
		SS_Shutdown();
		break;
	}
	
	VoiceList.start = NULL;
	VoiceList.end = NULL;
	VoicePool.start = NULL;
	VoicePool.end = NULL;
	
	MV_IsPlaying = false;
	MV_MaxVoices = 0;
	farfree(MV_MixBufferAlloc);
	MV_MixBufferAlloc = NULL;
	
	for (i=0; i<MIX_PAGES; i++)
	{
		MV_MixBuffer[i] = NULL;
	}
	
	MV_Installed = false;
	return 0;
}
