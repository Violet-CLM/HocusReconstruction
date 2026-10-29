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

#include <CONIO.H>
#include <MEM.H>
#include <STDIO.H>
#include <STDLIB.H>

#include "HP_MAIN.H"
#include "HP_LEVEL.H"
#include "HP_GRAPH.H"
#include "HP_FILES.H"
#include "HP_MENU.H"
#include "HP_GAME.H"
#include "HP_DRAW.H"
#include "HP_SOUND.H"
#include "HP_SYS.H"

//////////////////////////
// Local Constants:
//////////////////////////

#define MAX_SNOW 20
#define MAX_ACTORS 8
#define MAX_SHOTS 10
#define MAX_ENEMYSHOTS 8
#define MAX_TWINKS 8
#define MAX_PARTICLES 8
#define PIXELS_PER_PARTICLE 16
#define MAX_SCORETAGS 10
#define MAX_SPRITES 10
#define MAX_SPAWNQUEUE 32

//////////////////////////
// Initialized Variables:
//////////////////////////

Sint16 snowIndex = 0;

Sint16 levelMusicNumbers[4][9] =
{
	{3, 3, 4, 4, 0, 0, 2, 2, 2},
	{9, 9, 7, 7, 8, 8, 1, 1, 1},
	{4, 4, 0, 0, 7, 7, 6, 6, 6},
	{2, 2, 8, 8, 9, 9, 5, 5, 5}
};

Sint16 levelElevatorTiles[4][10][2] =
{
	{ {-1,-1}, {-1,-1}, { -1, -1}, { -1, -1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1} },
	{ {-1,-1}, {-1,-1}, { -1, -1}, { -1, -1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1} },
	{ {78,79}, {78,79}, { 62, 63}, { 62, 63}, {40,41}, {40,41}, {85,86}, {85,86}, {85,86}, {-1,-1} },
	{ {58,59}, {58,59}, {122,123}, {122,123}, {70,71}, {70,71}, {60,61}, {60,61}, {60,61}, {-1,-1} }
};

Sint16 boss4Xs[4] = {128, 128, 113, 128};
Sint16 boss4Ys[4] = { 54,  48,  42,  36};
Sint16 boss4TileOffs[4] = {0x3320, 0x2D80, 0x27D1, 0x2240};	//= x + y*240
Sint16 boss4Dirs[4] = {1, 1, 0, 1};
Sint16 boss4Health[4] = {600, 400, 200, -1};

Sint16 playerDeltaYsHi[25] =
{
	-16,-16,-16,-16, -8, -8, -4, -4, -4, -2, -1, -1,  0,
	  1,  1,  2,  4,  4,  4,  8,  8, 16, 16, 16, 16
};
Sint16 playerDeltaYs[19] =
{
	-16, -8, -8, -4, -4, -4, -2, -1, -1,  0,
	  1,  1,  2,  4,  4,  4,  8,  8, 16
};
Sint16 damage[4] = {-4, -12, -16, 0};

////////////////////////////
// Uninitialized Variables:
////////////////////////////

Sint16 hudLevelNumFlashCounter;
Uint32 nextTickCount;
FILE *_unknown_fp;

Sint16 snowX[MAX_SNOW];
Sint16 snowY[MAX_SNOW];
Sint16 snowState[MAX_SNOW];

Sint16 upKeyCounter;
Sint16 downKeyCounter;
Sint16 oldPlayerX;
Sint16 oldPlayerY;
Sint16 wizardActivated;
Sint16 elevatorTile1;
Sint16 elevatorTile2;
Uint8 playerFiring;
Uint8 playerAimingUp;

Sint16 boss4Stage;

struct {
	Uint8 up;
	Uint8 left;
	Uint8 right;
	Uint8 fire;
	Uint8 jump;
	Uint8 down;	//never used, but struct must be 6 bytes!
} demoKeys;
Sint16 demoIndex;
Sint16 demoSize;

Sint16 levelIndex;
Sint16 camWarp;
Sint16 lookDeltaY;
Sint16 zero_0;
Sint16 playerX;
Sint16 playerTileX;
Sint16 playerTileY;
Sint16 playerX_pix;
Sint16 playerScreenX;
Sint16 playerY;
Sint16 playerScreenY;
Sint16 playerCanMoveW;
Sint16 playerCanMoveE;
Sint16 playerCanMoveNW;
Sint16 playerCanMoveNE;
Sint16 playerWalking;
Sint16 *jumpDeltaYs;
Sint16 playerJumpIndex;
Sint16 maxJumpIndex;
Sint16 maxDeltaYIndex;
Sint16 superJump;
Sint16 superJumpCounter;
Sint16 playerJumping;
Sint16 camScrolled;
Sint16 playerFalling;
Sint16 teleportCounter;
Sint16 teleportEndY;
Sint16 teleportEndX;

Sint16 shotX[MAX_SHOTS];
Sint16 shotY[MAX_SHOTS];
Sint16 shotFlyUp[MAX_SHOTS];
Sint16 shotDir[MAX_SHOTS];
Sint16 shotIndex;
Sint16 playerShotsActive;
Sint16 shotType[MAX_SHOTS];

Sint16 tick1;
Sint16 tick5;
Sint16 powershotsLeft;
Sint16 playerFrame;
Sint16 playerDir;
Sint16 hideCountdown;
Sint16 playerDeathCountdown;
Sint16 levelDoneCountdown;
Sint16 rapidFireCountdown;
Sint16 oldFirepower;
Sint16 gotKey1;
Sint16 gotKey2;
Sint16 camX_t8;
Sint16 camX;
Sint16 oddCamX;
Sint16 camY;
Sint16 levelFlashRedCountdown;
Sint16 tileCountdown;
Sint16 hurtCountdown;
Uint16 pixOffWhite;
Uint16 pixOffRed;
Sint16 numEnemiesTotal;
Sint16 numEnemiesKilled;
Sint16 treasureFound;
Sint16 treasureTotal;

struct
{
	Sint16 x, y, frame, state, dir;
} scoretags[MAX_SCORETAGS];

Sint16 twinkIndex;
struct
{
	Sint16 x, y, state, dir;
} twinks[MAX_TWINKS];

Sint16 particleIndex;
struct
{
	Sint16 x, y;
} particlePos[MAX_PARTICLES][PIXELS_PER_PARTICLE];

#pragma option -a-	// word alignment OFF
struct
{
	Sint8 vx, vy;
	Uint8 color;
} particleInfo[MAX_PARTICLES][PIXELS_PER_PARTICLE];
#pragma option -a	// word alignment ON

// Note: The fact that word alignment needs to be turned OFF for the struct
// above might indicate that the original code declared this as a 3-dimensional
// array of Sint8 values instead of a struct. But I find it easier to work
// with named structure members.


Sint8 particleState[MAX_PARTICLES];

TSpriteInfo spriteinfo[MAX_SPRITES];
Uint16 spriteOffsets[MAX_SPRITES];
Uint16 pixOffLineardata;
Uint16 pixOffSpritestart;
Sint16 spriteNumbers[MAX_SPRITES];

Sint16 spawnTileindex[MAX_SPAWNQUEUE];

Sint16 actorIsShooting[MAX_ACTORS];
Sint16 actorShootsInWalkDirection[MAX_ACTORS];
Sint16 actorCanShoot[MAX_ACTORS];
Sint16 actorSeeking[MAX_ACTORS];
Sint16 actor_unused[MAX_ACTORS];
Sint16 actorShotDeltaX[MAX_ACTORS];
Sint16 actorMoveCountdown[MAX_ACTORS];
Sint16 actorX[MAX_ACTORS];
Sint16 actorY[MAX_ACTORS];
Sint16 actorVX[MAX_ACTORS];
Sint16 actorVY[MAX_ACTORS];
Sint16 actorNewDirCountdown[MAX_ACTORS];
Sint16 actorWidth[MAX_ACTORS];
Sint16 actorHeight[MAX_ACTORS];
Sint16 actorDir[MAX_ACTORS];
Sint16 actorFrame[MAX_ACTORS];
Sint16 actorHealth[MAX_ACTORS];
Sint16 actorFlashing[MAX_ACTORS];
Sint16 actorTileindex[MAX_ACTORS];
Sint16 actorSprite[MAX_ACTORS];
Sint16 actorSettingsIndex[MAX_ACTORS];
Sint16 actorType[MAX_ACTORS];
Sint16 actorOne[MAX_ACTORS];
Sint16 actorJumpCountdown[MAX_ACTORS];
Sint16 actorJumpSpeed[MAX_ACTORS];
Sint16 actorCountdown[MAX_ACTORS];
Sint16 actorSpawnCountdown[MAX_ACTORS];
Sint16 actorMaxHealth[MAX_ACTORS];
Sint16 actorShootCountdown[MAX_ACTORS];

Sint16 enemyshotDelay[MAX_ENEMYSHOTS];
Sint16 enemyshotWobble[MAX_ENEMYSHOTS];
Sint16 enemyshotFrame[MAX_ENEMYSHOTS];
Sint16 enemyshotMaxFrame[MAX_ENEMYSHOTS];
Sint16 enemyshotState[MAX_ENEMYSHOTS];
Sint16 enemyshotX[MAX_ENEMYSHOTS];
Sint16 enemyshotY[MAX_ENEMYSHOTS];
Sint16 enemyshotWidth[MAX_ENEMYSHOTS];
Sint16 enemyshotHeight[MAX_ENEMYSHOTS];
Sint16 enemyshotDir[MAX_ENEMYSHOTS];
Sint16 enemyshotSprite[MAX_ENEMYSHOTS];
Sint16 enemyshotSpeedX[MAX_ENEMYSHOTS];
Sint16 enemyshotSpeedY[MAX_ENEMYSHOTS];
Sint16 enemyshotSeeking[MAX_ENEMYSHOTS];
Sint16 enemyshotZero[MAX_ENEMYSHOTS];

/*
=============================================================================

 !!!!!!! WARNING: Do NOT modify the lengths of the following arrays !!!!!!!

 These arrays are part of the level data and get filled with the contents of
 the respective chunks from HOCUS.DAT while the game is running. Increasing
 the lengths of these arrays would be pointless because the game won't use
 the extra space anyway (unless you modify the level file formats and write
 your own level editor for this game). Reducing the lengthts of these arrays
 will most likely lead to memory corruption when the game tries to load the
 chunks from HOCUS.DAT into these arrays.
 
=============================================================================
*/
TMessage messages[10];
static char _dummy8[8];
TTeleporter teleporters[10];
TSwitch switches[23];
TTrigger inserts[25];
TTrigger keyholes[25];
TEnemyInfo enemyinfos[10];
TEnemyTrigger enemytriggers[250];

static char _dummy9[24];	// never used

/////////////////////

//////////////
// Functions:
//////////////

void CheckEnemyShotHitsPlayer(void)
{
	register Sint16 i;
	Sint16 maxX, minX, maxY, minY;
	
	maxX = playerX_pix + 3;
	minX = playerX_pix + 2;
	maxY = playerY + 25;
	minY = playerY + 6;
	for (i=0; i<MAX_ENEMYSHOTS; i++)
	{
		if (enemyshotState[i] != -1)
		{
			if (!enemyshotDelay[i] && !hurtCountdown && !hideCountdown)
			{
				if (enemyshotX[i] <= maxX && enemyshotX[i]+enemyshotWidth[i]-1 >= minX
				&& enemyshotY[i] <= maxY && enemyshotY[i]+enemyshotHeight[i]-1 >= minY)
				{
					playerHealth += damage[skill];
					hurtCountdown = 20;
					PlaySound(SND_OUCH);
					enemyshotState[i] = -1;
				}
			}
		}
	}
}

void CheckEnemyHitsPlayer(void)
{
	register Sint16 i;
	Sint16 maxX, minX, maxY, minY;
	
	maxX = playerX_pix + 3;
	minX = playerX_pix + 2;
	maxY = playerY + 25;
	minY = playerY + 6;
	for (i=0; i<MAX_ACTORS; i++)
	{
		if (actorType[i] != -1 && !hideCountdown && !actorSpawnCountdown[i])
		{
			if (actorX[i] <= maxX && actorX[i]+actorWidth[i]-1 >= minX
			&& actorY[i] <= maxY && actorY[i]+actorHeight[i]-1 >= minY)
			{
				if (actorMaxHealth[i] > 20 && skill != 3)
				{
					playerHealth = 0;	//bosses kill player on touch
				}
				else if (!hurtCountdown)
				{
					playerHealth += damage[skill];
					hurtCountdown = 20;
					PlaySound(SND_OUCH);
				}
			}
		}
	}
}

void KillActiveEnemies(void)
{
	register Sint16 i;
	Sint16 baseX, baseY, pixY, pixX;
	
	for (i=0; i<MAX_ACTORS; i++)
	{
		if (actorType[i] != -1)
		{
			baseX = actorX[i] - (camX_t8 << 1);
			baseY = actorY[i] - (camY << 4);
			pixX = (baseX + (actorWidth[i]/2)) << 2;
			pixY = baseY + (actorHeight[i]/2);
			SpawnParticles(pixX, pixY);
			PlaySound(SND_EXPLODE);
			actorType[i] = -1;
			mapInfoLayer[actorTileindex[i]] = 30000;	//make sure the enemy cannot respawn
			numEnemiesKilled++;
		}
	}
}

void CheckEnemyHitsPlayerShot(void)
{
	register Sint16 i, n;
	Sint16 minX, maxX, sx, sy, minY, maxY, pixY, pixX;
	
	for (i=0; i<MAX_ACTORS; i++)
	{
		if (actorType[i] != -1 && !actorSpawnCountdown[i])
		{
			minX = actorX[i] - (camX_t8 << 1);
			maxX = minX + actorWidth[i] -1;
			minY = actorY[i] - (camY << 4);
			maxY = minY + actorHeight[i] -1;
			pixX = (minX + (actorWidth[i]/2)) << 2;
			pixY = minY + (actorHeight[i]/2);
			for (n=0; n<MAX_SHOTS; n++)
			{
				sx = shotX[n];
				sy = shotY[n]+2;
				if (sx != -99 && sx >= minX && sx <= maxX && sy >= minY && sy <= maxY)
				{
					SpawnParticles(pixX, pixY);
					if (!shotType[n])
					{
						shotX[n] = -99;
						playerShotsActive--;
					}
					if (actorHealth[i] == -2)	//is it a smart bomb?
					{
						actorHealth[i] = -1;
						KillActiveEnemies();
					}
					else if (actorHealth[i] != -1)
					{
						if (actorHealth[i] && !shotType[n])
						{
							actorHealth[i]--;
							actorFlashing[i] = 10;
						}
						else
						{
							PlaySound(SND_EXPLODE);
							SpawnParticles(pixX, pixY);
							SpawnParticles(pixX, pixY);
							SpawnParticles(pixX, pixY);
							actorType[i] = -1;
							mapInfoLayer[actorTileindex[i]] = 30000;	//make sure the enemy cannot respawn
							numEnemiesKilled++;
						}
					}
				}
			}
		}
	}
}

void RemoveHiddenActors(void)
{
	register Sint16 i, hidden;
	
	for (i=0; i<MAX_ACTORS; i++)
	{
		if (actorType[i] != -1)
		{
			hidden = 0;
			if (actorX[i] < (camX_t8 << 1) - 50)
			{
				hidden = 1;
				goto check;
			}
			if (actorX[i] > (camX_t8 << 1) + 120)
			{
				hidden = 1;
				goto check;
			}
			if (actorY[i] < (camY << 4) - 120)
			{
				hidden = 1;
				goto check;
			}
			if (actorY[i] > (camY << 4) + 510)
			{
				hidden = 1;
			}
check:
			if (hidden && actorMaxHealth[i] < 500)	//do not remove boss 4
			{
				actorType[i] = -1;
				actorTileindex[i] = -1;
			}
		}
	}
}

Sint16 SpawnActor(Sint16 num, Sint16 x, Sint16 y, Sint16 tileindex)
{
	register Sint16 i;
	Sint16 nearPlayer, spritenum;
	
	for (i=0; i<MAX_ACTORS; i++)
	{
		if (actorType[i] == -1)
			break;
	}
	if (i == MAX_ACTORS)
	{
		return 0;
	}
	spritenum = SPR_ENEMY_0 + num;
	actorShootCountdown[i] = 0;
	actorSpawnCountdown[i] = 20;
	actorIsShooting[i] = 0;
	actorShotDeltaX[i] = enemyinfos[num].projectileDeltaX;
	actorSprite[i] = spritenum;
	actorSettingsIndex[i] = num;
	actorType[i] = enemyinfos[num].type;
	actorShootsInWalkDirection[i] = enemyinfos[num].shootInWalkDirection;
	actorCanShoot[i] = enemyinfos[num].canShoot;
	actor_unused[i] = enemyinfos[num]._unused;
	actorSeeking[i] = enemyinfos[num].projectileSeeking;
	actorMoveCountdown[i] = enemyinfos[num].moveCountdown;
	actorX[i] = x;
	actorY[i] = y;
	actorHeight[i] = spriteinfo[spritenum].height;
	actorHealth[i] = enemyinfos[num].health >= 0? enemyinfos[num].health+(skill<<1) : enemyinfos[num].health;
	actorMaxHealth[i] = actorHealth[i];
	actorWidth[i] = spriteinfo[spritenum].width;
	if (x > playerX_pix)
	{
		actorDir[i] = 1;
	}
	else
	{
		actorDir[i] = 0;
	}
	if (actorType[i] == 2)
	{
		actorDir[i] = 0;
	}
	actorFrame[i] = 0;
	actorFlashing[i] = 0;
	actorTileindex[i] = tileindex;
	actorOne[i] = 1;	//always 1
	actorJumpCountdown[i] = 1;
	actorCountdown[i] = 0;
	actorJumpSpeed[i] = -99;
	if (abs(x-playerX_pix) < 50 && abs(y-playerY) < 100)
	{
		nearPlayer = 1;
	}
	else
	{
		nearPlayer = 0;
	}
	if (pcTickCount > nextTickCount && nearPlayer)
	{
		PlaySound(SND_ENEMYSPAWN);
		nextTickCount = pcTickCount+5;
	}
	switch (actorType[i])
	{
	case 8:	//boss 3 (dragon)
		actorVX[i] = x;
		actorMoveCountdown[i] = Random(10)+10;
		break;
		
	case 2:	//cases 2 and 3 are identical!
		do {
			actorVX[i] = Random(2) - Random(2);	//result is -1, 0 or 1
			actorVY[i] = (Random(2) - Random(2)) << 2;	//result is -4, 0 or 4
		} while (actorVX[i]==0 && actorVY[i]==0);
		actorNewDirCountdown[i] = Random(20);
		break;
		
	case 3:
		do {
			actorVX[i] = Random(2) - Random(2);	//result is -1, 0 or 1
			actorVY[i] = (Random(2) - Random(2)) << 2;	//result is -4, 0 or 4
		} while (actorVX[i]==0 && actorVY[i]==0);
		actorNewDirCountdown[i] = Random(20);
		break;
		
	case 6:
		do {
			actorVX[i] = Random(2) - Random(2);	//result is -1, 0 or 1
			actorVY[i] = (Random(2) - Random(2)) << 2;	//result is -4, 0 or 4
		} while (actorVX[i]==0);	//the condition differs from cases 2 and 3
		actorNewDirCountdown[i] = Random(20);
		break;
		
	case 0:	//cases 0, 7 and 1 are identical!
		if (actorDir[i]==0)
		{
			actorVX[i] = 1;
		}
		else
		{
			actorVX[i] = -1;
		}
		actorVY[i] = 0;
		break;
		
	case 7:
		if (actorDir[i]==0)
		{
			actorVX[i] = 1;
		}
		else
		{
			actorVX[i] = -1;
		}
		actorVY[i] = 0;
		break;
		
	case 1:
		if (actorDir[i]==0)
		{
			actorVX[i] = 1;
		}
		else
		{
			actorVX[i] = -1;
		}
		actorVY[i] = 0;
		break;
		
	case 4:	//boss 1 (mad monk)
		actorVX[i] = 0;
		actorVY[i] = 0;
		actorDir[i] = 0;
		actorFrame[i] = 0;
		PlaySound(SND_LAUGH);
		break;
		
	case 5:	//flower flinger
		actorVX[i] = 0;
		actorVY[i] = 0;
		actorFrame[i] = 0;
		break;
	}
	return 1;
}

Sint16 SpawnEnemyShot(Sint16 x, Sint16 y, Sint16 adjustDir, Sint16 delay, Sint16 enemynum, Sint16 spritenum)
{
	register Sint16 i;
	Sint16 deltaX;
	
	for (i=0; i<MAX_ENEMYSHOTS; i++)
	{
		if (enemyshotState[i] == -1)
			break;
	}
	if (i==MAX_ENEMYSHOTS)
		return 0;
	enemyshotDelay[i] = delay;
	enemyshotDir[i] = 0;
	deltaX = 0;
	enemyshotWobble[i] = enemyinfos[enemynum].projectileTumbling;
	if (adjustDir)
	{
		if (x <= playerX_pix)
		{
			deltaX = spriteinfo[spritenum].width;
			enemyshotDir[i] = 0;
		}
		else
		{
			deltaX = 0;
			enemyshotDir[i] = 1;
		}
	}
	enemyshotWidth[i] = spriteinfo[spritenum].projectileWidth;
	enemyshotHeight[i] = spriteinfo[spritenum].projectileHeight+3;
	enemyshotFrame[i] = spriteinfo[spritenum].projectileStart;
	enemyshotMaxFrame[i] = spriteinfo[spritenum].projectileEnd;
	enemyshotState[i] = 30000;
	PlaySound(SND_ENEMYSHOT);
	enemyshotX[i] = x + deltaX;
	enemyshotY[i] = y + spriteinfo[spritenum].projectileDeltaY;
	enemyshotSprite[i] = spritenum;
	enemyshotSpeedX[i] = enemyinfos[enemynum].projectileSpeedX;
	enemyshotSpeedY[i] = enemyinfos[enemynum].projectileSpeedY;
	enemyshotSeeking[i] = enemyinfos[enemynum].projectileSeeking;
	enemyshotZero[i] = 0;	//always 0, never used!
	return 1;
}

void UpdateEnemyShots(void)
{
	register Sint16 i;
	Sint16 blocked, x, y;
	
	for (i=0; i<MAX_ENEMYSHOTS; i++)
	{
		if (enemyshotState[i] == -1)
			continue;
		if (!enemyshotDelay[i])
		{
			x = enemyshotX[i] - (camX_t8 << 1);
			y = enemyshotY[i] - (camY << 4);
			blocked = 0;
			if (x < -10 || x > 80 || y < -20 || y > 160)
			{
				blocked = 1;
			}
			if (*(mapMainLayer+(enemyshotY[i]/16)*240 + (enemyshotX[i]/4)) != 255)
			{
				blocked = 2;
			}
			if (blocked)
			{
				enemyshotState[i] = -1;
				if (blocked == 2)
				{
					SpawnParticles((x<<2)+8, y+8);
				}
			}
			else
			{
				DrawSpriteClipped(enemyshotSprite[i], spriteinfo[enemyshotSprite[i]].width, x, y, enemyshotDir[i], enemyshotFrame[i]);
				if (enemyshotFrame[i] < enemyshotMaxFrame[i])
				{
					enemyshotFrame[i]++;
				}
				if (enemyshotDir[i] == 0)
				{
					enemyshotX[i] += enemyshotSpeedX[i];
				}
				else
				{
					enemyshotX[i] -= enemyshotSpeedX[i];
				}
				if (enemyshotSeeking[i])
				{
					if (enemyshotX[i] < playerX_pix)
					{
						enemyshotX[i]++;
					}
					else if (enemyshotX[i] > playerX_pix)
					{
						enemyshotX[i]--;
					}
				}
				enemyshotY[i] += enemyshotSpeedY[i];
				if (enemyshotWobble[i])
				{
					enemyshotY[i] += Random(2)-Random(2);
				}
			}
		}
		else
		{
			enemyshotDelay[i]--;
		}
	}
}

void DrawBossHealthBar(Sint16 actornum)
{
	register Sint16 i = actornum;
	Sint16 width, color;
	
	SetWriteMode(0);
	SetPlaneMask(0xF);
	width = ((((float)actorHealth[i])/((float)actorMaxHealth[i])) * 78.0);	//((actorHealth[i]*78L)/actorMaxHealth[i]) would work, too
	for (i=0; i<width; i++)
	{
		color = 0x6F - (i/10);
		*(drawPtr+i+0x2F81) = color;
		*(drawPtr+i+0x2FD1) = color;
		*(drawPtr+i+0x3021) = color;
	}
}

void UpdateBoss3(Sint16 actornum)
{
	register Sint16 i=actornum;
	Sint16 x, y, spritenum;
	
	spritenum = actorSprite[i];
	DrawBossHealthBar(i);
	if (actorSpawnCountdown[i])
	{
		x = actorX[i] - (camX_t8 << 1) + (actorWidth[i]/2);
		y = actorY[i] - (camY << 4) + (actorHeight[i]/2);
		if (Random(5)==0)
		{
			SpawnTwink((x + Random(3)) - Random(3), (y + Random(12)) - Random(12));
		}
		actorSpawnCountdown[i]--;
	}
	else
	{
		x = actorX[i] - (camX_t8 << 1);
		y = actorY[i] - (camY << 4);
		{
			register Sint16 frame;
			
			if (actorFlashing[i] % 2)
			{
				if (actorFrame[i] < 2)
				{
					actorDir[i] = 0;
					frame = actorFrame[i];
				}
				else
				{
					actorDir[i] = 1;
					frame = actorFrame[i]-2;
				}
				DrawSpriteClipped_Red(spritenum, spriteinfo[spritenum].width, x, y, actorDir[i], frame);
			}
			else
			{
				if (actorFrame[i] < 2)
				{
					actorDir[i] = 0;
					frame = actorFrame[i];
				}
				else
				{
					actorDir[i] = 1;
					frame = actorFrame[i]-2;
				}
				DrawSpriteClipped(spritenum, spriteinfo[spritenum].width, x, y, actorDir[i], frame);
			}
		}
		if (actorFlashing[i])
		{
			actorFlashing[i]--;
		}
		if (actorFrame[i] == 1 && Random(10)==0 && actorX[i] > playerX_pix)
		{
			SpawnEnemyShot(actorX[i], actorY[i], 1, 0, actorSettingsIndex[i], spritenum);
			PlaySound(SND_DRAGONFIRE);
		}
		if (actorMoveCountdown[i]==0)
		{
			actorMoveCountdown[i] = Random(10)+10;
			if (actorX[i] < actorVX[i])
			{
				register Sint16 dx = actorVX[i] - actorX[i];
				
				if (dx >= 5)
				{
					actorX[i]++;
					actorFrame[i]--;
				}
				else if (Random(2)==0)
				{
					actorX[i]++;
					actorFrame[i]--;
				}
				else
				{
					actorX[i]--;
					actorFrame[i]++;
				}
			}
			else
			{
				register Sint16 dx = actorX[i] - actorVX[i];
				
				if (dx >= 5)
				{
					actorX[i]--;
					actorFrame[i]++;
				}
				else if (Random(2)==0)
				{
					actorX[i]--;
					actorFrame[i]++;
				}
				else
				{
					actorX[i]++;
					actorFrame[i]--;
				}
			}
		}
		if (actorFrame[i] > 3)
			actorFrame[i] = 0;
		if (actorFrame[i] < 0)
			actorFrame[i] = 3;
		actorMoveCountdown[i]--;
	}
}

void UpdateBoss4(void)
{
	register Sint16 spritenum=actorSprite[0], x;
	Sint16 y;
	
	DrawBossHealthBar(0);
	if (actorSpawnCountdown[0])
	{
		x = actorX[0] - (camX_t8 << 1) + (actorWidth[0]/2);
		y = actorY[0] - (camY << 4) + (actorHeight[0]/2);
		if (Random(5)==0)
		{
			SpawnTwink((x + Random(3)) - Random(3), (y + Random(12)) - Random(12));
		}
		actorSpawnCountdown[0]--;
	}
	else
	{
		if (actorX[0] > playerX_pix)
		{
			actorDir[0] = 1;
		}
		else
		{
			actorDir[0] = 0;
		}
		x = actorX[0] - (camX_t8 << 1);
		y = actorY[0] - (camY << 4);
		if (actorFlashing[0] % 2)
		{
			DrawSpriteClipped_Red(spritenum, spriteinfo[spritenum].width, x, y, actorDir[0], actorFrame[0]);
		}
		else
		{
			DrawSpriteClipped(spritenum, spriteinfo[spritenum].width, x, y, actorDir[0], actorFrame[0]);
		}
		if (actorFlashing[0])
		{
			actorFlashing[0]--;
		}
		if (!actorSeeking[0])
		{
			if (actorShotDeltaX[0] == 0)
			{
				actorFrame[0] = 1;
				actorShotDeltaX[0] = 1;
				actorSeeking[0] = 4;
			}
			else if (actorShotDeltaX[0] == 1)
			{
				actorShotDeltaX[0] = 2;
				actorSeeking[0] = 1;
				SpawnEnemyShot(actorX[0], actorY[0], 1, 0, actorSettingsIndex[0], spritenum);
			}
			else if (actorShotDeltaX[0] == 2)
			{
				actorFrame[0] = 0;
				actorShotDeltaX[0] = 0;
				actorSeeking[0] = Random(45)+5;
			}
		}
		else
		{
			actorSeeking[0]--;
		}
		if (boss4Health[boss4Stage] > actorHealth[0] && boss4Stage < 4)
		{
			if (actorShotDeltaX[0] < 3)
			{
				actorShotDeltaX[0] = 3;
				actorSeeking[0] = 20;
				actorFrame[0] = 2;
				PlaySound(SND_TELEPORT);
			}
			else if (actorSeeking[0] == 0)
			{
				boss4Stage++;
				actorType[0] = -1;
				SpawnActor(0, boss4Xs[boss4Stage]<<2, boss4Ys[boss4Stage]<<4, boss4TileOffs[boss4Stage]);
				actorHealth[0] = boss4Health[boss4Stage-1];
				actorDir[0] = boss4Dirs[boss4Stage];
				actorVX[0] = 0;
				actorSeeking[0] = Random(50)+5;
				actorShotDeltaX[0] = 0;
			}
		}
	}
}

void UpdateActors(void)
{
	register Sint16 i, ti;
	Sint16 blocked, ix, iy, oldX, oldY, tileindex, x, y;
	Uint8 tw, th, spritenum;
	
	for (i=0; i<MAX_ACTORS; i++)
	{
		if (actorType[i] != -1)
		{
			if (actorType[i] == 8)
			{
				UpdateBoss3(i);
			}
			else if (actorType[i] == 99)
			{
				UpdateBoss4();
			}
			else
			{
				if (actorMaxHealth[i] > 20)
				{
					DrawBossHealthBar(i);
				}
				if (!actorSpawnCountdown[i])
				{
					spritenum = actorSprite[i];
					if (!actorShootCountdown[i])
					{
						if (!actorJumpCountdown[i])
						{
							oldX = actorX[i];
							oldY = actorY[i];
							if (actorJumpSpeed[i] != -99)
							{
								actorY[i] += actorJumpSpeed[i];
								actorJumpSpeed[i]++;
								if (actorJumpSpeed[i] == 7)
								{
									actorJumpSpeed[i] = -99;
									actorFrame[i] = 0;
								}
							}
							else
							{
								actorX[i] += actorVX[i];
								actorY[i] += actorVY[i];
								if (actorJumpSpeed[i] == -99)	//condition is ALWAYS true here
								{
									tileindex = ((actorY[i]/16)*240 + actorX[i]/4);
									if (actorDir[i] == 0)
									{
										tileindex++;
									}
									ti = tileindex;
									tw = actorWidth[i]/4;
									if (actorX[i] % 4)
									{
										tw++;
									}
									th = actorHeight[i]/16;
									if (actorY[i] % 16)
									{
										th++;
									}
									blocked = 0;
									
									for (iy=0; iy<th; iy++)
									{
										ti = (actorY[i]/16 + iy)*240 + actorX[i]/4;	//redundant for iy==0
										for (ix=0; ix<tw; ix++)
										{
											if (mapMainLayer[ti] != 255)
												blocked = 1;
											ti++;
										}
										// Note: It would be faster to do "ti += 240-tw;" here instead of the re-initialization before the x-loop.
									}
								}
							}
							
							switch (actorType[i])
							{
							case 2:
								if (actorNewDirCountdown[i] <= 0)
								{
									blocked = 1;
								}
								if (blocked)
								{
									actorX[i] = oldX;
									actorY[i] = oldY;
									actorNewDirCountdown[i] = Random(30)+5;
									do {
										if (!actorSeeking[i])
										{
											actorVX[i] = Random(2) - Random(2);
										}
										else
										{
											actorVX[i] = (actorX[i] > playerX_pix)? -Random(2) : Random(2);
										}
										actorVY[i] = (Random(2)-Random(2)) << 2;
									} while (!actorVX[i] && !actorVY[i]);
								}
								actorNewDirCountdown[i]--;
								break;
								
							case 3:
								if (actorNewDirCountdown[i] <= 0)
								{
									blocked = 1;
								}
								if (blocked)
								{
									actorX[i] = oldX;
									actorY[i] = oldY;
									actorNewDirCountdown[i] = Random(30)+5;
									do {
										actorVX[i] = Random(2)-Random(2);
										actorVY[i] = (Random(2)-Random(2)) << 2;
									} while (!actorVX[i] && !actorVY[i]);
									
									if (actorX[i] > playerX_pix)
									{
										actorDir[i] = 1;
									}
									else
									{
										actorDir[i] = 0;
									}
								}
								actorNewDirCountdown[i]--;
								break;
								
							case 6:
								if (actorNewDirCountdown[i] <= 0)
								{
									blocked = 1;
								}
								if (actorX[i] > playerX_pix)
								{
									actorDir[i] = 1;
								}
								else
								{
									actorDir[i] = 0;
								}
								if (blocked)
								{
									actorX[i] = oldX;
									actorY[i] = oldY;
									actorNewDirCountdown[i] = Random(30)+5;
									do {
										actorVX[i] = Random(2)-Random(2);
										actorVY[i] = (Random(2)-Random(2)) << 2;
										actorVX[i] *= 2;
									} while (!actorVX[i]);
								}
								actorNewDirCountdown[i]--;
								break;
								
							case 0:
								ti = tileindex + th*240;
								if (actorDir[i] == 0)
								{
									ti += tw-2;
								}
								if (mapMainLayer[ti] == 255)
								{
									blocked = 1;
								}
								if (blocked)
								{
									actorX[i] = oldX;
									actorY[i] = oldY;
									actorDir[i] = !actorDir[i];
									if (!actorDir[i])
									{
										actorVX[i] = 1;
									}
									else
									{
										actorVX[i] = -1;
									}
								}
								if ( ( (actorX[i] <= playerX_pix && !actorDir[i]) || (actorX[i] > playerX_pix && actorDir[i]) ) && actorCanShoot[i] && !Random(coordinates.randomness) )
								{
									if (spriteinfo[spritenum].shootStart != -1)
									{
										actorFrame[i] = spriteinfo[spritenum].shootStart;
									}
									actorShootCountdown[i] = 9;
								}
								else if (!Random(20))
								{
									if (actorX[i] > playerX_pix)
									{
										actorDir[i] = 1;
									}
									else
									{
										actorDir[i] = 0;
									}
									if (!actorDir[i])
									{
										actorVX[i] = 1;
									}
									else
									{
										actorVX[i] = -1;
									}
								}
								break;
								
							case 7:	//never used! -- this is buggy!
								if (actorJumpSpeed[i] != -99)
								{
									ti = tileindex + th*240;
									if (!actorDir[i])
									{
										ti += tw-2;
									}
									if (mapMainLayer[ti] == 255)
									{
										blocked = 1;
									}
									if (blocked)
									{
										actorX[i] = oldX;
										actorY[i] = oldY;
										actorDir[i] = !actorDir[i];
										if (!actorDir[i])
										{
											actorVX[i] = 1;
										}
										else
										{
											actorVX[i] = -1;
										}
									}
									if (!Random(20))
									{
										if (actorX[i] > playerX_pix)
										{
											actorDir[i] = 1;
										}
										else
										{
											actorDir[i] = 0;
										}
										if (!actorDir[i])
										{
											actorVX[i] = 1;
										}
										else
										{
											actorVX[i] = -1;
										}
									}
								}
								else
								{
									for (i=0; i<MAX_SHOTS; i++)	//BUG: index variable i used for shots AND actor!
									{
										if (shotX[i] != -99)
										{
											if (abs(shotX[i]-actorX[i] < 10))	//BUG: abs() is used on a boolean value (0 or 1)!
											{
												actorJumpSpeed[i] = -6;
											}
										}
									}
								}
								break;
								
							case 1:
								ti = tileindex + 240*th;
								if (!actorDir[i])
								{
									ti += tw-2;
								}
								if (mapMainLayer[ti] == 255)
								{
									blocked = 1;
								}
								if (blocked)
								{
									actorX[i] = oldX;
									actorY[i] = oldY;
									actorDir[i] = !actorDir[i];
									if (!actorDir[i])
									{
										actorVX[i] = 1;
									}
									else
									{
										actorVX[i] = -1;
									}
								}
								if (!Random(20))
								{
									if (actorX[i] > playerX_pix)
									{
										actorDir[i] = 1;
										actorCountdown[i] = Random(15)+10;
										if (spriteinfo[spritenum].shootStart != -1)
										{
											actorFrame[i] = spriteinfo[spritenum].shootStart;
										}
									}
									else
									{
										actorDir[i] = 0;
										actorCountdown[i] = Random(15)+10;
										if (spriteinfo[spritenum].shootStart != -1)
										{
											actorFrame[i] = spriteinfo[spritenum].shootStart;
										}
									}
									if (!actorDir[i])
									{
										actorVX[i] = 1;
									}
									else
									{
										actorVX[i] = -1;
									}
								}
								break;
								
							case 5:
								if (actorX[i] > playerX_pix)
								{
									actorDir[i] = 1;
								}
								else
								{
									actorDir[i] = 0;
								}
								break;
								
								//Note: case 4 has no code here
								
							}	/* end of 'switch (actorType[i])' */
							
							actorFrame[i]++;
							if (actorCountdown[i])
							{
								if (spriteinfo[spritenum].shootStart != -1)
								{
									if (spriteinfo[spritenum].shootEnd < actorFrame[i])
									{
										actorFrame[i] = spriteinfo[spritenum].shootStart;
									}
								}
								else
								{
									if (spriteinfo[spritenum].walkEnd < actorFrame[i])
									{
										actorFrame[i] = spriteinfo[spritenum].walkStart;
									}
								}
							}
							else if (actorIsShooting[i])
							{
								if (spriteinfo[spritenum].shootEnd < actorFrame[i])
								{
									actorIsShooting[i] = 0;
									actorFrame[i] = spriteinfo[spritenum].walkStart;
								}
							}
							else
							{
								if (spriteinfo[spritenum].walkEnd < actorFrame[i])
								{
									actorFrame[i] = spriteinfo[spritenum].walkStart;
								}
							}
							if (!actorCountdown[i])
							{
								actorJumpCountdown[i] = actorOne[i];	//set to 1
							}
							else
							{
								actorJumpCountdown[i] = 0;
								actorCountdown[i]--;
							}
						}	/* end of 'if (!actorJumpCountdown[i])' */
						else
						{
							actorJumpCountdown[i]--;
						}
					}	/* end of 'if (!actorShootCountdown[i])' */
					else
					{
						actorShootCountdown[i]--;
					}
					
					if ((actorCanShoot[i] && Random(coordinates.randomness)==0 && actorType[i]) || actorShootCountdown[i] == 1)
					{
						ix = (spriteinfo[spritenum].shootEnd - spriteinfo[spritenum].shootEnd)+1;	//result is always 1
						iy = 0;
						actorIsShooting[i] = 1;
						if (enemyinfos[actorSettingsIndex[i]].projectileSpeedX)
						{
							if (actorShootsInWalkDirection[i])
							{
								if (actorType[i] == 5)	//is "flower flinger"?
								{
									ix = spriteinfo[spritenum].shootEnd - spriteinfo[spritenum].shootStart;
								}
								if (actorX[i] <= playerX_pix && !actorDir[i])
								{
									iy = SpawnEnemyShot(actorX[i], actorY[i], 1, ix, actorSettingsIndex[i], spritenum);
								}
								else if (actorX[i] > playerX_pix && actorDir[i])
								{
									iy = SpawnEnemyShot(actorX[i], actorY[i], 1, ix, actorSettingsIndex[i], spritenum);
								}
							}
							else
							{
								if (actorX[i] <= playerX_pix)
								{
									iy = SpawnEnemyShot(actorX[i], actorY[i], 1, ix, actorSettingsIndex[i], spritenum);
								}
								else
								{
									iy = SpawnEnemyShot(actorX[i], actorY[i], 1, ix, actorSettingsIndex[i], spritenum);
								}
							}
						}
						else
						{
							iy = SpawnEnemyShot(actorX[i]+actorShotDeltaX[i], actorY[i]+actorHeight[i], 0, ix, actorSettingsIndex[i], spritenum);
						}
						if (!iy)
						{
							actorIsShooting[i] = 0;
						}
						if (spriteinfo[spritenum].shootStart != -1)
						{
							actorFrame[i] = spriteinfo[spritenum].shootStart;
						}
					}
					//1E13:
					if (actorJumpSpeed[i] != -99)
					{
						actorFrame[i] = spriteinfo[spritenum].jumpStart;
					}
					x = actorX[i] - (camX_t8 << 1);
					y = actorY[i] - (camY << 4);
					if (actorFlashing[i] % 2)
					{
						DrawSpriteClipped_Red(spritenum, spriteinfo[spritenum].width, x, y, actorDir[i], actorFrame[i]);
					}
					else
					{
						DrawSpriteClipped(spritenum, spriteinfo[spritenum].width, x, y, actorDir[i], actorFrame[i]);
					}
					if (actorFlashing[i])
					{
						actorFlashing[i]--;
					}
					
				}	/* end of 'if (!actorSpawnCountdown[i])' */
				
				if (actorSpawnCountdown[i])
				{
					x = actorX[i] - (camX_t8 << 1) + (actorWidth[i]/2);
					y = actorY[i] - (camY << 4) + (actorHeight[i]/2);
					if (Random(5)==0)
					{
						SpawnTwink((x + Random(3)) - Random(3), (y + Random(12)) - Random(12));
					}
					actorSpawnCountdown[i]--;
				}
			}
		}
		
	}	/* end of 'for (i=0; i<MAX_ACTORS; i++)' */
}

void UpdateTwinks(void)
{
	register Sint16 i, state;
	Sint16 x, y, dir;
	
	SetWriteMode(0);
	for (i=0; i<MAX_TWINKS; i++)
	{
		x = twinks[i].x;
		y = twinks[i].y;
		state = twinks[i].state;
		dir = twinks[i].dir;
		if (!state)
		{
			continue;
		}
		if (state == 9 || state == 1)
		{
			DrawSpriteClipped(SPR_TWINKS, spriteinfo[SPR_TWINKS].width, x, y, dir, 0);
		}
		if (state == 8 || state == 2)
		{
			DrawSpriteClipped(SPR_TWINKS, spriteinfo[SPR_TWINKS].width, x, y, dir, 1);
		}
		if (state == 7 || state == 3)
		{
			DrawSpriteClipped(SPR_TWINKS, spriteinfo[SPR_TWINKS].width, x, y, dir, 2);
		}
		if (state == 6 || state == 4)
		{
			DrawSpriteClipped(SPR_TWINKS, spriteinfo[SPR_TWINKS].width, x, y, dir, 3);
		}
		if (state == 5)
		{
			DrawSpriteClipped(SPR_TWINKS, spriteinfo[SPR_TWINKS].width, x, y, dir, 4);
		}
		twinks[i].state--;
	}
}

void UpdateSnow(void)
{
	/*register*/ Sint16 i;
	
	SetWriteMode(0);
	outp(0x3C4, 2);
	
	for (i=0; i<MAX_SNOW; i++)
	{
		if (snowX[i] >= 0 && snowX[i] < 80 && snowY[i] < 160 && snowY[i] >= 0)
		{
			outp(0x3C5, 1 << Random(4));
			if (Random(2)==0)
			{
				*(drawPtr + snowY[i]*80 + snowX[i]) = snowState[i]/4 + 0x78;
			}
		}
		snowY[i]++;
		snowState[i]++;
		if (snowState[i] >= 16)
		{
			snowX[i] = -99;
		}
	}
}

void SpawnTwink(Sint16 x, Sint16 y)
{
	twinks[twinkIndex].x = x;
	twinks[twinkIndex].y = y;
	twinks[twinkIndex].state = 9;
	twinks[twinkIndex].dir = Random(2);
	twinkIndex++;
	if (twinkIndex >= MAX_TWINKS)
	{
		twinkIndex = 0;
	}
}

void SpawnSnow(Sint16 x, Sint16 y)
{
	if (snowX[snowIndex] == -99)
	{
		snowX[snowIndex] = x;
		snowY[snowIndex] = y;
		snowState[snowIndex] = 0;
	}
	snowIndex++;
	if (snowIndex >= MAX_SNOW)
	{
		snowIndex = 0;
	}
}

void FloodViewportRed(void)
{
	SetWriteMode(0);
	setmem(drawPtr, 12800, 0x77-levelFlashRedCountdown);
	levelFlashRedCountdown--;
	SetWriteMode(1);
}

void LoadSprites(void)
{
	register Sint16 i;
	register Uint16 off;
	Uint16 spritenum;
	Uint16 endoff;
	
	OpenDatFile();
	
	for (i=0; i<MAX_SPRITES; i++)
	{
		if (spriteNumbers[i] == -1)
			continue;
		
		spritenum = spriteNumbers[i];
		// read the sprite info data:
		SeekDatChunk(SPRITES_SPR);
		fseek(dat_fp, spritenum*sizeof(TSpriteInfo), SEEK_CUR);
		fread(&spriteinfo[i], sizeof(TSpriteInfo), 1, dat_fp);
		
		// read the sprite layout data:
		SeekDatChunk(SPRITES_SPR);
		fseek(dat_fp, spriteinfo[i].offset, SEEK_CUR);
		fread(spriteLayouts[i], spriteinfo[i].layoutSize, 1, dat_fp);
		
		// load the sprite's pixel data into our big buffer
		// and then copy it into VGA memory:
		SetDrawPage(2);
		drawPtr += pixOffSpritestart;
		fread(buffer_64000, spriteinfo[i].pixelsSize, 1, dat_fp);
		SetWriteMode(0);
		spriteOffsets[i] = pixOffLineardata >> 2;
		endoff = pixOffLineardata + spriteinfo[i].pixelsSize;
		for (off=pixOffLineardata; off<endoff; off++)
		{
			DrawPixel(off % 320, off / 320, *(buffer_64000+off-pixOffLineardata));
		}
		drawPtr -= pixOffSpritestart;
		pixOffLineardata += spriteinfo[i].pixelsSize;
	}
	CloseDatFile();
}

void DrawSprite(Sint16 sprite, Sint16 x, Sint16 y, Sint16 dir, Sint16 frame)
{
	register Uint16 layoutOff;
	Uint16 pixOff, screenOff;
	Uint8 flag;
	
	layoutOff = spriteinfo[sprite].layoutStarts[dir][frame];
	pixOff = spriteOffsets[sprite] + spriteinfo[sprite].pixelsStarts[dir][frame];
	screenOff = y*80 + x;
	SetWriteMode(1);
	
draw_loop:
	flag = spriteLayouts[sprite][layoutOff];
	layoutOff++;
	if (flag == 0)
	{
		SetPlaneMask(spriteLayouts[sprite][layoutOff]);
		screenOff = y*80 + x;
		layoutOff++;
		goto draw_loop;
	}
	else if (flag == 1)
	{
		screenOff += *(Uint16 *)(&spriteLayouts[sprite][layoutOff]);
		layoutOff += 2;
		goto draw_loop;
	}
	else if (flag == 2)
	{
		drawPtr[screenOff] = *(screenpagePtr[2]+pixOffSpritestart+pixOff);
		pixOff++;
		screenOff++;
		goto draw_loop;
	}
}

void DrawSpriteClipped(Sint16 sprite, Sint16 width, Sint16 x, Sint16 y, Sint16 dir, Sint16 frame)
{
	register Sint16 ix;
	Uint16 layoutOff;
	Sint16 skiplen;
	Sint16 iy;
	Uint16 pixOff, screenOff;
	Uint8 flag;
	
	ix = iy = 0;
	layoutOff = spriteinfo[sprite].layoutStarts[dir][frame];
	pixOff = spriteOffsets[sprite] + spriteinfo[sprite].pixelsStarts[dir][frame];
	screenOff = y*80 + x;
	SetWriteMode(1);
	
draw_loop:
	flag = spriteLayouts[sprite][layoutOff];
	layoutOff++;
	if (flag == 0)
	{
		SetPlaneMask(spriteLayouts[sprite][layoutOff]);
		screenOff = y*80 + x;
		layoutOff++;
		ix = iy = 0;
		goto draw_loop;
	}
	else if (flag == 1)
	{
		skiplen = *(Sint16 *)(&spriteLayouts[sprite][layoutOff]);
		layoutOff += 2;
		screenOff += skiplen;
		ix += skiplen;
		if (ix > width)
		{
			iy += ix / 80;
			ix = ix % 80;
		}
		goto draw_loop;
	}
	else if (flag == 2)
	{
		if (ix+x >= 0 && ix+x < 80 && iy+y >= 0 && iy+y < 160)
		{
			drawPtr[screenOff] = *(screenpagePtr[2]+pixOffSpritestart+pixOff);
		}
		pixOff++;
		screenOff++;
		ix++;
		if (ix > width)
		{
			iy += ix / 80;
			ix = ix % 80;
		}
		goto draw_loop;
	}
}

void DrawSprite_White(Sint16 sprite, Sint16 x, Sint16 y, Sint16 dir, Sint16 frame)
{
	register Uint16 layoutOff, screenOff;
	Uint16 pixOff;
	Uint8 flag;
	
	layoutOff = spriteinfo[sprite].layoutStarts[dir][frame];
	pixOff = pixOffWhite;
	screenOff = y*80 + x;
	SetWriteMode(1);
	
draw_loop:
	flag = spriteLayouts[sprite][layoutOff];
	layoutOff++;
	if (flag == 0)
	{
		SetPlaneMask(spriteLayouts[sprite][layoutOff]);
		screenOff = y*80 + x;
		layoutOff++;
		goto draw_loop;
	}
	else if (flag == 1)
	{
		screenOff += *(Uint16 *)(&spriteLayouts[sprite][layoutOff]);
		layoutOff += 2;
		goto draw_loop;
	}
	else if (flag == 2)
	{
		drawPtr[screenOff] = screenpagePtr[2][pixOff];
		screenOff++;
		goto draw_loop;
	}
}

void DrawSpriteClipped_White(Sint16 sprite, Sint16 width, Sint16 x, Sint16 y, Sint16 dir, Sint16 frame)
{
	register Sint16 ix;
	Uint16 layoutOff;
	Sint16 skiplen;
	Sint16 iy;
	Uint16 pixOff, screenOff;
	Uint8 flag;
	
	ix = iy = 0;
	layoutOff = spriteinfo[sprite].layoutStarts[dir][frame];
	pixOff = pixOffWhite;
	screenOff = y*80 + x;
	SetWriteMode(1);
	
draw_loop:
	flag = spriteLayouts[sprite][layoutOff];
	layoutOff++;
	if (flag == 0)
	{
		SetPlaneMask(spriteLayouts[sprite][layoutOff]);
		screenOff = y*80 + x;
		layoutOff++;
		ix = iy = 0;
		goto draw_loop;
	}
	else if (flag == 1)
	{
		skiplen = *(Sint16 *)(&spriteLayouts[sprite][layoutOff]);
		layoutOff += 2;
		screenOff += skiplen;
		ix += skiplen;
		if (ix > width)
		{
			iy += ix / 80;
			ix = ix % 80;
		}
		goto draw_loop;
	}
	else if (flag == 2)
	{
		if (ix+x >= 0 && ix+x < 80 && iy+y >= 0 && iy+y < 160)
		{
			drawPtr[screenOff] = screenpagePtr[2][pixOff];
		}
		screenOff++;
		ix++;
		if (ix > width)
		{
			iy += ix / 80;
			ix = ix % 80;
		}
		goto draw_loop;
	}
}

void DrawSprite_Red(Sint16 sprite, Sint16 x, Sint16 y, Sint16 dir, Sint16 frame)
{
	register Uint16 layoutOff, screenOff;
	Uint16 pixOff;
	Uint8 flag;
	
	layoutOff = spriteinfo[sprite].layoutStarts[dir][frame];
	pixOff = pixOffRed;
	screenOff = y*80 + x;
	SetWriteMode(1);
	
draw_loop:
	flag = spriteLayouts[sprite][layoutOff];
	layoutOff++;
	if (flag == 0)
	{
		SetPlaneMask(spriteLayouts[sprite][layoutOff]);
		screenOff = y*80 + x;
		layoutOff++;
		goto draw_loop;
	}
	else if (flag == 1)
	{
		screenOff += *(Uint16 *)(&spriteLayouts[sprite][layoutOff]);
		layoutOff += 2;
		goto draw_loop;
	}
	else if (flag == 2)
	{
		drawPtr[screenOff] = screenpagePtr[2][pixOff];
		screenOff++;
		goto draw_loop;
	}
}

void DrawSpriteClipped_Red(Sint16 sprite, Sint16 width, Sint16 x, Sint16 y, Sint16 dir, Sint16 frame)
{
	register Sint16 ix;
	Uint16 layoutOff;
	Sint16 skiplen;
	Sint16 iy;
	Uint16 pixOff, screenOff;
	Uint8 flag;
	
	ix = iy = 0;
	layoutOff = spriteinfo[sprite].layoutStarts[dir][frame];
	pixOff = pixOffRed;
	screenOff = y*80 + x;
	SetWriteMode(1);
	
draw_loop:
	flag = spriteLayouts[sprite][layoutOff];
	layoutOff++;
	if (flag == 0)
	{
		SetPlaneMask(spriteLayouts[sprite][layoutOff]);
		screenOff = y*80 + x;
		layoutOff++;
		ix = iy = 0;
		goto draw_loop;
	}
	else if (flag == 1)
	{
		skiplen = *(Sint16 *)(&spriteLayouts[sprite][layoutOff]);
		layoutOff += 2;
		screenOff += skiplen;
		ix += skiplen;
		if (ix > width)
		{
			iy += ix / 80;
			ix = ix % 80;
		}
		goto draw_loop;
	}
	else if (flag == 2)
	{
		if (ix+x >= 0 && ix+x < 80 && iy+y >= 0 && iy+y < 160)
		{
			drawPtr[screenOff] = screenpagePtr[2][pixOff];
		}
		screenOff++;
		ix++;
		if (ix > width)
		{
			iy += ix / 80;
			ix = ix % 80;
		}
		goto draw_loop;
	}
}

void UpdateCam(void)
{
	/*register*/ Sint16 i, deltaX;
	Sint16 oldY, deltaY, oldX;
	Sint16 n;
	
	oldY = camY;
	oldX = camX_t8;
	playerX_pix = playerX << 1;
	playerTileY = playerY / 16;
	oddCamX = 0;
	if (camWarp)
	{
		if (playerX - 20 < 0)
		{
			camX_t8 = 0;
		}
		else if (playerX + 20 >= 480)
		{
			camX_t8 = 440;
		}
		else
		{
			camX_t8 = playerX-20;
			if (playerX % 2)
			{
				oddCamX = 1;
			}
		}
	}
	else
	{
		if (playerX - 20 < 0)
		{
			camX_t8 = 0;
		}
		else if (playerX + 20 >= 480)
		{
			camX_t8 = 440;
		}
		else
		{
			if (playerX - 20 > camX_t8)
			{
				camX_t8++;
			}
			else if (playerX - 20 < camX_t8)
			{
				camX_t8--;
			}
			if (playerX % 2)
			{
				oddCamX = 1;
			}
			deltaX = (oldX-camX_t8) << 3;	//same as " * 8"
			for (i=0; i<MAX_PARTICLES; i++)
			{
				if (particleState[i] > 0)
				{
					//register Sint16 n;
					
					for (n=0; n<PIXELS_PER_PARTICLE; n++)
					{
						particlePos[i][n].x += deltaX;
					}
				}
			}
			deltaX /= 4;
			for (i=0; i<MAX_SCORETAGS; i++)
			{
				scoretags[i].x += deltaX;
			}
			for (i=0; i<MAX_TWINKS; i++)
			{
				twinks[i].x += deltaX;
			}
			for (i=0; i<MAX_SNOW; i++)
			{
				snowX[i] += deltaX;
			}
		}
	}
	playerTileX = playerX / 2;
	camX = camX_t8 / 2;
	camScrolled = 0;
	if (!camWarp)
	{
		if (!playerJumping)
		{
			if (playerTileY - lookDeltaY < 0)
			{
				camY = 0;
			}
			else if (playerTileY + (10-lookDeltaY) >= 60)
			{
				camY = 50;
			}
			else if (playerTileY-lookDeltaY > camY)
			{
				camY++;
				camScrolled = 1;
			}
			else if (playerTileY-lookDeltaY < camY)
			{
				camY--;
				camScrolled = 1;
			}
			deltaY = (oldY-camY) << 4;	//same as " * 16"
			for (i=0; i<MAX_SHOTS; i++)
			{
				shotY[i] += deltaY;
			}
			for (i=0; i<MAX_PARTICLES; i++)
			{
				if (particleState[i] > 0)
				{
					//register Sint16 n;
					
					for (n=0; n<PIXELS_PER_PARTICLE; n++)
					{
						particlePos[i][n].y += deltaY;
					}
				}
			}
			for (i=0; i<MAX_SCORETAGS; i++)
			{
				scoretags[i].y += deltaY;
			}
			for (i=0; i<MAX_TWINKS; i++)
			{
				twinks[i].y += deltaY;
			}
			for (i=0; i<MAX_SNOW; i++)
			{
				snowY[i] += deltaY;
			}
		}
	}
	else
	{
		if (!playerJumping)
		{
			if (playerTileY - lookDeltaY < 0)
			{
				camY = 0;
			}
			else if (playerTileY + lookDeltaY >= 60)
			{
				camY = 50;
			}
			else
			{
				camY = playerTileY - lookDeltaY;
			}
		}
	}
	playerScreenX = (playerX-camX_t8) << 1;
	playerScreenY = playerY - (camY << 4);
}

void UpdateTiles(void)
{
	register Sint16 trigger, ti;
	Sint16 x, y, randnum;
	
	tileCountdown--;
	ti = camY*240 + camX;
	for (y=0; y<10; ti+=219,y++)
	{
		for (x=0; x<21; ti++,x++)
		{
			trigger = mapTriggeredLayer[ti];
			if (trigger)
			{
				if (!tileCountdown)
				{
					if (trigger == 1)
					{
						mapMainLayer[ti] = 255;
						mapTriggeredLayer[ti] = 0;
					}
					else if (trigger == 2)
					{
						mapMainLayer[ti] = mapInsertLayer[ti];
						mapTriggeredLayer[ti] = 0;
					}
				}
				else
				{
					// inline version of 'randnum = Random(3);':
					rand_index++;
					if (rand_index > 999)
						rand_index = 0;
					randnum = rand_table[rand_index] % 3;
					
					if (!randnum)
					{
						if (trigger == 1)
						{
							mapMainLayer[ti] = 255;
							mapTriggeredLayer[ti] = 0;
							if (x < 20)
							{
								SpawnTwink((x << 2)-(oddCamX << 1), y << 4);
							}
						}
						else if (trigger == 2)
						{
							mapMainLayer[ti] = mapInsertLayer[ti];
							mapTriggeredLayer[ti] = 0;
							if (x < 20)
							{
								SpawnTwink((x << 2)-(oddCamX << 1), y << 4);
							}
						}
					}
				}
			}
			//animate:
			trigger = mapBackLayer[ti];
			randnum = trigger;
			if (trigger < 240 && tileinfo.anim[trigger].type)
			{
				if (tileinfo.anim[trigger].type == 1)
				{
					randnum++;
					if (tileinfo.anim[trigger].last < randnum)
					{
						randnum = tileinfo.anim[trigger].first;
					}
				}
				else if (tileinfo.anim[trigger].type == 2)
				{
					if (tileinfo.anim[trigger].first != trigger)
					{
						randnum++;
						if (tileinfo.anim[trigger].last < randnum)
						{
							randnum = tileinfo.anim[trigger].first;
						}
					}
					else
					{
						// inline version of 'if (!Random(20)) randnum++;':
						rand_index++;
						if (rand_index > 999)
							rand_index = 0;
						if (!(rand_table[rand_index] % 20))
							randnum++;
					}
				}
				mapBackLayer[ti] = randnum;
			}
		}
	}
}

void UpdateActorSpawns(void)
{
	register Sint16 i, ti;
	Sint16 infoval, x, y;
	
	for (i=0; i<MAX_SPAWNQUEUE; i++)
	{
		if (spawnTileindex[i] != -1)
		{
			ti = spawnTileindex[i];
			infoval = mapInfoLayer[ti];
			y = ti / 240;	//Note: calculating x and y AFTER checking if infoval is 30000 would speed this up a little
			x = ti % 240;
			if (infoval != 30000)
			{
				if (SpawnActor(infoval-106, x<<2, y<<4, ti))
				{
					spawnTileindex[i] = -1;
				}
			}
		}
	}
}

void SpawnParticles(Sint16 x, Sint16 y)
{
	register Sint16 i;
	Sint16 white;
	
	particleState[particleIndex] = 16;
	for (i = 0; i < PIXELS_PER_PARTICLE; i++)
	{
		particlePos[particleIndex][i].x = x;
		particleInfo[particleIndex][i].vx = Random(10)-Random(10);
		particlePos[particleIndex][i].y = y;
		particleInfo[particleIndex][i].vy = -(Random(7)+5);
		white = Random(2);
		if (!white)
		{
			particleInfo[particleIndex][i].color = 0x70;	//red
		}
		else
		{
			particleInfo[particleIndex][i].color = 0x60;	//white (actually very bright blue)
		}
	}
	particleIndex++;
	if (particleIndex >= MAX_PARTICLES)
	{
		particleIndex = 0;
	}
}

void UpdateParticles(void)
{
	register Sint16 p, i;
	Sint16 x, y;
	
	SetWriteMode(0);
	outp(0x3C4, 2);
	
	for (p = 0; p < MAX_PARTICLES; p++)
	{
		if (particleState[p] <= 0)
			continue;
		
		for (i = 0; i < PIXELS_PER_PARTICLE; i++)
		{
			// move the pixels:
			particlePos[p][i].y += particleInfo[p][i].vy;
			particleInfo[p][i].vy++;	// "gravity"
			particlePos[p][i].x += particleInfo[p][i].vx;
			
			// update color:
			if ((!particleState[p]) % 4)	//BUG? using '... % 4' on a boolean value seems pointless
			{
				particleInfo[p][i].color++;
			}
			
			// draw at new poisition (if still on screen):
			x = particlePos[p][i].x;
			y = particlePos[p][i].y;
			if (x>=0 && x <= 319 && y >= 0 && y <= 159)
			{
				outp(0x3C5, 1 << (x % 4));
				*(drawPtr + y*80 + (x >> 2)) = particleInfo[p][i].color;
			}
		}
		particleState[p]--;
	}
}

void DrawNumberTile(Sint16 drawOff, Sint16 tilenum)
{
	/*register*/ Uint16 dstoff, srcoff;
	
	srcoff = (tilenum << 1) + 0x39D0;
	dstoff = drawOff;
	//draw 8x8 pixels:
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;	//useless!
	srcoff += 79;	//useless!
}

void DrawKeyTile(Sint16 drawOff, Sint16 tilenum)
{
	/*register*/ Uint16 dstoff, srcoff;
	
	srcoff = (tilenum << 1) + 0x39E6;
	dstoff = drawOff;
	//draw 8x11 pixels:
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;
	srcoff += 79;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff++;
	srcoff++;
	drawPtr[dstoff] = screenpagePtr[3][srcoff];
	dstoff += 79;	//useless!
	srcoff += 79;	//useless!
}

void UpdateHUD(void)
{
	register Sint16 l, i;
	
	if (hudLevelNumFlashCounter)
	{
		if (hudLevelNumFlashCounter < 10)
			DrawNumberTile(0x392A, 10);
		else
			DrawNumberTile(0x392A, level+1);
		
		hudLevelNumFlashCounter++;
		if (hudLevelNumFlashCounter > 20)
			hudLevelNumFlashCounter = 1;
	}
	else
	{
		DrawNumberTile(0x392A, level+1);
	}
	
	ltoa(score, numbuf, 10);
	for (l=0; numbuf[l]; l++);	//get string length
	for (i=0; i<l; i++)
	{
		DrawNumberTile(0x389A-l+(i<<1), numbuf[i]-'0');
	}
	if (playerHealth > 100)
		playerHealth = 100;
	if (playerHealth < 0)
		playerHealth = 0;
	
	itoa(playerHealth, numbuf, 10);
	for (l=0; numbuf[l]; l++);	//get string length
	for (i=0; i<3; i++)
	{
		DrawNumberTile(0x38A6+(i<<1), 10);
	}
	for (i=0; i<l; i++)
	{
		DrawNumberTile(0x38A9-l+(i<<1), numbuf[i]-'0');
	}
	
	// Note: Converting crystalsCollected and crystalsTotal into a string and
	// drawing a number tile for the first character of the string is a bit
	// wasteful.  You could skip the string conversion and simply pass the
	// respective integer variable to the DrawNumberTile function instead.
	// None of the original game's levels have more than 9 crystals anyway.
	
	itoa(crystalsCollected, numbuf, 10);
	DrawNumberTile(0x38B7, numbuf[0]-'0');
	
	itoa(crystalsTotal, numbuf, 10);
	DrawNumberTile(0x38BB, numbuf[0]-'0');
	
	// erase key icons:
	DrawKeyTile(0x3875, 2);
	DrawKeyTile(0x3877, 2);
	
	// draw key icons, if player has keys:
	if (gotKey1+gotKey2 > 1)
	{
		DrawKeyTile(0x3875, 0);
		DrawKeyTile(0x3877, 1);
	}
	if (gotKey1+gotKey2 == 1)
	{
		if (gotKey1)
			DrawKeyTile(0x3876, 0);
		else
			DrawKeyTile(0x3876, 1);
	}
}

void CountCrystals(void)
{
	register Sint16 i;
	
	crystalsTotal = 0;
	for (i=0; i<14400; i++)
	{
		if (mapInfoLayer[i] < 23)
		{
			if (iteminfos[mapInfoLayer[i]].type == 1)
			{
				crystalsTotal++;
			}
		}
	}
}

void SpawnScoreTag(Sint16 x, Sint16 y, Sint16 tag)
{
	register Sint16 i;
	
	for (i=0; i<MAX_SCORETAGS; i++)
	{
		if (!scoretags[i].state)
		{
			if (tag > 4)
			{
				scoretags[i].dir = 1;
				tag -= 5;
			}
			else
			{
				scoretags[i].dir = 0;
			}
			scoretags[i].x = x;
			scoretags[i].y = y;
			scoretags[i].frame = tag;
			scoretags[i].state = 17;
			break;
		}
	}
}

void UpdateScoreTags(void)
{
	register Sint16 i;
	
	for (i=0; i<MAX_SCORETAGS; i++)
	{
		if (scoretags[i].state)
		{
			DrawSpriteClipped(SPR_SCORETAGS, spriteinfo[SPR_SCORETAGS].width, scoretags[i].x, scoretags[i].y, scoretags[i].dir, scoretags[i].frame);
			scoretags[i].y -= 2;
			scoretags[i].state--;
		}
	}
}

void ShowPaused(void)
{
	register Sint16 y, x;
	Sint16 width;
	
	width = TextWidth("Game Paused");
	SetDrawPage(currDisplayPage);
	y = 76;
	x = 160 - width/2;
	DrawText(x+1, y+1, 1, "Game Paused");	// black "shadow"
	DrawText(x, y, 0x68, "Game Paused");	// yellow text
	if (config.controlMode)
	{
		lastKey = 0x80;
		WaitVBL();
		while (!joyBtn1Down && !joyBtn2Down && lastKey >= 0x80)
		{
			HandleJoystick();
			WaitVBL();
		}
	}
	else
	{
		while (lastKey < 0x80);
		while (lastKey >= 0x80);
	}
	SetDrawPage(!currDisplayPage);
}

void ShowMessage(char *text)
{
	SetDrawPage(currDisplayPage);
	DrawText((320-TextWidth(text))/2 +1, 81, 1, text);
	CENTER(DrawText, 80, 0x68, text);
	if (config.controlMode)
	{
		lastKey = 0x80;
		while (!joyBtn1Down && !joyBtn2Down && lastKey >= 0x80)
		{
			HandleJoystick();
			WaitVBL();
		}
	}
	else
	{
		while (lastKey < 0x80);
		while (lastKey >= 0x80);
	}
	SetDrawPage(!currDisplayPage);
}

void ShowWizardText(Sint16 textnum)
{
	register Sint16 i, lines;
	Sint16 textY, maxwidth, textX, width;
	
	// get number of lines used and maximum text width:
	maxwidth = 0;
	lines = 0;
	for (i = 0; i < ARRAY_LENGTH(messages[textnum].lines); i++)
	{
		width = TextWidth(messages[textnum].lines[i]);
		if (width)
		{
			if (width > maxwidth)
				maxwidth = width;
			lines++;
		}
		else
		{
			break;
		}
	}
	
	// draw the message lines:
	SetDrawPage(currDisplayPage);	// draw directly to the visible screen page
	textY = 80-12*lines;
	textX = 160-maxwidth/2;
	for (i=0; i<lines; i++)
	{
		DrawText(textX+1, textY+12*i+1, 1, messages[textnum].lines[i]);	// black "shadow"
		DrawText(textX, textY+12*i, 0x68, messages[textnum].lines[i]);	// yellow text
	}
	
	// wait for timer or key press:
	if (!playingDemo)
	{
		if (config.controlMode)
		{
			lastKey = 0x80;
			while (!joyBtn1Down && !joyBtn2Down && lastKey >= 0x80)
			{
				HandleJoystick();
				WaitVBL();
			}
		}
		else
		{
			while (lastKey < 0x80);
			while (lastKey >= 0x80);
		}
	}
	else
	{
		SleepMilliSecs(8000);
	}
	
	// back to double-buffered page order:
	SetDrawPage(!currDisplayPage);
}

void CheckTiles(void)
{
	register Sint16 i, num;
	Sint16 infoindex;
	Sint16 dy;
	Sint16 spawnindex;
	Sint16 n;
	Sint16 spawn;
	Sint16 correct;
	Sint16 infoval;
	Sint16 itemtype;
	Sint16 sx, sy;
	Sint16 keep;
	Sint16 tx, ty;
	Sint16 switchtype;
	Sint16 dx;
	
#define offset correct
	
	if (hurtCountdown)
	{
		hurtCountdown--;
	}
	for (dx=-1; dx<=1; dx++)
	{
		infoindex = playerTileY*240 + (playerX+1)/2 + dx;
		sx = playerScreenX-1;
		sy = playerScreenY-16;
		wizardActivated = 0;
		for (dy=0; dy<2; dy++, infoindex+=240, sy+=16)
		{
			infoval = mapInfoLayer[infoindex];
			keep = 0;
			if (infoval == 30000)
				continue;
			if (infoval < 23)	//Note: this would also handle values < 0, but the info layer should not contain any
			{
				itemtype = iteminfos[infoval].type;
				if (iteminfos[infoval].heal && playerHealth < 100)
				{
					PlaySound(SND_HEAL);
				}
				else if (iteminfos[infoval].heal)
				{
					keep = 1;
				}
				else if (iteminfos[infoval].score)
				{
					treasureFound++;
					if (treasureFound == treasureTotal)
					{
						hudLevelNumFlashCounter = 1;
						PlaySound(SND_YAHOO);
					}
					num = iteminfos[infoval].score;
					if (num == 100)
					{
						SpawnScoreTag(sx, sy, 0);
					}
					else if (num == 250)
					{
						SpawnScoreTag(sx, sy, 1);
					}
					else if (num == 500)
					{
						SpawnScoreTag(sx, sy, 2);
					}
					else if (num == 1000)
					{
						SpawnScoreTag(sx, sy, 3);
					}
					else if (num == 5000)
					{
						SpawnScoreTag(sx, sy, 4);
					}
					PlaySound(SND_BONUS1 + Random(2));
				}
				else if (itemtype == 1)
				{
					crystalsCollected++;
					levelFlashRedCountdown = 8;
					PlaySound(SND_CRYSTAL);
				}
				else if (itemtype == 2)
				{
					keep = 1;
					if (!hurtCountdown && !hideCountdown && !dx)
					{
						playerHealth += damage[skill];
						hurtCountdown = 20;
						PlaySound(SND_OUCH);
					}
				}
				else if (itemtype == 9)
				{
					if (!hurtCountdown && !hideCountdown && !dx)
					{
						playerHealth += damage[skill];
						hurtCountdown = 10;
						PlaySound(SND_OUCH);
					}
					keep = 1;
				}
				else if (itemtype == 3)
				{
					hideCountdown = 400;
					SpawnTwink(playerScreenX-1, playerScreenY);
					SpawnTwink(playerScreenX+4, playerScreenY);
					SpawnTwink(playerScreenX-1, playerScreenY+16);
					SpawnTwink(playerScreenX+4, playerScreenY+16);
					hurtCountdown = 0;
					PlaySound(SND_TELEPORT);
					SpawnScoreTag(sx, sy, 8);
				}
				else if (itemtype == 4)
				{
					if (!superJump && !dx)
					{
						superJump = true;
						superJumpCounter = 25;
						PlaySound(SND_YAHOO);
						SpawnScoreTag(sx, sy, 7);
					}
					else
					{
						keep = 1;
					}
				}
				else if (itemtype == 5)
				{
					if (dx)
					{
						keep = 1;
					}
					else
					{
						if (!rapidFireCountdown)
						{
							oldFirepower = playerFirepower;
							playerFirepower = 10;
						}
						rapidFireCountdown = 600;
						PlaySound(SND_YAHOO);
						SpawnScoreTag(sx, sy, 9);
					}
				}
				//Note: itemtype 6 ("Smart bomb") is not implemented!
				else if (itemtype == 7)
				{
					gotKey1 = true;
					PlaySound(SND_YAHOO);
					SpawnScoreTag(sx, sy, 5);
				}
				else if (itemtype == 8)
				{
					gotKey2 = true;
					PlaySound(SND_YAHOO);
					SpawnScoreTag(sx, sy, 5);
				}
				else if (itemtype == 10)
				{
					keep = 1;
					if (useKeyDown)
					{
						for (sx=0; sx<10; sx++)
						{
							if (messages[sx].posY == playerTileY && abs(messages[sx].posX-playerTileX) < 3)
							{
								SetWriteMode(0);
								SetPlaneMask(0xF);
								PlaySound(SND_TELEPORT);
								ShowWizardText(sx);
								wizardActivated = 1;
								SetWriteMode(1);
								SetPlaneMask(0xF);
							}
						}
						useKeyDown = 0;
					}
				}
				else if (itemtype == 11)
				{
					if (dx)
					{
						keep = 1;
					}
					else
					{
						powershotsLeft += 3;
						PlaySound(SND_YAHOO);
						tick1 = 1;
						tick5 = 5;
					}
				}
				else if (itemtype == 12)
				{
					//this probably sets a respawn-point -- never used in any level (and the game does not respawn the player anyway)
					oldPlayerX = playerX;
					oldPlayerY = playerY;
					keep = 1;
					mapInfoLayer[infoindex] = 30000;
					mapBackLayer[infoindex] = tileinfo.backtile;
					*(mapInfoLayer+infoindex+1) = 30000;
					*(mapBackLayer+infoindex+1) = tileinfo.backtile;
					PlaySound(SND_YAHOO);
				}
				if (iteminfos[infoval].firepower)
				{
					PlaySound(SND_YAHOO);
				}
				playerFirepower += iteminfos[infoval].firepower;
				if (playerFirepower > 10)
				{
					playerFirepower = 10;
				}
				if (rapidFireCountdown)
				{
					oldFirepower += iteminfos[infoval].firepower;
					if (oldFirepower > 10)
					{
						oldFirepower = 10;
					}
				}
				score += iteminfos[infoval].score;
				playerHealth += iteminfos[infoval].heal;
				if (!keep)
				{
					mapInfoLayer[infoindex] = 30000;
					mapBackLayer[infoindex] = tileinfo.backtile;
				}
			}
			else if (infoval < 33)
			{
				if (teleporters[infoval-23].startOff == infoindex && !dx)
				{
					teleportCounter = 1;
					num = infoval-23;
					teleportEndY = (teleporters[num].endOff / 240) << 4;
					teleportEndX = (teleporters[num].endOff % 240) << 1;
					playerFrame = spriteinfo[SPR_PLAYER].standFrame;
					playerJumping = 0;
					playerY = (playerY / 16) << 4;
					mapInfoLayer[infoindex] = 30000;
					mapBackLayer[infoindex] = tileinfo.backtile;
					PlaySound(SND_TELEPORT);
					SpawnScoreTag(sx, sy, 6);
				}
			}
			else if (infoval < 56)
			{
				if (useKeyDown && !dx)
				{
					useKeyDown = 0;
					if (mapBackLayer[infoindex] == tileinfo.switchDownTile)
					{
						mapBackLayer[infoindex] = tileinfo.switchUpTile;
					}
					else
					{
						mapBackLayer[infoindex] = tileinfo.switchDownTile;
					}
					num = infoval-33;
					for (i=0; i<4; i++)
					{
						if (switches[num].switchOffsets[i] != -1)
						{
							i = 99;
							break;
						}
					}
					correct = 0;
					if (i == 99)
					{
						correct = 1;
						for (i=0; i<4; i++)
						{
							if (switches[num].switchOffsets[i] != -1)
							{
								if (switches[num].desiredTiles[i] == tileinfo.switchDownTile && mapBackLayer[switches[num].switchOffsets[i]] != tileinfo.switchDownTile)
								{
									correct = 0;
								}
								if (switches[num].desiredTiles[i] == tileinfo.switchUpTile && mapBackLayer[switches[num].switchOffsets[i]] != tileinfo.switchUpTile)
								{
									correct = 0;
								}
							}
						}
						if (correct)
						{
							PlaySound(SND_SWOOSH);
							tileCountdown = 25;
							for (ty=switches[num].top; ty<=switches[num].bottom; ty++)
							{
								for (tx=switches[num].left; tx<=switches[num].right; tx++)
								{
									switchtype = switches[num].type;
									if (switchtype == 0)
									{
										if (*(mapMainLayer + ty*240 + tx) != 255)
										{
											*(mapTriggeredLayer + ty*240 + tx) = 1;
										}
									}
									else
									{
										if (*(mapInsertLayer + ty*240 + tx) != 255)
										{
											*(mapTriggeredLayer + ty*240 + tx) = 2;
										}
									}
								}
							}
							for (i=0; i<4; i++)
							{
								switches[num].switchOffsets[i] = -1;
							}
						}
						else
						{
							PlaySound(SND_FLIPSWITCH);
						}
					}
					else
					{
						PlaySound(SND_FLIPSWITCH);
					}
				}
			}
			else if (infoval < 81)
			{
				num = infoval-56;
				i = 0;
				
				if (inserts[num].requiredKey == 0)
				{
					i = 1;
				}
				if (inserts[num].requiredKey == 1 && gotKey1)
				{
					i = 2;
				}
				if (inserts[num].requiredKey == 2 && gotKey2)
				{
					i = 3;
				}
				if (i)
				{
					PlaySound(SND_SWOOSH);
					tileCountdown = 10;
					for (ty=inserts[num].top; ty<=inserts[num].bottom; ty++)
					{
						for (tx=inserts[num].left; tx<=inserts[num].right; tx++)
						{
							if (*(mapInsertLayer + ty*240 + tx) != 255)
							{
								*(mapTriggeredLayer + ty*240 + tx) = 2;
							}
						}
					}
					mapInfoLayer[infoindex] = 30000;
					if (inserts[num].requiredKey == 0)	//"if (i == 1)" would be faster
					{
						mapBackLayer[infoindex] = inserts[num].replaceTile;
					}
					else
					{
						mapBackLayer[infoindex] = tileinfo.backtile;
					}
					if (i > 1)
					{
						SpawnTwink(((infoindex % 240)-camX)<<2, ((infoindex / 240)-camY)<<4);
					}
				}
				if (i == 2)
				{
					gotKey1 = false;
				}
				if (i == 3)
				{
					gotKey2 = false;
				}
			}
			else if (infoval < 106)
			{
				num = infoval-81;
				i = 0;
				
				if (keyholes[num].requiredKey == 0)
				{
					i = 1;
				}
				if (keyholes[num].requiredKey == 1 && gotKey1)
				{
					i = 2;
				}
				if (keyholes[num].requiredKey == 2 && gotKey2)
				{
					i = 3;
				}
				if (i)
				{
					PlaySound(SND_SWOOSH);
					tileCountdown = 10;
					for (ty=keyholes[num].top; ty<=keyholes[num].bottom; ty++)
					{
						for (tx=keyholes[num].left; tx<=keyholes[num].right; tx++)
						{
							if (*(mapMainLayer + ty*240 + tx) != 255)
							{
								*(mapTriggeredLayer + ty*240 + tx) = 1;
							}
						}
					}
					mapInfoLayer[infoindex] = 30000;
					if (keyholes[num].requiredKey == 0)	//"if (i == 1)" would be faster
					{
						mapBackLayer[infoindex] = keyholes[num].replaceTile;
					}
					else
					{
						mapBackLayer[infoindex] = tileinfo.backtile;
					}
					if (i > 1)
					{
						SpawnTwink(((infoindex % 240)-camX)<<2, ((infoindex / 240)-camY)<<4);
					}
				}
				if (i == 2)
				{
					gotKey1 = false;
				}
				if (i == 3)
				{
					gotKey2 = false;
				}
			}
			else if (infoval >= 116 && infoval <= 365 && dx == 0)
			{
				num = infoval-116;
				
				for (i=0; i<8; i++)
				{
					offset = enemytriggers[num].offsets[i];
					if (mapInfoLayer[offset] != 30000)
					{
						for (spawnindex=0; spawnindex<MAX_SPAWNQUEUE; spawnindex++)
						{
							if (spawnTileindex[spawnindex] == -1)
							{
								spawn = 1;
								
								// check if this actor has already spawned:
								for (n=0; n<MAX_ACTORS; n++)
								{
									if (actorTileindex[n] == offset)
									{
										spawn = 0;
									}
								}
								
								// check if this actor is already in the spawn queue:
								for (n=0; n<MAX_SPAWNQUEUE; n++)
								{
									if (spawnTileindex[n] == offset)
									{
										spawn = 0;
									}
								}
								
								// add to spawn queue if it's okay to add it
								if (spawn)
								{
									spawnTileindex[spawnindex] = offset;
									break;
								}
							}
						}
					}
				}
			}
		}	//dy loop
	}	//dx loop
	
#undef offset
}

Sint16 GameLoop(Sint16 demoMode, Uint16 isNewGame)
{
	enum {end_not, end_ended, end_restart, end_loaded};
	register Sint16 i, tileindex;
	Sint16 x, y, pixindex, result, helpindex;
	Uint8 levelEnded, delaytime, tiletick, jumpEnded, wasFalling;
	Sint32 lastTickCount;
	
	result = 0;
	delaytime = 0;
	
restart:

#ifdef VERSION_11
	playerFirepower = 1;
#endif
	elevatorTile1 = levelElevatorTiles[episode][level][0];
	elevatorTile2 = levelElevatorTiles[episode][level][1];
	playerFiring = playerAimingUp = 0;
	demoIndex = 0;
	if (demoMode == DEMO_PLAY)
	{
		playingDemo = true;
	}
	else
	{
		playingDemo = false;
	}
	SetDrawPage(0);
	ClearScreen();
	SetDrawPage(1);
	ClearScreen();
	SetDisplayPage(1);
	PlayMusicChunk(MUSIC01_MID + levelMusicNumbers[episode][level], true);
	tiletick = 0;
	levelFlashRedCountdown = 0;
	teleportCounter = 0;
	twinkIndex = 0;
	hurtCountdown = 0;
	playerShotsActive = 0;
	hideCountdown = 0;
	superJump = false;
	rapidFireCountdown = 0;
	playerDeathCountdown = 0;
	wasFalling = false;
	gotKey1 = gotKey2 = false;
	superJumpCounter = 0;
	powershotsLeft = 0;
	playerWalking = 0;
	lookDeltaY = 5;
	zero_0 = 0;
	levelIndex = episode*9 + level;
	shotIndex = 0;
	for (i=0; i<MAX_SHOTS; i++)
	{
		shotX[i] = -99;
	}
	particleIndex = 0;
	for (i=0; i<MAX_PARTICLES; i++)
	{
		particleState[i] = -1;
	}
	for (i=0; i<MAX_TWINKS; i++)
	{
		twinks[i].state = 0;
	}
	for (i=0; i<MAX_SCORETAGS; i++)
	{
		scoretags[i].state = 0;
	}
	for (i=0; i<MAX_SPAWNQUEUE; i++)
	{
		spawnTileindex[i] = -1;
	}
	for (i=0; i<MAX_ACTORS; i++)
	{
		actorType[i] = -1;
	}
	for (i=0; i<MAX_ACTORS; i++)
	{
		actorTileindex[i] = -1;
	}
	for (i=0; i<MAX_ENEMYSHOTS; i++)
	{
		enemyshotState[i] = -1;
	}
	for (i=0; i<MAX_SNOW; i++)
	{
		snowX[i] = -99;
	}
	OpenDatFile();
	ReadDatChunk(E1L1_000 + levelNumbers[episode][level], &coordinates);
	playerX = coordinates.playerX << 1;
	playerY = coordinates.playerY << 4;
	oldPlayerX = playerX;
	oldPlayerY = playerY;
	ReadDatChunk(E1L1_001 + levelNumbers[episode][level], &tileinfo);
	ReadDatChunk(E1L1_002 + levelNumbers[episode][level], &messages);
	ReadDatChunk(E1L1_003 + levelNumbers[episode][level], &teleporters);
	ReadDatChunk(E1L1_004 + levelNumbers[episode][level], &switches);
	ReadDatChunk(E1L1_005 + levelNumbers[episode][level], &inserts);
	ReadDatChunk(E1L1_006 + levelNumbers[episode][level], &keyholes);
	ReadDatChunk(E1L1_007 + levelNumbers[episode][level], &enemyinfos);
	ReadDatChunk(E1L1_008 + levelNumbers[episode][level], &enemytriggers);
	ReadDatChunk(E1L1_009 + levelNumbers[episode][level], mapBackLayer);
	ReadDatChunk(E1L1_010 + levelNumbers[episode][level], mapMainLayer);
	ReadDatChunk(E1L1_011 + levelNumbers[episode][level], mapInsertLayer);
	ReadDatChunk(E1L1_012 + levelNumbers[episode][level], mapInfoLayer);
	setmem(mapTriggeredLayer, 14400, 0);
	CloseDatFile();
	treasureTotal=treasureFound=0;
	for (i=0; i<14400; i++)
	{
		x = mapInfoLayer[i];
		if (x >= 0 && x <= 22 && iteminfos[x].score)
		{
			treasureTotal++;
		}
	}
	hudLevelNumFlashCounter=0;
	///////////////////////////////////////////
	// useless code: (variables are never read)
	numEnemiesTotal=numEnemiesKilled=0;
	for (i=0; i<14400; i++)
	{
		x = mapInfoLayer[i];
		if (x >= 106 && x <= 115 && enemyinfos[x-106].health != -1)
		{
			numEnemiesTotal++;
		}
	}
	///////////////////////////////////////////
	SetDrawPage(3);
	DrawPCX_Chunk(TILES01_PCX + levelTilesetNumbers[episode][level], 1);
	GetPalette(currPalette, 0, 0x100);	//not required, palette is overwritten below
	
	//
	// re-arrange the tile graphics into serialized pixel data
	//
	pixindex = 0;
	for (i=0; i<280; i++)
	{
		SetWriteMode(0);
		if (ReadPixel((i%20)<<4, (i/20)<<4)==255)
		{
			pixOffLineardata = pixindex<<2;
			break;
		}
		SetWriteMode(1);
		SetPlaneMask(0xF);
		for (y=0; y<16; y++)
		{
			for (x=0; x<4; x++)
			{
				screenpagePtr[2][pixindex] = *(drawPtr+(y+((i/20)<<4))*80 + ((i%20)<<2)+x);
				pixindex++;
			}
		}
	}
	//Note: you could use pixindex instead of pixOffLineardata>>2 below! (increase by 1 instead of 4)
	screenpagePtr[2][pixOffLineardata>>2] = 0x60;	//white (actually very light blue)
	pixOffWhite = pixOffLineardata>>2;
	pixOffLineardata += 4;
	screenpagePtr[2][pixOffLineardata>>2] = 0x70;	//red
	pixOffRed = pixOffLineardata>>2;
	pixOffLineardata += 4;
	pixOffSpritestart = pixOffLineardata>>2;
	pixOffLineardata = 0;
	for (i=0; i<MAX_SPRITES; i++)
	{
		spriteNumbers[i] = -1;
	}
	spriteNumbers[SPR_PLAYER] = 0;
	spriteNumbers[SPR_SCORETAGS] = 1;
	spriteNumbers[SPR_MORPH] = 3;
	spriteNumbers[SPR_TWINKS] = 2;
	x = 0;
	for (i=0; i<6; i++)
	{
		if (enemyinfos[i].sprite != -1)
		{
			spriteNumbers[SPR_ENEMY_0 + x] = enemyinfos[i].sprite;
			x++;
		}
	}
	LoadSprites();
	SetDrawPage(3);	//LoadSprites() changed it to 2
	drawPtr += 2000;	//leave more room for tile & sprite data
	DrawPCX_Chunk(BACK01_PCX + levelBackdropNumbers[episode][level], 0);	//Note: this draws parts of the backdrop to page 0!
	drawPtr -= 2000;	//restore original pointer
	LoadPaletteChunk(GAMEPAL_PAL, 0, 1);
	LoadPaletteChunk(BACK01_PAL + levelBackdropNumbers[episode][level], 128, 1);
	DrawImageChunk(HUDSTUFF_IMG, 0, 185);
	DrawImageChunk(BULLIT_IMG, 28, 185);
	DrawImageChunk(VOL_BAR_IMG, 60, 185);
	FadeToBlack(1);
	SetDrawPage(0);
	DrawImageChunk(NEW_HUD_IMG, 0, 160);
	SetDrawPage(1);
	DrawImageChunk(NEW_HUD_IMG, 0, 160);
	//////////////////////////////
	//same as above (redundant)
	SetDrawPage(0);
	DrawImageChunk(NEW_HUD_IMG, 0, 160);
	SetDrawPage(1);
	DrawImageChunk(NEW_HUD_IMG, 0, 160);
	//////////////////////////////
	CountCrystals();
	crystalsCollected = 0;
	playerHealth = 100;
	playerX = coordinates.playerX << 1;
	playerY = coordinates.playerY << 4;
	playerFrame = spriteinfo[SPR_PLAYER].standFrame;
	playerDir = 0;
	playerJumping = 0;
	playerFalling = 0;
	camWarp = 1;
	UpdateCam();
	camWarp = 0;
	if (demoMode == DEMO_PLAY)
	{
		OpenDatFile();
		ReadDatChunk(DEMO1_DMO + (level/2), buffer_64000);
		CloseDatFile();
		demoSize = *((Sint16 *)buffer_64000);
	}
	if (episode == 3 && level == 8)
	{
		boss4Stage = 0;
		SpawnActor(0, boss4Xs[0]<<2, boss4Ys[0]<<4, boss4TileOffs[0]);
		actorDir[0] = boss4Dirs[0];
		actorVX[0] = 0;
		actorMoveCountdown[0] = 100;	//useless, not used by boss 4
		actorSeeking[0] = 10+Random(20);
		actorShotDeltaX[0] = 0;
	}
	KB_Startup();
	zero = 0;
	rand_index = 0;
	SetDrawPage(1);
	SetDisplayPage(0);
	levelEnded = end_not;
	SetCurrentPalette();
	nextTickCount=pcTickCount=lastTickCount = 0;
	
	while (!levelEnded)
	{
		
		//flip pages:
		SetDrawPage(!currDrawPage);
		SetDisplayPage(!currDisplayPage);
#if 0
		////////////////
		// HACK:
		if (lastKey == KEY_C)
		{
			FILE *fp = fopen("mem.dat", "wb");
			while (lastKey == KEY_C);
			if (fp)
			{
				Sint16 i;
				fwrite(currPalette, 256, 3, fp);
				outp(0x3CE, 4);
				for (i=0; i<4; i++)
				{
					outp(0x3CF, i);
					fwrite((void*)0xA0000000L, 0x8000, 2, fp);
				}
				fclose(fp);
			}
		}
		////////////////
#endif

		//limit framerate:
		while (pcTickCount-lastTickCount < speedTicks[gamespeed]);
		lastTickCount = pcTickCount;
		
		if (config.controlMode && !playingDemo)
		{
			ReadJoy();
		}
		UpdateCam();
		SetWriteMode(1);
		SetPlaneMask(0xF);
		if (!teleportCounter)
		{
			CheckTiles();
		}
		if (levelFlashRedCountdown)
		{
			FloodViewportRed();
		}
		else
		{
			if (oddCamX)
			{
				DrawLevel_Odd(drawPtr, screenpagePtr[2], mapMainLayer, mapBackLayer, camY*240 + camX);
				screenpagePtr[3] += 2000;
				DrawBackdrop_Odd(screenpagePtr[3], drawPtr);
			}
			else
			{
				DrawLevel_Even(drawPtr, screenpagePtr[2], mapMainLayer, mapBackLayer, camY*240 + camX);
				screenpagePtr[3] += 2000;
				DrawBackdrop_Even(screenpagePtr[3], drawPtr);
			}
			screenpagePtr[3] -= 2000;
		}
		UpdateHUD();
		if (!playerHealth && !playerDeathCountdown)
		{
			playerDeathCountdown = 90;
			hideCountdown = 30000;
			ClearKeys();
		}
		if (playerDeathCountdown)
		{
			playerDeathCountdown--;
			if ((Random(4)==0 && playerDeathCountdown > 25) || playerDeathCountdown == 89)
			{
				SpawnParticles(((playerScreenX<<2)+Random(10)+12)-Random(10), (playerScreenY+Random(10)+16)-Random(10));
				SpawnTwink((playerScreenX+Random(3))-Random(3), (playerScreenY+Random(12))-Random(12));
				SpawnParticles(((playerScreenX<<2)+Random(10)+12)-Random(10), (playerScreenY+Random(10)+16)-Random(10));
				SpawnTwink((playerScreenX+Random(3))-Random(3), (playerScreenY+Random(12))-Random(12));
				PlaySound(SND_EXPLODE);
			}
		}
		if (crystalsTotal == crystalsCollected && !levelDoneCountdown)
		{
			levelDoneCountdown = 90;
			hideCountdown = 30000;
			ClearKeys();
			KillActiveEnemies();
		}
		if (levelDoneCountdown)
		{
			levelDoneCountdown--;
			if (Random(7)==0 && levelDoneCountdown > 25)
			{
				SpawnTwink((playerScreenX+Random(3))-Random(3), (playerScreenY+Random(12))-Random(12));
				SpawnTwink((playerScreenX+Random(3))-Random(3), (playerScreenY+Random(12))-Random(12));
				PlaySound(SND_YAHOO);
			}
		}
		if (!hideCountdown)
		{
			if (!teleportCounter)
			{
				if (playerFiring)
				{
					if (playerAimingUp && !playerWalking)
					{
						playerFrame = spriteinfo[SPR_PLAYER].shootStart +1;
					}
					else if (!playerWalking)
					{
						playerFrame = spriteinfo[SPR_PLAYER].shootStart;
					}
				}
				if (playerJumping)
				{
					playerFrame = spriteinfo[SPR_PLAYER].jumpStart;
				}
				if (playerJumping && playerJumpIndex > maxJumpIndex)
				{
					playerFrame = spriteinfo[SPR_PLAYER].jumpEnd;
				}
				if (playerFalling)
				{
					playerFrame = spriteinfo[SPR_PLAYER].jumpEnd;
				}
				// draw player sprite:
				///////////////////////
				// this is redundant:
				if (playerScreenY < 0)
				{
					DrawSpriteClipped(SPR_PLAYER, spriteinfo[SPR_PLAYER].width, playerScreenX, playerScreenY, playerDir, playerFrame);
				}
				///////////////////////
				if (hurtCountdown % 2)	//Note: & 1 would be faster than % 2
				{
					if (playerScreenY < 0)
					{
						DrawSpriteClipped_Red(SPR_PLAYER, spriteinfo[SPR_PLAYER].width, playerScreenX, playerScreenY, playerDir, playerFrame);
					}
					else
					{
						DrawSprite_Red(SPR_PLAYER, playerScreenX, playerScreenY, playerDir, playerFrame);
					}
				}
				else if (rapidFireCountdown % 2)	//Note: & 1 would be faster than % 2
				{
					if (playerScreenY < 0)
					{
						DrawSpriteClipped_White(SPR_PLAYER, spriteinfo[SPR_PLAYER].width, playerScreenX, playerScreenY, playerDir, playerFrame);
					}
					else
					{
						DrawSprite_White(SPR_PLAYER, playerScreenX, playerScreenY, playerDir, playerFrame);
					}
				}
				else
				{
					if (playerScreenY < 0)
					{
						DrawSpriteClipped(SPR_PLAYER, spriteinfo[SPR_PLAYER].width, playerScreenX, playerScreenY, playerDir, playerFrame);
					}
					else
					{
						DrawSprite(SPR_PLAYER, playerScreenX, playerScreenY, playerDir, playerFrame);
					}
				}
			}
			else
			{
				if (teleportCounter < 5)
				{
					DrawSprite(SPR_MORPH, playerScreenX, playerScreenY, playerDir, 0);
				}
				else if (teleportCounter < 10)
				{
					DrawSprite(SPR_MORPH, playerScreenX, playerScreenY, playerDir, 1);
				}
				else if (teleportCounter < 15)
				{
					DrawSprite(SPR_MORPH, playerScreenX, playerScreenY, playerDir, 2);
				}
				else if (teleportCounter < 20)
				{
					DrawSprite(SPR_MORPH, playerScreenX, playerScreenY, playerDir, 3);
				}
				else if (teleportCounter <= 25)
				{
					DrawSprite(SPR_MORPH, playerScreenX, playerScreenY + Random(2) - Random(2), playerDir, 4);
				}
				else if (teleportCounter < 30)
				{
					DrawSprite(SPR_MORPH, playerScreenX, playerScreenY, playerDir, 3);
				}
				else if (teleportCounter < 35)
				{
					DrawSprite(SPR_MORPH, playerScreenX, playerScreenY, playerDir, 2);
				}
				else if (teleportCounter < 40)
				{
					DrawSprite(SPR_MORPH, playerScreenX, playerScreenY, playerDir, 1);
				}
			}
			if (superJumpCounter)
			{
				SpawnTwink(playerScreenX + Random(7) - 2, playerScreenY + Random(10) + 20);
				if (!superJump)
				{
					superJumpCounter--;
				}
			}
			if (powershotsLeft)
			{
				if (tick1)
				{
					x = powershotsLeft;
					if (x > 3)
						x = 3;
					for (i=0; i<x; i++)
					{
						DrawSpriteClipped(SPR_PLAYER, spriteinfo[SPR_PLAYER].projectileWidth, playerScreenX+1, (playerScreenY-14)-(14*i), playerDir, spriteinfo[SPR_PLAYER].projectileStart+2);
					}
				}
				tick5--;
				if (tick5 == 0)
				{
					tick5 = 5;
					tick1 = !tick1;
				}
			}
		}
		else
		{
			hideCountdown--;
			if (!hideCountdown)
				PlaySound(SND_TELEPORT);
		}
		if (isNewGame)
		{
			KB_Shutdown();
			DrawImageChunk(CRYSTAL_IMG, 12, 46);
			SetDisplayPageAtVBL(!currDisplayPage);
			while (!HandleJoystick() && !kbhit())
			{
				WaitVBL();
			}
			while (kbhit())
			{
				getch();
			}
			KB_Startup();
			isNewGame = 0;
			SetDisplayPageAtVBL(!currDisplayPage);
		}
		CheckEnemyHitsPlayerShot();
		if (!teleportCounter) { //don't get hurt while warping (https://pckf.com/viewtopic.php?t=18165)
			CheckEnemyShotHitsPlayer();
			CheckEnemyHitsPlayer();
		}
		UpdateActorSpawns();
		UpdateEnemyShots();
		UpdateActors();
		RemoveHiddenActors();
		UpdateParticles();
		UpdateScoreTags();
		UpdateTwinks();
		UpdateSnow();
		if (tiletick)
		{
			UpdateTiles();
		}
		tiletick = !tiletick;
		if (delaytime)
		{
			delaytime--;
		}
		if (demoMode == DEMO_RECORD)
		{
			demoKeys.fire  = false;
			demoKeys.jump  = false;
			demoKeys.up    = false;
			demoKeys.left  = false;
			demoKeys.right = false;
		}
		if (demoMode == DEMO_PLAY)
		{
			memcpy(&demoKeys, buffer_64000+2+(demoIndex*6), 6);
			fireKeyDown = demoKeys.fire;
			jumpKeyDown = demoKeys.jump;
			leftKeyDown = demoKeys.left;
			rightKeyDown = demoKeys.right;
			useKeyDown = demoKeys.up;
			if (fireKeyDown)
			{
				fireKeyUp = 0;
			}
			else
			{
				fireKeyUp = 1;
			}
			if (useKeyDown)
			{
				useKeyUp = 0;
			}
			else
			{
				useKeyUp = 1;
			}
			demoIndex++;
		}
		if (!fireKeyUp)
		{
			playerFiring = 1;
		}
		else
		{
			playerFiring = 0;
		}
		if (!useKeyUp)
		{
			playerAimingUp = 1;
		}
		else
		{
			playerAimingUp = 0;
		}
		if (!teleportCounter && !playerDeathCountdown && ! levelDoneCountdown && config.controlMode && !playingDemo)
		{
			//get joystick input:
			leftKeyDown = rightKeyDown = useKeyDown = downKeyDown = 0;
			if (joyStickLeft)
				leftKeyDown = 1;
			if (joyStickRight)
				rightKeyDown = 1;
			if (joyStickUp && useKeyUp)
			{
				upKeyDown = 1;
				useKeyDown = 1;
				useKeyUp = 0;
				playerAimingUp = 1;
			}
			if (!joyStickUp)
			{
				useKeyUp = 1;
				playerAimingUp = 0;
				upKeyDown = 0;
			}
			if (joyStickDown)
				downKeyDown = 1;
			if (joyFireButton == 0)
			{
				if (joyBtn1Down)
				{
					playerFiring = 1;
				}
				else
				{
					playerFiring = 0;
				}
				if (!joyBtn1Down)
				{
					fireKeyDown = 0;
					fireKeyUp = 1;
				}
				if (joyBtn1Down && fireKeyUp)
				{
					fireKeyDown = 1;
					fireKeyUp = 0;
				}
				if (!joyBtn2Down)
				{
					jumpKeyDown = 0;
					jumpKeyUp = 1;
				}
				if (joyBtn2Down && jumpKeyUp)
				{
					jumpKeyDown = 1;
					jumpKeyUp = 0;
				}
			}
			else
			{
				if (joyBtn2Down)
				{
					playerFiring = 1;
				}
				else
				{
					playerFiring = 0;
				}
				if (!joyBtn2Down)
				{
					fireKeyDown = 0;
					fireKeyUp = 1;
				}
				if (joyBtn2Down && fireKeyUp)
				{
					fireKeyDown = 1;
					fireKeyUp = 0;
				}
				if (!joyBtn1Down)
				{
					jumpKeyDown = 0;
					jumpKeyUp = 1;
				}
				if (joyBtn1Down && jumpKeyUp)
				{
					jumpKeyDown = 1;
					jumpKeyUp = 0;
				}
			}
		}
		// handle player shooting:
		if (!teleportCounter && fireKeyDown)
		{
			if (demoMode == DEMO_RECORD)
			{
				demoKeys.fire = true;
			}
			if (!rapidFireCountdown)
			{
				fireKeyDown = false;
			}
			for (shotIndex = 0; shotIndex < 10 && shotX[shotIndex] != -99; shotIndex++);
			if (shotIndex < 10 && playerShotsActive < playerFirepower)
			{
				playerShotsActive++;
				if (powershotsLeft)
				{
					powershotsLeft--;
					shotType[shotIndex] = 1;
				}
				else
				{
					shotType[shotIndex] = 0;
				}
				if (playerAimingUp)
				{
					shotFlyUp[shotIndex] = 1;
					shotX[shotIndex] = playerScreenX+1;
					shotY[shotIndex] = playerScreenY;
				}
				else
				{
					shotFlyUp[shotIndex] = 0;
					if (playerDir)
					{
						shotX[shotIndex] = playerScreenX-2;
					}
					else
					{
						shotX[shotIndex] = playerScreenX+2;
					}
					shotY[shotIndex] = playerScreenY + spriteinfo[SPR_PLAYER].projectileDeltaY;
				}
				shotDir[shotIndex] = playerDir;
				///////////////////
				//this is useless:
				shotIndex++;
				if (shotIndex > 4)
				{
					shotIndex = 0;
				}
				///////////////////
				if (!rapidFireCountdown)
				{
					PlaySound(SND_PLAYERSHOT);
				}
				else
				{
					PlaySound(SND_ENEMYSHOT);
				}
			}
		}
		if (rapidFireCountdown)
		{
			rapidFireCountdown--;
		}
		if (rapidFireCountdown == 1)
		{
			rapidFireCountdown = 0;
			playerFirepower = oldFirepower;
		}
		// update & draw player shots:
		for (i=0; i<MAX_SHOTS; i++)
		{
			if (shotX[i] == -99)
				continue;
			if (!shotFlyUp[i])
			{
				if (!shotDir[i])
				{
					tileindex = (shotY[i]/16 + camY)*240 + shotX[i]/4 + camX + 1;
					if (*(mapMainLayer+tileindex) == tileinfo.shootableTile)
					{
						*(mapMainLayer+tileindex) = 255;
						SpawnTwink(shotX[i]+2-(oddCamX<<1), shotY[i]-8);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else if (*(mapMainLayer+tileindex+240) == tileinfo.shootableTile)
					{
						*(mapMainLayer+tileindex+240) = 255;
						SpawnTwink(shotX[i]+2-(oddCamX<<1), shotY[i]+8);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else if (*(mapMainLayer+tileindex+1) == tileinfo.shootableTile)
					{
						*(mapMainLayer+tileindex+1) = 255;
						SpawnTwink(shotX[i]+6-(oddCamX<<1), shotY[i]-8);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else if (*(mapMainLayer+tileindex+241) == tileinfo.shootableTile)
					{
						*(mapMainLayer+tileindex+241) = 255;
						SpawnTwink(shotX[i]+6-(oddCamX<<1), shotY[i]+8);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else if (*(mapMainLayer+tileindex) != 255 || *(mapMainLayer+tileindex+1) != 255)
					{
						SpawnParticles((shotX[i]<<2)+24, shotY[i]+10);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else
					{
						if (shotFlyUp[i])	//always false here
						{
							shotY[i] -= 16;	//unreachable
						}
						else
						{
							shotX[i] += 4;
						}
						if (shotX[i] > 79 || shotY[i] < 0)
						{
							shotX[i] = -99;
							playerShotsActive--;
						}
						else
						{
							if (shotType[i])
							{
								DrawSpriteClipped(SPR_PLAYER, spriteinfo[SPR_PLAYER].projectileWidth, shotX[i], shotY[i], shotDir[i], spriteinfo[SPR_PLAYER].projectileStart+2);
							}
							else
							{
								DrawSpriteClipped(SPR_PLAYER, spriteinfo[SPR_PLAYER].projectileWidth, shotX[i], shotY[i], shotDir[i], spriteinfo[SPR_PLAYER].projectileStart);
							}
							if (Random(10) > 2)
							{
								SpawnSnow(shotX[i], shotY[i]+5);
							}
						}
					}
				}
				else	//shot is flying left!
				{
					tileindex = (shotY[i]/16 + camY)*240 + shotX[i]/4 + camX + 1;
					if (*(mapMainLayer+tileindex) == tileinfo.shootableTile)
					{
						*(mapMainLayer+tileindex) = 255;
						SpawnTwink(shotX[i]-2-(oddCamX<<1)+2, shotY[i]-8);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else if (*(mapMainLayer+tileindex+240) == tileinfo.shootableTile)
					{
						*(mapMainLayer+tileindex+240) = 255;
						SpawnTwink(shotX[i]-2-(oddCamX<<1)+2, shotY[i]+8);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else if (*(mapMainLayer+tileindex-1) == tileinfo.shootableTile)
					{
						*(mapMainLayer+tileindex-1) = 255;
						SpawnTwink(shotX[i]-4-(oddCamX<<1)+2, shotY[i]-8);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else if (*(mapMainLayer+tileindex+239) == tileinfo.shootableTile)
					{
						*(mapMainLayer+tileindex+239) = 255;
						SpawnTwink(shotX[i]-4-(oddCamX<<1)+2, shotY[i]+8);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else if (*(mapMainLayer+tileindex-1) != 255 || *(mapMainLayer+tileindex) != 255)
					{
						SpawnParticles((shotX[i]<<2)+1, shotY[i]+2);
						shotX[i] = -99;
						playerShotsActive--;
					}
					else
					{
						if (shotFlyUp[i])	//always false here
						{
							shotY[i] -= 16;	//unreachable
						}
						else
						{
							shotX[i] -= 4;
						}
						if (shotX[i] < -3 || shotY[i] < 0)
						{
							shotX[i] = -99;
							playerShotsActive--;
						}
						else
						{
							if (shotType[i])
							{
								DrawSpriteClipped(SPR_PLAYER, spriteinfo[SPR_PLAYER].projectileWidth, shotX[i], shotY[i], shotDir[i], spriteinfo[SPR_PLAYER].projectileStart+2);
							}
							else
							{
								DrawSpriteClipped(SPR_PLAYER, spriteinfo[SPR_PLAYER].projectileWidth, shotX[i], shotY[i], shotDir[i], spriteinfo[SPR_PLAYER].projectileStart);
							}
							if (Random(10) > 2)
							{
								SpawnSnow(shotX[i], shotY[i]+5);
							}
						}
					}
				}
			}
			else	//shot is flying up!
			{
				if (shotDir[i] == 0)
				{
					tileindex = (shotY[i]/16 + camY)*240 + shotX[i]/4 + camX;
				}
				else
				{
					tileindex = (shotY[i]/16 + camY)*240 + shotX[i]/4 + camX + 1;
				}
				tileindex -= 240;
				if (tileindex < 0)
				{
					tileindex += 240;
				}
				if (*(mapMainLayer+tileindex-playerDir) == tileinfo.shootableTile)
				{
					*(mapMainLayer+tileindex-playerDir) = 255;
					SpawnTwink(shotX[i]-1-(oddCamX<<1), shotY[i]-15);
					shotX[i] = -99;
					playerShotsActive--;
				}
				else if (*(mapMainLayer+tileindex-playerDir+1) == tileinfo.shootableTile)
				{
					*(mapMainLayer+tileindex-playerDir+1) = 255;
					SpawnTwink(shotX[i]+3-(oddCamX<<1), shotY[i]-15);
					shotX[i] = -99;
					playerShotsActive--;
				}
				else if (*(mapMainLayer+tileindex) != 255)
				{
					SpawnParticles((shotX[i]<<2)+8, shotY[i]+2);
					shotX[i] = -99;
					playerShotsActive--;
				}
				else
				{
					if (shotFlyUp[i])	//always true here
					{
						shotY[i] -= 16;
					}
					else
					{
						shotX[i] += 4;	//unreachable
					}
					if (shotX[i] > 79 || shotY[i] < 0)
					{
						shotX[i] = -99;
						playerShotsActive--;
					}
					else
					{
						if (shotType[i])
						{
							DrawSpriteClipped(SPR_PLAYER, spriteinfo[SPR_PLAYER].projectileWidth, shotX[i], shotY[i], shotDir[i], spriteinfo[SPR_PLAYER].projectileStart+3);
						}
						else
						{
							DrawSpriteClipped(SPR_PLAYER, spriteinfo[SPR_PLAYER].projectileWidth, shotX[i], shotY[i], shotDir[i], spriteinfo[SPR_PLAYER].projectileStart+1);
						}
						if (Random(10) > 2)
						{
							SpawnSnow(shotX[i]+(playerDir<<1), shotY[i]+5);
						}
					}
				}
			}
		}	// end of "for (i=0; i<MAX_SHOTS; i++)
		
		if (!teleportCounter)
		{
			if (jumpKeyDown && !playerJumping && !camScrolled && !playerFalling)
			{
				if (demoMode == DEMO_RECORD)
				{
					demoKeys.jump = true;
				}
				PlaySound(SND_HITFLOOR);
				jumpKeyDown = 0;
				playerJumping = 1;
				playerJumpIndex = 0;
				if (!superJump)
				{
					jumpDeltaYs = playerDeltaYs;
					maxJumpIndex = 9;
					maxDeltaYIndex = 19;
				}
				else
				{
					superJump = false;
					jumpDeltaYs = playerDeltaYsHi;
					maxJumpIndex = 12;
					maxDeltaYIndex = 25;
					PlaySound(SND_TELEPORT);
				}
			}
			jumpEnded = 0;
			if (playerJumping && !playerDeathCountdown && !levelDoneCountdown)
			{
				if (playerJumpIndex < maxDeltaYIndex)
				{
					playerY += jumpDeltaYs[playerJumpIndex];
					playerTileY = playerY/16;
					tileindex = playerTileY*240 + (playerX+1)/2;
					if (mapMainLayer[tileindex] != 255)
					{
						playerJumpIndex = (maxDeltaYIndex-1)-playerJumpIndex;
						playerY += jumpDeltaYs[playerJumpIndex];
						playerTileY = playerY/16;
					}
					playerJumpIndex++;
					if (playerJumpIndex >= maxDeltaYIndex)
					{
						playerJumping = 0;
						jumpEnded = 1;
					}
				}
				else
				{
					playerJumping = 0;
					jumpEnded = 1;
				}
			}
			tileindex = playerTileY*240 + (playerX+1)/2 + 480;
			if (mapMainLayer[tileindex] != 255)
			{
				if (playerJumping && playerJumpIndex >= maxJumpIndex)
				{
					playerJumping = 0;
					playerY = playerTileY << 4;
					PlaySound(SND_HITFLOOR);
				}
				playerFalling = 0;
				if (wasFalling)
				{
					wasFalling = false;
					PlaySound(SND_HITFLOOR);
				}
			}
			else if (!playerJumping && !playerDeathCountdown && !levelDoneCountdown)
			{
				playerY += 16;
				if (*(mapMainLayer+tileindex+240) == 255)
				{
					playerFalling = 1;
					wasFalling = true;
				}
			}
			if (jumpEnded && !playerFalling)
			{
				PlaySound(SND_HITFLOOR);
			}
			//handle East/West movement:
			playerCanMoveW = playerCanMoveE = playerCanMoveNW = playerCanMoveNE = 0;
			tileindex = playerTileY*240 + (playerX/2);
			i = playerY % 16;
			if (!i)
			{
				if (*(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+240)==255)
				{
					playerCanMoveW = 1;
				}
				if (*(mapMainLayer+tileindex-240)==255 && *(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+240)!=255)
				{
					playerCanMoveNW = 1;
				}
				tileindex = playerTileY*240 + (playerX+2)/2;
				if (*(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+240)==255)
				{
					playerCanMoveE = 1;
				}
				if (*(mapMainLayer+tileindex-240)==255 && *(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+240)!=255)
				{
					playerCanMoveNE = 1;
				}
			}
			else
			{
				if (*(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+240)==255 && *(mapMainLayer+tileindex+480)==255)
				{
					playerCanMoveW = 1;
				}
				if (*(mapMainLayer+tileindex-240)==255 && *(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+240)!=255)
				{
					playerCanMoveNW = 1;
				}
				tileindex = playerTileY*240 + (playerX+2)/2;
				if (*(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+240)==255 && *(mapMainLayer+tileindex+480)==255)
				{
					playerCanMoveE = 1;
				}
				if (*(mapMainLayer+tileindex-240)==255 && *(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+240)!=255)
				{
					playerCanMoveNE = 1;
				}
			}
			if (playerFalling || playerJumping)
			{
				playerCanMoveNW = playerCanMoveNE = 0;
			}
			playerWalking = 0;
			if (leftKeyDown && playerX)
			{
				if (demoMode == DEMO_RECORD)
				{
					demoKeys.left = true;
				}
				if (!playerDir)
				{
					playerDir = 1;
				}
				else if (playerCanMoveW)
				{
					playerX--;
					playerWalking = 1;
					playerFrame++;
					if (playerFrame > spriteinfo[SPR_PLAYER].walkEnd)
						playerFrame = spriteinfo[SPR_PLAYER].walkStart;
				}
				else if (playerCanMoveNW)
				{
					playerX--;
					playerY -= 16;
					playerWalking = 1;
					playerFrame++;
					if (playerFrame > spriteinfo[SPR_PLAYER].walkEnd)
						playerFrame = spriteinfo[SPR_PLAYER].walkStart;
				}
			}
			if (rightKeyDown && playerX < 477)
			{
				if (demoMode == DEMO_RECORD)
				{
					demoKeys.right = 1;
				}
				if (playerDir)
				{
					playerDir = 0;
				}
				else if (playerCanMoveE)
				{
					playerX++;
					playerWalking = 1;
					playerFrame++;
					if (playerFrame > spriteinfo[SPR_PLAYER].walkEnd)
						playerFrame = spriteinfo[SPR_PLAYER].walkStart;
				}
				else if (playerCanMoveNE)
				{
					playerX++;
					playerY -= 16;
					playerWalking = 1;
					playerFrame++;
					if (playerFrame > spriteinfo[SPR_PLAYER].walkEnd)
						playerFrame = spriteinfo[SPR_PLAYER].walkStart;
				}
			}
			// handle elevators:
			if (!wizardActivated && elevatorTile1 != -1)
			{
				i = 0;	//not used in this branch
				tileindex = playerTileY*240 + (playerX+1)/2 + 480;
				if (*(mapMainLayer+tileindex) == elevatorTile1 || *(mapMainLayer+tileindex) == elevatorTile2)
				{
					if (*(mapMainLayer+tileindex) == elevatorTile2)
					{
						tileindex--;	//make sure tileindex points to the left tile of the elevator
					}
					if (upKeyDown || useKeyDown)
					{
						if (demoMode == DEMO_RECORD)
						{
							demoKeys.up = true;
						}
						tileindex -= 720;
						useKeyDown = false;
						useKeyUp = true;
						if (*(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+1)==255)
						{
							playerY -= 16;
							tileindex += 720;
							helpindex = tileindex-240;
							*(mapMainLayer+helpindex) = *(mapMainLayer+tileindex);
							*(mapMainLayer+tileindex) = 255;
							*(mapMainLayer+helpindex+1) = *(mapMainLayer+tileindex+1);
							*(mapMainLayer+tileindex+1) = 255;
							upKeyCounter = 0;
						}
					}
					else if (downKeyDown)
					{
						//BUG? this doesn't write anything to the demo data
						tileindex += 240;
						if (*(mapMainLayer+tileindex)==255 && *(mapMainLayer+tileindex+1)==255)
						{
							playerY += 16;
							helpindex = tileindex-240;
							*(mapMainLayer+tileindex) = *(mapMainLayer+helpindex);
							*(mapMainLayer+helpindex) = 255;
							*(mapMainLayer+tileindex+1) = *(mapMainLayer+helpindex+1);
							*(mapMainLayer+helpindex+1) = 255;
							downKeyCounter = 0;
						}
					}
				}
			}
			if (!playerWalking && !playerFiring)
			{
				playerFrame = spriteinfo[SPR_PLAYER].standFrame;
			}
		}
		else	//teleportCounter is not 0!
		{
			// handle teleport:
			if (teleportEndX == playerX && teleportEndY == playerY)
			{
				if (teleportCounter == 25)
				{
					PlaySound(SND_TELEPORT);
				}
				if (teleportCounter == 40)
				{
					teleportCounter = 0;
				}
				else
				{
					teleportCounter++;
				}
			}
			else if (teleportCounter < 25)
			{
				teleportCounter++;
			}
			if (teleportCounter == 25)
			{
				if (playerY < teleportEndY)
				{
					playerY += 16;
				}
				if (playerY > teleportEndY)
				{
					playerY -= 16;
				}
				if (playerX < teleportEndX)
				{
					playerX++;
				}
				if (playerX > teleportEndX)
				{
					playerX--;
				}
			}
		}
		// handle looking up/down:
		if (upKeyDown)
		{
			upKeyCounter++;
			if (upKeyCounter > 10)
			{
				upKeyCounter = 10;
			}
		}
		else
		{
			upKeyCounter = 0;
		}
		if (downKeyDown)
		{
			downKeyCounter++;
			if (downKeyCounter > 10)
			{
				downKeyCounter = 10;
			}
		}
		else
		{
			downKeyCounter = 0;
		}
		if ((
#ifdef VERSION_11
			joyBtn3Down ||
#endif
			pgdnKeyDown || downKeyCounter) && lookDeltaY > 0 && zero_0 == 0)
		{
			lookDeltaY--;
			zero_0 = 0;
		}
		if ((
#ifdef VERSION_11
			joyBtn4Down ||
#endif
			pgupKeyDown || upKeyCounter) && lookDeltaY < 8 && zero_0 == 0)
		{
			lookDeltaY++;
			zero_0 = 0;
		}
		if (
#ifdef VERSION_11
			!joyBtn3Down && !joyBtn4Down &&
#endif
			!pgdnKeyDown && !pgupKeyDown && upKeyCounter != 10 && downKeyCounter != 10)
		{
			lookDeltaY = 5;
		}
		// handle hotkeys:
		if (keyHitP && !delaytime)
		{
			keyHitP = false;
			ShowPaused();
			delaytime = 2;
		}
		if (demoMode == DEMO_PLAY && demoIndex >= demoSize)
		{
			levelEnded = end_ended;
		}
		if (keyDownESC && !delaytime)
		{
			delaytime = 2;
			if (demoMode > DEMO_OFF)
			{
				levelEnded = end_ended;
			}
			else
			{
				i = IngameMenu();
				if (i == 1)	// Abandon level & restart
				{
					levelDoneCountdown = playerDeathCountdown = 0;
					levelEnded = end_restart;
				}
				if (i == 2)	// Quit to Main Menu
				{
					result = -1;
					levelEnded = end_ended;
				}
				if (i == 3)	// loaded a game
				{
					levelDoneCountdown = playerDeathCountdown = 0;
					levelEnded = end_loaded;
				}
			}
		}
		if (keyHitF2)
		{
			IngameSave();
			keyHitF2 = false;
		}
		if (keyHitF3)
		{
			if (IngameRestore())
			{
				KB_Shutdown();
				goto restart;
			}
			keyHitF3 = false;
		}
		if (keyHitF10)
		{
			if (IngameQuit())
			{
				result = -1;
				levelEnded = end_ended;
			}
			keyHitF10 = false;
		}
		if (keyHitF1)
		{
			ShowHelp(1);
			keyHitF1 = false;
		}
		if (keyHitC)
		{
			if (config.controlMode)
			{
				IngameCalibrate();
			}
			keyHitC = false;
		}
		if (keyHitV)
		{
			IngameVolume();
			keyHitV = false;
		}
		if (keyHitM && !delaytime)
		{
			delaytime = 2;
			keyHitM = false;
			config.musicOn = !config.musicOn;
			if (useMusic || useDigitized)
			{
				if (!config.musicOn)
				{
					StopMusic();
					ShowMessage("Music is now off");
				}
				else
				{
					ShowMessage("Music is now on");
					PlayMusicChunk(MUSIC01_MID + levelMusicNumbers[episode][level], true);
				}
			}
			if (!useMusic && !useDigitized)
			{
				config.musicOn = 0;
			}
			WriteToFile(configPath, &config, sizeof(config));
		}
		if (keyHitS && !delaytime)
		{
			delaytime = 2;
			keyHitS = false;
			config.soundOn = !config.soundOn;
			WriteToFile(configPath, &config, sizeof(config));
			if (config.soundOn)
			{
				ShowMessage("Sound effects are now on");
			}
			else
			{
				ShowMessage("Sound effects are now off");
			}
		}
		// misc stuff:
		if (playerDeathCountdown == 1)
		{
			levelEnded = end_ended;
		}
		if (levelDoneCountdown == 1) //natural end of the countdown
		{
			levelEnded = end_ended;
		}
		else if (levelDoneCountdown < 0) //in response to a cheat code
		{
			levelDoneCountdown = 0; //don't keep skipping future levels too
			levelEnded = end_ended;
			result = 1;
		}
		if (demoMode == DEMO_RECORD)
		{
			if (useKeyDown)
			{
				demoKeys.up = true;
			}
			memcpy(buffer_64000+6*demoIndex+2, &demoKeys, 6);
			demoIndex++;
		}
		
	}	// end of "while (levelEnded == end_not)"
	
	
	KB_Shutdown();
	levelTime = pcTickCount;
	if (demoMode == DEMO_RECORD)
	{
		*((Uint16 *)buffer_64000) = demoIndex;
		WriteToFile("demo19.lvl", buffer_64000, demoIndex*6+2);
	}
	FadeToBlack(20);
	if (levelEnded != end_restart)
	{
		LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
		LoadPaletteChunk(MENUPAL_PAL, 128, 0);
		PlayMusicChunk(MUSIC03_MID, true);
	}
	if (levelDoneCountdown == 1)
	{
		if (IntermissionScreen(1, treasureTotal, treasureFound) == 1)
		{
			levelEnded = end_restart;	//unreachable - IntermissionScreen() ALWAYS returns 0
		}
		result = 1;
	}
	if (playerDeathCountdown == 1)
	{
		IntermissionScreen(0, treasureTotal, treasureFound);
		result = 0;
	}
	if (levelEnded == end_restart)
	{
		score = oldscore;
		playerFirepower = 1;	//redundant (in v1.1) - is also set at label 'restart'
		goto restart;
	}
	if (levelEnded == end_loaded)
	{
		goto restart;
	}
	fclose(_unknown_fp);	//useless - the variable is not used anywhere else
	return result;
}