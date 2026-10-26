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

#ifndef __HP_DEFS_H__
#define __HP_DEFS_H__

#include "VERSION.H"

#ifndef VERSION
#define VERSION 1.0
#endif

#if VERSION == 1.1
#define VERSION_11
#else
#undef VERSION_11	// just in case...
#endif

#ifndef SHAREWARE
#define SHAREWARE 0
#endif

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

// Note: I would usually write #define'd constants in all upper case, but
// Notepad++ will display 'true' and 'false' as keywords when they are written
// in lower case and I like that, so I broke my own rule here.
#ifndef true
#define true (0==0)
#define false (!true)
#endif

//SDL-style integer types - just to make future SDL ports easier
typedef unsigned short Uint16;
typedef signed short Sint16;
typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned long Uint32;
typedef signed long Sint32;

enum {
	KEY_NULL = 0,
	KEY_ESCAPE = 1,
	KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_ASSIGN, KEY_BACKSPACE,
	KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LBRACK, KEY_RBRACK, KEY_ENTER,
	KEY_CTRL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_COLON, KEY_QUOTE, KEY_TILDE,
	KEY_LSHIFT, KEY_BACKSLASH, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RSHIFT,
	KEY_NUMMUL, KEY_ALT, KEY_SPACE, KEY_CAPSLK,
	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_NUMLK, KEY_SCRLK,
	KEY_HOME, KEY_UP, KEY_PGUP, KEY_NUMSUB, KEY_LEFT, KEY_NUM5, KEY_RIGHT, KEY_NUMADD, KEY_END, KEY_DOWN, KEY_PGDN, KEY_INS, KEY_DEL,
	KEY_F11 = 87, KEY_F12,
	KEY_FINALKEY
};

#define CHAR_BACKSPACE 8
#define CHAR_ENTER 13
#define CHAR_ESCAPE 27

//////////////////////////////////////////////
// SHARED STUFF (used by Game and Setup tool)
//////////////////////////////////////////////

#define CONFIG_ENV	"APOGEECD"
#define CONFIG_FILE	"HOCUS.SAV"

enum
{
	CK_LSHIFT,
	CK_RSHIFT,
	CK_CTRL,
	CK_ALT,
	CK_CAPSLK,
	CK_SPACE,
	CK_ENTER,
	CK_INS,
	CK_DEL,
	CK_HOME,
	CK_END,
	CK_PGUP,
	CK_PGDN,
	CK_UP,
	CK_DOWN,
	CK_LEFT,
	CK_RIGHT,
	CK_NUM5,
	
	NUMSPECIALKEYS
};

enum
{
	C_LEFT,
	C_RIGHT,
	C_JUMP,
	C_FIRE,
	C_UP,
	C_DOWN,
	C_SCROLLUP,
	C_SCROLLDOWN,
	
	NUMCONTROLS
};

#define NUMSAVEGAMES 9
#define NUMEPISODES 4
#define NUMHIGHSCORES 5
#define MAXNAMELENGTH 25

typedef struct {
	Uint16 soundOn, musicOn;
	Uint16 controlMode;
	Uint16 gamespeed;
	Uint16 joyLoX, joyMiX, joyHiX;
	Uint16 joyLoY, joyMiY, joyHiY;
	Uint16 joyFireButton;
	Uint8 controlKeys[NUMCONTROLS];
	//saved games:
	Sint16 sg_episodes[NUMSAVEGAMES];
	Sint16 sg_levels[NUMSAVEGAMES];
	Sint16 sg_skill[NUMSAVEGAMES];
	char sg_names[NUMSAVEGAMES][MAXNAMELENGTH+1];	//9 entries, 26 chars each (25 chars + terminating 0)
	Uint8 unused[10];	//only used by setup (first 9 bytes set to 1)
	Sint32 sg_scores[NUMSAVEGAMES];
	//high scores:
	char hs_names[NUMEPISODES][NUMHIGHSCORES][MAXNAMELENGTH+1];	//4*5 entries, 26 chars each (25 chars + terminating 0)
	Sint32 hs_scores[NUMEPISODES][NUMHIGHSCORES];
	//sound setup:
	Sint16 soundCard;
	Uint16 numVoices;
	Uint16 mixBits;
	Sint16 musicCard;
	Uint16 midiPort;
	//settings for setup tool:
	Sint16 soundCardIndex;
	Sint16 numVoicesIndex;
	Sint16 mixingQualityIndex;
	Sint16 musicCardIndex;
	Sint16 midiPortIndex;
	//more sound settings:
	Sint16 soundVolume;
	Sint16 musicVolume;
	Uint16 usesGravis;
} TGameConfig;

#define VERIFY_EXPRESSION(expr, name) typedef char name[(expr)? 1 : -1];
#define VERIFY_SIZE(type, size) VERIFY_EXPRESSION(sizeof(type) == size, type##SizeTest)

VERIFY_SIZE(TGameConfig, 990)

#endif