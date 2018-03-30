 # Copyright (C) 2016  Nexell Co., Ltd.
 # Author: Sangjong, Han <hans@nexell.co.kr>
 #
 # This program is free software; you can redistribute it and/or
 # modify it under the terms of the GNU General Public License
 #
 # as published by the Free Software Foundation; either version 2
 # of the License, or (at your option) any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Build Version info
VERINFO				= V01

#########################################################################
# build environment
#########################################################################
CFLAGS				:=
DEBUG				?= y

# chip name
CHIPNAME			= riscv64

# archtecture type
OPMODE				?= riscv64

# (ddr3/lpddr3) memory type
MEMTYPE				?= ddr1
#MEMTYPE			?= lpddr3
MEMTEST				?= n

# power management ic(pmic) on/off
PMIC_ON				?= y
#PMIC_ON				?= n

CRC_CHECK			?= n

# supported (thridboot) boot mode
SUPPORT_USB_BOOT		?= y
SUPPORT_SDMMC_BOOT		?= y

BOARD				?= swallow

# sd/mmc, spi, sdfs boot configuration
DEVICE_PORT			?= 2

# supported kernel version (3.18-3.4/4.1-4.4)
KERNEL_VER			?= 4

# system log message
SYSLOG				?= y

# secure Boot
SECURE_ON			?= n

CROSS_COMPILE_TOP		?=
CROSS_COMPILE			?= $(CROSS_TOOL_TOP)riscv64-unknown-elf-

# Top Names
PROJECT_NAME			= $(CHIPNAME)_2ndboot_$(OPMODE)_$(MEMTYPE)_$(VERINFO)
TARGET_NAME			= bl1-$(shell echo $(BOARD) | tr A-Z a-z)
LDS_NAME			= bl1_$(OPMODE)

# Directories
DIR_PROJECT_TOP			=

DIR_OBJOUTPUT			= obj
DIR_TARGETOUTPUT		= out

CODE_MAIN_INCLUDE		=

ARCH				= rv64imafdc
ABI				= lp64d

CC				= $(CROSS_COMPILE)gcc
LD 				= $(CROSS_COMPILE)ld
AS 				= $(CROSS_COMPILE)as
AR 				= $(CROSS_COMPILE)ar
MAKEBIN				= $(CROSS_COMPILE)objcopy
OBJCOPY				= $(CROSS_COMPILE)objcopy
RANLIB 				= $(CROSS_COMPILE)ranlib

GCC_LIB				= $(shell $(CC) -print-libgcc-file-name)

ifeq ($(DEBUG), y)
CFLAGS				= -DNX_DEBUG -O0
Q				=
else
CFLAGS				= -DNX_RELEASE -Os
Q				= @
endif

# MISC tools for MS-DOS
ifeq ($(OS),Windows_NT)
MKDIR				= mkdir
RM				= del /q /F
MV				= move
CD				= cd
CP				= copy
ECHO				= echo
RMDIR				= rmdir /S /Q
else
MKDIR				= mkdir
RM				= rm -f
MV				= mv
CD				= cd
CP				= cp
ECHO				= echo
RMDIR				= rm -rf
endif

#########################################################################
# flags variables
#########################################################################
# FLAGS
ARFLAGS				= rcs
ARFLAGS_REMOVE			= -d
ARLIBFLAGS			= -v -s

ASFLAG				= -D__ASSEMBLY__ -D$(OPMODE)

CFLAGS				+=	-g -ggdb -Wall						\
					$(CODE_MAIN_INCLUDE)					\
                                        -DNONSMP_HART=0                                         \
                                        -O2 -std=gnu11                                          \
                                        -Wall -nostartfiles                                     \
                                        -fno-common -DENTROPY=0
                                        

ASFLAG				+=	-march=$(ARCH) -mabi=$(ABI)

CFLAGS				+=	-mcmodel=medany				\
					-march=$(ARCH)                          \
					-mabi=$(ABI)

# secure boot - (thirdboot) decrypt
ifeq ($(SECURE_ON), y)
CFLAGS				+=	-DSECURE_ON
endif

# supported kernel version (3.18-4.3/4.1-4.4)
ifeq ($(KERNEL_VER), 3)
CFLAGS				+=	-DKERNEL_VER_3_4
endif

# system log messgae
ifeq ($(SYSLOG), y)
CFLAGS				+=	-DSYSLOG_ON
endif

# power managemnt ic(pmic) on/off
ifeq ($(PMIC_ON), y)
CFLAGS				+=	-DPMIC_ON
CFLAGS				+=	-D$(shell echo $(BOARD) | tr a-z A-Z)_PMIC
endif

# memory test
ifeq ($(MEMTEST), y)
#MEMTEST_TYPE			+=	STANDARD
MEMTEST_TYPE			+=	SIMPLE
CFLAGS				+=	-D$(MEMTEST_TYPE)_MEMTEST
endif

# supported crc check
ifeq ($(CRC_CHECK), y)
CHECKSUM			+=	CRC_CHECK
CFLAGS				+=	-D$(CHECKSUM)_ON
endif
