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

#include <ALLOC.H>
#include <CONIO.H>
#include <DOS.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H> //memcmp, memmove

#include "HP_DEFS.H"
#include "HP_MAIN.H"
#include "HP_GRAPH.H"
#include "HP_FILES.H"
#include "HP_MENU.H"
#include "HP_GAME.H"
#include "HP_SOUND.H"
#include "HP_SYS.H"
#include "HP_COPY.H"

/*
=============================================================================

	EXTERN DECLARATIONS (see note in HP_GAME.H)

=============================================================================
*/

extern Sint16 playerDeathCountdown;
extern Sint16 levelDoneCountdown;
extern Sint16 gotKey1;
extern Sint16 gotKey2;
extern Sint16 rapidFireCountdown;
extern Sint16 oldFirepower;
extern Sint16 powershotsLeft;
extern Sint16 tick1;
extern Sint16 tick5;


//////////////////////////
// Initialized Variables:
//////////////////////////

Uint16 pcTickRate = 140;

////////////////////////////
// Uninitialized Variables:
////////////////////////////

void interrupt (*oldInt9)(void);
char cheatcode[8]; //no cheat code is more than 8 bytes long, but this compiler won't support 64-bit ints
Uint16 joyDirX, joyDirY;

//cheat codes, for memcmp:
const char CHEAT_FEELGOOD[] = { KEY_F, KEY_E, KEY_E, KEY_L, KEY_G, KEY_O, KEY_O, KEY_D };
const char CHEAT_BLAKE[] = { KEY_B, KEY_L, KEY_A, KEY_K, KEY_E };
const char CHEAT_BANANA[] = { KEY_B, KEY_A, KEY_N, KEY_A, KEY_N, KEY_A };
const char CHEAT_QUARK[] = { KEY_Q, KEY_U, KEY_A, KEY_R, KEY_K };

/*
=============================================================================

	FUNCTIONS

=============================================================================
*/

void PC_Service(void)
{
#ifdef FULL_PC_SOUNDS
	static Uint16 counter=0;	//560 Hz
	if ((++counter & 3) == 0)	//560 Hz
	{
		pcTickCount++;	//still increases at 140 Hz!
	}
#else
	pcTickCount++;
#endif
	if (pcSoundNumber != -1)
	{
		sound(pcSounds[pcSoundNumber][pcSoundIndex]);
#ifdef FULL_PC_SOUNDS
		pcSoundIndex += 1;	//560 Hz
#else
		pcSoundIndex += 4;	//140 Hz
#endif
		if (pcSoundIndex >= pcSoundLengths[pcSoundNumber])
		{
			pcSoundNumber = -1;
			pcPlaying = false;
			nosound();
		}
	}
}

void PC_Startup(void)
{
#ifdef FULL_PC_SOUNDS
	pcTask = TS_ScheduleTask(PC_Service, 140*4, 1, &pcTickCount);
#else
	pcTask = TS_ScheduleTask(PC_Service, 140, 1, &pcTickCount);
#endif
	TS_Dispatch();
}

void PC_Shutdown(void)
{
	TS_Terminate(pcTask);
#ifdef VERSION_11
	TS_Shutdown();
#endif
	// NOTE: Calling TS_Shutdown() isn't really necessary, it's added as an
	// atexit function and will be run when exit() is called or main() returns.
}

void SleepMilliSecs(Sint16 millis)
{
	Uint32 startCount;
	
	millis = ((millis / 1000.0) * pcTickRate);	//using ((Sint32)millis*(Sint32)pcTickRate)/1000L would work, too (32bit integer instead of float arithmetics - should be a no-brainer)
	startCount = pcTickCount;
	while (millis > pcTickCount-startCount);
	
	return;
}

Sint16 WaitMilliSecs(Sint16 millis)
{
	/*register*/ Sint16 hit;
	Uint32 startCount;

	hit = false;
	millis = ((millis / 1000.0) * pcTickRate);	//using ((Sint32)millis*(Sint32)pcTickRate)/1000L would work, too (32bit integer instead of float arithmetics - should be a no-brainer)
	startCount = pcTickCount;
	while (millis > pcTickCount-startCount && !kbhit() && HandleJoystick() != JOY_BTN1)
		WaitVBL();
	while (kbhit())
	{
		getch();
		hit = true;
	}
	// BUG: hit is still false when the delay was aborted via joystick button 1
	// (but the result is never used anyway)
	return hit;
}

void ClearKeys(void)
{
	upKeyDown=
		useKeyDown=
		downKeyDown=
		leftKeyDown=
		rightKeyDown=
		fireKeyDown=
		jumpKeyDown=
		keyDownESC=
		pgupKeyDown=
		pgdnKeyDown= false;
		
	useKeyUp=
		jumpKeyUp=
		fireKeyUp= true;
		
	keyHitF2=
		keyHitF3=
		keyHitF10=
		keyHitF1=
		keyHitC=
		keyHitV= false;
	keyHitP = false;
}

void ClearKeyboardBuffer(void)	//unused, not sure what this really does
{
	union REGS regs;
	
	regs.h.ah = 0xC;
	regs.h.al = 0;
	int86(0x21, &regs, &regs);
}

void ClearCheatCode(void) {
	cheatcode[sizeof(cheatcode) - 1)] = 0; //only need to null the most recently typed letter
}

void KB_Startup(void)
{
	Uint8 *flags1 = MK_FP(0, 0x417), *flags2 = MK_FP(0, 0x418);

	asm	pushf;
	asm	cli;

	oldInt9 = getvect(9);
	setvect(9, KB_Service);
	ClearCheatCode();
	ClearKeys();
	*flags1 = 0;
	*flags2 = 0;

	asm	popf;
}

void KB_Shutdown(void)
{
	asm	pushf;
	asm	cli;

	setvect(9, oldInt9);

	asm	popf;
}

void interrupt KB_Service(void)
{
	register Uint8 val;
	
	lastKey = inp(0x60);
	val = inp(0x61);
	outp(0x61, val|0x80);
	outp(0x61, val);
	if (config.controlMode == 0 && !playerDeathCountdown && !levelDoneCountdown && !playingDemo)
	{
		if (lastKey < 0x80)
			keyHit = true;
		if (lastKey == kbSpecialKeys[controlKeys[4]] && useKeyUp != false)
		{
			upKeyDown = true;
			useKeyDown = true;
			useKeyUp = false;
		}
		if (lastKey == kbSpecialKeys[controlKeys[4]]+0x80)
		{
			upKeyDown = false;
			useKeyDown = false;
			useKeyUp = true;
		}
		if (lastKey == kbSpecialKeys[controlKeys[5]])
		{
			downKeyDown = true;
		}
		if (lastKey == kbSpecialKeys[controlKeys[5]]+0x80)
		{
			downKeyDown = false;
		}
		if (lastKey == kbSpecialKeys[controlKeys[0]])
		{
			leftKeyDown = true;
		}
		if (lastKey == kbSpecialKeys[controlKeys[0]]+0x80)
		{
			leftKeyDown = false;
		}
		if (lastKey == kbSpecialKeys[controlKeys[1]])
		{
			rightKeyDown = true;
		}
		if (lastKey == kbSpecialKeys[controlKeys[1]]+0x80)
		{
			rightKeyDown = false;
		}
		if (lastKey == kbSpecialKeys[controlKeys[2]] && jumpKeyUp != 0)
		{
			jumpKeyDown = true;
			jumpKeyUp = false;
		}
		if (lastKey == kbSpecialKeys[controlKeys[2]]+0x80)
		{
			jumpKeyDown = false;
			jumpKeyUp = true;
		}
		if (lastKey == kbSpecialKeys[controlKeys[3]] && fireKeyUp)
		{
			fireKeyDown = true;
			fireKeyUp = false;
		}
		if (lastKey == kbSpecialKeys[controlKeys[3]]+0x80)
		{
			fireKeyDown = false;
			fireKeyUp = true;
		}
	}
	//handle cheat codes:
	if (lastKey && !(lastKey & 0x80)) { //key down, not key up
		memmove(cheatcode, cheatcode + 1, sizeof(cheatcode) - 1); //move the last seven letters, one byte to the left
		cheatcode[sizeof(cheatcode) - 1] = lastKey; //most recently typed letter is at the far right

#define COMPARE_CHEAT(x) !memcmp(cheatcode + (sizeof(cheatcode) - sizeof(x)), x, sizeof(x))
		if (COMPARE_CHEAT(CHEAT_FEELGOOD)) {
			ClearCheatCode();
			playerHealth = 100;
		} else if (COMPARE_CHEAT(CHEAT_BLAKE)) { //compares the rightmost (most recently typed) five bytes of the cheatcode array
			gotKey1 = true;
			gotKey2 = true;
		}
#if !SHAREWARE
		else if (COMPARE_CHEAT(CHEAT_QUARK)) {
			if (!rapidFireCountdown)
			{
				oldFirepower = playerFirepower;
				playerFirepower = 5;
			}
			rapidFireCountdown = 600;
		} else if (COMPARE_CHEAT(CHEAT_BANANA)) {
			if (!powershotsLeft)
				powershotsLeft = 3;
			tick1 = 1;
			tick5 = 5;
		}
#endif
#undef COMPARE_CHEAT
	}
	
	//handle hotkeys:
	if (lastKey == KEY_P)
		keyHitP = true;
	
	if (lastKey == kbSpecialKeys[controlKeys[C_SCROLLUP]])
		pgupKeyDown = true;
	if (lastKey == kbSpecialKeys[controlKeys[C_SCROLLUP]]+0x80)
		pgupKeyDown = false;
	if (lastKey == kbSpecialKeys[controlKeys[C_SCROLLDOWN]])
		pgdnKeyDown = true;
	if (lastKey == kbSpecialKeys[controlKeys[C_SCROLLDOWN]]+0x80)
		pgdnKeyDown = false;
	if (lastKey == KEY_ESCAPE)
		keyDownESC = true;
	if (lastKey == KEY_ESCAPE+0x80)
		keyDownESC = false;
	
	if (lastKey == KEY_F2)
		keyHitF2 = true;
	if (lastKey == KEY_F3)
		keyHitF3 = true;
	if (lastKey == KEY_F10)
		keyHitF10 = true;
	if (lastKey == KEY_F1)
		keyHitF1 = true;
	if (lastKey == KEY_C)
		keyHitC = true;
	if (lastKey == KEY_M)
		keyHitM = true;
	if (lastKey == KEY_S)
		keyHitS = true;
	if (lastKey == KEY_V)
		keyHitV = true;
	
	outp (0x20, 0x20);
}

void KB_TestKeycodes(void)
{
	KB_Startup();
	lastKey = 0;
	for (;;)
	{
		if (lastKey == KEY_ESCAPE)
		{
			break;
		}
		printf("%d\n", lastKey);
	}
	KB_Shutdown();
}

Sint16 Random(Sint16 val)
{
	rand_index++;
	if (rand_index > 999)
		rand_index = 0;
	return rand_table[rand_index] % val;
}

void Quit(void)
{
	void *screen = MK_FP(0xB800, 0);
	
	OpenDatFile();
	SetTextMode();
	clrscr();	//is this required for gotoxy to work properly?
	gotoxy(1, 24);
	GetPalette(currPalette, 0, 0x100);
	SetToBlack();
#if SHAREWARE
	ReadDatChunk(DOSSHARE_BIN, buffer_64000);
#else
	ReadDatChunk(DOSREG_BIN, buffer_64000);
#endif
	CopyBytes(screen, buffer_64000, 4000);
	FadeFromBlack(30);
	ShutdownSound();
	CloseDatFile();
	PC_Shutdown();
}

void OutOfMemory(void)
{
	SetTextMode();
	clrscr();	//is this required for gotoxy to work properly?
	ShutdownSound();
	PC_Shutdown();
	printf(
#ifdef VERSION_11
		"Not enough memory to run Hocus Pocus.\n"
#else
		"Not enough memory to run Hocus Pocus Version 1.0\n"
#endif
		"\n"
		"You need 580,000 (567 KB) to run Hocus Pocus. Try the following\n"
		"remedies to increase your amount of free memory.\n"
		"\n"
		"1) Do not run Hocus Pocus using a DOS Shell program.\n"
		"2) Load DOS into high memory. (DOS V5.0+)\n"
		"3) Remove any unused TSR programs or load them into high memory.\n"
		"4) Do not run Hocus Pocus from Windows.\n"
		"\n"
		"Please run HP-HELP for more information.\n"
		"\n"
		"Apogee Software");
	gotoxy(1, 24);
	exit(3);
}

void QuitMessage(char *message)	//never used
{
	ShutdownSound();
	SetTextMode();
	printf("%s\n", message);
	exit(3);
}

void QuitError(Sint16 errortype, char *info)
{
	ShutdownSound();
	SetTextMode();
	printf("%s [%s]\n", errorMessages[errortype], info);
	exit(3);
}

#pragma argsused
void AllocBufferB(Uint8 **buffptr, Uint32 size, char *str)
{
	if ((*buffptr = farmalloc(size)) == NULL)
		OutOfMemory();	//Note: I think this used to be QuitError(ERROR_OUT_OF_MEMORY, str); during development
}

#pragma argsused
void AllocBufferW(Sint16 **buffptr, Uint32 size, char *str)
{
	if ((*buffptr = farmalloc(size)) == NULL)
		OutOfMemory();	//Note: I think this used to be QuitError(ERROR_OUT_OF_MEMORY, str); during development
}

#pragma argsused
void AllocBufferD(Uint32 **buffptr, Uint32 size, char *str)	//never used
{
	if ((*buffptr = farmalloc(size)) == NULL)
		OutOfMemory();	//Note: I think this used to be QuitError(ERROR_OUT_OF_MEMORY, str); during development
}

void GetFatEntry(Uint16 number, Uint32 *offset, Uint32 *size)
{
	*offset = FAT[number].offset;
	*size = FAT[number].size;
}

void GetFatOffset(Uint16 number, Uint32 *offset)
{
	*offset = FAT[number].offset;
}

#define JOY_PORT 0x201

void ReadJoy(void)
{
	register Uint8 gotX, gotY;
	Uint8 val;
	
	if (config.controlMode)
	{
		joyCountX = joyCountY = 0;

		asm	pushf;
		asm	cli;
#if 1
		asm	mov	dx, JOY_PORT;
		asm	mov	al, 0;
		asm	out	dx, al;
		asm	in 	al, dx;
		asm	mov	val, al;
#else
		outp(JOY_PORT, 0);
		val = inp(JOY_PORT);
#endif

		if (val & 0x10)
			joyBtn1Down = false;
		else
			joyBtn1Down = true;
		if (val & 0x20)
			joyBtn2Down = false;
		else
			joyBtn2Down = true;
#ifdef VERSION_11
		if (val & 0x40)
			joyBtn3Down = false;
		else
			joyBtn3Down = true;
		if (val & 0x80)
			joyBtn4Down = false;
		else
			joyBtn4Down = true;
#endif
		
		do
		{
			// Note: This assumes that register dx doesn't change, which might not
			// always be the case here. Reset dx to JOY_PORT or use the version
			// without inline assembly to avoid problems.
#if 1
			asm	in 	al, dx;
			asm	mov	val, al;
#else
			val = inp(JOY_PORT);
#endif

			if (val & 1)
			{
				gotX = true;
				joyCountX++;
			}
			else
			{
				gotX = false;
			}
			if (val & 2)
			{
				gotY = true;
				joyCountY++;
			}
			else
			{
				gotY = false;
			}
#if FIX_JOY
			//bugfix: avoid endless loop in case no joystick is present
			if (joyCountX <= 0 || joyCountY <= 0)
				break;
#endif
		} while (gotX || gotY);

		asm	popf;

		joyStickLeft=
			joyStickRight=
			joyStickUp=
			joyStickDown= false;
		if (joyCountX <= config.joyLoX)
		{
			if (joyDirX == JOY_LEFT)
				joyStickLeft = true;
			else
				joyDirX = JOY_LEFT;
		}
		if (joyCountX >= config.joyHiX)
		{
			if (joyDirX == JOY_RIGHT)
				joyStickRight = true;
			else
				joyDirX = JOY_RIGHT;
		}
		if (joyCountY <= config.joyLoY)
		{
			if (joyDirY == JOY_UP)
				joyStickUp = true;
			else
				joyDirY = JOY_UP;
		}
		if (joyCountY >= config.joyHiY)
		{
			if (joyDirY == JOY_DOWN)
				joyStickDown = true;
			else
				joyDirY = JOY_DOWN;
		}
	}
}

void JoyCalibrateCentered(void)
{
	register Uint8 gotX;
	Uint8 val, gotY;
	
	if (config.controlMode)
	{
		joyCountX = joyCountY = 0;

		asm	pushf;
		asm	cli;
#if 1
		asm	mov	dx, JOY_PORT;
		asm	mov	al, 0;
		asm	out	dx, al;
		asm	in 	al, dx;
		asm	mov	val, al;
#else
		outp(JOY_PORT, 0);
		val = inp(JOY_PORT);
#endif

		if (val & 0x10)
			joyBtn1Down = false;
		else
			joyBtn1Down = true;
		if (val & 0x20)
			joyBtn2Down = false;
		else
			joyBtn2Down = true;
#ifdef VERSION_11
		if (val & 0x40)
			joyBtn3Down = false;
		else
			joyBtn3Down = true;
		if (val & 0x80)
			joyBtn4Down = false;
		else
			joyBtn4Down = true;
#endif

		do
		{
			// Note: This assumes that register dx doesn't change, which might not
			// always be the case here. Reset dx to JOY_PORT or use the version
			// without inline assembly to avoid problems.
#if 1
			asm	in 	al, dx;
			asm	mov	val, al;
#else
			val = inp(JOY_PORT);
#endif
			
			if (val & 1)
			{
				gotX = true;
				joyCountX++;
			}
			else
			{
				gotX = false;
			}
			if (val & 2)
			{
				gotY = true;
				joyCountY++;
			}
			else
			{
				gotY = false;
			}
#if FIX_JOY
			//bugfix: avoid endless loop in case no joystick is present
			if (joyCountX <= 0 || joyCountY <= 0)
				break;
#endif
		} while (gotX || gotY);

		asm	popf;

		config.joyMiX = joyCountX;
		config.joyLoX = joyCountX/3;
		config.joyHiX = joyCountX + joyCountX/3;	//BUG? center/high distance is only half the center/low distance
		config.joyMiY = joyCountY;
		config.joyLoY = joyCountY/3;
		config.joyHiY = joyCountY + joyCountY/3;	//BUG? center/high distance is only half the center/low distance
	}
	joyDirX = JOY_NOTHING;
	joyDirY = JOY_NOTHING;
	joyStickLeft=
		joyStickRight=
		joyStickUp=
		joyStickDown=
		joyBtn1Down=
		joyBtn2Down= false;
}
