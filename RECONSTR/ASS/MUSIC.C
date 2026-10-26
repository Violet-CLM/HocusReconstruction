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
#include "MIDI.H"
#include "MUSIC.H"
#include "AL_MIDI.H"
#include "BLASTER.H"
#include "PAS16.H"
#include "MPU401.H"

static unsigned MUSIC_SoundDevice = CARD_ADLIB;
static int MUSIC_ErrorCode = 0;

static midifuncs MUSIC_MidiFunctions;

int MUSIC_InitFM(int SoundCard, midifuncs *midi);
int MUSIC_InitMidi(int SoundCard, midifuncs *midi, int Port);


char * MUSIC_ErrorString(int error)
{
	char *msg;
	
	switch (error)
	{
	case -1:
	case -2:
		msg = MUSIC_ErrorString(MUSIC_ErrorCode);
		break;
		
	case 0:
		msg = "Music ok.";
		break;
		
	case 1:
		switch (MUSIC_SoundDevice)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			msg = "Music sound card error.";
			break;
			
		default:
			msg = "Invalid Music device error.";
			break;
		}
		break;
		
	case 2:
		msg = "Invalid Music device.";
		break;
		
	case 3:
		msg = "Error playing MIDI file.";
		break;
		
	case 4:
		msg = "TaskMan error.";
		break;
		
	case 5:
		msg = "Could not detect FM chip.";
		break;
		
	default:
		msg = "Unknown Music error code.";
		break;
		
	}
	
	return msg;
}

int MUSIC_Init(int SoundCard, int MidiPort)
{
	int status;
	
	status = 0;
	MUSIC_SoundDevice = SoundCard;
	
	switch (SoundCard)
	{
	case 0:
	case 1:
	case 2:
		status = MUSIC_InitFM(SoundCard, &MUSIC_MidiFunctions);
		break;
	
	case 3:
	case 4:
		status = MUSIC_InitMidi(SoundCard, &MUSIC_MidiFunctions, MidiPort);
		break;
		
	default:
		MUSIC_ErrorCode = 2;
		status = -1;
	}
	
	return status;
}

int MUSIC_Shutdown(void)
{
	int status;
	
	status = 0;
	
	MIDI_StopSong();
	
	switch (MUSIC_SoundDevice)
	{
	case 2:
		AL_Shutdown();
		break;
		
	case 0:
		AL_Shutdown();
		// no break here
	case 4:
		BLASTER_RestoreMidiVolume();
		break;
		
	case 1:
		AL_Shutdown();
		PAS_RestoreMusicVolume();
		break;
	}
	
	return status;
}

int MUSIC_SetVolume(int volume)
{
	volume = max(0, volume);
	volume = min(volume, 255);
	
	return MIDI_SetVolume(volume);
}

int MUSIC_GetVolume(void)
{
	return MIDI_GetVolume();
}

void MUSIC_SetLoopFlag(int loop)
{
	MIDI_SetLoopFlag(loop);
}

int MUSIC_SongPlaying(void)
{
	return MIDI_SongPlaying();
}

void MUSIC_Continue(void)
{
	MIDI_ContinueSong();
}

void MUSIC_Pause(void)
{
	MIDI_PauseSong();
}

int MUSIC_StopSong(void)
{
	MIDI_StopSong();
	MUSIC_ErrorCode = 0;
	return 0;
}

int MUSIC_PlaySong(char far *MusicBuffer, int looping)
{
	int status;
	
	switch (MUSIC_SoundDevice)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		MIDI_StopSong();
		status = MIDI_PlaySong(MusicBuffer, looping);
		if (status != 0)
		{
			MUSIC_ErrorCode = 3;
			return -2;
		}
		break;
		
	default:
		MUSIC_ErrorCode = 2;
		return -2;
	}
	
	return 0;
}

int MUSIC_InitFM(int SoundCard, midifuncs *midi)
{
	int status;
	
	status = 0;
	
	if (!AL_DetectFM())
	{
		MUSIC_ErrorCode = 5;
		return -1;
	}
	
	AL_Init(SoundCard);
	
	midi->NoteOff = AL_NoteOff;
	midi->NoteOn = AL_NoteOn;
	midi->PolyAftertouch = NULL;
	midi->ControlChange = AL_ControlChange;
	midi->ProgramChange = AL_ProgramChange;
	midi->ChannelAftertouch = NULL;
	midi->PitchBend = AL_SetPitchBend;
	midi->ReleasePatches = NULL;
	midi->LoadPatch = NULL;
	
	switch (SoundCard)
	{
	case CARD_SOUNDBLASTER:
		if (BLASTER_CardHasMixer())
		{
			BLASTER_SaveMidiVolume();
			midi->SetVolume = BLASTER_SetMidiVolume;
			midi->GetVolume = BLASTER_GetMidiVolume;
		}
		else
		{
			midi->SetVolume = NULL;
			midi->GetVolume = NULL;
		}
		break;
		
	case CARD_ADLIB:
		midi->SetVolume = NULL;
		midi->GetVolume = NULL;
		break;
		
	case CARD_PROAUDIO:
		if (PAS_SaveMusicVolume() == 0)
		{
			midi->SetVolume = PAS_SetFMVolume;
			midi->GetVolume = PAS_GetFMVolume;
		}
		break;
	}
	
	if (MIDI_SetMidiFuncs(midi) != 0)
	{
		MUSIC_ErrorCode = 1;
		status = -1;
	}
	
	return status;
}

int MUSIC_InitMidi(int SoundCard, midifuncs *midi, int Port)
{
	int status;
	
	status = 0;
	
	if (MPU_Init(Port) != 0)
	{
		MUSIC_ErrorCode = 1;
		return -1;
	}
	
	midi->NoteOff = MPU_NoteOff;
	midi->NoteOn = MPU_NoteOn;
	midi->PolyAftertouch = MPU_PolyAftertouch;
	midi->ControlChange = MPU_ControlChange;
	midi->ProgramChange = MPU_ProgramChange;
	midi->ChannelAftertouch = MPU_ChannelAftertouch;
	midi->PitchBend = MPU_PitchBend;
	midi->ReleasePatches = NULL;
	midi->LoadPatch = NULL;
	midi->SetVolume = NULL;
	midi->GetVolume = NULL;
	
	if (SoundCard == CARD_WAVEBLASTER && BLASTER_CardHasMixer())
	{
		BLASTER_SaveMidiVolume();
		midi->SetVolume = BLASTER_SetMidiVolume;
		midi->GetVolume = BLASTER_GetMidiVolume;
	}
	
	if (MIDI_SetMidiFuncs(midi) != 0)
	{
		MUSIC_ErrorCode = 1;
		status = -1;
	}
	
	return status;
}
