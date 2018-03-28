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

#include "include/nx_type.h"

CBOOL	DebugInit(void);
void	DebugPutch(S8 ch);
S8	DebugGetch(void);
CBOOL	DebugIsUartTxDone(void);

CBOOL	DebugIsTXEmpty(void);
CBOOL	DebugIsBusy(void);
S8	DebugGetch(void);

void    nx_putchar(char ch);
char    nx_getchar(void);

#endif // __DEBUG_H__
