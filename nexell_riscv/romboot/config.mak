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
DEBUG			= y
DEBUG_GDB               = n

QEMU                    = n
SIM                     = y

CROSS_TOOL_TOP		=
ifeq ($(OS),Windows_NT)
CROSS_TOOL		= $(CROSS_TOOL_TOP)riscv64-unknown-elf-
else
CROSS_TOOL		= $(CROSS_TOOL_TOP)riscv64-unknown-elf-
endif


###########################################################################
# Top Names
###########################################################################
PROJECT_NAME		= swallow
ifeq ($(QEMU), y)
LDS_NAME		= $(PROJECT_NAME)_qemu.lds
else
LDS_NAME		= $(PROJECT_NAME).lds
endif

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
ARCH			= rv64g #imafd
ABI			= lp64d

CC			= $(CROSS_TOOL)gcc
LD 			= $(CROSS_TOOL)ld
AS 			= $(CROSS_TOOL)as
AR 			= $(CROSS_TOOL)ar
MAKEBIN			= $(CROSS_TOOL)objcopy
RANLIB 			= $(CROSS_TOOL)ranlib

GCC_LIB			= $(shell $(CC) -print-libgcc-file-name)

ifeq ($(DEBUG), y)
CFLAGS			= -DNX_DEBUG -DDEBUG -DNX_RELEASE -O0
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
