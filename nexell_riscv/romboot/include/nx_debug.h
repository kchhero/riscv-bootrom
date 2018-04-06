/*==============================================================================
//
// Copyright (C) 2009 Nexell Co. All Rights Reserved
// Nexell Co. Proprietary & Confidential
//
// Nexell informs that this code and information is provided "as is" base
// and without warranty of any kind, either expressed or implied, including
// but not limited to the implied warranties of merchantability and/or
// fitness for a particular puporse.
//
// Module	: bl0
// File		: debug.h
// Description	:
// Author	: Hans
// History	:
// 		2017.10.09 First implementation
==============================================================================*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

int	DebugInit(void);
void	DebugPutch(char ch);
char	DebugGetch(void);
int	DebugIsUartTxDone(void);

int	DebugIsTXEmpty(void);
int	DebugIsBusy(void);
char	DebugGetch(void);

void    nx_putchar(char ch);
char    nx_getchar(void);

#endif // __DEBUG_H__
