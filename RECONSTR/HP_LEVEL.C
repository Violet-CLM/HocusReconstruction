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

#include "HP_LEVEL.H"

char *layerNames[3] = {"[BKG]", "[SLD]", "[FNC]"};	//never used!

TItemInfo iteminfos[23] =
{
	{"Ruby",              100,  0, 0,  0, 0},
	{"Diamond",           250,  0, 0,  0, 0},
	{"Goblet",            500,  0, 0,  0, 0},
	{"Crown",            1000,  0, 0,  0, 0},
	{"Heal (green)",        0, 10, 0,  0, 0},
	{"Crystal",             0,  0, 0,  1, 0},
	{"Hurt",                0,  0, 0,  2, 0},
	{"Add shot",            0,  0, 1,  0, 0},
	{"Invisible (black)",   0,  0, 0,  3, 0},
	{"Super jump (yellow)", 0,  0, 0,  4, 0},
	{"Super shot (white)",  0,  0, 0,  5, 0},
	{"Smart bomb",          0,  0, 0,  6, 0},
	{"Silver Key",          0,  0, 0,  7, 0},
	{"Gold Key",            0,  0, 0,  8, 0},
	{"Kill",                0,  0, 0,  9, 0},
	{"Wizard note",         0,  0, 0, 10, 0},
	{"Laser Shot",          0,  0, 0, 11, 0},
	{"FISCHER PRICE",       0,  0, 0, 12, 0},
	{"Unused",              0,  0, 0,  0, 0},
	{"Unused",              0,  0, 0,  0, 0},
	{"Unused",              0,  0, 0,  0, 0},
	{"Unused",              0,  0, 0,  0, 0},
	{"Unused",              0,  0, 0,  0, 0}
};

Sint16 levelTilesetNumbers[4][10] =
{
	{ 0,  0,  1,  1,  2,  2,  3,  3,  3,  0},
	{ 4,  4,  5,  5,  6,  6,  7,  7,  7,  0},
	{ 8,  8,  9,  9, 10, 10, 11, 11, 11,  0},
	{12, 12, 13, 13, 14, 14, 15, 15, 15,  0}
};

Sint16 levelNumbers[4][10] =
{
	{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  0},
	{ 9, 10, 11, 12, 13, 14, 15, 16, 17,  0},
	{18, 19, 20, 21, 22, 23, 24, 25, 26,  0},
	{27, 28, 29, 30, 31, 32, 33, 34, 35,  0}
};

Sint16 levelBackdropNumbers[4][10] =	//Note: same as levelTilesetNumbers
{
	{ 0,  0,  1,  1,  2,  2,  3,  3,  3,  0},
	{ 4,  4,  5,  5,  6,  6,  7,  7,  7,  0},
	{ 8,  8,  9,  9, 10, 10, 11, 11, 11,  0},
	{12, 12, 13, 13, 14, 14, 15, 15, 15,  0}
};

// Note: This source file contains nothing but data. It was probably shared by
// the game code and the level editor. The level editor might actually have been
// part of the game executable during development. One of Moonlite Software's
// other games, "Taking Care Of Business", still contains the level editor code
// in the final game executable.
