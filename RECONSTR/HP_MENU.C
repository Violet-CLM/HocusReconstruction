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
#include <CTYPE.H>
#include <DOS.H>
#include <MEM.H>
#include <TIME.H>
#include <STDLIB.H>
#include <STRING.H>

#include "HP_MAIN.H"
#include "HP_GRAPH.H"
#include "HP_FILES.H"
#include "HP_MENU.H"
#include "HP_GAME.H"
#include "HP_SOUND.H"
#include "HP_SYS.H"
#include "HP_COPY.H"

//////////////////////////
// Initialized Variables:
//////////////////////////

#define LINE_COLOR(i) ((textpage.lineInfo+1)[(i)*2])
#define LINE_X(i) ((textpage.lineInfo+0)[(i)*2])

TTextPic storypics[10] =
{
	{PIC2_IMG,  4, 16},
	{PIC1_IMG, 50, 25},
	{PIC8_IMG,  4, 25},
	{PIC3_IMG,  4, 21},
	{PIC1_IMG, 50, 25},
	{      -1,  0,  0},
	{      -1,  0,  0},
	{      -1,  0,  0},
	{      -1,  0,  0},
	{PIC1_IMG, 50, 25}
};

TTextPic finale1pics[2] =
{
	{PIC7_IMG,  4, 21},
	{PIC1_IMG, 50, 25}
};

TTextPic finale2pics[2] =
{
	{PIC1_IMG, 50, 25},
	{PIC1_IMG, 50, 25}
};

TTextPic finale3pics[4] =
{
	{PIC6_IMG,  4, 42},
	{PIC1_IMG, 50, 25},
	{      -1,  0,  0},
	{PIC1_IMG, 50, 25}
};

TTextPic finale4pics[2] =
{
	{PIC5_IMG,  4, 19},
	{      -1,  0,  0}
};

Sint16 parTimes[4][9] =
{
	{150, 300, 400, 350, 350, 350, 420, 350, 400},
	{325, 275, 300, 350, 400, 350, 400, 375, 250},
	{525, 450, 520, 475, 525, 450, 550, 450, 300},
	{480, 375, 350, 300, 400, 350, 450, 400, 180}
};

Sint16 menuCursorTick = 0;
static Sint16 _dummy4 = 0;
Sint16 newMenuPos = 0;
Sint16 saveMenuPos = 0;
Sint16 restoreMenuPos = 0;

char *menuBottomLines[10] =
{
	"Use UP/DOWN/LETTER to move - ENTER to select",
	"Use UP/DOWN/LETTER to move - ENTER to select",
	"Press any key to continue",
	"Press Y for yes - N for no - ESC to exit",
	"Use UP/DOWN/NUMBER to move - ENTER to select",
	"ESC to exit",
	"Enter name then press ENTER",
	"Press LETTER to select - ESC to exit",
	"Use UP/DOWN to move - ENTER to select",
	"UP/DOWN/LEFT/RIGHT to move - ENTER when done"
};

Sint16 mainMenuPos = 0;
char *mainMenuLines[] =
{
	"Begin a new game",
	"Restore an old game",
	"Ordering Information~",
	"Instructions",
#if SHAREWARE
	"Legends and hints!~",
#else
	"Legends and hints~",
#endif
	"Change game options",
	"High scores",
#if SHAREWARE
	"Preview future levels~",
#endif
	"Quit - return to DOS"
#if 0
	///////////////
	//HACK:
	"~","Warp to a level"
	,"Demo Recorder"
	// end of hack (code to handle these options must be enabled in 'main')
	///////////////
#endif
};

Sint16 episodeMenuPos = 0;
char *episodeMenuLines[] =
{
	"Which game do you want to play?",
	"Time Tripping",
	"Shattered Worlds",
	"Warped and Weary",
	"Destination Home"
};

// Note: The Sound, Music and Joystick lines are being modified by InitMenu,
// which means you have to make sure the initial strings in here are long enough
// to store whatever the code will insert. This is potentially unsafe and will
// be a problem when porting the code to modern systems because the strings
// will most likely be "const" and cannot be modified. You might need to turn
// all the string arrays from 'char *' into 'const char *' to compile this code
// in the first place. This, however, will make it impossible to modify the
// menu strings in InitMenu. The solution would be to move the strings that
// need to be modified out of the initialization and into separate arrays of
// characters that are safe to modify.
#if 0

Sint16 configMenuPos = 0;
char soundline[] = "Sound is now off";
char musicline[] = "Music is now off";
char joystickline[] = "Joystick is now off";
// Note: InitMenu will need to modify soundline, musicline and joystickline
// instead of menuLines[0], menuLines[1] and menuLines[3] to make this work.
char *configMenuLines[] =
{
	soundline,
	musicline,
	"Volume control",
	joystickline,
	"Game playing speed",
	"Define key controls"
};

#else

Sint16 configMenuPos = 0;
char *configMenuLines[] =
{
	"Sound is now off",
	"Music is now off",
	"Volume control",
	"Joystick is now off",
	"Game playing speed",
	"Define key controls"
};
#endif

Sint16 levelMenuPos = 0;
char *levelMenuLines[] =
{
	"Which level do you want to play?",
	"1) Level #1",
	"2) Level #2",
	"3) Level #3",
	"4) Level #4",
	"5) Level #5",
	"6) Level #6",
	"7) Level #7",
	"8) Level #8",
	"9) Boss Level!"
};

Sint16 skillMenuPos = 1;
char *skillMenuLines[] =
{
	"Choose a skill level",
	"Easy game - good for beginners",
	"Moderate - a resonable challenge",
	"Hard - the ultimate battle!"
#if 0
//HACK:
	"~","GODMODE"	// works without any extra code (last entry in 'damage' array is 0)
#endif
};

Sint16 helpMenuPos = 0;
char *helpMenuLines[] =
{
	"How to play Hocus Pocus",
	"Abandon level & restart",
	"Save this game",
	"Restore an old game",
	"Change game options",
	"Back to the action!",
	"Quit to Main Menu"
};

char *controlNames[NUMCONTROLS] =
{
	"Left",
	"Right",
	"Jump",
	"Fire",
	"Up",
	"Down",
	"Scroll up",
	"Scroll down",
};

Sint16 speedMenuPos = 1;
char *speedMenuLines[4] =
{
	"Select Game Speed",
	"Slow",
	"Medium",
	"Fast"
};

void *_unusedPtr = MK_FP(0xB800, 0);

Uint16 gradientBaseColors[9] =
{
	0xC0,
	0xC8,
	0xD0,
	0xD8,
	0xE0,
	0xE8,
	0x70,
	0x68,
	0xD0
};

Uint16 menuJoyDelay = 0;

////////////////////////////
// Uninitialized Variables:
////////////////////////////

Uint8 backupPalette[0x300];
Sint16 menuCursorX;
Sint16 menuCursorYs[20];	//only 10 slots required since there are only 10 slots in menuLines
Sint16 menuPos;
Sint16 menuNumOptions;
Sint16 menuNumber;

TTextPage textpage;

char *menuLines[10];
Sint16 menuNumLines;
Sint16 menuBottomIndex;
Sint16 menuHasHeadline;

Uint16 *textscreenPtr;

#define NUMSTARS 75
Uint16 starAngle[NUMSTARS];
Sint16 starMoved[NUMSTARS];
Uint8 starSpeed[NUMSTARS];
Uint8 starBrightness[NUMSTARS];
Uint8 starDeltaBright[NUMSTARS];

//////////////
// Functions:
//////////////

Uint16 HandleJoystick(void)
{
	register Uint16 result = JOY_NOTHING;
	
	if (config.controlMode)
	{
		if (!menuJoyDelay)
		{
			ReadJoy();
			if (joyStickRight)
				result = JOY_RIGHT;
			if (joyStickLeft)
				result = JOY_LEFT;
			if (joyStickUp)
				result = JOY_UP;
			if (joyStickDown)
				result = JOY_DOWN;
			if (joyBtn1Down)
				result = JOY_BTN1;
			if (joyBtn2Down)
				result = JOY_BTN2;
			if (result != JOY_NOTHING)
				menuJoyDelay = 10;
			else
				menuJoyDelay = 0;
		}
		else
		{
			menuJoyDelay--;
		}
	}
	return result;
}

void DrawMenuCursor(Sint16 x, Sint16 y)
{
	//Note: this ALWAYS draws to page 0!
	register Sint16 ix, iy;
	Sint16 cursorOff = (menuCursorTick/5) << 2;
	
	SetWriteMode(1);
	SetPlaneMask(0xF);
	for (iy=0; iy<15; iy++)
	{
		for (ix=0; ix<4; ix++)
		{
			//change screenpagePtr[0] to drawPtr if you want to draw to other pages
			(screenpagePtr[0]+(iy+y)*80+x)[ix] = (screenpagePtr[3]+(iy+185)*80+cursorOff+ix)[28];
		}
	}
	SetWriteMode(0);
	menuCursorTick++;
	if (menuCursorTick >= 40)
		menuCursorTick = 0;
}

void DrawStarfield(void)
{
	register Sint16 i, x;
	Sint16 y;
	
	for (i=0; i<NUMSTARS; i++)
	{
//		x = ((starMoved[i] * sin_cos_table[starAngle[i]].sin)/FIXED_FACTOR) + 160;
//		y = ((starMoved[i] * sin_cos_table[starAngle[i]].cos)/FIXED_FACTOR) + 112;
		x = SIN_MUL(starMoved[i], starAngle[i]) + 160;
		y = COS_MUL(starMoved[i], starAngle[i]) + 112;
		if (ReadPixel(x, y) >= 0xF0)
			DrawPixel(x, y, 0);
		starMoved[i] += starSpeed[i];
		starBrightness[i] += starDeltaBright[i];
		if (starBrightness[i] > 127)
			starBrightness[i] = 127;
//		x = ((starMoved[i] * sin_cos_table[starAngle[i]].sin)/FIXED_FACTOR) + 160;
//		y = ((starMoved[i] * sin_cos_table[starAngle[i]].cos)/FIXED_FACTOR) + 112;
		x = SIN_MUL(starMoved[i], starAngle[i]) + 160;
		y = COS_MUL(starMoved[i], starAngle[i]) + 112;
		if (x < 0 | x > 319 | y < 0 | y > 199)	//using || instead of | would make this more efficient
		{
			do {
				starAngle[i] = Random(360);
				starMoved[i] = Random(160);
//				x = ((starMoved[i] * sin_cos_table[starAngle[i]].sin)/FIXED_FACTOR) + 160;
//				y = ((starMoved[i] * sin_cos_table[starAngle[i]].cos)/FIXED_FACTOR) + 112;
				x = SIN_MUL(starMoved[i], starAngle[i]) + 160;
				y = COS_MUL(starMoved[i], starAngle[i]) + 112;
			} while (x < 0 | x > 319 | y < 0 | y > 199);	//same as above
			starSpeed[i] = Random(5)+1;
			starBrightness[i] = 0;
			starDeltaBright[i] = Random(5)+1;
		}
		if (ReadPixel(x, y) == 0)
		{
			DrawPixel(x, y, (starBrightness[i] >> 3)+0xF0);	//BUG: some PCX images have colors 0xF0-0xFE set to black, so only the brightest stars are actually visible!
		}
	}
	WaitVBL();
}

void InitStarfield(void)
{
	register Sint16 i, x;
	Sint16 y;
	
	srand(time(NULL));	//useless because this uses Random() instead of rand()
	for (i=0; i<NUMSTARS; i++)
	{
		do {
			starAngle[i] = Random(360);
			starMoved[i] = Random(160);
//			x = ((starMoved[i] * sin_cos_table[starAngle[i]].sin)/FIXED_FACTOR) + 160;
//			y = ((starMoved[i] * sin_cos_table[starAngle[i]].cos)/FIXED_FACTOR) + 112;
			x = SIN_MUL(starMoved[i], starAngle[i]) + 160;
			y = COS_MUL(starMoved[i], starAngle[i]) + 112;
		} while (x < 0 | x > 319 | y < 0 | y > 199);	//same as above
		starSpeed[i] = Random(5)+1;
		starBrightness[i] = 0;
		starDeltaBright[i] = Random(5)+1;
	}
}

void DrawText_P(Sint16 x, Sint16 y, Uint16 color, char *text)	//Plain color, used for drawing outlines
{
	/*register*/ Sint16 n;
	unsigned char c;
	Uint8 pix, mask;
	Sint16 i, width, plane;
	Uint16 pixoff, maskShifted;
	
	SetWriteMode(0);
	outp(0x3C4, 2);
	for (; *text; text++)
	{
		c = *text;
		if (c >= '!' && c <= 'z')
		{
			c = c - '!';
			width = 0;
			pixoff = y*80 + (x >> 2);
			for (i=0; i<8; i++)
			{
				pix = *(fontmask+(c << 3)+i);
				mask = pix;
				for (n=0; n<8; n++)
				{
					if ((pix & 1) && n > width)
						width = n;
					pix >>= 1;
				}
				plane = x % 4;
				maskShifted = mask;
				maskShifted <<= 8;
				maskShifted >>= 4-plane;
				outp(0x3C5, (maskShifted >> 4));
				*(drawPtr+pixoff) = color;
				outp(0x3C5, (maskShifted >> 8));
				*(drawPtr+pixoff+1) = color;
				outp(0x3C5, (maskShifted >> 12));
				*(drawPtr+pixoff+2) = color;
				pixoff += 80;
			}
			x += width +2;
		}
		else if (c == ' ')
		{
			x += 4;
		}
	}
}

void DrawText(Sint16 x, Sint16 y, Uint16 color, char *text)
{
	/*register*/ Sint16 n;
	unsigned char c;
	Uint8 pix, mask;
	Sint16 i, width, plane;
	Uint16 pixoff, maskShifted;
	
	SetWriteMode(0);
	outp(0x3C4, 2);
	for (; *text; text++)
	{
		c = *text;
		if (c >= '!' && c <= 'z')
		{
			c = c - '!';
			width = 0;
			pixoff = y*80 + (x >> 2);
			for (i=0; i<8; i++)
			{
				pix = *(fontmask+(c << 3)+i);
				mask = pix;
				for (n=0; n<8; n++)
				{
					if ((pix & 1) && n > width)
						width = n;
					pix >>= 1;
				}
				plane = x % 4;
				maskShifted = mask;
				maskShifted <<= 8;
				maskShifted >>= 4-plane;
				outp(0x3C5, (maskShifted >> 4));
				*(drawPtr+pixoff) = color;
				outp(0x3C5, (maskShifted >> 8));
				*(drawPtr+pixoff+1) = color;
				outp(0x3C5, (maskShifted >> 12));
				*(drawPtr+pixoff+2) = color;
				pixoff += 80;
			}
			x += width +2;
		}
		else if (c == ' ')
		{
			x += 4;
		}
	}
	SetWriteMode(1);	//only difference from DrawText_P
}

void DrawText_G(Sint16 x, Sint16 y, Uint16 color, char *text)	//Gradient
{
	/*register*/ Sint16 n;
	unsigned char c;
	Uint8 pix, mask, isBlack;
	Sint16 i, width, currColor, plane;
	Uint16 pixoff, maskShifted;
	
	SetWriteMode(0);
	outp(0x3C4, 2);
	if (!color)
	{
		isBlack = 1;
	}
	else
	{
		isBlack = 0;
		color--;
	}
	for (; *text; text++)
	{
		c = *text;
		if (c >= '!' && c <= 'z')
		{
			c = c - '!';
			width = 0;
			pixoff = y*80 + (x >> 2);
			for (i=0; i<8; i++)
			{
				if (isBlack)
				{
					currColor = 0x80;
				}
				else
				{
					currColor = gradientBaseColors[color]+i;
				}
				pix = *(fontmask+(c << 3)+i);
				mask = pix;
				for (n=0; n<8; n++)
				{
					if ((pix & 1) && n > width)
						width = n;
					pix >>= 1;
				}
				plane = x % 4;
				maskShifted = mask;
				maskShifted <<= 8;
				maskShifted >>= 4-plane;
				outp(0x3C5, (maskShifted >> 4));
				*(drawPtr+pixoff) = currColor;
				outp(0x3C5, (maskShifted >> 8));
				*(drawPtr+pixoff+1) = currColor;
				outp(0x3C5, (maskShifted >> 12));
				*(drawPtr+pixoff+2) = currColor;
				pixoff += 80;
			}
			x += width +2;
		}
		else if (c == ' ')
		{
			x += 4;
		}
	}
}

Sint16 TextWidth(char *text)
{
	Sint16 n, i, result;
	unsigned char c;
	Uint8 pix;
	Sint16 width;
	
	result = 0;
	
	for (; *text; text++)
	{
		c = *text;
		if (c >= '!' && c <= 'z')
		{
			c = c - '!';
			width = 0;
			for (i=0; i<8; i++)
			{
				pix = *(fontmask+(c << 3)+i);
				for (n=0; n<8; n++)
				{
					if ((pix & 1) && n > width)
						width = n;
					pix >>= 1;
				}
			}
			result += width +2;
		}
		else if (c == ' ')
		{
			result += 4;
		}
	}
	return result;
}

void DrawMenuItem(Sint16 x, Sint16 y, Uint16 color1, Uint16 color2, char *text)
{
#if 0
	//optimized version:
	char buf[2];
	buf[0] = text[0];
#else
	//original code:
	char buf[82];
	strcpy(buf, text);	//only the first char is used, so why copy the entire string?
#endif
	buf[1] = 0;
	DrawText_G(x, y, color2, text);
	DrawText_G(x, y, color1, buf);
}

void DrawText_GS(Sint16 x, Sint16 y, Uint16 color, char *text)	//Gradient with Shadow
{
	DrawText_G(x+1, y+1, 0, text);
	DrawText_G(x, y, color, text);
}

void DrawText_GOW(Sint16 x, Sint16 y, Uint16 color, char *text)	//Gradient with Outline = White(? color 0xFE) -- never used!
{
	DrawText_P(x+1, y, 0xFE, text);
	DrawText_P(x-1, y, 0xFE, text);
	DrawText_P(x, y+1, 0xFE, text);
	DrawText_P(x, y-1, 0xFE, text);
	DrawText_G(x, y, color, text);
}

void DrawText_GOB(Sint16 x, Sint16 y, Uint16 color, char *text)	//Gradient with Outline = Black
{
	DrawText_P(x+1, y, 1, text);
	DrawText_P(x-1, y, 1, text);
	DrawText_P(x, y+1, 1, text);
	DrawText_P(x, y-1, 1, text);
	DrawText_G(x, y, color, text);
}

void PutText(Sint16 x, Sint16 y, Uint16 textColor, Uint16 backColor, Uint16 maxlen, unsigned char *text)	//never used!
{
	register Sint16 offset, color;
	
	offset = y*80 +x;
	color = ((backColor << 4) | textColor) << 8;
	while (*text && maxlen--)
	{
		*(textscreenPtr+offset) = color | *text;
		text++;
		offset++;
	}
}

void ClearTextScreen(void)	//never used!
{
	setmem(textscreenPtr, 4000, 0);
}

void PutRect(Uint16 minX, Sint16 minY, Uint16 maxX, Sint16 maxY, Uint16 textColor, Uint16 backColor)	//never used!
{
	register Sint16 x;
	register Uint16 offset;
	register Uint16 color;
	
	color = ((backColor << 4) | textColor) << 8;
	
	for (offset = minY*80; minY<=maxY; minY++, offset += 80)
	{
		for (x=minX; x<=maxX; x++)
		{
			*(textscreenPtr+offset+x) = color;
		}
	}
}

void InitMenu(Uint16 menuNum)
{
	register Sint16 i;
	
	menuNumber = menuNum;
	if (menuNum == 0)
	{
		for (i=0; i<ARRAY_LENGTH(mainMenuLines); i++)
		{
			menuLines[i] = mainMenuLines[i];
		}
		menuNumLines = ARRAY_LENGTH(mainMenuLines);
		menuBottomIndex = 0;
		menuHasHeadline = 0;
		newMenuPos = mainMenuPos;
	}
	if (menuNum == 2)
	{
		for (i=0; i<ARRAY_LENGTH(configMenuLines); i++)
		{
			menuLines[i] = configMenuLines[i];
		}
		menuNumLines = ARRAY_LENGTH(configMenuLines);
		menuBottomIndex = 1;
		menuHasHeadline = 0;
		
		// Note: These string manipulations are potentially unsafe (see comments for configMenuLines)
		strcpy(menuLines[0], "Sound is now ");
		if (config.soundOn)
			strcat(menuLines[0], "on ");
		else
			strcat(menuLines[0], "off");
		
		strcpy(menuLines[1], "Music is now ");
		if (config.musicOn)
			strcat(menuLines[1], "on ");
		else
			strcat(menuLines[1], "off");
		
		strcpy(menuLines[3], "Joystick is now ");
		if (config.controlMode)
			strcat(menuLines[3], "on ");
		else
			strcat(menuLines[3], "off");
		
		newMenuPos = configMenuPos;
	}
	if (menuNum == 1)
	{
		for (i=0; i<ARRAY_LENGTH(episodeMenuLines); i++)
		{
			menuLines[i] = episodeMenuLines[i];
		}
		menuNumLines = ARRAY_LENGTH(episodeMenuLines);
		menuBottomIndex = 1;
		menuHasHeadline = 1;
		newMenuPos = episodeMenuPos;
	}
	if (menuNum == 3)
	{
		for (i=0; i<ARRAY_LENGTH(levelMenuLines); i++)
		{
			menuLines[i] = levelMenuLines[i];
		}
		menuNumLines = ARRAY_LENGTH(levelMenuLines);
		menuBottomIndex = 1;
		menuHasHeadline = 1;
		newMenuPos = levelMenuPos;
	}
	if (menuNum == 4)
	{
		for (i=0; i<ARRAY_LENGTH(skillMenuLines); i++)
		{
			menuLines[i] = skillMenuLines[i];
		}
		menuNumLines = ARRAY_LENGTH(skillMenuLines);
		menuBottomIndex = 1;
		menuHasHeadline = 1;
		newMenuPos = skillMenuPos;
	}
	if (menuNum == 5)
	{
		for (i=0; i<ARRAY_LENGTH(helpMenuLines); i++)
		{
			menuLines[i] = helpMenuLines[i];
		}
		menuNumLines = ARRAY_LENGTH(helpMenuLines);
		menuBottomIndex = 1;
		menuHasHeadline = 0;
		newMenuPos = helpMenuPos;
	}
	if (menuNum == 7)
	{
		for (i=0; i<ARRAY_LENGTH(speedMenuLines); i++)
		{
			menuLines[i] = speedMenuLines[i];
		}
		menuNumLines = ARRAY_LENGTH(speedMenuLines);
		menuBottomIndex = 1;
		menuHasHeadline = 1;
		newMenuPos = speedMenuPos;
	}
}

void MenuDraw1Line(char *text, Uint16 botLineIndex)
{
	register Sint16 x;
	Sint16 y;
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	x = TextWidth(text);
	x = (320-x) / 2;
	y = 107;
	DrawText_G(x, y, 5, text);
	MENUFRAME(botLineIndex);
	FadeFromBlack(20);
}

void MenuDraw2Lines(char *line1, char *line2, Uint16 botLineIndex)
{
	register Sint16 x;
	Sint16 y;
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	x = TextWidth(line1);
	x = (320-x) / 2;
	y = 97;
	DrawText_G(x, y, 5, line1);
	x = TextWidth(line2);
	x = (320-x) / 2;
	DrawText_G(x, y+20, 3, line2);
	MENUFRAME(botLineIndex);
	FadeFromBlack(20);
}

Sint8 MenuWait(void)
{
	unsigned char c;
	
	while (kbhit())
		getch();
	for (;;)
	{
		c = HandleJoystick();
		if (c == JOY_BTN1 || c == JOY_BTN2)
		{
			FadeToBlack(20);
			return 0;
		}
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				getch();
			}
			else
			{
				c = toupper(c);
				if (c == CHAR_ESCAPE)
				{
					FadeToBlack(20);
					return -1;
				}
			}

			FadeToBlack(20);
			return 0;
		}
		DrawStarfield();
	}
}

Sint8 MenuConfirm(void)
{
	unsigned char c;
	
	while (kbhit())
		getch();
	for (;;)
	{
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				getch();
			}
			else
			{
				c = toupper(c);
				if (c == 'Y' || c == 'N')
				{
					FadeToBlack(20);
					return c;
				}
				if (c == CHAR_ESCAPE)
				{
					FadeToBlack(20);
					return -1;
				}
			}
		}
		DrawStarfield();
	}
}

Sint8 MenuWaitDigit(void)	//never used!
{
	unsigned char c;
	
	while (kbhit())
		getch();
	for (;;)
	{
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				getch();
			}
			else
			{
				c = toupper(c);	//not required here
				if (c >= '1' && c <= '9')
				{
					FadeToBlack(20);
					return c-'1';
				}
				if (c == CHAR_ESCAPE)
				{
					FadeToBlack(20);
					return -1;
				}
			}
		}
		DrawStarfield();
	}
}

void MenuWaitNewKey(Uint16 keynum)
{
	register Uint16 x, y;
	Sint16 i;
	unsigned char c;
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	y = 72;
	strcpy(strbuf, "Select new key for: ");
	strcat(strbuf, controlNames[keynum]);
	DrawText_G((320-TextWidth(strbuf))/2, y, 5, strbuf);
	y += 20;
	MENUFRAME(7);
	//strbuf[1] = 0;	//faster than strcpy
	for (i=0; i<6; i++)
	{
		x = 27;
		strcpy(strbuf, " ");	//putting this outside the loop would speed things up
		strbuf[0] = i+'A';
		DrawText_G(x, y, 4, strbuf);
		DrawText_G(x+10, y, 2, "-");
		DrawText_G(x+20, y, 2, kbSpecialNames[i]);
		x = 125;
		strcpy(strbuf, " ");	//not required -- strbuf[1] is already 0 and strbuf[0] is overwritten below
		strbuf[0] = i+'G';
		DrawText_G(x, y, 4, strbuf);
		DrawText_G(x+10, y, 2, "-");
		DrawText_G(x+20, y, 2, kbSpecialNames[i+6]);
		x = 200;
		strcpy(strbuf, " ");	//not required -- strbuf[1] is already 0 and strbuf[0] is overwritten below
		strbuf[0] = i+'M';
		DrawText_G(x, y, 4, strbuf);
		DrawText_G(x+10, y, 2, "-");
		DrawText_G(x+20, y, 2, kbSpecialNames[i+12]);
		y += 10;
	}
	FadeFromBlack(20);
	while (kbhit())
		getch();
	for (;;)
	{
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				getch();
			}
			else
			{
				c = toupper(c);
				if (c == CHAR_ESCAPE)
				{
					FadeToBlack(20);
					break;
				}
				if (c >= 'A' && c <= 'R')
				{
					controlKeys[keynum] = c-'A';
					FadeToBlack(20);
					break;
				}
			}
		}
		DrawStarfield();
	}
}

void MenuDrawSlider(Sint16 x, Sint16 y, Sint16 frame)
{
	//Note: this ALWAYS draws to page 0!
	register Sint16 ix, iy;
	
	SetWriteMode(1);
	SetPlaneMask(0xF);
	for (iy=0; iy<13; iy++)
	{
		for (ix=0; ix<4; ix++)
		{
			//change screenpagePtr[0] to drawPtr if you want to draw to other pages
			*(screenpagePtr[0]+(iy+y)*80+x+ix) = *(screenpagePtr[3]+(iy+185)*80+frame*4+ix+60);
		}
	}
	SetWriteMode(0);
}

void VolumeMenu(void)
{
	register Uint16 y, pos;
	Sint16 i, drawTick, vols[2], ys[2], soundTick;
	unsigned char c;
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	y = 62;
	DrawText_G((320-TextWidth("Sound FX Volume"))/2, y, 5, "Sound FX Volume");
	y += 20;
	for (i=0; i<16; i++)
	{
		MenuDrawSlider((i<<2)+8, y, 0);
	}
	vols[0] = config.soundVolume / 16;
	MenuDrawSlider((vols[0]<<2)+8, y, 1);
	ys[0] = y;
	y += 40;
	DrawText_G((320-TextWidth("Music Volume"))/2, y, 5, "Music Volume");
	y += 20;
	for (i=0; i<16; i++)
	{
		MenuDrawSlider((i<<2)+8, y, 0);
	}
	vols[1] = config.musicVolume / 16;
	MenuDrawSlider((vols[1]<<2)+8, y, 1);
	ys[1] = y;
	y += 40;
	MENUFRAME(9);
	FadeFromBlack(20);
	drawTick = 0;
	pos = 0;
	soundTick = 0;
	while (kbhit())
		getch();
	for (;;)
	{
		c = HandleJoystick();
		if (c == JOY_UP)
		{
			c = KEY_UP;
			goto check_scancode;
		}
		if (c == JOY_DOWN)
		{
			c = KEY_DOWN;
			goto check_scancode;
		}
		if (c == JOY_RIGHT)
		{
			c = KEY_RIGHT;
			goto check_scancode;
		}
		if (c == JOY_LEFT)
		{
			c = KEY_LEFT;
			goto check_scancode;
		}
		if (c == JOY_BTN1 || c == JOY_BTN2)
		{
			c = CHAR_ENTER;
			goto check_char;
		}
		if (drawTick < 10)
			MenuDrawSlider((vols[pos]<<2)+8, ys[pos], 1);
		else
			MenuDrawSlider((vols[pos]<<2)+8, ys[pos], 0);
		drawTick++;
		if (drawTick > 20)
			drawTick = 0;
		if (soundTick == 0)
			PlaySound(Random(16));
		soundTick++;
		if (soundTick > 50)
			soundTick = 0;
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				c = getch();
check_scancode:
				if (c == KEY_RIGHT && vols[pos] < 15)
				{
					MenuDrawSlider((vols[pos]<<2)+8, ys[pos], 0);
					vols[pos]++;
				}
				if (c == KEY_LEFT && vols[pos] != 0)
				{
					MenuDrawSlider((vols[pos]<<2)+8, ys[pos], 0);
					vols[pos]--;
				}
				if (c == KEY_UP)
				{
					MenuDrawSlider((vols[pos]<<2)+8, ys[pos], 1);
					pos = 0;
				}
				if (c == KEY_DOWN)
				{
					MenuDrawSlider((vols[pos]<<2)+8, ys[pos], 1);
					pos = 1;
				}
				config.soundVolume = (vols[0] << 4) +15;
				config.musicVolume = (vols[1] << 4) +15;
				FX_SetVolume(config.soundVolume);
				MUSIC_SetVolume(config.musicVolume);
			}
			else
			{
				c = toupper(c);
check_char:
				if (c==CHAR_ESCAPE || c==CHAR_ENTER)
				{
					if (!WriteToFile(configPath, &config, sizeof(config)))
						Show1Line("I/O Error - HOCUS.SAV");
					FadeToBlack(20);
					break;
				}
			}
		}
		DrawStarfield();
	}
}

void IngameVolume(void)
{
	Uint16 oldDrawPage, oldDisplayPage;
	
	KB_Shutdown();
	oldDrawPage= currDrawPage;
	oldDisplayPage = currDisplayPage;
	FadeToBlack(20);
	CopyBytes(backupPalette, currPalette, 0x300);
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);

	VolumeMenu();

	CopyBytes(currPalette, backupPalette, 0x300);
	CopyScreenPage(0, 1);
	SetDisplayPage(oldDisplayPage);
	SetDrawPage(oldDrawPage);
	FadeFromBlack(1);
	KB_Startup();
}

void KeyControlsMenu(void)
{
	register Uint16 y;
	Sint16 i;
	unsigned char c;

redo:
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	y = 62;
	CENTER(DrawText_G, y, GRD_GOLD, "Select a key to define");
	y += 20;
	MENUFRAME(7);
	//strbuf[1] = 0;	//faster than strcpy
	for (i=0; i<NUMCONTROLS; i++)
	{
		strcpy(strbuf, " ");	//putting this outside this loop would speed things up
		strbuf[0] = i+'A';
		DrawText_G(70, y, GRD_WHITE, strbuf);
		DrawText_G(80, y, GRD_RED, "-");
		DrawText_G(90, y, GRD_RED, controlNames[i]);
		LEFT(DrawText_G, 250, y, GRD_GREEN, kbSpecialNames[controlKeys[i]]);
		y += 10;
	}
	FadeFromBlack(20);
	while(kbhit())
		getch();
	for (;;)
	{
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				getch();
			}
			else
			{
				c = toupper(c);
				if (c == CHAR_ESCAPE)
				{
					FadeToBlack(20);
					return;
				}
				if (c >= 'A' && c <= 'H')
				{
					FadeToBlack(20);
					MenuWaitNewKey(c-'A');
					goto redo;
				}
			}
		}
		DrawStarfield();
	}
}

void DrawMenu(void)
{
	register Sint16 i, y;
	Sint16 deltaY, x, firstOption;
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	if (menuHasHeadline)
		firstOption = 1;
	else
		firstOption = 0;
	x = 0;
	for (i=firstOption; i<menuNumLines; i++)
	{
		y = TextWidth(menuLines[i]);
		if (y > x)
			x = y;
	}
	x = (320-x)/2;
	deltaY = 0;
	for (i=0; i<menuNumLines; i++)
	{
		if (menuLines[i][strlen(menuLines[i])-1] == '~')
			deltaY += 4;
	}
	if (menuHasHeadline)
	{
		y = ((144 - (menuNumLines+1)*10) + deltaY)/2 + 40;
		CENTER(DrawText_G, y, GRD_GOLD, menuLines[0]);
		y += 20;
	}
	else
	{
		y = (144 - (menuNumLines*10 + deltaY))/2 + 40;
	}
	MENUFRAME(menuBottomIndex);
	menuCursorX = (x-24)/4;
	menuPos = 0;
	for (i=firstOption; i<menuNumLines; i++)
	{
#if SHAREWARE
		if (menuNumber == 1)
		{
			if (i == 1)
			{
				DrawMenuItem(x, y, GRD_WHITE, GRD_RED, menuLines[i]);
			}
			else
			{
				DrawText_G(x, y, GRD_RED, menuLines[i]);
			}
		}
		else
		{
			DrawMenuItem(x, y, GRD_WHITE, GRD_RED, menuLines[i]);
		}
#else
		DrawMenuItem(x, y, GRD_WHITE, GRD_RED, menuLines[i]);
#endif
		menuCursorYs[menuPos] = y-4;
		menuPos++;
		y += 10;
		if (menuLines[i][strlen(menuLines[i])-1] == '~')
			y += 4;
	}
	menuNumOptions = menuPos;
	menuPos = 0;
	FadeFromBlack(20);
}

Sint16 CheckHighscore(Sint16 ep, Sint32 points)
{
	register Sint16 i;
	Sint16 n;
	
	for (i=0; i<5; i++)
	{
		if (config.hs_scores[ep][i] < points)
		{
			//move remaining entries down 1 slot:
			for (n=3; n >= i; n--)
			{
				strcpy(config.hs_names[ep][n+1], config.hs_names[ep][n]);
				config.hs_scores[ep][n+1] = config.hs_scores[ep][n];
			}
			//clear current name:
			strcpy(config.hs_names[ep][i], "");	//config.hs_names[ep][i][0] = 0; would be faster
			config.hs_scores[ep][i] = points;
			return i+1;
		}
	}
	return 0;
}

void EnterHighscore(Sint16 ep, Sint16 pos)
{
	register Sint16 i, y;
	Sint16 x, gradient, textY, textColor;
	unsigned char c;
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	y = 72;
	strcpy(strbuf, "High Scores For Game ");
	strcat(strbuf, itoa(ep+1, numbuf, 10));
	CENTER(DrawText_G, y, GRD_GOLD, strbuf);
	y += 20;
	MENUFRAME(6);
	for (i=0; i<NUMHIGHSCORES; i++)
	{
		gradient = GRD_GREEN;
		if (!i)
			gradient = GRD_WHITE;
		x = 27;
		DrawText_G(x, y, gradient, config.hs_names[ep][i]);
		strcpy(strbuf, ltoa(config.hs_scores[ep][i], numbuf, 10));	//strcpy is unnecessary, just do ltoa with strbuf instead of numbuf or draw numbuf instead of strbuf
		LEFT(DrawText_G, 290, y, gradient, strbuf);
		if (i==pos)
		{
			textY = y;
			textColor = gradient;
		}
		y += 10;
		if (!i)
			y += 10;
	}
	FadeFromBlack(20);
	
	c = 0;
	strcpy(numbuf, " ");	//numbuf[1] = 0; would be faster
	while (kbhit())
		getch();
	while (c != CHAR_ENTER)
	{
		DrawText_G(27, textY, textColor, config.hs_names[ep][pos]);
		i = strlen(config.hs_names[ep][pos]);
		x = TextWidth(config.hs_names[ep][pos]) + 27;
		numbuf[0] = '_';
		DrawText_G(x, textY, GRD_RED, numbuf);	//using a string constant would reduce code size a bit
		for (;;)
		{
			if (kbhit())
			{
				c = getch();
				if (!c)
				{
					getch();
				}
				else
				{
					if (c >= ' ' && c <= 'z' && i < MAXNAMELENGTH)
					{
						ClearRect(x, textY, x+10, textY+9);	//remove text cursor
						config.hs_names[ep][pos][i] = c;
						i++;
						config.hs_names[ep][pos][i] = 0;
						break;
					}
					if (c == CHAR_BACKSPACE && i)
					{
						ClearRect(x, textY, x+10, textY+9);	//remove text cursor
						y = TextWidth(config.hs_names[ep][pos]) + 27;
						i--;
						config.hs_names[ep][pos][i] = 0;
						x = TextWidth(config.hs_names[ep][pos]) + 27;
						ClearRect(x, textY, y, textY+9);	//remove character
						break;
					}
					if (c == CHAR_ENTER)
					{
						break;
					}
				}
			}
			DrawStarfield();
		}
	}
	if (!WriteToFile(configPath, &config, sizeof(config)))
	{
		Show1Line("I/O Error - HOCUS.SAV");
	}
}

void HighscoreMenu(Uint16 timed)
{
	register Sint16 i, y;
	Sint16 x, gradient, ep;
	Sint8 result;
	Sint32 oldtick;
	
	ep = 0;
#if !SHAREWARE
	do
	{
#endif
		SetToBlack();
		SetDrawPage(0);
		ClearScreen();
		SetDisplayPage(0);
		y = 72;
		strcpy(strbuf, "High Scores For Game ");
		strcat(strbuf, itoa(ep+1, numbuf, 10));
		CENTER(DrawText_G, y, GRD_GOLD, strbuf);
		y += 20;
		MENUFRAME(2);
		for (i=0; i<5; i++)
		{
			gradient = GRD_GREEN;
			if (!i)
				gradient = GRD_WHITE;
			x = 27;
			DrawText_G(x, y, gradient, config.hs_names[ep][i]);
			strcpy(strbuf, ltoa(config.hs_scores[ep][i], numbuf, 10));	//strcpy is unnecessary, just do ltoa with strbuf instead of numbuf or draw numbuf instead of strbuf
			LEFT(DrawText_G, 290, y, gradient, strbuf);
			y += 10;
			if (!i)
				y += 10;
		}
		FadeFromBlack(20);
		if (!timed)
		{
			result = MenuWait();
			if (result == -1)
				return;
		}
		else
		{
			//this is never executed, timed is always 0
			oldtick = pcTickCount;
			while (pcTickCount - oldtick < 600 && !kbhit())
			{
				DrawStarfield();
			}
			if (kbhit())
			{
				while (kbhit())
					getch();
				return;
			}
		}
		
#if !SHAREWARE
		ep++;
	} while (ep < 4);
#endif
}

void StoreMenuPos(void)
{
	switch (menuNumber)
	{
	case 0:
		mainMenuPos = menuPos;
		break;
	case 2:
		configMenuPos = menuPos;
		break;
	case 1:
		episodeMenuPos = menuPos;
		break;
	case 3:
		levelMenuPos = menuPos;
		break;
		//BUG? skill menu (case 4) is not handled here
	case 5:
		helpMenuPos = menuPos;
		break;
	case 7:
		speedMenuPos = menuPos;
		break;
	}
}

Sint8 HandleMenu(Uint16 exitToDemo)
{
	register Sint16 first, i;
	unsigned char c;
	Uint16 creditsDone = 0;
	Sint32 oldtick;
	
	oldtick = pcTickCount;
	menuPos = newMenuPos;
	while (kbhit())
		getch();
	if (menuHasHeadline)
	{
		first = 1;
	}
	else
	{
		first = 0;
	}
	for (;;)
	{
		c = HandleJoystick();
		if (c == JOY_UP)
		{
			c = KEY_UP;
			goto check_move;
		}
		if (c == JOY_DOWN)
		{
			c = KEY_DOWN;
			goto check_move;
		}
		if (c == JOY_BTN1)
		{
			c = CHAR_ENTER;
			goto check_select;
		}
		if (c == JOY_BTN2)
		{
			c = CHAR_ESCAPE;
			goto check_select;
		}
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				c = getch();
check_move:
				oldtick = pcTickCount;
				if (c == KEY_UP)
				{
					ClearRect(menuCursorX<<2, menuCursorYs[menuPos], (menuCursorX<<2)+15, menuCursorYs[menuPos]+14);
					menuPos--;
					if (menuPos < 0)
					{
						menuPos = menuNumOptions -1;
					}
					StoreMenuPos();
				}
				if (c == KEY_DOWN)
				{
					ClearRect(menuCursorX<<2, menuCursorYs[menuPos], (menuCursorX<<2)+15, menuCursorYs[menuPos]+14);
					menuPos++;
					if (menuPos >= menuNumOptions)
					{
						menuPos = 0;
					}
					StoreMenuPos();
				}
				c = 0;	//useless assignment
			}
			else
			{
				c = toupper(c);
				for (i=first; i<menuNumLines; i++)
				{
					if (menuLines[i][0] == c)
					{
						ClearRect(menuCursorX<<2, menuCursorYs[menuPos], (menuCursorX<<2)+15, menuCursorYs[menuPos]+14);
						menuPos = i-first;
						StoreMenuPos();
					}
				}
check_select:
				oldtick = pcTickCount;
				if (c == CHAR_ENTER)
				{
					FadeToBlack(20);
					return menuPos;
				}
				if (c == CHAR_ESCAPE && menuBottomIndex != 0 && menuBottomIndex != 3)
				{
					FadeToBlack(20);
					return -1;
				}
			}
		}
		DrawMenuCursor(menuCursorX, menuCursorYs[menuPos]);
		DrawStarfield();
		if (exitToDemo)
		{
			if (pcTickCount - oldtick > 4500)
			{
				if (!creditsDone)
				{
					ShowCredits();
					InitMenu(0);	//main menu
					DrawMenu();
					oldtick = pcTickCount;
					creditsDone = 1;
				}
				else
				{
					FadeToBlack(20);
					return -99;
				}
			}
		}
	}
}

Sint8 MainMenu(void)
{
	InitMenu(0);
	DrawMenu();
	return HandleMenu(1);
}

Sint8 EpisodeMenu(void)
{
	InitMenu(1);
	DrawMenu();
	return HandleMenu(0);
}

Sint8 OptionsMenu(void)
{
	InitMenu(2);
	DrawMenu();
	return HandleMenu(0);
}

Sint8 LevelMenu(void)
{
	InitMenu(3);
	DrawMenu();
	return HandleMenu(0);
}

Sint8 SkillMenu(void)
{
	InitMenu(4);
	DrawMenu();
	return HandleMenu(0);
}

Sint8 GameMenu(void)
{
	InitMenu(5);
	DrawMenu();
	return HandleMenu(0);
}

Sint8 GameSpeedMenu(void)
{
	speedMenuPos = gamespeed;
	InitMenu(7);
	DrawMenu();
	return HandleMenu(0);
}

void Show1Line(char *text)
{
	unsigned char c;
	
	MenuDraw1Line(text, 2);
	while (kbhit())
		getch();
	for (;;)
	{
		if (kbhit())
		{
			c = getch();
			if (!c)
				getch();
			FadeToBlack(20);
			return;
		}
		DrawStarfield();
	}
}

Uint16 SaveMenu(void)
{
	register Sint16 i, slot;
	Uint16 saved = 0;
	char c, ch;
	Sint16 x, y, textY;
	char buff[26];
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	newMenuPos = mainMenuPos;	//probably useless assignment
	y = 59;
	strcpy(strbuf, "Select SAVE slot");
	CENTER(DrawText_G, y, GRD_GOLD, strbuf);
	y += 20;
	MENUFRAME(4);
	menuPos = 0;
	menuCursorX = 4;
	for (i=0; i<NUMSAVEGAMES; i++)
	{
		x = 35;
		strcpy(strbuf, itoa(i+1, numbuf, 10));
		DrawText_G(x, y, GRD_WHITE, strbuf);
		x += 10;
		strcpy(strbuf, config.sg_names[i]);
		DrawText_G(x, y, GRD_GREEN, strbuf);
		menuCursorYs[menuPos] = y-4;
		menuPos++;
		y += 10;
	}
	menuNumOptions = menuPos;
	menuPos = saveMenuPos;
	FadeFromBlack(20);
	c = 0;
	while (c != CHAR_ENTER)
	{
		c = HandleJoystick();
		if (c == JOY_UP)
		{
			c = KEY_UP;
			goto check_move;
		}
		if (c == JOY_DOWN)
		{
			c = KEY_DOWN;
			goto check_move;
		}
		if (c == JOY_BTN1)
		{
			c = CHAR_ENTER;
			goto check_select;
		}
		if (c == JOY_BTN2)
		{
			c = CHAR_ESCAPE;
			goto check_select;
		}
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				c = getch();
check_move:
				if (c == KEY_UP)
				{
					ClearRect(menuCursorX<<2, menuCursorYs[menuPos], (menuCursorX<<2)+15, menuCursorYs[menuPos]+14);
					menuPos--;
					if (menuPos < 0)
					{
						menuPos = menuNumOptions-1;
					}
					saveMenuPos = menuPos;
				}
				if (c == KEY_DOWN)
				{
					ClearRect(menuCursorX<<2, menuCursorYs[menuPos], (menuCursorX<<2)+15, menuCursorYs[menuPos]+14);
					menuPos++;
					if (menuPos >= menuNumOptions)
					{
						menuPos = 0;
					}
					saveMenuPos = menuPos;
				}
			}
			else
			{
				if (c >= '1' && c <= '9')
				{
					ClearRect(menuCursorX<<2, menuCursorYs[menuPos], (menuCursorX<<2)+15, menuCursorYs[menuPos]+14);
					menuPos = c-'1';
					saveMenuPos = menuPos;
				}
check_select:
				if (c == CHAR_ESCAPE)
				{
					FadeToBlack(20);
					goto done;
				}
				if (c == CHAR_ENTER)
				{
					slot = menuPos;
					textY = menuCursorYs[menuPos]+4;
					strcpy(buff, config.sg_names[slot]);
					if (config.sg_episodes[slot] == -1)
					{
						strcpy(config.sg_names[slot], "");
						ClearRect(45, textY, 319, textY+9);
					}
					ch = 0;
					strcpy(numbuf, " ");
					while (kbhit())
						getch();
					while (ch != CHAR_ENTER && ch != CHAR_ESCAPE)
					{
						DrawText_G(45, textY, GRD_GREEN, config.sg_names[slot]);
						i = strlen(config.sg_names[slot]);
						x = TextWidth(config.sg_names[slot]) + 45;
						numbuf[0] = '_';
						DrawText_G(x, textY, GRD_RED, numbuf);	//draw cursor
						for (;;)
						{
							if (kbhit())
							{
								ch = getch();
								if (!ch)
								{
									getch(); 
								}
								else
								{
									if (ch >= ' ' && ch <= 'z' && i < MAXNAMELENGTH)
									{
										ClearRect(x, textY, x+10, textY+9);	//remove text cursor
										config.sg_names[slot][i] = ch;
										i++;
										config.sg_names[slot][i] = 0;
										break;
									}
									if (ch == CHAR_BACKSPACE && i != 0)
									{
										ClearRect(x, textY, x+10, textY+9);	//remove text cursor
										y = TextWidth(config.sg_names[slot]) + 45;
										i--;
										config.sg_names[slot][i] = 0;
										x = TextWidth(config.sg_names[slot]) + 45;
										ClearRect(x, textY, y, textY+9);	//remove character
										break;
									}
									if (ch == CHAR_ENTER)
									{
										break;
									}
									if (ch == CHAR_ESCAPE)
									{
										strcpy(config.sg_names[slot], buff);
										ClearRect(45, textY, 319, textY+9);
										DrawText_G(45, textY, GRD_GREEN, config.sg_names[slot]);
										c = 0;
										break;
									}
								}
							}
							DrawMenuCursor(menuCursorX, menuCursorYs[menuPos]);
							DrawStarfield();
						}
					}
				}
			}
		}
		DrawMenuCursor(menuCursorX, menuCursorYs[menuPos]);
		DrawStarfield();
	}
	if (c == CHAR_ENTER)
	{
		config.sg_episodes[slot] = episode;
		config.sg_levels[slot] = level;
		config.sg_skill[slot] = skill;
		config.sg_scores[slot] = oldscore;
		if (!WriteToFile(configPath, &config, sizeof(config)))
		{
			Show1Line("I/O Error - HOCUS.SAV");
			FadeToBlack(20);
			goto done;
		}
		saved = 1;
		FadeToBlack(20);
	}
done:
	return saved;
}

Uint16 RestoreMenu(void)
{
	register Sint16 y, i;
	Uint16 restored = 0;
	char c;
	Sint16 x, slot;
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	y = 59;
	strcpy(strbuf, "Select RESTORE slot");
	CENTER(DrawText_G, y, GRD_GOLD, strbuf);
	y += 20;
	MENUFRAME(4);
	menuPos = 0;
	menuCursorX = 4;
	for (i=0; i<9; i++)
	{
		x = 35;
		strcpy(strbuf, itoa(i+1, numbuf, 10));
		DrawText_G(x, y, GRD_WHITE, strbuf);
		x += 10;
		strcpy(strbuf, config.sg_names[i]);
		DrawText_G(x, y, GRD_GREEN, strbuf);
		menuCursorYs[menuPos] = y-4;
		menuPos++;
		y += 10;
	}
	menuNumOptions = menuPos;
	menuPos = restoreMenuPos;
	FadeFromBlack(20);
	c = 0;
	while (c != CHAR_ENTER)
	{
		c = HandleJoystick();
		if (c == JOY_UP)
		{
			c = KEY_UP;
			goto check_move;
		}
		if (c == JOY_DOWN)
		{
			c = KEY_DOWN;
			goto check_move;
		}
		if (c == JOY_BTN1)
		{
			c = CHAR_ENTER;
			goto check_select;
		}
		if (c == JOY_BTN2)
		{
			c = CHAR_ESCAPE;
			goto check_select;
		}
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				c = getch();
check_move:
				if (c == KEY_UP)
				{
					ClearRect(menuCursorX<<2, menuCursorYs[menuPos], (menuCursorX<<2)+15, menuCursorYs[menuPos]+14);
					menuPos--;
					if (menuPos < 0)
					{
						menuPos = menuNumOptions-1;
					}
					restoreMenuPos = menuPos;
				}
				if (c == KEY_DOWN)
				{
					ClearRect(menuCursorX<<2, menuCursorYs[menuPos], (menuCursorX<<2)+15, menuCursorYs[menuPos]+14);
					menuPos++;
					if (menuPos >= menuNumOptions)
					{
						menuPos = 0;
					}
					restoreMenuPos = menuPos;
				}
			}
			else
			{
				if (c >= '1' && c <= '9')
				{
					ClearRect(menuCursorX<<2, menuCursorYs[menuPos], (menuCursorX<<2)+15, menuCursorYs[menuPos]+14);
					menuPos = c-'1';
					restoreMenuPos = menuPos;
				}
check_select:
				if (c == CHAR_ESCAPE)
				{
					FadeToBlack(20);
					goto done;
				}
				if (c == CHAR_ENTER)
				{
					slot = menuPos;
					if (config.sg_episodes[slot] == -1)
					{
						c = 0;
					}
				}
			}
		}
		DrawMenuCursor(menuCursorX, menuCursorYs[menuPos]);
		DrawStarfield();
	}
	if (c == CHAR_ENTER)
	{
		episode = config.sg_episodes[slot];
		level = config.sg_levels[slot];
		skill = config.sg_skill[slot];
		score = config.sg_scores[slot];
		oldscore = score;
		restored = 1;
		FadeToBlack(20);
	}
done:
	return restored;
}

Uint16 AskQuit(void)
{
	register Uint16 yes = 0;
	Sint8 result;
	
	MenuDraw2Lines("Reminder: Save your game before quitting", "Quit game?", 3);
	result = MenuConfirm();
	if (result != -1 && result != 'N')
	{
		yes = 1;
	}
	return yes;
}

Uint16 CalibrateJoy(void)
{
	while (kbhit())
		getch();
	MenuDraw1Line("Move joystick to center and press FIRE button", 5);
	do
	{
		ReadJoy();
		DrawStarfield();
		if (kbhit())
		{
			if (getch() == CHAR_ESCAPE)
			{
				//FadeToBlack(20);	//BUG: the game does not fade out here
				return 0;
			}
		}
	} while (!(joyBtn1Down || joyBtn2Down));
		
	if (joyBtn1Down)
		joyFireButton = 0;
	if (joyBtn2Down)
		joyFireButton = 1;
	JoyCalibrateCentered();
	config.joyFireButton = joyFireButton;
	while (kbhit())
		getch();
	FadeToBlack(20);
	return 1;
}

void IngameCalibrate(void)
{
	Uint16 oldDrawPage, oldDisplayPage;
	
	if (config.controlMode)
	{
		KB_Shutdown();
		oldDrawPage = currDrawPage;
		oldDisplayPage = currDisplayPage;
		FadeToBlack(20);
		CopyBytes(backupPalette, currPalette, 0x300);
		SetToBlack();
		LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
		LoadPaletteChunk(MENUPAL_PAL, 128, 0);

		CalibrateJoy();

		CopyBytes(currPalette, backupPalette, 0x300);
		CopyScreenPage(0, 1);
		SetDisplayPage(oldDisplayPage);
		SetDrawPage(oldDrawPage);
		FadeFromBlack(1);
		KB_Startup();
	}
}

void IngameSave(void)
{
	Uint16 oldDrawPage, oldDisplayPage;
	
	KB_Shutdown();
	oldDrawPage = currDrawPage;
	oldDisplayPage = currDisplayPage;
	FadeToBlack(20);
	CopyBytes(backupPalette, currPalette, 0x300);
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);

	SaveMenu();

	CopyBytes(currPalette, backupPalette, 0x300);
	CopyScreenPage(0, 1);
	SetDisplayPage(oldDisplayPage);
	SetDrawPage(oldDrawPage);
	FadeFromBlack(1);
	KB_Startup();
}

Uint16 IngameRestore(void)
{
	register Uint16 restored;
	Uint16 oldDrawPage, oldDisplayPage;
	
	KB_Shutdown();
	oldDrawPage = currDrawPage;
	oldDisplayPage = currDisplayPage;
	FadeToBlack(20);
	CopyBytes(backupPalette, currPalette, 0x300);
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);

	restored = RestoreMenu();
	if (!restored)
	{
		CopyBytes(currPalette, backupPalette, 0x300);
		CopyScreenPage(0, 1);
		SetDisplayPage(oldDisplayPage);
		SetDrawPage(oldDrawPage);
		FadeFromBlack(1);
	}
	else
	{
		ClearScreen();
	}
	KB_Startup();
	return restored;
}

Uint16 IngameQuit(void)
{
	register Uint16 quitting;
	Uint16 oldDrawPage, oldDisplayPage;
	
	KB_Shutdown();
	oldDrawPage = currDrawPage;
	oldDisplayPage = currDisplayPage;
	FadeToBlack(20);
	CopyBytes(backupPalette, currPalette, 0x300);
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);

	quitting = AskQuit();
	if (!quitting)
	{
		CopyBytes(currPalette, backupPalette, 0x300);
		CopyScreenPage(0, 1);
		SetDisplayPage(oldDisplayPage);
		SetDrawPage(oldDrawPage);
		FadeFromBlack(1);
	}
	else
	{
		ClearScreen();
	}
	KB_Startup();
	return quitting;
}

void DrawOrderPage(Sint16 page, Sint16 pages)
{
	register Sint16 i, y;
	char str[82], num[22];
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	////////////////////
	// draw menu frame:
	DrawImageChunk(BOTTOM_IMG, 0, 184);
	if (pages == 1)
	{
		strcpy(str, menuBottomLines[2]);
	}
	else
	{
		strcpy(str, "Page ");
		strcat(str, itoa(page+1, num, 10));
		strcat(str, " of ");
		strcat(str, itoa(pages, num, 10));
		if (page == 0)
		{
			strcat(str, " - Press PGDN - ESC to cancel");
		}
		else if (page == pages-1)
		{
			strcat(str, " - Press PGUP - ESC to cancel");
		}
		else
		{
			strcat(str, " - Press PGUP/PGDN - ESC to cancel");
		}
	}
	CENTER(DrawText_GS, 188, GRD_WHITE, str);
	DrawImageChunk(HOCUS_IMG, 0, 0);
	// end of menu frame
	/////////////////////
	for (i=0, y=0; i<textpage.lineCount; i++)
	{
		if (!strlen(textpage.lines[i]))
		{
			y += textpage.nullHeight;
		}
		else
		{
			y += textpage.lineHeight;
		}
	}
	y = (144-y)/2 +40;
	for (i=0; i<textpage.lineCount; i++)
	{
		CENTER(DrawText_G, y, LINE_COLOR(i), textpage.lines[i]);
		if (!strlen(textpage.lines[i]))
		{
			y += textpage.nullHeight;
		}
		else
		{
			y += textpage.lineHeight;
		}
	}
	FadeFromBlack(20);
}

void DrawStoryPage(Sint16 page, Sint16 pages)
{
	register Sint16 i, y;
	char str[82], num[22];
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	////////////////////
	// draw menu bottom:
	DrawImageChunk(BOTTOM_IMG, 0, 184);
	if (pages == 1)
	{
		strcpy(str, menuBottomLines[2]);
	}
	else
	{
		strcpy(str, "Page ");
		strcat(str, itoa(page+1, num, 10));
		strcat(str, " of ");
		strcat(str, itoa(pages, num, 10));
		if (page == 0)
		{
			strcat(str, " - Press PGDN - ESC to cancel");
		}
		else if (page == pages-1)
		{
			strcat(str, " - Press PGUP - ESC to cancel");
		}
		else
		{
			strcat(str, " - Press PGUP/PGDN - ESC to cancel");
		}
	}
	CENTER(DrawText_GS, 188, GRD_WHITE, str);
	// end of menu bottom
	/////////////////////
	if (storypics[page].number != -1)
	{
		DrawImageChunk(storypics[page].number, storypics[page].x, storypics[page].y);
	}
	for (i=0, y=0; i<textpage.lineCount; i++)
	{
		if (!strlen(textpage.lines[i]))
		{
			y += textpage.nullHeight;
		}
		else
		{
			y += textpage.lineHeight;
		}
	}
	y = (184-y)/2;
	for (i=0; i<textpage.lineCount; i++)
	{
		if (LINE_X(i) == 0)
		{
			CENTER(DrawText_G, y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
		else
		{
			DrawText_G(LINE_X(i), y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
	}
	FadeFromBlack(20);
}

void DrawFinale1Page(Sint16 page, Sint16 pages)
{
	register Sint16 i, y;
	char str[82], num[22];
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	////////////////////
	// draw menu bottom:
	DrawImageChunk(BOTTOM_IMG, 0, 184);
	if (pages == 1)
	{
		strcpy(str, menuBottomLines[2]);
	}
	else
	{
		strcpy(str, "Page ");
		strcat(str, itoa(page+1, num, 10));
		strcat(str, " of ");
		strcat(str, itoa(pages, num, 10));
		if (page == 0)
		{
			strcat(str, " - Press PGDN - ESC to cancel");
		}
		else if (page == pages-1)
		{
			strcat(str, " - Press PGUP - ESC to cancel");
		}
		else
		{
			strcat(str, " - Press PGUP/PGDN - ESC to cancel");
		}
	}
	CENTER(DrawText_GS, 188, GRD_WHITE, str);
	// end of menu bottom
	/////////////////////
	if (finale1pics[page].number != -1)
	{
		DrawImageChunk(finale1pics[page].number, finale1pics[page].x, finale1pics[page].y);
	}
	for (i=0, y=0; i<textpage.lineCount; i++)
	{
		if (!strlen(textpage.lines[i]))
		{
			y += textpage.nullHeight;
		}
		else
		{
			y += textpage.lineHeight;
		}
	}
	y = (184-y)/2;
	for (i=0; i<textpage.lineCount; i++)
	{
		if (LINE_X(i) == 0)
		{
			CENTER(DrawText_G, y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
		else
		{
			DrawText_G(LINE_X(i), y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
	}
	FadeFromBlack(20);
}

void DrawFinale2Page(Sint16 page, Sint16 pages)
{
	register Sint16 i, y;
	char str[82], num[22];
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	////////////////////
	// draw menu bottom:
	DrawImageChunk(BOTTOM_IMG, 0, 184);
	if (pages == 1)
	{
		strcpy(str, menuBottomLines[2]);
	}
	else
	{
		strcpy(str, "Page ");
		strcat(str, itoa(page+1, num, 10));
		strcat(str, " of ");
		strcat(str, itoa(pages, num, 10));
		if (page == 0)
		{
			strcat(str, " - Press PGDN - ESC to cancel");
		}
		else if (page == pages-1)
		{
			strcat(str, " - Press PGUP - ESC to cancel");
		}
		else
		{
			strcat(str, " - Press PGUP/PGDN - ESC to cancel");
		}
	}
	CENTER(DrawText_GS, 188, GRD_WHITE, str);
	// end of menu bottom
	/////////////////////
	if (finale2pics[page].number != -1)
	{
		DrawImageChunk(finale2pics[page].number, finale2pics[page].x, finale2pics[page].y);
	}
	for (i=0, y=0; i<textpage.lineCount; i++)
	{
		if (!strlen(textpage.lines[i]))
		{
			y += textpage.nullHeight;
		}
		else
		{
			y += textpage.lineHeight;
		}
	}
	y = (184-y)/2;
	for (i=0; i<textpage.lineCount; i++)
	{
		if (LINE_X(i) == 0)
		{
			CENTER(DrawText_G, y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
		else
		{
			DrawText_G(LINE_X(i), y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
	}
	FadeFromBlack(20);
}

void DrawFinale3Page(Sint16 page, Sint16 pages)
{
	register Sint16 i, y;
	char str[82], num[22];
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	////////////////////
	// draw menu bottom:
	DrawImageChunk(BOTTOM_IMG, 0, 184);
	if (pages == 1)
	{
		strcpy(str, menuBottomLines[2]);
	}
	else
	{
		strcpy(str, "Page ");
		strcat(str, itoa(page+1, num, 10));
		strcat(str, " of ");
		strcat(str, itoa(pages, num, 10));
		if (page == 0)
		{
			strcat(str, " - Press PGDN - ESC to cancel");
		}
		else if (page == pages-1)
		{
			strcat(str, " - Press PGUP - ESC to cancel");
		}
		else
		{
			strcat(str, " - Press PGUP/PGDN - ESC to cancel");
		}
	}
	CENTER(DrawText_GS, 188, GRD_WHITE, str);
	// end of menu bottom
	/////////////////////
	if (finale3pics[page].number != -1)
	{
		DrawImageChunk(finale3pics[page].number, finale3pics[page].x, finale3pics[page].y);
	}
	for (i=0, y=0; i<textpage.lineCount; i++)
	{
		if (!strlen(textpage.lines[i]))
		{
			y += textpage.nullHeight;
		}
		else
		{
			y += textpage.lineHeight;
		}
	}
	y = (184-y)/2;
	for (i=0; i<textpage.lineCount; i++)
	{
		if (LINE_X(i) == 0)
		{
			CENTER(DrawText_G, y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
		else
		{
			DrawText_G(LINE_X(i), y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
	}
	FadeFromBlack(20);
}

void DrawFinale4Page(Sint16 page, Sint16 pages)
{
	register Sint16 i, y;
	char str[82], num[22];
	
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	////////////////////
	// draw menu bottom:
	DrawImageChunk(BOTTOM_IMG, 0, 184);
	if (pages == 1)
	{
		strcpy(str, menuBottomLines[2]);
	}
	else
	{
		strcpy(str, "Page ");
		strcat(str, itoa(page+1, num, 10));
		strcat(str, " of ");
		strcat(str, itoa(pages, num, 10));
		if (page == 0)
		{
			strcat(str, " - Press PGDN - ESC to cancel");
		}
		else if (page == pages-1)
		{
			strcat(str, " - Press PGUP - ESC to cancel");
		}
		else
		{
			strcat(str, " - Press PGUP/PGDN - ESC to cancel");
		}
	}
	CENTER(DrawText_GS, 188, GRD_WHITE, str);
	// end of menu bottom
	/////////////////////
	if (finale4pics[page].number != -1)
	{
		DrawImageChunk(finale4pics[page].number, finale4pics[page].x, finale4pics[page].y);
	}
	for (i=0, y=0; i<textpage.lineCount; i++)
	{
		if (!strlen(textpage.lines[i]))
		{
			y += textpage.nullHeight;
		}
		else
		{
			y += textpage.lineHeight;
		}
	}
	y = (184-y)/2;
	for (i=0; i<textpage.lineCount; i++)
	{
		if (LINE_X(i) == 0)
		{
			CENTER(DrawText_G, y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
		else
		{
			DrawText_G(LINE_X(i), y, LINE_COLOR(i), textpage.lines[i]);
			if (!strlen(textpage.lines[i]))
			{
				y += textpage.nullHeight;
			}
			else
			{
				y += textpage.lineHeight;
			}
		}
	}
	FadeFromBlack(20);
}

Sint16 HandleText(Sint16 page, Sint16 pages)
{
	unsigned char c;
	
	while (kbhit())
		getch();
	for (;;)
	{
		c = HandleJoystick();
		if (c == JOY_UP || c == JOY_LEFT)
		{
			c = KEY_PGUP;
			goto check_move;
		}
		if (c == JOY_DOWN || c == JOY_RIGHT)
		{
			c = KEY_PGDN;
			goto check_move;
		}
		if (c == JOY_BTN2)
		{
			c = CHAR_ESCAPE;
			goto check_exit;
		}
		if (kbhit())
		{
			c = getch();
			if (!c)
			{
				c = getch();
				if (pages == 1)
				{
					page = -1;
					break;
				}
check_move:
				if ((c == KEY_PGUP || c == KEY_LEFT || c == KEY_UP) && page != 0)
				{
					page--;
					break;
				}
				if ((c == KEY_PGDN || c == KEY_RIGHT || c == KEY_DOWN) && page < pages-1)
				{
					page++;
					break;
				}
				if (c == KEY_HOME && page != 0)
				{
					page = 0;
					break;
				}
				if (c == KEY_END && page < pages-1)
				{
					page = pages-1;
					break;
				}
			}
			else
			{
check_exit:
				if (c == CHAR_ESCAPE || pages == 1)
				{
					page = -1;
					break;
				}
			}
		}
		DrawStarfield();
	}
	FadeToBlack(20);
	return page;
}

void Ordering(void)
{
	register Sint16 page = 0;
	
	do {
		OpenDatFile();
#if SHAREWARE
		ReadDatChunk(ORDER1_PAG + page, &textpage);
		CloseDatFile();
		DrawOrderPage(page, 7);
		page = HandleText(page, 7);
#else
		ReadDatChunk(ORDER_R1_PAG + page, &textpage);
		CloseDatFile();
		DrawOrderPage(page, 9);
		page = HandleText(page, 9);
#endif
	} while (page != -1);
}

void Story(void)
{
	register Sint16 page = 0;
	
	do {
		OpenDatFile();
		ReadDatChunk(STORY01_PAG + page, &textpage);
		CloseDatFile();
		DrawStoryPage(page, 10);
		page = HandleText(page, 10);
	} while (page != -1);
}

void Finale1(void)
{
	register Sint16 page = 0;
	
	do {
		OpenDatFile();
		ReadDatChunk(E1END1_PAG + page, &textpage);
		CloseDatFile();
		DrawFinale1Page(page, 2);
		page = HandleText(page, 2);
	} while (page != -1);
}

#if !SHAREWARE

void Finale2(void)
{
	register Sint16 page = 0;
	
	do {
		OpenDatFile();
		ReadDatChunk(E2END1_PAG + page, &textpage);
		CloseDatFile();
		DrawFinale2Page(page, 2);
		page = HandleText(page, 2);
	} while (page != -1);
}

void Finale3(void)
{
	register Sint16 page = 0;
	
	do {
		OpenDatFile();
		ReadDatChunk(E3END1_PAG + page, &textpage);
		CloseDatFile();
		DrawFinale3Page(page, 4);
		page = HandleText(page, 4);
	} while (page != -1);
}

void Finale4(void)
{
	register Sint16 page = 0;
	
	do {
		OpenDatFile();
		ReadDatChunk(E4END1_PAG + page, &textpage);
		CloseDatFile();
		DrawFinale4Page(page, 2);
		page = HandleText(page, 2);
	} while (page != -1);
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	DrawPCX_Chunk(KISS_PCX, 0);
	PlayMusicChunk(WEDDING_MID, false);
	FadeFromBlack(20);
	while (kbhit())
		getch();
	while (!kbhit());
	while (kbhit())
		getch();
	FadeToBlack(20);
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);
	PlayMusicChunk(MUSIC03_MID, true);
}

#endif

void Registering(void)
{
	OpenDatFile();
	ReadDatChunk(REGISTER_PAG, &textpage);
	CloseDatFile();
	DrawOrderPage(0, 1);
	MenuWait();
#if SHAREWARE
	episodeMenuPos = 0;
#endif
	OpenDatFile();	//BUG? does this cause errors when DAT file is re-opened while it's still open?
}

void ShowCredits(void)
{
	register Sint16 page=0, hit;	//Note: the variable 'hit' is not really required
	Sint32 oldtick;
	
	FadeToBlack(20);
	for (; page<2; page++)
	{
		DrawPCX_Chunk(CREDITS1_PCX + page, 0);
		FadeFromBlack(20);
		oldtick = pcTickCount;
		hit = 0;
		while (pcTickCount-oldtick < 2500 && !hit)	//BUG: maybe '&& !hit' was supposed to be added to the for loop instead of the while loop?
		{
			DrawStarfield();
			if (kbhit())
			{
				hit = 1;	//useless assignment...
				break;	//this breaks the while loop and the variable is not used anywhere else
			}
			if (config.controlMode)
			{
				HandleJoystick();
				if (joyBtn1Down || joyBtn2Down)
				{
					hit = 1;	//useless assignment...
					break;	//this breaks the while loop and the variable is not used anywhere else
				}
			}
		}
		FadeToBlack(15);
		while (kbhit())
			getch();
	}
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);
}

void ShowPreviews(void)
{
	register Sint16 page;
	char c;
	
	for (page=0; page<11; page++)
	{
		DrawPCX_Chunk(PREVIEW1_PCX + page, 0);
		DrawImageChunk(OLD_HUD_IMG, 0, 160);
		strcpy(strbuf, "Check out the entire 4 game series!");
		CENTER(DrawText_GOB, 170, GRD_RED_HI, strbuf);
		strcpy(strbuf, "Order Hocus Pocus today!");
		CENTER(DrawText_GOB, 183, GRD_RED_HI, strbuf);
		while (kbhit())
			getch();
		FadeFromBlack(20);
		for (;;)
		{
			c = HandleJoystick();
			WaitVBL();
			if (c == JOY_BTN1)
			{
				break;
			}
			if (c == JOY_BTN2)
			{
				c = CHAR_ESCAPE;
				break;
			}
			if (kbhit())
			{
				c = getch();
				break;
			}
		}
		FadeToBlack(20);
		while (kbhit())
			getch();
		if (c == CHAR_ESCAPE)
			break;
	}
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);
}

void ShowHelp(Uint16 isInGame)
{
	register Sint16 page;
	char c;
	Uint16 oldDrawPage, oldDisplayPage;
	
	if (isInGame)
	{
		KB_Shutdown();
		oldDrawPage = currDrawPage;
		oldDisplayPage = currDisplayPage;
		FadeToBlack(20);
		CopyBytes(backupPalette, currPalette, 0x300);
		SetToBlack();
		LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
		LoadPaletteChunk(MENUPAL_PAL, 128, 0);
		SetToBlack();
		SetDrawPage(0);
		ClearScreen();
		SetDisplayPage(0);
	}
	if (config.controlMode)
	{
		page = 1;
	}
	else
	{
		page = 0;
	}
	while (page < HELP3_PCX+1-HELP1_K_PCX)
	{
		DrawPCX_Chunk(HELP1_K_PCX + page, 0);
		FadeFromBlack(20);
		c = MenuWait();
		while (kbhit())
			getch();
		if (c == CHAR_ESCAPE)
		{
			break;
		}
		page++;
		if (page == 1)
			page = 2;
	}
	if (isInGame)
	{
		CopyBytes(currPalette, backupPalette, 0x300);
		CopyScreenPage(0, 1);
		SetDisplayPage(oldDisplayPage);
		SetDrawPage(oldDrawPage);
		FadeFromBlack(1);
		KB_Startup();
	}
	else
	{
		SetToBlack();
		LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
		LoadPaletteChunk(MENUPAL_PAL, 128, 0);
	}
}

Sint16 IngameMenu(void)
{
	register Sint16 menuresult, i;
	Sint8 c;
	Uint16 oldDrawPage, oldDisplayPage;
	
	KB_Shutdown();
	oldDrawPage = currDrawPage;
	oldDisplayPage = currDisplayPage;
	FadeToBlack(20);
	CopyBytes(backupPalette, currPalette, 0x300);
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);
	menuresult = 0;
	c = 0;
	while (c != -1)
	{
		c = GameMenu();
		switch (c)
		{
		case 0:	// "How to play Hocus Pocus"
			ShowHelp(0);
			break;
			
		case 1:	// "Abandon level & restart"
			menuresult = 1;
			goto after_loop;
			
		case 3:	// "Restore an old game"
			if (RestoreMenu())
			{
				menuresult = 3;
			}
			else
			{
				menuresult = 0;
			}
			goto after_loop;
			
		case 2:	// "Save this game"
			SaveMenu();
			menuresult = 0;
			goto after_loop;
			
		case 5:	// "Back to the action!"
			goto after_loop;
			
		case 4:	// "Change game options"
options_menu:
			if (!useMusic && !useDigitized)
			{
				config.musicOn = 0;
			}
			c = OptionsMenu();
			switch (c)
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
				{
					StopMusic();
				}
				else
				{
					PlayMusicChunk(MUSIC01_MID + levelMusicNumbers[episode][level], true);
				}
				goto options_menu;
				
			case 2:
				VolumeMenu();
				goto options_menu;
				
			case 3:
				config.controlMode = !config.controlMode;
				if (config.controlMode)
				{
					if (!CalibrateJoy())
					{
						config.controlMode = 0;
					}
				}
				goto options_menu;
				
			case 4:
				c = GameSpeedMenu();
				if (c != -1)
				{
					gamespeed = c;
					config.gamespeed = c;
				}
				goto options_menu;
				
			case 5:
				KeyControlsMenu();
				for (i=0; i<8; i++)
				{
					config.controlKeys[i] = controlKeys[i];
				}
				goto options_menu;
			}
			if (!useMusic && !useDigitized)
			{
				config.musicOn = 0;
			}
			if (!WriteToFile(configPath, &config, sizeof(config)))
			{
				Show1Line("I/O Error - HOCUS.SAV");
			}
			menuresult = 0;
			goto after_loop;
			
		case 6:	// "Quit to Main Menu"
			if (AskQuit())
			{
				menuresult = 2;
				goto after_loop;
			}
			break;
		}
	}
after_loop:
	if (menuresult == 0)
	{
		CopyBytes(currPalette, backupPalette, 0x300);
		CopyScreenPage(0, 1);
		SetDisplayPage(oldDisplayPage);
		SetDrawPage(oldDrawPage);
		FadeFromBlack(1);
	}
	else
	{
		ClearScreen();
	}
	KB_Startup();
	return menuresult;
}

Uint16 IntermissionScreen(Sint16 won, Sint16 total, Sint16 found)
{
	register Sint16 y;
	Uint16 seconds;
	Sint32 bonus;
	float percent;
	
	seconds = levelTime/140;
	y = (144-((won*40)+64))/2 +40;	//seriously, just use 'y = won? 60 : 80;' or 'y = 80 - 20*won;'
	bonus = 0;
	SetToBlack();
	SetDrawPage(0);
	ClearScreen();
	SetDisplayPage(0);
	if (won)
		strcpy(strbuf, "Castle complete - congratulations!");
	else
		strcpy(strbuf, "Tough break - you'll get it next time!");
	CENTER(DrawText_G, y, GRD_WHITE, strbuf);
	y += 16;
	strcpy(strbuf, "Results for Level ");
	strcat(strbuf, itoa(level+1, numbuf, 10));
	CENTER(DrawText_G, y, GRD_RED, strbuf);
	y += 24;
	strcpy(strbuf, "Treasures found: ");
	strcat(strbuf, itoa(found, numbuf, 10));
	strcat(strbuf, "  Treasures available: ");
	strcat(strbuf, itoa(total, numbuf, 10));
	CENTER(DrawText_G, y, GRD_GREEN, strbuf);
	y += 16;
	if (found == 0)
	{
		percent = 0.0;
	}
	else
	{
		percent = (float)found / (float)total * 100.0;	//using ((Sint32)found*100L)/(Sint32)total would work, too (32bit integer instead of float arithmetics - should be a no-brainer)
	}
	strcpy(strbuf, "Accuracy: ");
	strcat(strbuf, itoa(percent, numbuf, 10));
	strcat(strbuf, "%  ");
	if (won)
	{
		if (percent >= 100.0)
		{
			if (skill == 0)
			{
				strcat(strbuf, "BONUS 25,000 points!");
				bonus += 25000;
			}
			if (skill == 1)
			{
				strcat(strbuf, "BONUS 50,000 points!");
				bonus += 50000;
			}
			if (skill >= 2)
			{
				strcat(strbuf, "BONUS 75,000 points!");
				bonus += 75000;
			}
		}
		else
		{
			strcat(strbuf, "Less than 100% - NO BONUS");
		}
	}
	else
	{
		strcat(strbuf, "Castle not complete");
	}
	CENTER(DrawText_G, y, GRD_RED, strbuf);
	y += 24;
	if (won)
	{
		strcpy(strbuf, "Time to beat: ");
		strcat(strbuf, itoa(parTimes[episode][level], numbuf, 10));
		strcat(strbuf, "  Your time: ");
		strcat(strbuf, itoa(seconds, numbuf, 10));
		CENTER(DrawText_G, y, GRD_GREEN, strbuf);
		y += 16;
		if (seconds < parTimes[episode][level])
		{
			if (skill == 0)
			{
				strcpy(strbuf, "BONUS 25,000 points!");
				bonus += 25000;
			}
			if (skill == 1)
			{
				strcpy(strbuf, "BONUS 50,000 points!");
				bonus += 50000;
			}
			if (skill >= 2)
			{
				strcpy(strbuf, "BONUS 75,000 points!");
				bonus += 75000;
			}
		}
		else
		{
			strcpy(strbuf, "Not fast enough - NO BONUS");
		}
		CENTER(DrawText_G, y, GRD_RED, strbuf);
		y += 24;	//not required, y is set to 0 below
	}
	score += bonus;
	DrawImageChunk(BOTTOM_IMG, 0, 184);
	CENTER(DrawText_GS, 188, GRD_WHITE, "Press any key");
	DrawImageChunk(HOCUS_IMG, 0, 0);
	FadeFromBlack(20);
	y = 0;
	while (kbhit())
		getch();
	if (config.controlMode)
	{
		do {
			HandleJoystick();
			DrawStarfield();
		} while (joyBtn1Down || joyBtn2Down);
	}
	while (!y)
	{
		if (config.controlMode)
		{
			HandleJoystick();
			if (joyBtn1Down || joyBtn2Down)
				y = 1;
		}
		if (kbhit())
		{
			y = 1;
		}
		DrawStarfield();
	}
	while (kbhit())
		getch();
	FadeToBlack(20);
	return 0;
}

void WarpScreen(void)
{
	SetDrawPage(0);
	ClearScreen();
	SetDrawPage(1);
	ClearScreen();
	SetDrawPage(0);
	DrawPCX_Chunk(WARP_PCX, 0);	//warp.pcx
	PlayVocChunk(SOUND04_VOC);	//ohyeah.voc
	SetDisplayPage(0);
	FadeFromBlack(60);
	FadeToWhite();
	FadeToBlack(20);
	SetToBlack();
	LoadPaletteChunk(GAMEPAL_PAL, 0, 0);
	LoadPaletteChunk(MENUPAL_PAL, 128, 0);
}