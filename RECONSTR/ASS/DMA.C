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

#include <CONIO.H>
#include <DOS.H>
#include <STDLIB.H>
#include "DMA.H"

typedef struct
{
	int Valid;
	int Width;
	int Mask;
	int Mode;
	int Clear;
	int Page;
	int Address;
	int Length;
} DMA_PORT;

static DMA_PORT DMA_PortInfo[8] =
{
	{1, 0, 0x0A, 0x0B, 0x0C, 0x87, 0x00, 0x01},
	{1, 0, 0x0A, 0x0B, 0x0C, 0x83, 0x02, 0x03},
	{0, 0, 0x0A, 0x0B, 0x0C, 0x81, 0x04, 0x05},
	{1, 0, 0x0A, 0x0B, 0x0C, 0x82, 0x06, 0x07},
	{0, 1, 0xD4, 0xD6, 0xD8, 0x8F, 0xC0, 0xC2},
	{1, 1, 0xD4, 0xD6, 0xD8, 0x8B, 0xC4, 0xC6},
	{1, 1, 0xD4, 0xD6, 0xD8, 0x89, 0xC8, 0xCA},
	{1, 1, 0xD4, 0xD6, 0xD8, 0x8A, 0xCC, 0xCE}
};
static int DMA_ErrorCode = 0;

char *DMA_ErrorString(int error)
{
	char *errmsg;
	
	switch (error)
	{
	case -1:
		errmsg = DMA_ErrorString(DMA_ErrorCode);
		break;
		
	case 0:
		errmsg = "DMA channel ok.";
		break;
		
	case 1:
		errmsg = "DMA channel out of valid range.";
		break;
		
	case 2:
		errmsg = "Unsupported DMA channel.";
		break;
		
	default:
		errmsg = "Unknown DMA error code.";
	}
	
	return errmsg;
}

int DMA_VerifyChannel(int channel)
{
	int status;
	int err;
	
	status = 0;
	err = 0;
	
	if (channel < 0 || channel > 7)
	{
		err = 1;
		status = -1;
	}
	else if (!DMA_PortInfo[channel].Valid)
	{
		err = 2;
		status = -1;
	}
	
	DMA_ErrorCode = err;
	return status;
}

int DMA_SetupTransfer(int channel, char far *address, unsigned length)
{
	DMA_PORT *Port;
	unsigned long addr;
	int ChannelSelect;
	unsigned long linearaddr;
	int Page;
	int HiByte;
	int LoByte;
	unsigned TransferLength;
	int status;
	
	status = DMA_VerifyChannel(channel);
	if (status == 0)
	{
		Port = DMA_PortInfo + channel;
		ChannelSelect = channel & 3;
		
		addr = (unsigned long)address;
		linearaddr = ((addr >> 16) << 4) + (unsigned)addr;
		
		if (Port->Width == 1)
		{
			Page = (linearaddr >> 16) & 0xFF;
			// Note: I'm not sure if the following code is correct. This shifts
			// the lowest bit of the "page" number into the high byte of the
			// address and the lowest bit of the high byte of the address into the
			// low byte of the address. This might be okay if the DMA controller
			// ignores the highest bit of the high address byte when in WORD mode,
			// but I don't know if that is how it works.
			//
			// EXAMPLE: address is 7123:0004
			//
			// linearaddr is: 0x00071234
			//       Page is: 7
			//     HiByte is: 0x89
			//     LoByte is: 0x1A
			//
			// This would mean the two address bytes form the address 0x891A, but
			// I think the address within Page 7 should be 0x1234 >> 1, which is
			// 0x91A, not 0x891A.
			HiByte = (linearaddr >> 9) & 0xFF;
			LoByte = (linearaddr >> 1) & 0xFF;
			
			TransferLength = (length + 1) >> 1;	// round UP to number of WORDS
			TransferLength--;
		}
		else
		{
			Page = (linearaddr >> 16) & 0xFF;
			HiByte = (linearaddr >> 8) & 0xFF;
			LoByte = linearaddr & 0xFF;
			
			TransferLength = length - 1;
		}
		
		outp(Port->Mask, 4 | ChannelSelect);
		outp(Port->Clear, 0);
		outp(Port->Mode, 0x58 | ChannelSelect);
		outp(Port->Address, LoByte);
		outp(Port->Address, HiByte);
		outp(Port->Page, Page);
		outp(Port->Length, TransferLength);
		outp(Port->Length, (TransferLength >> 8));
		outp(Port->Mask, ChannelSelect);
	}
	return status;
}

void far * DMA_GetCurrentPos(int channel)
{
	register unsigned LoByte, Page;
	DMA_PORT *Port;
	int status;
	unsigned /*LoByte, */HiByte;
	
	Page = 0;
	LoByte = 0;
	HiByte = 0;
	
	status = DMA_VerifyChannel(channel);
	if (status == 0)
	{
		Port = DMA_PortInfo + channel;
		
		if (Port->Width == 1)
		{
			// Note: I'm not sure if this is correct, either. At least the
			// variables are 16-bit variables, so no bits are lost by the shifts.
			// But the highest bit of the high byte may be problematic for the
			// pointer address calculation at the end (the highest bit gets added
			// to the "page"). This wouldn't be a problem if the highest bit of
			// the high byte of the address is ALWAYS clear when reading it from
			// the port, but I don't know if that's the case.
			LoByte = inp(Port->Address) << 1;
			HiByte = inp(Port->Address) << 1;
		}
		else
		{
			LoByte = inp(Port->Address);
			HiByte = inp(Port->Address);
		}
		Page = inp(Port->Page);
	}
	
	return MK_FP((Page << 12) + (HiByte << 4) + (LoByte >> 4), LoByte & 15);
}

unsigned DMA_GetTransferCount(int channel)
{
	DMA_PORT *Port;
	int status;
	unsigned count;
	
	status = 0;
	count = 0;
	
	if (channel < 0 || channel > 7)
	{
		status = 1;
	}
	else if (!DMA_PortInfo[channel].Valid)
	{
		status = 2;
	}
	
	if (status == 0)
	{
		Port = DMA_PortInfo + channel;
		
		outp(Port->Clear, 0);
		count = inp(Port->Length);
		count += inp(Port->Length) << 8;
		if (Port->Width == 1)
		{
			count <<= 1;
		}
	}
	
	DMA_ErrorCode = status;
	return count;
}
