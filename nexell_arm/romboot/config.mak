#
# Copyright (C) 2012 Nexell Co., All Rights Reserved
# Nexell Co. Proprietary & Confidential
#
# NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
# AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
# FITNESS FOR A PARTICULAR PURPOSE.
#
# Moudle	: bl0
# File		: config.mak
# Description	:
# Author	: Firware Team
# History	: 2017.07.21 hans first draft
#

###########################################################################
# Build Environment
###########################################################################
DEBUG			= n

#EMUL			= y
EMUL			?= n
SIMUL			?= n

CROSS_TOOL_TOP		=
ifeq ($(OS),Windows_NT)
CROSS_TOOL		= $(CROSS_TOOL_TOP)arm-none-eabi-
else
#CROSS_TOOL		= $(CROSS_TOOL_TOP)arm-eabi-
CROSS_TOOL		= $(CROSS_TOOL_TOP)arm-none-eabi-
endif


###########################################################################
# Top Names
###########################################################################
ifeq ($(EMUL),y)
PROJECT_NAME		= antares_nxp4330
else
ifeq ($(SIMUL),y)
PROJECT_NAME        = antares_simul
else
PROJECT_NAME		= antares
endif
endif
LDS_NAME		= $(PROJECT_NAME).lds


###########################################################################
# Directories
###########################################################################
DIR_PROJECT_TOP		=
DIR_OBJOUTPUT		= obj
DIR_TARGETOUTPUT	= build

CODE_MAIN_INCLUDE	=

###########################################################################
# Build Environment
###########################################################################
ifeq ($(EMUL),y)
ARCH			= armv7-a
CPU			= cortex-a9
else
ARCH			= armv7-a
CPU			= cortex-a7
endif

CC			= $(CROSS_TOOL)gcc
LD 			= $(CROSS_TOOL)ld
AS 			= $(CROSS_TOOL)as
AR 			= $(CROSS_TOOL)ar
MAKEBIN			= $(CROSS_TOOL)objcopy
RANLIB 			= $(CROSS_TOOL)ranlib

GCC_LIB			= $(shell $(CC) -print-libgcc-file-name)

ifeq ($(DEBUG), y)
CFLAGS			= -DNX_RELEASE -O0
Q			= @
else
CFLAGS			= -DNX_RELEASE -Os
Q			= @
endif

###########################################################################
# MISC tools for MS-DOS
###########################################################################
ifeq ($(OS),Windows_NT)
MKDIR			= mkdir
RM			= del /q /f
MV			= move
CD			= cd
CP			= copy
ECHO			= echo
RMDIR			= rmdir /s /q
else
MKDIR			= mkdir -p
RM			= rm -f
MV			= mv
CD			= cd
CP			= cp
ECHO			= echo
RMDIR			= rm -rf
endif
###########################################################################
# FLAGS
###########################################################################
ARFLAGS			= rcs
ARFLAGS_REMOVE		= -d
ARLIBFLAGS		= -v -s

ASFLAG			= -D__ASSEMBLY__


CFLAGS			+= -g -Wall					\
			-Wextra -ffreestanding -fno-builtin		\
			-mlittle-endian					\
			-mtune=$(CPU) -march=$(ARCH)			\
			$(CODE_MAIN_INCLUDE)				\
			-D__arm						\
			-msoft-float					\
			-mstructure-size-boundary=32

ifeq ($(EMUL),y)
CFLAGS			+= -DNXP4330
else
CFLAGS			+= -DNXP3220
ifeq ($(SIMUL),y)
CFLAGS			+= -DBL1SIMUL
endif
endif

