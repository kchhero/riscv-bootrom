/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module          : Exception Handler
 * File            : armv7_exceptions.S
 * Description     :
 * Author          : Firware Team
 * History         : 2017.10.09 Hans Create
 */

//#include "context.h"
#include "armv7.h"

	.align  2

.global smc_handler
smc_handler:

SErrorA32:
1:
	wfi
	b	1b

