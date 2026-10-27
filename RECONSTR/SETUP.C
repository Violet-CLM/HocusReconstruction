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
#include <DOS.H>
#include <IO.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>

#include "HP_DEFS.H"
#include "ASS.H"

typedef struct {
	char *title;
	Sint16 numItems;
	char *items[12];
} TMenu;

TMenu menus[7] =
{
	{
		"Main Menu", 5,
		{
			"Select Music Card",
			"Select Sound FX Card",
			"Select Controller",
			"Save Settings & Exit",
			"Abort Without Saving",
			"",
			"",
			"",
			"",
			"",
			"",
			""
		}
	},
	{
		"Select Music Card", 9,
		{
			"General MIDI",
			"Sound Canvas",
			"Wave Blaster",
			"Sound Blaster",
			"Pro Audio Spectrum",
			"Sound Man 16",
			"Gravis UltraSound",
			"AdLib",
			"NONE",
			"",
			"",
			""
		}
	},
	{
		"Select MIDI Port", 12,
		{
			"220",
			"230",
			"240",
			"250",
			"300",
			"320",
			"330 (default)",
			"332",
			"334",
			"336",
			"340",
			"360"
		}
	},
	{
		"Select Sound FX Card", 6,
		{
			"Sound Blaster",
			"Pro Audio Spectrum",
			"Sound Man 16",
			"Gravis UltraSound",
			"Sound Source",
			"NONE",	// Note: NONE means PC Speaker sounds here
			"",
			"",
			"",
			"",
			"",
			""
		}
	},
	{
		"Select Controller", 2,
		{
			"Keyboard",
			"Joystick/Gamepad",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			""
		}
	},
	{
		"Select Number of Voices", 8,
		{
			"One",
			"Two",
			"Three",
			"Four (default)",
			"Five",
			"Six",
			"Seven",
			"Eight",
			"",
			"",
			"",
			""
		}
	},
	{
		"Select Mixing Quality", 2,
		{
			"8 bits (lower quality - faster)",
			"16 bits (higher quality - slower)",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			""
		}
	}
};

Sint16 musicCardMapping[] = {CARD_GENERALMIDI, CARD_GENERALMIDI, CARD_WAVEBLASTER, CARD_SOUNDBLASTER, CARD_PROAUDIO, CARD_PROAUDIO, CARD_GRAVIS, CARD_ADLIB, CARD_NONE};
Sint16 portMapping[] = {0x220, 0x230, 0x240, 0x250, 0x300, 0x320, 0x330, 0x332, 0x334, 0x336, 0x340, 0x360};
char *portStrings[12] = {"220", "230", "240", "250", "300", "320", "330", "332", "334", "336", "340", "360"};
char *voiceStrings[8] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight"};
Sint16 soundCardMapping[] = {CARD_SOUNDBLASTER, CARD_PROAUDIO, CARD_PROAUDIO, CARD_GRAVIS, CARD_SOUNDSOURCE, CARD_NONE};

Sint16 far *textscreenPtr = MK_FP(0xB800, 0);

char *defaultNames[NUMHIGHSCORES] =
{
	"Hocus Pocus",
	"Andre Foucault",
	"Jamie Cook",
	"Karim Sultan",
	"Chris tenDen"
};

char dummy[42] = "";

Uint8 colorText;
Uint8 colorBack;
Uint8 colorval;
TGameConfig config;
char configPath[82];
Sint16 menuItemsX, menuItemsY;

Sint16 joyBtn1Down, joyBtn2Down;
Sint16 joyX, joyY;
char dummy2[14];

#define JOY_PORT	0x201

void ReadJoystick(void)	//never really used
{
	Uint8 portval, x, y;
	
	joyX = joyY = 0;

	asm	pushf;
	asm	cli;
#if 1
	asm	mov	dx, JOY_PORT;
	asm	mov	al, 0;
	asm	out	dx, al;
	asm	in 	al, dx;
	asm	mov	portval, al;
#else
	outp(JOY_PORT, 0);
	portval = inp(JOY_PORT);
#endif

	if (portval & 0x10)
	{
		joyBtn1Down = 0;
	}
	else
	{
		joyBtn1Down = 1;
	}
	if (portval & 0x20)
	{
		joyBtn2Down = 0;
	}
	else
	{
		joyBtn2Down = 1;
	}

	do {
#if 1
		asm	in 	al, dx;
		asm	mov	portval, al;
#else
		portval = inp(JOY_PORT);
#endif
		if (portval & 1)
		{
			x = 1;
			joyX++;
		}
		else
		{
			x = 0;
		}
		if (portval & 2)
		{
			y = 1;
			joyY++; 
		}
		else
		{
			y = 0;
		}
	} while (x || y);	//BUG: this loop is endless if no joystick is present!

	asm	popf;
}

void TestJoystick(void)	//never used!
{
	while (!kbhit())
	{
		ReadJoystick();
		printf("B1:%d B2:%d JOYX:%d JOYY:%d\n", joyBtn1Down, joyBtn2Down, joyX, joyY);
	}
}

void SetTextMode(void)
{
	union REGS regs;
	
	regs.h.ah = 0;
	regs.h.al = 3;
	int86(0x10, &regs, &regs);
}

void HideTextCursor(void)
{
	union REGS regs;
	
	regs.h.ah = 2;
	regs.h.bh = 0;
	regs.h.dh = 25;
	regs.h.dl = 0;
	int86(0x10, &regs, &regs);
}

void SetColors(Uint16 text, Uint16 back)
{
	colorText = text;
	colorBack = back;
	colorval = (back << 4) | text;
}

void PutText(Sint16 x, Sint16 y, unsigned char *text)
{
	Sint16 pos = y*80 + x;
	
	while (*text)
	{
		textscreenPtr[pos] = (colorval << 8) | *text;
		text++;
		pos++;
	}
}

void PutTextCentered(Sint16 y, unsigned char *text)
{
	Sint16 pos = y*80 + 40 - (strlen(text) / 2);
	
	while (*text)
	{
		textscreenPtr[pos] = (colorval << 8) | *text;
		text++;
		pos++;
	}
}

void PutChars(Sint16 x, Sint16 y, unsigned char c, Sint16 len)
{
	Sint16 pos = y*80 + x;
	while (len--)
	{
		textscreenPtr[pos] = (colorval << 8) | c;
		pos++;
	}
}

void PutChar(Sint16 x, Sint16 y, unsigned char c)
{
	(textscreenPtr+y*80)[x] = (colorval << 8) | c;
}

void MoveTextCursor(Uint8 x, Uint8 y)	//never used!
{
	union REGS regs;
	
	regs.h.ah = 2;
	regs.x.bx = 0;
	regs.h.dh = y;
	regs.h.dl = x;
	int86(0x10, &regs, &regs);
}

void PutTopAndBottomLines(void)
{
	SetColors(15, 1);
	PutChars(0, 0, ' ', 80);
#ifdef VERSION_11
	PutTextCentered(0, "Hocus Pocus Setup - Version 1.1 ");
#else
	PutTextCentered(0, "Hocus Pocus Setup - Version 1.00");
#endif
	PutChars(0, 24, ' ', 80);
	PutTextCentered(24, "Use UP/DOWN arrow keys to select - ENTER to accept - ESC to abort");
}

void PutTopAndBottomLines2(void)	//never used!
{
	SetColors(15, 1);
	PutChars(0, 0, ' ', 80);
#ifdef VERSION_11
	PutTextCentered(0, "Hocus Pocus Setup - Version 1.1 ");
#else
	PutTextCentered(0, "Hocus Pocus Setup - Version 1.00");
#endif
	PutChars(0, 24, ' ', 80);
	PutTextCentered(24, "Use UP/DOWN arrow keys to select - ENTER to accept - ESC to abort");
}

void PutConfigDescription(void)
{
	SetColors(15, 0);
	PutText(0, 19, "      Music Card    :                     MIDI Port :      ");
	PutText(0, 20, "      Sound FX Card :                        Voices :        Mix :        ");
	PutText(0, 22, "               Controller :            Music :       Sound FX :    ");
}

void PutConfig(void)
{
	SetColors(2, 0);
	PutText(22, 19, menus[1].items[config.musicCardIndex]);
	if (config.musicCardIndex > 2)
	{
		PutText(54, 19, "N/A");
	}
	else
	{
		PutText(54, 19, portStrings[config.midiPortIndex]);
	}
	PutText(22, 20, menus[3].items[config.soundCardIndex]);
	if (config.soundCardIndex == 3 || config.soundCardIndex == 5)
	{
		PutText(54, 20, "N/A");
	}
	else
	{
		PutText(54, 20, voiceStrings[config.numVoicesIndex]);
	}
	if (config.soundCardIndex >= 3)
	{
		PutText(67, 20, "N/A");
	}
	else if (config.mixingQualityIndex == 0)
	{
		PutText(67, 20, "8 bits");
	}
	else	//assuming config.mixingQualityIndex == 1
	{
		PutText(67, 20, "16 bits");
	}
	if (config.controlMode)
	{
		PutText(28, 22, "Joystick");
	}
	else
	{
		PutText(28, 22, "Keyboard");
	}
	if (config.musicOn)
	{
		PutText(47, 22, "On");
	}
	else
	{
		PutText(47, 22, "Off");
	}
	if (config.soundOn)
	{
		PutText(64, 22, "On");
	}
	else
	{
		PutText(64, 22, "Off");
	}
}

void AdjustConfig(void)
{
	config.soundCard = soundCardMapping[config.soundCardIndex];
	config.numVoices = config.numVoicesIndex+1;
	config.mixBits = (config.mixingQualityIndex << 3)+8;
	config.musicCard = musicCardMapping[config.musicCardIndex];
	config.midiPort = portMapping[config.midiPortIndex];
	
	if (config.musicCardIndex > 2)
		config.midiPort = 0;
	
	if (config.soundCard == CARD_GRAVIS || config.musicCard == CARD_GRAVIS)
	{
		config.usesGravis = 1;
	}
	else
	{
		config.usesGravis = 0;
	}
}

void SetDefaultConfig(void)
{
	Sint16 i, n;
	
	config.controlKeys[C_LEFT] = CK_LEFT;
	config.controlKeys[C_RIGHT] = CK_RIGHT;
	config.controlKeys[C_JUMP] = CK_CTRL;
	config.controlKeys[C_FIRE] = CK_ALT;
	config.controlKeys[C_UP] = CK_UP;
	config.controlKeys[C_DOWN] = CK_DOWN;
	config.controlKeys[C_SCROLLUP] = CK_PGUP;
	config.controlKeys[C_SCROLLDOWN] = CK_PGDN;
	
	config.soundOn = 1;
	config.musicOn = 1;
	config.controlMode = 0;
	config.gamespeed = 1;
	config.joyLoX = config.joyHiX = config.joyLoY = config.joyHiY = 0;
	config.joyMiX = config.joyMiY = 0;
	config.joyFireButton = 0;
	
	for (i=0; i<NUMSAVEGAMES; i++)
	{
		config.sg_episodes[i] = -1;	//indicates an empty slot
		strcpy(config.sg_names[i], "<empty>");
	}
	for (i=0; i<NUMSAVEGAMES; i++)
	{
		config.unused[i] = 1;	//these values are never used by the game
	}
	for (i=0; i<NUMEPISODES; i++)
	{
		for (n=0; n<NUMHIGHSCORES; n++)
		{
			strcpy(config.hs_names[i][n], defaultNames[n]);
			config.hs_scores[i][n] = 200000L * (5-n);
		}
	}
	
	config.soundCard = CARD_NONE;
	config.numVoices = 4;
	config.mixBits = 16;
	config.musicCard = CARD_NONE;
	config.midiPort = 0x330;
	config.soundCardIndex = 5;
	config.numVoicesIndex = 3;
	config.mixingQualityIndex = 1;
	config.musicCardIndex = 8;
	config.midiPortIndex = 6;
}

void LoadConfig(void)
{
	if (access(configPath, 0))
	{
		SetDefaultConfig();
	}
	else
	{
		FILE *fp = fopen(configPath, "rb");
		fread(&config, sizeof(config), 1, fp);
		fclose(fp);
	}
}

void SaveConfig(void)
{
	FILE *fp;
	
	AdjustConfig();
	unlink(configPath);
	fp = fopen(configPath, "wb");
	fwrite(&config, sizeof(config), 1, fp);
	fclose(fp);
}

void DrawMenu(Sint16 menuNum)
{
	Sint16 h, w, x, y, i;
	
	w = 40;
	x = (80-w) / 2;
	menuItemsX = x+1;
	h = menus[menuNum].numItems+4;
	y = ((16-h) / 2) + 2;
	menuItemsY = y+3;

	SetColors(11, 1);

	PutChar(x, y, 0xDA);
	PutChars(x+1, y, 0xC4, w-2);
	PutChar(x+w-1, y, 0xBF);

	PutChar(x, y+1, 0xB3);
	SetColors(15, 1);
	PutChars(x+1, y+1, ' ', w-2);
	PutTextCentered(y+1, menus[menuNum].title);
	SetColors(11, 1);
	PutChar(x+w-1, y+1, 0xB3);

	PutChar(x, y+2, 0xC3);
	PutChars(x+1, y+2, 0xC4, w-2);
	PutChar(x+w-1, y+2, 0xB4);

	for (i=0; i<menus[menuNum].numItems; i++)
	{
		PutChar(x, y+i+3, 0xB3);
		SetColors(7, 1);
		PutChars(x+1, y+i+3, ' ', w-2);
		PutText(x+2, y+i+3, menus[menuNum].items[i]);
		SetColors(11, 1);
		PutChar(x+w-1, y+i+3, 0xB3);
	}
	
	PutChar(x, y+h-1, 0xC0);
	PutChars(x+1, y+h-1, 0xC4, w-2);
	PutChar(x+w-1, y+h-1, 0xD9);
}

void ClearMenuArea(void)
{
	SetColors(7, 0);
	PutChars(0, 2, ' ', 16*80);
}

Sint16 HandleMenu(Sint16 menuNum, Sint16 menuPos)
{
	Uint8 c;
	Sint16 done, result, maxPos;
	
	maxPos = menus[menuNum].numItems-1;
	done = 0;
	
	while (!done)
	{
		SetColors(14, 4);
		PutChars(menuItemsX, menuItemsY+menuPos, ' ', 38);
		PutText(menuItemsX+1, menuItemsY+menuPos, menus[menuNum].items[menuPos]);
		c = getch();
		SetColors(7, 1);
		PutChars(menuItemsX, menuItemsY+menuPos, ' ', 38);
		PutText(menuItemsX+1, menuItemsY+menuPos, menus[menuNum].items[menuPos]);
		if (!c)
		{
			c = getch();
			if (c==KEY_UP || c==KEY_LEFT || c==KEY_PGUP)
				menuPos--;
			if (c==KEY_DOWN || c==KEY_RIGHT || c==KEY_PGDN)
				menuPos++;
			if (c==KEY_HOME)
				menuPos = 0;
			if (c==KEY_END)
				menuPos = maxPos;
			if (menuPos < 0)
				menuPos = maxPos;
			if (menuPos > maxPos)
				menuPos = 0;
		}
		else
		{
			if (c == CHAR_ENTER)
			{
				result = menuPos;
				done = 1;
			}
			if (c == CHAR_ESCAPE)
			{
				result = -1;
				done = 1;
			}
		}
	}
	return result;
}

void SelectMidiPort(void)
{
	Sint16 result;
	
	ClearMenuArea();
	DrawMenu(2);
	result = HandleMenu(2, config.midiPortIndex);
	if (result != -1)
	{
		config.midiPortIndex = result;
		PutConfigDescription();
		PutConfig();
	}
}

void SelectMusicCard(void)
{
	Sint16 result;
	
	ClearMenuArea();
	DrawMenu(1);
	result = HandleMenu(1, config.musicCardIndex);
	if (result != -1)
	{
		config.musicCardIndex = result;
		PutConfigDescription();
		PutConfig();
		if (result < 3)
			SelectMidiPort();
		config.musicVolume = 192;
	}
}

void SelectMixingQuality(void)
{
	Sint16 result;
	
	ClearMenuArea();
	DrawMenu(6);
	result = HandleMenu(6, config.mixingQualityIndex);
	if (result != -1)
	{
		config.mixingQualityIndex = result;
		PutConfigDescription();
		PutConfig();
	}
}

void SelectNumberOfVoices(void)
{
	Sint16 result;
	
	ClearMenuArea();
	DrawMenu(5);
	result = HandleMenu(5, config.numVoicesIndex);
	if (result != -1)
	{
		config.numVoicesIndex = result;
		PutConfigDescription();
		PutConfig();
		if (config.soundCardIndex < 3)
			SelectMixingQuality();
	}
}

void SelectSoundFxCard(void)
{
	Sint16 result;
	
	ClearMenuArea();
	DrawMenu(3);
	result = HandleMenu(3, config.soundCardIndex);
	if (result != -1)
	{
		config.soundCardIndex = result;
		PutConfigDescription();
		PutConfig();
		if (result != 3 && result != 5)
			SelectNumberOfVoices();
		config.soundVolume = 255;
	}
}

void SelectController(void)
{
	Sint16 result;
	
	ClearMenuArea();
	DrawMenu(4);
	result = HandleMenu(4, config.controlMode);
	if (result != -1)
	{
		config.controlMode = result;
		PutConfigDescription();
		PutConfig();
	}
}

void MainMenu(void)
{
	Sint16 result=0;
	
	for (;;)
	{
		ClearMenuArea();
		DrawMenu(0);
		result = HandleMenu(0, result);
		switch (result)
		{
		case 0:
			SelectMusicCard();
		default:
			continue;
		case 1:
			SelectSoundFxCard();
			continue;
		case 2:
			SelectController();
			continue;
		case 3:
			SaveConfig();
		case -1:
		case 4:
			break;
		}
		break;
	}
}

void main(void)
{
	strcpy(configPath, "");	//useless call
	strcpy(configPath, (!getenv(CONFIG_ENV)? "" : getenv(CONFIG_ENV)));
	if (strlen(configPath) && configPath[strlen(configPath)-1] != '\\')
		strcat(configPath, "\\");
	strcat (configPath, CONFIG_FILE);

	LoadConfig();
	SetTextMode();	//clear the screen
	HideTextCursor();
	PutTopAndBottomLines();
	PutConfigDescription();
	PutConfig();
	MainMenu();
	SetTextMode();	//clear the screen

	if (config.usesGravis)
	{
		printf("Run HOCUSG to play.\n");
	}
	else
	{
		printf("Run HOCUS to play.\n");
	}
}