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

#ifndef __HP_GAME_H__
#define __HP_GAME_H__

#include "HP_DEFS.H"

enum {SPR_PLAYER=0, SPR_SCORETAGS, SPR_MORPH, SPR_TWINKS, SPR_ENEMY_0};

enum {DEMO_OFF, DEMO_RECORD, DEMO_PLAY};

typedef struct
{
	Sint16 flags[8];	//never used!
	Sint16 offsets[8];
} TEnemyTrigger;

typedef struct
{
	Sint16 sprite;
	Sint16 health;
	Sint16 projectileSpeedX;
	Sint16 projectileSpeedY;
	Sint16 projectileDeltaX;
	Sint16 projectileSeeking;
	Sint16 shootInWalkDirection;
	Sint16 canShoot;
	Sint16 _unused;
	Sint16 projectileTumbling;
	Sint16 moveCountdown;
	Sint16 type;
} TEnemyInfo;

#if sizeof(TEnemyInfo) != 24
#error TEnemyInfo size
#endif

typedef struct
{
	Sint16 requiredKey;
	Uint8 replaceTile;
	Uint8 _padding;
	Sint16 left;
	Sint16 top;
	Sint16 right;
	Sint16 bottom;
} TTrigger;

typedef struct
{
	Uint8 type, _padding_;
	Sint16 switchOffsets[4];
	Uint8 desiredTiles[4];
	Sint16 left;
	Sint16 top;
	Sint16 right;
	Sint16 bottom;
} TSwitch;

typedef struct
{
	Sint16 startOff;
	Sint16 endOff;
} TTeleporter;

typedef struct
{
	Sint16 posX;
	Sint16 posY;
	char lines[10][50];	//10 lines, 50 chars each
} TMessage;

typedef struct
{
	Uint32 offset;
	char name[22];
	Sint16 width;
	Sint16 height;
	Sint16 standFrame;	//only used for player sprite
	Sint16 standEnd;	//never used
	Sint16 walkStart;
	Sint16 walkEnd;
	Sint16 jumpStart;
	Sint16 jumpEnd;
	Sint16 shootStart;
	Sint16 shootEnd;
	Sint16 projectileWidth;
	Sint16 projectileHeight;
	Sint16 projectileDeltaY;
	Sint16 projectileStart;
	Sint16 projectileEnd;
	Uint16 layoutSize;
	Uint16 pixelsSize;
	Uint16 layoutStarts[2][20];
	Uint16 pixelsStarts[2][20];
} TSpriteInfo;

// These structures are part of the level data. If their size doesn't match the
// original size, the code won't be able to load the level data from HOCUS.DAT
// correctly.
VERIFY_SIZE(TEnemyTrigger, 32)
VERIFY_SIZE(TEnemyInfo, 24)
VERIFY_SIZE(TTrigger, 12)
VERIFY_SIZE(TSwitch, 22)
VERIFY_SIZE(TTeleporter, 4)
VERIFY_SIZE(TMessage, 504)
VERIFY_SIZE(TSpriteInfo, 220)

// Variables (initialized):

extern Sint16 levelMusicNumbers[4][9];


/*
==============================================================================
Note:

The extern declarations would affect the order of the uninitialized variables,
so I decided to omit them and move them into the C files that need to access
them.

This is potentially unsafe! Since the extern declarations and the actual
declarations are no longer part of the same file, the compiler will not be
able to detect if the extern declarations match the actual declarations or
not. But it is easier than adding extern declarations for ALL the variables
to make them appear in the correct order.
==============================================================================
*/

/*
// Variables (uninitialized):

extern Sint16 camY;
extern Sint16 oddCamX;
extern Sint16 camX;
extern Sint16 camX_t8;
extern Sint16 gotKey2;
extern Sint16 gotKey1;
extern Sint16 oldFirepower;
extern Sint16 rapidFireCountdown;
extern Sint16 levelDoneCountdown;
extern Sint16 playerDeathCountdown;
extern Sint16 hideCountdown;
extern Sint16 playerDir;
extern Sint16 playerFrame;
extern Sint16 powershotsLeft;
extern Sint16 tick5;
extern Sint16 tick1;
*/

// Functions:

void SpawnParticles(Sint16 x, Sint16 y);
void SpawnTwink(Sint16 x, Sint16 y);
void DrawSpriteClipped(Sint16 sprite, Sint16 width, Sint16 x, Sint16 y, Sint16 dir, Sint16 frame);
void DrawSpriteClipped_Red(Sint16 sprite, Sint16 width, Sint16 x, Sint16 y, Sint16 dir, Sint16 frame);
Sint16 GameLoop(Sint16 demoMode, Uint16 isNewGame);

#endif