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

#include "HP_DEFS.H"
#include "HP_MAIN.H"
#include "HP_SYS.H"

void OpenDatFile(void)
{
	if ((dat_fp = fopen("HOCUS.DAT", "r+b")) == NULL)
	{
		QuitError(ERROR_CANNOT_OPEN, "DB");
	}
}

void CloseDatFile(void)
{
	fclose(dat_fp);
}

void SeekDat(Uint32 offset)
{
	fseek(dat_fp, offset, SEEK_SET);
}

void SeekDatChunk(Uint16 chunknum)
{
	fseek(dat_fp, FAT[chunknum].offset, SEEK_SET);
}

void ReadFromDat(Uint32 offset, Uint32 size, void *buffer)
{
	fseek(dat_fp, offset, SEEK_SET);
	fread(buffer, size, 1, dat_fp);
}

void WriteToDat(Uint32 offset, Uint32 size, void *buffer)
{
	fseek(dat_fp, offset, SEEK_SET);
	fwrite(buffer, size, 1, dat_fp);
}

void ReadDatChunk(Uint16 chunknum, void *buffer)
{
	Uint32 offset, size;
	
	GetFatEntry(chunknum, &offset, &size);
	fseek(dat_fp, offset, SEEK_SET);
	fread(buffer, size, 1, dat_fp);
}

Sint16 ReadFromFile(char *filename, void *buffer)
{
	FILE *fp;
	Uint32 size;
	
	fp = fopen(filename, "rb");
	if (!fp)
		return 0;
	
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (fread(buffer, size, 1, fp) < 1)
	{
		fclose(fp);
		return 0;
	}
	
	fclose(fp);
	return 1;
}

Sint16 WriteToFile(char *filename, void *buffer, Uint16 size)
{
	FILE *fp;
	
	unlink(filename);
	fp = fopen(filename, "wb");
	if (!fp)
		return 0;
	
	if (fwrite(buffer, size, 1, fp) < 1)
	{
		fclose(fp);
		return 0;
	}
	
	fclose(fp);
	return 1;
}

void WriteDatChunk(Uint16 chunknum, void *buffer)
{
	Uint32 offset, size;
	
	GetFatEntry(chunknum, &offset, &size);
	fseek(dat_fp, offset, SEEK_SET);
	fwrite(buffer, size, 1, dat_fp);
}

Uint8 ReadByteFromDat(void)
{
	return fgetc(dat_fp);
}