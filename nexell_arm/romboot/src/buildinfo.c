////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Nexell Co., Ltd All Rights Reserved
// Nexell Co. Proprietary & Confidential
//
// Nexell informs that this code and information is provided "as is" base
// and without warranty of any kind, either expressed or implied, including
// but not limited to the implied warranties of merchantability and/or
// fitness for a particular puporse.
//
// Module	: bl0
// File		: buildinfo.c
// Description	:
// Author	: Hans
// History	:
//		2014-01-12	Hans create
//		2016-08-09	Hans modified for NXP5540
////////////////////////////////////////////////////////////////////////////////
#include "printf.h"
#include "libarm.h"
#include "fnptr.h"

void buildinfo(void)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->printf("\r\n\tBL0 by Nexell Co. : Built on %s %s\r\n\n",
			__DATE__, __TIME__);
}

