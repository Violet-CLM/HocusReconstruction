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

#ifndef __HP_LEVEL_H__
#define __HP_LEVEL_H__

#include "HP_DEFS.H"

typedef struct {
	char name[36];
	Sint16 score;
	Uint8 heal;
	Uint8 firepower;
	Uint8 type;
	Uint8 _padding;
} TItemInfo;

extern TItemInfo iteminfos[23];
extern Sint16 levelTilesetNumbers[4][10];
extern Sint16 levelNumbers[4][10];
extern Sint16 levelBackdropNumbers[4][10];

#endif