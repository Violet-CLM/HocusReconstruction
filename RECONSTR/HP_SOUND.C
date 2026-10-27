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

#include <STDIO.H>
#include <STDLIB.H>

#include "HP_DEFS.H"
#include "HP_MAIN.H"
#include "HP_FILES.H"
#include "HP_SYS.H"
#include "HP_SOUND.H"

void PlaySound(Sint16 number)
{
	if (config.soundOn)
	{
		if (useDigitized)
		{
			FX_PlayVOC(vocSounds[number], 255, 7, soundEntries[number].priority);
			return;
		}
		// don't play the last two sounds when using the PC Speaker
		if (number >= SND_ENEMYSPAWN)
		{
			return;
		}
		
		if (!pcPlaying || soundEntries[number].priority >= pcPriority)
		{
			pcPriority = soundEntries[number].priority;
			pcSoundNumber = number;
			pcSoundIndex = 0;
			pcPlaying = true;
		}
	}
}

#pragma argsused
void PlayVocData(void *data)	//never used!
{
	FX_PlayVOC(data, 255, 7, 1);
}

void StartupSound(void)
{
	register Sint16 error;
	fx_device device;
	
#ifdef VERSION_11
	printf("\nHOCUS POCUS Version 1.1\n");
#else
	printf("\nHOCUS POCUS Version 1.0\n");
#endif
	useMusic=useDigitized=1;
	if (noAudio)
	{
		useMusic=useDigitized=0;
	}
	else
	{
		if (config.soundCard != CARD_NONE)
		{
			error = FX_SetupCard(config.soundCard, &device);
			if (error)
			{
				useDigitized = 0;
				printf("\n%s\n", FX_ErrorString(-1));
				exit(3);
			}
			error = FX_Init(config.soundCard, config.numVoices, config.mixBits);
			if (error)
			{
				useDigitized = 0;
				printf("\n%s\n", FX_ErrorString(-1));
				exit(3);
			}
			FX_SetVolume(config.soundVolume);
		}
		else
		{
			useDigitized = 0;
		}
		if (config.musicCard != CARD_NONE)
		{
			error = MUSIC_Init(config.musicCard, config.midiPort);
			if (error)
			{
				useMusic = 0;
				printf("\n%s\n", MUSIC_ErrorString(-1));
				exit(3);
			}
			MUSIC_SetVolume(config.musicVolume);
		}
		else
		{
			useMusic = 0;
			config.musicOn = 0;
		}
	}
}

void PlayVocChunk(Uint16 number)
{
	OpenDatFile();
	ReadDatChunk(number, buffer_64000);
	CloseDatFile();
	if (useDigitized && config.soundOn)
		FX_PlayVOC(buffer_64000, 255, 7, 1);
}

#pragma argsused
void PlayMusicChunk(Uint16 number, Uint16 looping)
{
	Uint32 offset, size;
	
	OpenDatFile();
	GetFatEntry(number, &offset, &size);
	if (config.musicOn && useMusic)
	{
		ReadFromDat(offset, size, musicBuffer);
		MUSIC_PlaySong(musicBuffer, looping);
	}
	CloseDatFile();
}

void StopMusic(void)
{
	MUSIC_StopSong();
}

void ShutdownSound(void)
{
	FX_Shutdown();
	MUSIC_Shutdown();
}