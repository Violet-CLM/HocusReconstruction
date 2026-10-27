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

#ifndef __TASK_MAN_H
#define __TASK_MAN_H

typedef struct task
{
	struct task *next;
	struct task *prev;
	
	void (*TaskService)(struct task *);
	void far *data;
	
	long rate;
	long count;
	signed char priority;
} task;

typedef void (*taskfunctype)(task*);

void TS_Startup(void);
void TS_Shutdown(void);
task * TS_ScheduleTask(taskfunctype func, int rate, int priority, void far *dataptr);
void TS_AddTask(task *item);
int TS_Terminate(task *NodeToRemove);
void TS_Dispatch(void);
void TS_Halt(void);
void TS_SetTaskRate(task *Task, int rate);

#endif