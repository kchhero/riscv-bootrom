/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Module          : bl0
 * File            : printf.h
 * Description     :
 * Author          : Hans
 * History         :
 */
#ifndef __NX_PRINTF__
#define __NX_PRINTF__

#include <stdarg.h>

#ifndef SOC_SIM
int _dprintf(const char *format, ...);
#else
#include <nx_debug.h>
extern void kputs(const char *);
extern void _dprintf(const char *, ...);
#endif //SOC_SIM
#endif
