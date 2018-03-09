/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Module          : bl0
 * File            : libarm.h
 * Description     :
 * Author          : Hans
 * History         : 2018.02.14 Hans create
 */
#include "fnptr.h"

#ifndef __LIBARM_H__
#define __LIBARM_H__

void SetICache(int enable);
unsigned int GetBootOption(void);
void SetBootOption(unsigned int);
unsigned int *GetMempoolPtr(void);
void SetMempoolPtr(unsigned int mempoolptr);
unsigned int *GetStringPtr(void);
void SetStringPtr(unsigned int *StringPtr);
void Setbl0fnPtr(const struct nx_bl0_fn *);
struct nx_bl0_fn *Getbl0fnPtr(void);
unsigned int GetPMCnt(void);
//; void asmudelay(unsigned int utime);

#endif /* __LIBARM_H__ */
