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

#include <STDDEF.H>
#include "LL_MAN.H"

#define OFFSET(item, of) (*( (void**) ( (char*)(item) + (of) ) ))

void LL_AddNode(void *item, void **head, void **tail, int next, int prev)
{
	/*
	item->prev = NULL;
	item->next = *head;
	if (*head)
	{
		(*head)->prev = item;
	}
	else
	{
		*tail = item;
	}
	*head = item;
	*/
	
	OFFSET(item, prev) = NULL;
	OFFSET(item, next) = *head;
	if (*head)
	{
		OFFSET(*head, prev) = item;
	}
	else
	{
		*tail = item;
	}
	*head = item;
}

void LL_RemoveNode(void *item, void **head, void **tail, int next, int prev)
{
	/*
	if (!item->prev)
	{
		*head = item->next;
	}
	else
	{
		item->prev->next = item->next;
	}
	if (!item->next)
	{
		*tail = item->prev;
	}
	else
	{
		item->next->prev = item->prev;
	}
	item->next = NULL;
	item->prev = NULL;
	*/

	if (!OFFSET(item, prev))
	{
		*head = OFFSET(item, next);
	}
	else
	{
		OFFSET( OFFSET(item, prev), next) = OFFSET(item, next);
	}
	if (!OFFSET(item, next))
	{
		*tail = OFFSET(item, prev);
	}
	else
	{
		OFFSET( OFFSET(item, next), prev) = OFFSET(item, prev);
	}
	OFFSET(item, next) = NULL;
	OFFSET(item, prev) = NULL;
}