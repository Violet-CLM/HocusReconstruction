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
#include <STDLIB.H>
#include "COMMON.H"
#include "MIDI.H"
#include "TASK_MAN.H"
#include "LL_MAN.H"

#define NUM_CHANNELS 16

typedef struct TTrack
{
	struct TTrack *next;
	struct TTrack *prev;
	
	unsigned long delay;
	int active;
	int RunningStatus;
	unsigned char far *start;
	unsigned char far *pos;
} track;

typedef struct
{
	track *start;
	track *end;
} TrackList;


// initialized variables:
static int _MIDI_CommandLengths[16] = {0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 1, 1, 2, 0};
static int _MIDI_SongActive = false;
static int _MIDI_SongLoaded = false;
static int _MIDI_Loop = false;
static task *_MIDI_PlayRoutine = NULL;	// the MIDI task
static track *_MIDI_TrackPtr = NULL;
static int _MIDI_TotalVolume = 255;
static midifuncs *_MIDI_Funcs = NULL;

// uninitialized variables:
static int _ActiveTracks;	// was named '_MIDI_ActiveTracks' in the original source, but that name gave me the wrong variable order when compiling with Borland C++ 2.0
static int _MIDI_Division;
static int _MIDI_ChannelVolume[NUM_CHANNELS];
static TrackList _MIDI_Tracks;

// prototypes:
static void _MIDI_SetChannelVolume(int channel, int volume);

//
// implementation:
//

static long _MIDI_ReadNumber(unsigned char far *from, size_t size)
{
	unsigned char far *FromPtr;
	long value;
	
	if (size > 4)
	{
		size = 4;
	}
	
	FromPtr = from;
	value = 0;
	
	while (size--)
	{
		value <<= 8;
		value += *FromPtr;
		FromPtr++;
	}
	
	return value;
}

static unsigned long _MIDI_ReadDelta(track *ptr)
{
	unsigned long value;
	unsigned char c;
	
	//value = ;
	if ((value = *(ptr->pos++)) & 0x80)
	{
		value &= 0x7F;
		do
		{
			c = *(ptr->pos++);
			value = (value << 7) + (c & 0x7F);
		} while (c & 0x80);
	}

	return value;
}

static void _MIDI_ResetTracks(void)
{
	track *ptr;
	
	ptr = _MIDI_Tracks.start;
	_ActiveTracks = 0;
	
	while (ptr)
	{
		_ActiveTracks++;
		ptr->pos = ptr->start;
		ptr->delay = _MIDI_ReadDelta(ptr);
		ptr->active = true;
		ptr->RunningStatus = 0;
		ptr = ptr->next;
	}
}

static void _MIDI_ServiceRoutine(task *Task)
{
	int event;
	int command;
	int channel;
	int length;
	long tempo;
	track *Track;
	int c1, c2;
	
	if (!_MIDI_SongActive)
	{
		return;
	}
	
	Track = _MIDI_Tracks.start;
	while (Track)
	{
		while (Track->active && !Track->delay)
		{
			event = *(Track->pos++);
			if (event == 0xFF)
			{
				command = *(Track->pos++);
				length = *(Track->pos++);
				
				switch (command)
				{
				case 0x2F:
					Track->active = false;
					_ActiveTracks--;
					break;
					
				case 0x51:
					tempo = 60000000 / _MIDI_ReadNumber(Track->pos, 3);
					TS_SetTaskRate(Task, (tempo * _MIDI_Division)/60);
					break;
				}
				
				Track->pos += length;
			}
			else
			{
				if (event & 0x80)
				{
					Track->RunningStatus = event;
				}
				else
				{
					event = Track->RunningStatus;
					Track->pos--;
				}
				
				channel = event & 0xF;
				command = event >> 4;
				
				switch (command)
				{
				case 0x8:
					c1 = *(Track->pos++);
					c2 = *(Track->pos++);
					if (_MIDI_Funcs->NoteOff)
					{
						_MIDI_Funcs->NoteOff(channel, c1, c2);
					}
					break;
					
				case 0x9:
					c1 = *(Track->pos++);
					c2 = *(Track->pos++);
					if (_MIDI_Funcs->NoteOn)
					{
						_MIDI_Funcs->NoteOn(channel, c1, c2);
					}
					break;
					
				case 0xA:
					c1 = *(Track->pos++);
					c2 = *(Track->pos++);
					if (_MIDI_Funcs->PolyAftertouch)
					{
						_MIDI_Funcs->PolyAftertouch(channel, c1, c2);
					}
					break;
					
				case 0xB:
					c1 = *(Track->pos++);
					c2 = *(Track->pos++);
					if (c1 == 0x7E)
					{
						Track->pos++;
					}
					if (c1 == 7)
					{
						_MIDI_SetChannelVolume(channel, c2);
					}
					else if (_MIDI_Funcs->ControlChange)
					{
						_MIDI_Funcs->ControlChange(channel, c1, c2);
					}
					break;
					
				case 0xC:
					c1 = *(Track->pos++);
					//c2 = *(Track->pos++);
					if (_MIDI_Funcs->ProgramChange)
					{
						_MIDI_Funcs->ProgramChange(channel, c1);
					}
					break;
					
				case 0xD:
					c1 = *(Track->pos++);
					//c2 = *(Track->pos++);
					if (_MIDI_Funcs->ChannelAftertouch)
					{
						_MIDI_Funcs->ChannelAftertouch(channel, c1);
					}
					break;
					
				case 0xE:
					c1 = *(Track->pos++);
					c2 = *(Track->pos++);
					if (_MIDI_Funcs->PitchBend)
					{
						_MIDI_Funcs->PitchBend(channel, c1, c2);
					}
					break;
					
				}
			}
			
			Track->delay = _MIDI_ReadDelta(Track);
		}
		
		Track->delay--;
		Track = Track->next;
		
		if (_ActiveTracks == 0)
		{
			_MIDI_ResetTracks();
			if (_MIDI_Loop)
			{
				Track = _MIDI_Tracks.start;
				for (channel=0; channel<NUM_CHANNELS; channel++)
				{
					if (_MIDI_ChannelVolume[channel] != 90)
					{
						_MIDI_SetChannelVolume(channel, 90);
					}
				}
			}
			else
			{
				_MIDI_SongActive = false;
				Track = NULL;
				break;
			}
		}
	}
}

static int MIDI_AllNotesOff(void)
{
	int channel;
	
	if (!_MIDI_Funcs)
	{
		return -1;
	}
	
	// BUG? This doesn't check if ControlChange is valid. Both MUSIC_InitFM and
	// MUSIC_InitMidi set up a ControlChange function, so it *should* be valid.
	
	for (channel = 0; channel < NUM_CHANNELS; channel++)
	{
		_MIDI_Funcs->ControlChange(channel, 0x7B, 0);	// ALL NOTES OFF
	}
	
	return 0;
}

static void _MIDI_SetChannelVolume(int channel, int volume)
{
	_MIDI_ChannelVolume[channel] = volume;
	if (!_MIDI_Funcs->SetVolume)
	{
		volume *= _MIDI_TotalVolume;
		volume /= 255;
	}
	if (_MIDI_Funcs->ControlChange)
	{
		_MIDI_Funcs->ControlChange(channel, 7, volume);
	}
}

static void _MIDI_SendChannelVolumes(void)
{
	int channel;
	
	for (channel = 0; channel < NUM_CHANNELS; channel++)
	{
		_MIDI_SetChannelVolume(channel, _MIDI_ChannelVolume[channel]);
	}
}

static void _MIDI_Reset(void)
{
	int channel;
	
	for (channel = 0; channel < NUM_CHANNELS; channel++)
	{
		_MIDI_Funcs->ControlChange(channel, 0x79, 0);
		_MIDI_Funcs->ControlChange(channel, 0x64, 0);
		_MIDI_Funcs->ControlChange(channel, 0x65, 0);
		_MIDI_Funcs->ControlChange(channel, 0x06, 2);
		_MIDI_Funcs->ControlChange(channel, 0x26, 0);
		
		_MIDI_SetChannelVolume(channel, 90);
	}
}

static int MIDI_Reset(void)
{
	if (!_MIDI_Funcs)
	{
		return -1;
	}
	
	MIDI_AllNotesOff();
	_MIDI_Reset();
	
	return 0;
}

int MIDI_SetVolume(int volume)
{
	if (!_MIDI_Funcs)
	{
		return -254;
	}
	
	volume = min(255, volume);
	volume = max(0, volume);
	
	_MIDI_TotalVolume = volume;
	if (_MIDI_Funcs->SetVolume)
	{
		_MIDI_Funcs->SetVolume(volume);
	}
	else
	{
		_MIDI_SendChannelVolumes();
	}
	
	return 0;
}

int MIDI_GetVolume(void)
{
	int volume;
	
	if (!_MIDI_Funcs)
	{
		return -254;
	}
	
	if (_MIDI_Funcs->GetVolume)
	{
		volume = _MIDI_Funcs->GetVolume();
	}
	else
	{
		volume = _MIDI_TotalVolume;
	}
	
	return volume;
}

void MIDI_SetLoopFlag(int looping)
{
	_MIDI_Loop = looping;
}

void MIDI_ContinueSong(void)
{
	if (_MIDI_SongLoaded)
	{
		_MIDI_SongActive = true;
	}
}

void MIDI_PauseSong(void)
{
	if (_MIDI_SongLoaded)
	{
		_MIDI_SongActive = false;
		MIDI_AllNotesOff();
	}
}

int MIDI_SongPlaying(void)
{
	return _MIDI_SongActive;
}

int MIDI_SetMidiFuncs(midifuncs *funcs)
{
	// BUG? Interrupts should be disabled while changing _MIDI_Funcs. This code
	// is only run by MUSIC_InitFM and MUSIC_InitMidi in the original game, so
	// the MIDI service *should* not be running when this is executed, but this
	// is a public routine that could be called by any other part of the code.
	_MIDI_Funcs = funcs;
	
	return 0;
}

void MIDI_StopSong(void)
{
	if (_MIDI_SongLoaded)
	{
		TS_Terminate(_MIDI_PlayRoutine);
		_MIDI_SongActive = false;
		_MIDI_SongLoaded = false;
		MIDI_Reset();
		if (_MIDI_Funcs->ReleasePatches)
		{
			_MIDI_Funcs->ReleasePatches();
		}
		
		_MIDI_Tracks.start = NULL;
		_MIDI_Tracks.end = NULL;
		free(_MIDI_TrackPtr);
		_MIDI_TrackPtr = NULL;
	}
}

int MIDI_PlaySong(unsigned char far *song, int looping)
{
	int format;
	int numtracks;
	unsigned long headersize;
	unsigned long tracklength;
	track *CurrentTrack;
	unsigned char far *ptr;
	
	if (_MIDI_SongLoaded)
	{
		MIDI_StopSong();
	}
	
	if (!_MIDI_Funcs)
	{
		return -254;
	}
	
	if ( *((unsigned long far *)song) != 0x6468544D )	// "MThd"
	{
		return -253;
	}
	song += 4;
	
	headersize = _MIDI_ReadNumber(song, 4);
	song += 4;
	format = _MIDI_ReadNumber(song, 2);
	numtracks = _MIDI_ReadNumber(song+2, 2);
	_MIDI_Division = _MIDI_ReadNumber(song+4, 2);
	if (format > 1)	// BUG: This is a signed comparison, but anything < 0 isn't a valid format
	{
		return -252;
	}
	ptr = song + headersize;
	_MIDI_Tracks.start = NULL;
	_MIDI_Tracks.end = NULL;
	
	if (numtracks == 0)
	{
		return -251;
	}
	
	_MIDI_TrackPtr = malloc(numtracks * sizeof(track));
	if (!_MIDI_TrackPtr)
	{
		return -249;
	}
	
	for (CurrentTrack = _MIDI_TrackPtr; numtracks-- != 0; CurrentTrack++)
	{
		if ( *((unsigned long far *)ptr) != 0x6B72544D )	// "MTrk"
		{
			free(_MIDI_TrackPtr);
			_MIDI_TrackPtr = NULL;
			return -250;
		}
		
		tracklength = _MIDI_ReadNumber(ptr+4, 4);
		ptr += 8;
		CurrentTrack->start = ptr;
		ptr += tracklength;
		
		LL_AddToTail(track, &_MIDI_Tracks, CurrentTrack);
	}
	
	if (_MIDI_Funcs->GetVolume)
	{
		_MIDI_TotalVolume = _MIDI_Funcs->GetVolume();
	}
	
	_MIDI_ResetTracks();
	if (_MIDI_Funcs->LoadPatch)
	{
		MIDI_LoadTimbres();
	}
	MIDI_Reset();
	_MIDI_Loop = looping;
	
	_MIDI_PlayRoutine = TS_ScheduleTask(_MIDI_ServiceRoutine, (_MIDI_Division*120)/60, 1, NULL);
	TS_Dispatch();
	
	_MIDI_SongActive = true;
	_MIDI_SongLoaded = true;
	
	return 0;
}

void MIDI_LoadTimbres(void)
{
	int event;
	int command;
	int channel;
	int length;
	int Finished;
	track *Track;
	
	// Note: This is a public routine, but it doesn't check if _MIDI_Funcs and
	// _MIDI_Funcs->LoadPatch are valid (not NULL). The caller is responsible
	// for checking that *before* calling this routine.
	
	for (Track=_MIDI_Tracks.start; Track!=NULL; Track=Track->next)
	{
		Finished = false;
		while (!Finished)
		{
			event = *(Track->pos++);
			if (event == 0xFF)
			{
				command = *(Track->pos++);
				length = *(Track->pos++);
				if (command == 0x2F)
				{
					Finished = true;
				}
				
				Track->pos += length;	// skip the event ...
				_MIDI_ReadDelta(Track);	// ... and the following delay
			}
			else
			{
				if (event & 0x80)
				{
					Track->RunningStatus = event;
				}
				else
				{
					event = Track->RunningStatus;
					Track->pos--;
				}
				
				channel = event & 0xF;
				command = event >> 4;
				length = _MIDI_CommandLengths[command];
				
				if (command == 0xB && *(Track->pos) == 0x7E)
				{
					length++;
				}
				if (channel == 9)
				{
					if (command == 0x9)
					{
						_MIDI_Funcs->LoadPatch(*(Track->pos)+0x80);
					}
				}
				else if (command == 0xC)
				{
					_MIDI_Funcs->LoadPatch(*(Track->pos));
				}
				
				Track->pos += length;	// skip the event ...
				_MIDI_ReadDelta(Track);	// ... and the following delay
			}
		}
	}
	_MIDI_ResetTracks();
}
