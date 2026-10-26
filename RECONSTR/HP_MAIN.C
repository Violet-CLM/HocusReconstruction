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

#include <DOS.H>
#include <CONIO.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <TIME.H>

#include "HP_FILES.H"
#include "HP_MENU.H"
#include "HP_INTRO.H"
#include "HP_GAME.H"
#include "HP_GRAPH.H"
#include "HP_SOUND.H"
#include "HP_SYS.H"
#include "HP_MAIN.H"

//////////////////////////
// Initialized Variables:
//////////////////////////

TFatEntry FAT[] =
{
#define FILEENTRY(name, offset, size) {offset, size},
#include "HP_FAT.H"
};

Uint16 zero = 0;	//is set but never used
Uint16 isRegisteredVersion = !SHAREWARE;
Sint16 speedTicks[3] = {8, 7, 6};

Uint8 kbSpecialKeys[NUMSPECIALKEYS] =
{
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_CTRL,
	KEY_ALT,
	KEY_CAPSLK,
	KEY_SPACE,
	KEY_ENTER,
	KEY_INS,
	KEY_DEL,
	KEY_HOME,
	KEY_END,
	KEY_PGUP,
	KEY_PGDN,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_NUM5
};

char *kbSpecialNames[NUMSPECIALKEYS] =
{
	"Left shift",
	"Right shift",
	"Ctrl",
	"Alt",
	"Caps",
	"Space",
	"Enter",
	"Insert",
	"Delete",
	"Home",
	"End",
	"Page up",
	"Page down",
	"Up arrow",
	"Down arrow",
	"Left arrow",
	"Right arrow",
	"Cursor 5"
};

Uint8 controlKeys[8] =	//index for kbSpecialKeys and kbSpecialNames
{
	CK_LEFT,
	CK_RIGHT,
	CK_CTRL,
	CK_ALT,
	CK_UP,
	CK_DOWN,
	CK_PGUP,
	CK_PGDN
};

char *errorMessages[2] =
{
	"Out of memory",
	"Cannot open file"
};

Uint8 *screenptrTXT = MK_FP(0xB800, 0);	//never used!
Uint8 *screenptrVGA = MK_FP(0xA000, 0);

TSoundEntry soundEntries[16] =
{
	{SOUND08_VOC, SOUND08_SFX, 14},
	{SOUND10_VOC, SOUND10_SFX, 13},
	{SOUND11_VOC, SOUND11_SFX, 12},
	{SOUND12_VOC, SOUND12_SFX, 12},
	{SOUND15_VOC, SOUND15_SFX, 18},
	{SOUND13_VOC, SOUND13_SFX, 17},
	{SOUND16_VOC, SOUND16_SFX, 14},
	{SOUND17_VOC, SOUND17_SFX, 16},
	{SOUND18_VOC, SOUND18_SFX, 15},
	{SOUND19_VOC, SOUND19_SFX, 15},
	{SOUND20_VOC, SOUND20_SFX, 16},
	{SOUND02_VOC, SOUND02_SFX, 19},
	{SOUND07_VOC, SOUND07_SFX, 15},
	{SOUND09_VOC, SOUND09_SFX, 14},
	{SOUND21_VOC, SOUND21_SFX, 13},
	{SOUND22_VOC, SOUND22_SFX, 11}
};

char *defaultNames[5] =	//never used!
{
	"Hocus Pocus",
	"Andre Foucault",
	"Jamie Cook",
	"Karim Sultan",
	"Chris tenDen"
};

static char _unused[84] = "";

////////////////////////////
// Uninitialized Variables:
////////////////////////////

void *pcTask;	//should be moved to HP_SYS.C and use type from Apogee Sound System
char configPath[82];
Sint32 _dummy3;
Sint16 gamespeed;
Sint16 playingDemo;
volatile Sint32 pcTickCount;
Sint32 levelTime;
Sint32 totalTime;	//never used!

Sint16 joyCountX;
Sint16 joyCountY;
Sint16 joyBtn1Down;
Sint16 joyBtn2Down;
#ifdef VERSION_11
Sint16 joyBtn3Down;
Sint16 joyBtn4Down;
#endif
Sint16 joyStickLeft;
Sint16 joyStickRight;
Sint16 joyStickUp;
Sint16 joyStickDown;
Sint16 joyFireButton;

Sint16 rand_table[1000];
Sint16 rand_index;

char strbuf[82];
unsigned char numbuf[21];

Uint8 _dummy_1;
volatile Uint8 upKeyDown;
volatile Uint8 useKeyDown;
volatile Uint8 downKeyDown;
volatile Uint8 leftKeyDown;
volatile Uint8 rightKeyDown;
volatile Uint8 fireKeyDown;
volatile Uint8 jumpKeyDown;
volatile Uint8 keyDownESC;
volatile Uint8 useKeyUp;
volatile Uint8 jumpKeyUp;
volatile Uint8 fireKeyUp;
volatile Uint8 pgupKeyDown;
volatile Uint8 pgdnKeyDown;
volatile Uint8 keyHitF2;
volatile Uint8 keyHitF3;
volatile Uint8 keyHitF10;
volatile Uint8 keyHitF1;
volatile Uint8 keyHitC;
volatile Uint8 keyHitM;
volatile Uint8 keyHitS;
volatile Uint8 keyHitV;
volatile Uint8 keyHit;
volatile Uint8 lastKey;
volatile Uint8 keyHitP;

Uint8 *spriteLayouts[10];

Sint16 episode;
Sint16 level;
Sint16 skill;

Uint8 *mapBackLayer;
Uint8 *mapMainLayer;
Uint8 *mapInsertLayer;
Sint16 *mapInfoLayer;
Uint8 *mapTriggeredLayer;

Sint32 score;
Sint32 oldscore;
Sint16 playerHealth;
Sint16 crystalsTotal;	//should be moved to HP_GAME.C
Sint16 crystalsCollected;	//should be moved to HP_GAME.C
Sint16 playerFirepower;
Sint16 _dummy_2;

TCoordinates coordinates;	//should be moved to HP_GAME.C
TTileInfo tileinfo;	//should be moved to HP_GAME.C

FILE *dat_fp;

Uint8 currPalette[0x300];
Uint8 workPalette[0x300];
Uint8 unknPalette[0x300];	//never used! 768 bytes wasted

Uint8 *drawPtr;
Uint8 *screenpagePtr[4];
Uint16 screenpageOff[4];
Uint16 currDrawPage;
Uint16 currDisplayPage;

Uint8 *fontmask;

Uint16 useMusic;
Uint16 useDigitized;
Uint16 notUsed;	//is set but never used;
Uint16 noAudio;
Uint8 *musicBuffer;
Sint16 pcPriority;
Uint8 *vocSounds[16];
Uint8 *ptrs[16];	//never used! (maybe 32 slots for pcSounds?)
Sint16 *pcSounds[16];
Sint16 pcSoundNumber;
Sint16 pcSoundIndex;
Uint16 pcPlaying;
Sint32 pcSoundLengths[16];

TImageInfo imageInfo;

TSineTableF sin_cos_table_float[360];
TSineTable sin_cos_table[360];

TGameConfig config;
Uint8 *buffer_64000;

//////////////////


//////////////
// Functions:
//////////////

void nullsub(void)
{
}

void Startup(void)
{
	register Sint16 i;
	
	srand(time(NULL));
	OpenDatFile();
	pcSoundNumber = -1;
	pcTickCount = 0;
	JoyCalibrateCentered();
	StartupSound();
	PC_Startup();
	printf(
		"\n"
		"General MIDI timbres for Yamaha OPL-2- and OPL-3-based sound cards were\n"
		"produced by The Fat Man(TM) and developed by K. Weston Phelan and George\n"
		"Alistar Sanger.  Copyright 1993, The Fat Man(TM)\n"
		"\n"
		"Press a key...\n");
	WaitMilliSecs(5000);
	GetPalette(currPalette, 0, 256);
	FadeToBlack(140);
	InitGraphics();
	
#ifdef VERSION_11
	FadeToBlack(1);
	if (DrawPCX_File("tables.dat"))
	{
		FadeFromBlack(20);
		WaitMilliSecs(5000);
		FadeToBlack(20);
		for (i=0; i<0x300; i++)
			currPalette[i] = 0;
	}
#endif

	for (i=0; i<16; i++)
	{
		if (useDigitized)
		{
			AllocBufferB(&vocSounds[i], FAT[soundEntries[i].voc].size, "3");
		}
		else
		{
			AllocBufferW(&pcSounds[i], FAT[soundEntries[i].pc].size, "3");
		}
	}
	AllocBufferB(&mapBackLayer, 14400, "M");
	AllocBufferB(&mapMainLayer, 14400, "M");
	AllocBufferB(&mapInsertLayer, 14400, "M");
	AllocBufferW(&mapInfoLayer, 28800, "M");
	AllocBufferB(&mapTriggeredLayer, 14400, "M");
	AllocBufferB(&buffer_64000, 64000, "1");
	AllocBufferB(&musicBuffer, 30000, "2");
	AllocBufferB(&fontmask, FAT[FONT_MSK].size, "4");
	for (i=0; i<10; i++)
	{
		AllocBufferB(&spriteLayouts[i], 9939, "5");
	}
	pcSoundNumber = -1;	//redundant!
	pcPlaying = false;
	for (i=0; i<16; i++)
	{
		if (useDigitized)
		{
			ReadDatChunk(soundEntries[i].voc, vocSounds[i]);
		}
		else
		{
			ReadDatChunk(soundEntries[i].pc, pcSounds[i]);
			pcSoundLengths[i] = FAT[soundEntries[i].pc].size / 2L;
		}
	}
	for (i=0; i<16; i++)
	{
		if (useDigitized)
		{
			FX_ConvertVOC(vocSounds[i]);
		}
	}
	ReadDatChunk(FONT_MSK, fontmask);
	ReadDatChunk(SINCOS_TBL, sin_cos_table_float);
	ReadDatChunk(RANDOM_TBL, rand_table);
	for (i=0; i<NUMCONTROLS; i++)
	{
		controlKeys[i] = config.controlKeys[i];
	}
	gamespeed = config.gamespeed;
	joyFireButton = config.joyFireButton;
	rand_index = 0;
	
/*
Note:

The following is an example of bad optimization. Floating point calculations
were obviously too slow to be used for the starfield animation in the menu
backgrounds. The developers switched to 32 bit integers instead ("fixed point"
with a factor of 1 million).

If they had stored the converted integer table in the DAT file instead of the
floating point table, the conversion would not need to be done each time the
game is started. The conversion wastes CPU time at startup and the float table
takes up 2880 bytes of precious conventional memory.

The MIDI code uses floating point arithmetics, too, so it would not have been
possible to compile the game without the floating point emulation code anyway,
or at least not without also updating the audio library code. Otherwise the
code size could have been reduced by about 11600 bytes.

By the way, it might have been better to use a power of 2 (e.g. 0x100000) as
the factor for the integer table. This would enable optimizations like using
shift operations instead of the rather slow 32 bit "long division". See
HP_MENU.H for details.
*/
	for (i=0; i<360; i++)
	{
		sin_cos_table[i].sin = sin_cos_table_float[i].sin * FIXED_FACTOR;
		sin_cos_table[i].cos = sin_cos_table_float[i].cos * FIXED_FACTOR;
	}
	InitStarfield();
	WaitMilliSecs(700);	//the screen is black, so why wait here?
	CloseDatFile();
}

#pragma argsused
void main (int argc, char **argv)
{
	register Sint16 i;
	Sint8 menuResult, quit=false;
	Sint16 pos;

	strcpy(configPath, "");	//useless call
	if (!getenv(CONFIG_ENV))
		strcpy(configPath, "");
	else
		strcpy(configPath, getenv(CONFIG_ENV));
	if (strlen(configPath) && configPath[strlen(configPath)-1] != '\\')
		strcat(configPath, "\\");
	strcat (configPath, CONFIG_FILE);
	if (!ReadFromFile(configPath, &config))
	{
		SetTextMode();
		printf(CONFIG_FILE" not found. Please run SETUP.\n");
		exit(0);
	}

	for (i=0,pos=-1; strlen(argv[0])>i; i++)
	{
		if (argv[0][i] == ':' || argv[0][i] == '\\')
			pos = i;
	}
	
	if (pos != -1)
	{
		strcpy(strbuf, argv[0]+pos+1);
	}
	else
	{
		strcpy(strbuf, argv[0]);
	}
	
/*
Note:

I think it would be better to use #ifdef or #if to check if Gravis Ultrasound
support is built into the executable instead of checking the file name. 

Plus, I am not sure if argv[0] is always in upper case (strcmp is case-
sensitive). It does seem to work for DOSBox, though.
*/
	if (config.usesGravis == 1 && !strcmp(strbuf, "HOCUS.EXE"))
	{
		SetTextMode();
		printf("Please run HOCUSG to play.\n");
		exit(0);
	}
	if (config.usesGravis == 0 && !strcmp(strbuf, "__HPGRVS.EXE"))
	{
		SetTextMode();
		printf("Please run HOCUS to play.\n");
		exit(0);
	}

	noAudio = notUsed = 0;
	Startup();
	IntroScreens();
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);
	while (!quit)
	{
		SetDrawPage(3);
		DrawImageChunk(BULLIT_IMG, 28, 185);
		DrawImageChunk(VOL_BAR_IMG, 60, 185);
		menuResult = MainMenu();
		switch (menuResult)
		{
		case -99:
			episode = 0;
			level = random(5) << 1;
			skill = 1;
			score = 0;
			playerFirepower = 1;
			GameLoop(DEMO_PLAY, false);
			break;
			
#if 0
		///////////////
		//HACK:
		case 8:	// Warp to a level
			if ((episode=EpisodeMenu())!=-1 && (level=LevelMenu())!=-1 && (skill=SkillMenu())!=-1)
			{
				score=0;
				goto play_loop;
			}
			break;
			
		case 9:	// Remo Recorder
			if ((level=LevelMenu())!=-1)
			{
				episode=0;
				score=0;
				skill=1;
				GameLoop(DEMO_RECORD, false);
			}
			break;
		// end of hack
		///////////////
#endif

		case 0:	// Begin a new game
			menuResult = EpisodeMenu();
			if (menuResult != -1)
			{
				if (!isRegisteredVersion && menuResult >= 1 && menuResult <= 3)
				{
					Registering();
				}
				else
				{
					episode = menuResult;
					level = 0;
					menuResult = SkillMenu();
					if (menuResult != -1)
					{
						register Sint16 gameResult;
						
						skill = menuResult;
						score = 0;
						playerFirepower = 1;
						oldscore = score;
						if (episode==0 && level==0)
						{
							gameResult = GameLoop(DEMO_OFF, true);
						}
						else
						{
							gameResult = GameLoop(DEMO_OFF, false);
						}
						goto play_loop_check;
play_loop:
						playerFirepower = 1;
						oldscore = score;
						gameResult = GameLoop(DEMO_OFF, false);
play_loop_check:
						if (gameResult == 1)
						{
							if (level == 8)
							{
								WarpScreen();
								if (episode == 0)
									Finale1();
#if !SHAREWARE
								if (episode == 1)
									Finale2();
								if (episode == 2)
									Finale3();
								if (episode == 3)
									Finale4();
#endif
								i = CheckHighscore(episode, score);
								if (i)
									EnterHighscore(episode, i-1);
							}
							else
							{
								level++;
								goto play_loop;
							}
						}
						else if (gameResult == 0)
						{
							score = oldscore;
							goto play_loop;
						}
						else if (gameResult == -1)
						{
							i = CheckHighscore(episode, score);
							if (i)
								EnterHighscore(episode, i-1);
						}
					}
				}
			}
			break;
			
		case 1:	// Restore an old game
			if (RestoreMenu())
				goto play_loop;
			break;
			
		case 5:	// Change game options
options_menu:
			if (!useMusic && !useDigitized)
				config.musicOn = 0;
			menuResult = OptionsMenu();
			switch (menuResult)
			{
			case 0:
				if (useDigitized && !config.soundOn)
				{
					config.soundOn = 1;
				}
				else if (useDigitized)
				{
					config.soundOn = 0;
				}
				else
				{
					config.soundOn = !config.soundOn;
				}
				goto options_menu;
				
			case 1:
				if (!useMusic && !useDigitized)
				{
					config.musicOn = 0;
					Show1Line("Music requires a sound card");
				}
				else
				{
					config.musicOn = !config.musicOn;
				}
				if (!config.musicOn && useMusic)
					StopMusic();
				else
					PlayMusicChunk(MUSIC03_MID, true);
				goto options_menu;
				
			case 2:
				VolumeMenu();
				goto options_menu;
				
			case 3:
				config.controlMode = !config.controlMode;
				if (config.controlMode)
					config.controlMode = CalibrateJoy();
				goto options_menu;
				
			case 4:
				menuResult = GameSpeedMenu();
				if (menuResult != -1)
				{
					gamespeed = menuResult;
					config.gamespeed = menuResult;
				}
				goto options_menu;
				
			case 5:
				KeyControlsMenu();
				for (i=0; i<NUMCONTROLS; i++)
				{
					config.controlKeys[i] = controlKeys[i];
				}
				goto options_menu;
			}	// end of "switch (menuResult)" for OptionsMenu
			
			if (!useMusic && !useDigitized)
				config.musicOn = 0;
			if (!WriteToFile(configPath, &config, sizeof(config)))
				Show1Line("I/O Error - HOCUS.SAV");
			break;
			
		case 6:	// High scores
			HighscoreMenu(0);
			break;
			
		case 3:	// Instructions
			ShowHelp(0);
			break;
			
		case 4:	// Legends and hints
			Story();
			break;
			
		case 2:	// Ordering Information
			Ordering();
			break;
			
#if SHAREWARE
		case 7:	// Preview future levels
			ShowPreviews();
			break;
		
		case 8:	// Quit - return to DOS
#else
		case 7:	// Quit - return to DOS
#endif
			quit = true;
			break;
			
		}	// end of "switch (menuResult)" for MainMenu
		
	}	// end of "while (!quit)"
	
	SetDrawPage(0);
	SetDisplayPage(0);
	
#if SHAREWARE
	srand(time(NULL));
	DrawPCX_Chunk(random(11) + PREVIEW1_PCX, 0);
	DrawImageChunk(OLD_HUD_IMG, 0, 160);
	strcpy(strbuf, "Check out the entire 4 game series!");
	CENTER(DrawText_GOB, 170, GRD_RED_HI, strbuf);
	strcpy(strbuf, "Order Hocus Pocus today!");
	CENTER(DrawText_GOB, 183, GRD_RED_HI, strbuf);
	FadeFromBlack(20);
	while (kbhit()) getch();	// flush keyboard
	WaitMilliSecs(5000);
	FadeToBlack(20);
	while (kbhit()) getch();	// flush keyboard
#endif
	
	Quit();
}