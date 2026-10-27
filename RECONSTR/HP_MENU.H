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

#ifndef __HP_MENU_H__
#define __HP_MENU_H__

#include "HP_DEFS.H"

enum {JOY_NOTHING=0, JOY_UP, JOY_DOWN, JOY_BTN1, JOY_RIGHT, JOY_LEFT, JOY_BTN2};

enum {GRD_NOTHING=0, GRD_BLUE, GRD_RED, GRD_GREEN, GRD_WHITE, GRD_GOLD, GRD_PINK, GRD_RED_HI, GRD_YELLOW_HI, GRD_GREEN2};

#define CENTER(DrawFunc, y, color, text) DrawFunc((320-TextWidth(text))/2, y, color, text)

#define LEFT(DrawFunc, x, y, color, text) DrawFunc(x-TextWidth(text), y, color, text)

#define MENUFRAME(index) \
	DrawImageChunk(BOTTOM_IMG, 0, 184);	\
	CENTER(DrawText_GS, 188, GRD_WHITE, menuBottomLines[index]);	\
	DrawImageChunk(HOCUS_IMG, 0, 0);

//#define FAST_ANGLES	//makes the starfield animation a LOT faster

#ifdef FAST_ANGLES

#define FIXED_SHIFT 20
#define FIXED_FACTOR (((Sint32)1) << FIXED_SHIFT)
#define SIN_MUL(val, angle) (((val) * sin_cos_table[(angle)].sin) >> (FIXED_SHIFT))
#define COS_MUL(val, angle) (((val) * sin_cos_table[(angle)].cos) >> (FIXED_SHIFT))

#else

#define FIXED_FACTOR 1000000
#define SIN_MUL(val, angle) (((val) * sin_cos_table[(angle)].sin) / (FIXED_FACTOR))
#define COS_MUL(val, angle) (((val) * sin_cos_table[(angle)].cos) / (FIXED_FACTOR))

#endif

//////////
// Types:
//////////

typedef struct {
	Sint16 number, x, y;
} TTextPic;

/*
typedef struct {
	Sint16 x;
	Sint16 color;
} TLineInfo;
*/

typedef struct {
	Sint16 lineCount;
	Sint16 lineHeight;
	Sint16 nullHeight;
	Sint16 lineInfo[20*2];
	char lines[20][80];	//20 lines, each 80 chars long
} TTextPage;

VERIFY_SIZE(TTextPic, 6)
VERIFY_SIZE(TTextPage, 1686)

//exported functions:

//TODO: comment out what's only used locally in HP_MENU.C

Uint16 HandleJoystick(void);
void DrawMenuCursor(Sint16 x, Sint16 y);
void DrawStarfield(void);
void InitStarfield(void);
void DrawText_P(Sint16 x, Sint16 y, Uint16 color, char *text);
void DrawText(Sint16 x, Sint16 y, Uint16 color, char *text);
void DrawText_G(Sint16 x, Sint16 y, Uint16 color, char *text);
Sint16 TextWidth(char *text);
void DrawMenuItem(Sint16 x, Sint16 y, Uint16 color1, Uint16 color2, char *text);
void DrawText_GS(Sint16 x, Sint16 y, Uint16 color, char *text);
void DrawText_GOW(Sint16 x, Sint16 y, Uint16 color, char *text);
void DrawText_GOB(Sint16 x, Sint16 y, Uint16 color, char *text);
void PutText(Sint16 x, Sint16 y, Uint16 textColor, Uint16 backColor, Uint16 maxlen, unsigned char *text);
void ClearTextScreen(void);
void PutRect(Uint16 minX, Sint16 minY, Uint16 maxX, Sint16 maxY, Uint16 textColor, Uint16 backColor);
void InitMenu(Uint16 menuNum);
void MenuDraw1Line(char *text, Uint16 botLineIndex);
void MenuDraw2Lines(char *line1, char *line2, Uint16 botLineIndex);
Sint8 MenuWait(void);
Sint8 MenuConfirm(void);
Sint8 MenuWaitDigit(void);
void MenuWaitNewKey(Uint16 keynum);
void MenuDrawSlider(Sint16 x, Sint16 y, Sint16 frame);
void VolumeMenu(void);
void IngameVolume(void);
void KeyControlsMenu(void);
void DrawMenu(void);
Sint16 CheckHighscore(Sint16 ep, Sint32 points);
void EnterHighscore(Sint16 ep, Sint16 pos);
void HighscoreMenu(Uint16 timed);
void StoreMenuPos(void);
Sint8 HandleMenu(Uint16 exitToDemo);
Sint8 MainMenu(void);
Sint8 EpisodeMenu(void);
Sint8 OptionsMenu(void);
Sint8 LevelMenu(void);
Sint8 SkillMenu(void);
Sint8 GameMenu(void);
Sint8 GameSpeedMenu(void);
void Show1Line(char *text);
Uint16 SaveMenu(void);
Uint16 RestoreMenu(void);
Uint16 AskQuit(void);
Uint16 CalibrateJoy(void);
void IngameCalibrate(void);
void IngameSave(void);
Uint16 IngameRestore(void);
Uint16 IngameQuit(void);
void DrawOrderPage(Sint16 page, Sint16 pages);
void DrawStoryPage(Sint16 page, Sint16 pages);
void DrawFinale1Page(Sint16 page, Sint16 pages);
void DrawFinale2Page(Sint16 page, Sint16 pages);
void DrawFinale3Page(Sint16 page, Sint16 pages);
void DrawFinale4Page(Sint16 page, Sint16 pages);
Sint16 HandleText(Sint16 page, Sint16 pages);
void Ordering(void);
void Story(void);
void Finale1(void);
void Finale2(void);
void Finale3(void);
void Finale4(void);
void Registering(void);
void ShowCredits(void);
void ShowPreviews(void);
void ShowHelp(Uint16 isInGame);
Sint16 IngameMenu(void);
Uint16 IntermissionScreen(Sint16 won, Sint16 total, Sint16 found);
void WarpScreen(void);

#endif