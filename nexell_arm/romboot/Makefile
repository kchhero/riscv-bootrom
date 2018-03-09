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
# File		: Makefile
# Description	:
# Author	: Firware Team
# History	: 2017.07.21 hans first draft
#

include config.mak


LDFLAGS		= -Bstatic						\
		-Wl,-Map=$(DIR_TARGETOUTPUT)/$(PROJECT_NAME).map,--cref	\
		-T$(LDS_NAME)						\
		-Wl,--start-group					\
		-L$(DIR_OBJOUTPUT)					\
		-Wl,--end-group						\
		-Wl,--build-id=none					\
		-nostdlib
ifeq ($(EMUL), y)
SYS_OBJS	 = 	startup_cortexA9.o
SYS_OBJS	+=	aes_cbc.o
else
SYS_OBJS	 = 	startup_cortexA7.o
SYS_OBJS	+=	nx_clock.o
endif

SYS_OBJS	+= 	iROMBOOT.o libplat.o printf.o		\
			debug.o buildinfo.o lib_armv7.o		\
			libarm.o util_arm.o armv7_exceptions.o
ifneq ($(SIMUL),y)
SYS_OBJS	+= 	iSDHCBOOT.o
SYS_OBJS	+= 	iUSBBOOT.o
SYS_OBJS	+= 	iSPIBOOT.o
SYS_OBJS	+= 	iNANDBOOTEC.o
SYS_OBJS	+= 	iSDHCFSBOOT.o fatfs.o diskio.o gpt.o
SYS_OBJS	+=	iUARTBOOT.o xyz_modem.o
SYS_OBJS	+=	rsa_verify_pss.o bignum.o sha256.o
SYS_OBJS	+=	CRYPTO.o
endif
SYS_OBJS	+=	fnptr.o


SYS_OBJS_LIST	= 	$(addprefix $(DIR_OBJOUTPUT)/,$(SYS_OBJS))

ifeq ($(EMUL), y)
SYS_INCLUDES 	=	-I ./src			\
			-I ./src/oldinclude		\
			-I ./src/mbedtls
else
SYS_INCLUDES 	=	-I ./src			\
			-I ./src/include
endif


################################################################################
$(DIR_OBJOUTPUT)/%.o: src/%.c
	$(Q)echo [compile....$<]
	$(Q)$(CC) -MMD $< -c -o $@ $(CFLAGS) $(SYS_INCLUDES)
################################################################################
$(DIR_OBJOUTPUT)/%.o: src/%.S
	$(Q)echo [compile....$<]
	$(Q)$(CC) -MMD $< -c -o $@ $(ASFLAG) $(CFLAGS) $(SYS_INCLUDES)
################################################################################


#all: libs mkobjdir $(SYS_OBJS_LIST) link bin
all: mkobjdir $(SYS_OBJS_LIST) link bin

mkobjdir:
ifeq ($(OS),Windows_NT)
	$(Q)if not exist $(DIR_OBJOUTPUT)		\
		$(Q)$(MKDIR) $(DIR_OBJOUTPUT)
	$(Q)if not exist $(DIR_TARGETOUTPUT)		\
		$(Q)$(MKDIR) $(DIR_TARGETOUTPUT)
else
	$(Q)if [ ! -e $(DIR_OBJOUTPUT) ]; then		\
		$(MKDIR) $(DIR_OBJOUTPUT);		\
	fi;
	$(Q)if [ ! -e $(DIR_TARGETOUTPUT) ]; then	\
		$(MKDIR) $(DIR_TARGETOUTPUT);		\
	fi;
endif

#libs:
#	$(MAKE) -C src

link:
	$(Q)echo [link.... $(PROJECT_NAME).elf]
	$(Q)$(CC) $(SYS_OBJS_LIST) $(LDFLAGS)		\
	       	-o $(DIR_TARGETOUTPUT)/$(PROJECT_NAME).elf

bin:
	$(Q)echo [binary.... $(PROJECT_NAME).bin]
	$(Q)$(MAKEBIN) -O binary $(DIR_TARGETOUTPUT)/$(PROJECT_NAME).elf	\
		$(DIR_TARGETOUTPUT)/$(PROJECT_NAME).bin

ifeq ($(OS),Windows_NT)
	$(Q)if exist $(DIR_OBJOUTPUT)			\
		@$(RM) $(DIR_OBJOUTPUT)\buildinfo.o
else
	$(Q)if     [ -e $(DIR_OBJOUTPUT) ]; then	\
		$(RM) $(DIR_OBJOUTPUT)/buildinfo.o;	\
	fi;
endif

clean:
#	$(MAKE) -C src clean
ifeq ($(OS),Windows_NT)
	$(Q)if exist $(DIR_OBJOUTPUT)			\
		$(Q)$(RMDIR) $(DIR_OBJOUTPUT)
	$(Q)if exist $(DIR_TARGETOUTPUT)		\
		$(Q)$(RMDIR) $(DIR_TARGETOUTPUT)
else
	$(Q)if [ -e $(DIR_OBJOUTPUT) ]; then		\
		$(RMDIR) $(DIR_OBJOUTPUT);		\
	fi;
	$(Q)if [ -e $(DIR_TARGETOUTPUT) ]; then		\
		$(RMDIR) $(DIR_TARGETOUTPUT);		\
	fi;

endif


################################################################################
-include $(SYS_OBJS_LIST:.o=.d)

