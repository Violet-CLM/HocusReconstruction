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

#include <DOS.H>
#include <STRING.H>
#include "USER.H"
#include "COMMON.H"

int USER_CheckParameter(char *parameter)
{
	int found;
	int i;
	char *arg;
	
	found = false;
	for (i=1; i<_argc; i++)
	{
		arg = _argv[i];
		
		if (*arg == '-' || *arg == '/')
		{
			arg++;
			if (stricmp(parameter, arg) == 0)
			{
				found = true;
				break;
			}
		}
	}
	
	return found;
}

char *USER_GetText(char *setting)
{
	char *text;
	char *arg;
	int i;
	
	text = NULL;
	
	for (i=1; i<_argc; i++)
	{
		arg = _argv[i];
		
		if (*arg == '-' || *arg == '/')
		{
			arg++;
			if (stricmp(setting, arg) == 0)
			{
				i++;
				text = _argv[i];
				break;
			}
		}
	}
	
	return text;
}
