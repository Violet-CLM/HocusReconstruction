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

#ifndef __HP_MAIN_H__
#define __HP_MAIN_H__

#include <STDIO.H>	//for FILE struct
#include "HP_DEFS.H"
#include "ASS.H"

typedef struct {
	Uint32 offset;
	Sint32 size;
} TFatEntry;

typedef struct {
	Uint16 voc;
	Uint16 pc;
	Sint16 priority;
} TSoundEntry;

typedef struct {
	Sint32 sin;
	Sint32 cos;
} TSineTable;

typedef struct {
	float sin;
	float cos;
} TSineTableF;

typedef struct {
	Sint16 width;
	Sint16 height;
} TImageInfo;

#pragma option -a-
typedef struct {
	Uint8 first;
	Uint8 last;
	Uint8 type;
} TTileAnimation;

typedef struct {
	Uint8 backtile;
	Uint8 switchDownTile;
	Uint8 switchUpTile;
	Uint8 shootableTile;
	TTileAnimation anim[240];
} TTileInfo;
#pragma option -a

typedef struct {
	Sint16 _zero;	//never used
	Sint16 playerX;
	Sint16 playerY;
	Sint16 randomness;
} TCoordinates;

VERIFY_SIZE(TFatEntry, 8)
VERIFY_SIZE(TSoundEntry, 6)
VERIFY_SIZE(TSineTable, 8)
VERIFY_SIZE(TSineTableF, 8)
VERIFY_SIZE(TImageInfo, 4)
VERIFY_SIZE(TTileAnimation, 3)
VERIFY_SIZE(TTileInfo, 724)
VERIFY_SIZE(TCoordinates, 8)

// Variables:

extern TFatEntry FAT[];
extern Uint16 zero;
extern Uint16 isRegisteredVersion;
extern Sint16 speedTicks[3];
extern Uint8 kbSpecialKeys[18];
extern char *kbSpecialNames[18];
extern Uint8 controlKeys[8];
extern char *errorMessages[2];
extern Uint8 *screenptrVGA;
extern TSoundEntry soundEntries[16];

extern void *pcTask;	//should be moved to HP_SYS.C and use type from Apogee Sound System
extern char configPath[82];
extern Sint32 _dummy3;
extern Sint16 gamespeed;
extern Sint16 playingDemo;
extern volatile Sint32 pcTickCount;
extern Sint32 levelTime;
extern Sint32 totalTime;	//never used!
extern Sint16 joyCountX;
extern Sint16 joyCountY;
extern Sint16 joyBtn1Down;
extern Sint16 joyBtn2Down;
extern Sint16 joyBtn3Down;
extern Sint16 joyBtn4Down;
extern Sint16 joyStickLeft;
extern Sint16 joyStickRight;
extern Sint16 joyStickUp;
extern Sint16 joyStickDown;
extern Sint16 joyFireButton;
extern Sint16 rand_table[1000];
extern Sint16 rand_index;
extern char strbuf[82];
extern unsigned char numbuf[21];

extern Uint8 _dummy_1;
extern volatile Uint8 upKeyDown;
extern volatile Uint8 useKeyDown;
extern volatile Uint8 downKeyDown;
extern volatile Uint8 leftKeyDown;
extern volatile Uint8 rightKeyDown;
extern volatile Uint8 fireKeyDown;
extern volatile Uint8 jumpKeyDown;
extern volatile Uint8 keyDownESC;
extern volatile Uint8 useKeyUp;
extern volatile Uint8 jumpKeyUp;
extern volatile Uint8 fireKeyUp;
extern volatile Uint8 pgupKeyDown;
extern volatile Uint8 pgdnKeyDown;
extern volatile Uint8 keyHitF2;
extern volatile Uint8 keyHitF3;
extern volatile Uint8 keyHitF10;
extern volatile Uint8 keyHitF1;
extern volatile Uint8 keyHitC;
extern volatile Uint8 keyHitM;
extern volatile Uint8 keyHitS;
extern volatile Uint8 keyHitV;
extern volatile Uint8 keyHit;
extern volatile Uint8 lastKey;
extern volatile Uint8 keyHitP;
extern Uint8 *spriteLayouts[10];
extern Sint16 episode;
extern Sint16 level;
extern Sint16 skill;
extern Uint8 *mapBackLayer;
extern Uint8 *mapMainLayer;
extern Uint8 *mapInsertLayer;
extern Sint16 *mapInfoLayer;
extern Uint8 *mapTriggeredLayer;
extern Sint32 score;
extern Sint32 oldscore;
extern Sint16 playerHealth;
extern Sint16 crystalsTotal;	//should be moved to HP_GAME.C
extern Sint16 crystalsCollected;	//should be moved to HP_GAME.C
extern Sint16 playerFirepower;
extern Sint16 _dummy_2;

extern TCoordinates coordinates;	//should be moved to HP_GAME.C
extern TTileInfo tileinfo;	//should be moved to HP_GAME.C
extern FILE *dat_fp;

extern Uint8 currPalette[0x300];
extern Uint8 workPalette[0x300];
extern Uint8 unknPalette[0x300];	//never used! 768 bytes wasted
extern Uint8 *drawPtr;
extern Uint8 *screenpagePtr[4];
extern Uint16 screenpageOff[4];
extern Uint16 currDrawPage;
extern Uint16 currDisplayPage;
extern Uint8 *fontmask;

extern Uint16 useMusic;
extern Uint16 useDigitized;
extern Uint16 notUsed;	//is set but never used;
extern Uint16 noAudio;
extern Uint8 *musicBuffer;

extern Sint16 pcPriority;
extern Uint8 *vocSounds[16];
extern Uint8 *ptrs[16];	//never used! (maybe 32 slots for pcSounds?)
extern Sint16 *pcSounds[16];
extern Sint16 pcSoundNumber;
extern Sint16 pcSoundIndex;
extern Uint16 pcPlaying;
extern Sint32 pcSoundLengths[16];

extern TImageInfo imageInfo;
extern TSineTableF sin_cos_table_float[360];
extern TSineTable sin_cos_table[360];
extern TGameConfig config;

extern Uint8 *buffer_64000;


#endif