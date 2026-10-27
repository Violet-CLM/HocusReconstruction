/* Reconstructed Hocus Pocus Source Code
 * Copyright (C) 2026 K1n9_Duk3
 *
 * The code in this file is based on the Apogee Sound System by James R. Dose.
 * Copyright (C) 1994-1995 Apogee Software, Ltd.
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

#ifndef __LL_MAN_H__
#define __LL_MAN_H__

void LL_AddNode(void *item, void **head, void **tail, int next, int prev);
void LL_RemoveNode(void *item, void **head, void **tail, int next, int prev);

#define LL_AddToHead( type, listhead, node )         \
    LL_AddNode( ( node ),                            \
                (void**)&( ( listhead )->start ),    \
                (void**)&( ( listhead )->end ),      \
                ( int )&( ( type * ) 0 )->next,      \
                ( int )&( ( type * ) 0 )->prev )

#define LL_AddToTail( type, listhead, node )         \
    LL_AddNode( ( node ),                            \
                (void**)&( ( listhead )->end ),      \
                (void**)&( ( listhead )->start ),    \
                ( int )&( ( type * ) 0 )->prev,      \
                ( int )&( ( type * ) 0 )->next )

#define LL_Remove( type, listhead, node )            \
    LL_RemoveNode( ( node ),                         \
                   (void**)&( ( listhead )->start ), \
                   (void**)&( ( listhead )->end ),   \
                   ( int )&( ( type * ) 0 )->next,   \
                   ( int )&( ( type * ) 0 )->prev )

#endif