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
#include <ALLOC.H>
#include <DOS.H>
#include <STDLIB.H>
#include "COMMON.H"
#include "TASK_MAN.H"
#include "LL_MAN.H"

typedef struct
{
	task *start;
	task *end;
} TTaskList;


#define DEFAULT_TIMER_RATE 0x10000	// default timer rate (==18.2 Hz)

static long TaskServiceRate = DEFAULT_TIMER_RATE;
static long TaskServiceCount = 0;
static char TS_Enabled = false;
static char TS_Terminating = false;
static char TS_Installed = false;
static char TS_GotAtExit = false;
static char TS_InInterrupt = false;

static TTaskList TaskList;
static void interrupt (*OldInt8)(void);

// prototypes:
static void TS_Count(void);
static void TS_SetTimerToMaxRate(void);

static void TS_FreeTaskList(TTaskList *list)
{
	task *tsk;
	task *nexttask;
	
	for (tsk = list->start; tsk != NULL; tsk = nexttask)
	{
		nexttask = tsk->next;
		free(tsk);
	}
	list->start = NULL;
	list->end = NULL;
}

static void TS_SetClockSpeed(long speed)
{
	asm	pushf;
	asm	cli;
	
	if (speed > 0 && speed < DEFAULT_TIMER_RATE)
	{
		TaskServiceRate = speed;
	}
	else
	{
		TaskServiceRate = DEFAULT_TIMER_RATE;
	}
	
	outp(0x43, 0x36);
	outp(0x40, TaskServiceRate);
	outp(0x40, (TaskServiceRate >> 8));
	
	asm	popf;
}

static int TS_SetTimer(int TickBase)
{
	int rate;	// BUG: This should be long or at least unsigned!
	
	rate = 0x12305E / TickBase;
	// rate will be negative if TickBase is <= 36, causing the timer to slow down
	// to 18.2 Hz, even if there are tasks that are supposed to be run at a much
	// higher frequency.
	
	if (rate < TaskServiceRate)
	{
		TS_SetClockSpeed(rate);
	}
	
	return rate;
}

static void interrupt TS_ServiceSchedule(void)
{
	TS_InInterrupt = true;
	
	TS_Count();
	TS_SetTimerToMaxRate();
	
	TaskServiceCount += TaskServiceRate;
	if (TaskServiceCount > 0xFFFF)
	{
		TaskServiceCount &= 0xFFFF;
		OldInt8();
	}
	else
	{
		outp(0x20, 0x20);
		TS_InInterrupt = false;	// BUG: This should be done outside the if-block!
	}
}

static void TS_Count(void)
{
	task *tsk;
	task *next;
	
	if (TS_Enabled == false)
	{
		return;
	}
	
	for (tsk = TaskList.start; tsk != NULL; tsk = next)
	{
		next = tsk->next;	// read now because tsk might remove itself
		
		if (tsk->priority > 0 || !TS_Terminating)
		{
			tsk->count += TaskServiceRate;
			if (tsk->count > tsk->rate + (-1))
			{
				tsk->count -= tsk->rate;
				tsk->TaskService(tsk);
				
				if (!TS_Enabled)
				{
					return;
				}
			}
		}
	}
}

static void TS_SetTimerToMaxRate(void)
{
	task *ptr;
	task *next;
	long MaxServiceRate;	// bad name (we're looking for the SMALLEST value)
	
	if (TS_Terminating == false)
	{
		return;
	}
	
	MaxServiceRate = DEFAULT_TIMER_RATE;	// slowest possible value (18.2 Hz)
	for (ptr = TaskList.start; ptr != NULL; ptr = next)
	{
		next = ptr->next;	// read now because we might remove a task
		
		if (ptr->priority <= 0)
		{
			//
			// remove the task from the list
			//
			if (!ptr->next)
			{
				TaskList.end = ptr->prev;
			}
			else
			{
				next->prev = ptr->prev;
			}
			if (!ptr->prev)
			{
				TaskList.start = next;
			}
			else
			{
				ptr->prev->next = next;
			}
			
			ptr->priority = 0;
			ptr->next = NULL;
			ptr->prev = NULL;
			
			// BUG: Calling C library routines from an interrupt should be avoided!
			free(ptr);
			
			// A possible workaround for this would be to use a separate list of
			// unused task structures and add the task structure to that list in
			// here if the function was called from within an interrupt (i.e., if
			// TS_InInterrupt is true) and free all task structures in that list
			// (with TS_FreeTaskList, for example) when the function was NOT
			// called from within an interrupt.
		}
		else
		{
			if (ptr->rate < MaxServiceRate)
			{
				MaxServiceRate = ptr->rate;
			}
		}
	}
	
	TS_Terminating = false;
	
	if (TaskServiceRate != MaxServiceRate)
	{
		TS_SetClockSpeed(MaxServiceRate);
	}
}

static void TS_InsertTaskSorted(TTaskList *list, task *item)
{
	task *tsk;
	
	if (!list->start)
	{
		LL_AddToHead(task, list, item);
	}
	else if (item->priority >= list->end->priority)
	{
		LL_AddToTail(task, list, item);
	}
	else
	{
		for (tsk = list->start; item->priority >= tsk->priority; tsk = tsk->next);
		
		if (!tsk->prev)
		{
			LL_AddToHead(task, list, item);
		}
		else
		{
			item->prev = tsk->prev;
			item->next = tsk;
			tsk->prev = item;
			item->prev->next = item;
		}
	}
}

void TS_Startup(void)
{
	TaskList.start = NULL;
	TaskList.end = NULL;
	TS_Enabled = false;
	TS_Terminating = false;
	TaskServiceRate = DEFAULT_TIMER_RATE;
	TaskServiceCount = 0;
	
	OldInt8 = getvect(8);
	setvect(8, TS_ServiceSchedule);
	
	TS_Installed = true;
	if (TS_GotAtExit == false)
	{
		atexit(TS_Shutdown);
		TS_GotAtExit = true;
	}
}

void TS_Shutdown(void)
{
	if (TS_Installed)
	{
		TS_SetClockSpeed(0);
		setvect(8, OldInt8);
		TS_FreeTaskList(&TaskList);
		TS_Enabled = false;
		TS_Installed = false;
	}
}

task * TS_ScheduleTask(taskfunctype func, int rate, int priority, void far *dataptr)
{
	task *taskptr;
	
	// Note: This function MUST NOT be called from within a task service!
	taskptr = NULL;
	if (priority > 0)
	{
		taskptr = malloc(sizeof(task));
		if (taskptr)
		{
			if (!TS_Installed)
			{
				TS_Startup();
			}
			
			taskptr->TaskService = func;
			taskptr->data = dataptr;
			taskptr->rate = TS_SetTimer(rate);
			taskptr->count = 0;
			taskptr->priority = priority;
			TS_AddTask(taskptr);
		}
	}
	
	return taskptr;
}

void TS_AddTask(task *item)
{
	TS_InsertTaskSorted(&TaskList, item);
}

int TS_Terminate(task *NodeToRemove)
{
	// BUG: This doesn't check if NodeToRemove is NULL!
	if (TS_InInterrupt)
	{
		// TS_Terminate was called from within a task's TaskService routine here.
		// This will lead to 'free' being called from within TS_ServiceSchedule,
		// which means the 'free' function is called from within an interrupt.
		// Calling the function from an interrupt can lead to severe problems.
		// See also: TS_SetTimerToMaxRate
		
		NodeToRemove->priority = 0;
		TS_Terminating = true;
		return true;
	}
	
	asm	pushf;
	asm	cli;
	
	NodeToRemove->priority = -1;
	TS_Terminating = true;
	TS_SetTimerToMaxRate();
	
	asm	popf;
	
	if (NodeToRemove->priority != -1)
	{
		// SUCCESS: priority isn't -1 anymore, so it must have been set to 0 by
		// TS_SetTimerToMaxRate and the task was removed.
		return true;
	}
	return false;	// ERROR: NodeToRemove wasn't part of the task list
}

void TS_Dispatch(void)
{
	TS_Enabled = true;
}

void TS_Halt(void)
{
	TS_Enabled = false;
}

void TS_SetTaskRate(task *Task, int rate)
{
	task *ptr;
	long MaxServiceRate;	// bad name (we're looking for the SMALLEST value)
	
	Task->rate = TS_SetTimer(rate);
	
	MaxServiceRate = DEFAULT_TIMER_RATE;
	for (ptr = TaskList.start; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->rate < MaxServiceRate)
		{
			MaxServiceRate = ptr->rate;
		}
	}
	
	if (TaskServiceRate != MaxServiceRate)
	{
		TS_SetClockSpeed(MaxServiceRate);
	}
}
