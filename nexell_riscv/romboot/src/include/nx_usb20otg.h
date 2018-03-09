/*
 * Copyright (C) 2012 Nexell Co., All Rights Reserved
 * Nexell Co. Proprietary & Confidential
 *
 * NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 * AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Module          : USB20OTG
 * File            : nx_usb20otg.h
 * Description     :
 * Author          : Firware Team
 * History         :
 */
#ifndef __NX_USB20OTG_H__
#define __NX_USB20OTG_H__


#ifdef	__cplusplus
extern "C"
{
#endif

typedef struct
{
	volatile U32 GOTGCTL;		//000h
	volatile U32 GOTGINT;		//004h
	volatile U32 GAHBCFG;		//008h
	volatile U32 GUSBCFG;		//00Ch
	volatile U32 GRSTCTL;		//010h
	volatile U32 GINTSTS;		//014h
	volatile U32 GINTMSK;		//018h
	volatile U32 GRXSTSR;		//01Ch
	volatile U32 GRXSTSP;		//020h
	volatile U32 GRXFSIZ;		//024h
	volatile U32 GNPTXFSIZ;		//028h
	volatile U32 GNPTXSTS;		//02Ch
	volatile U32 GI2CCTL;		//030h
	volatile U32 GPVNDCTL;		//034h
	volatile U32 GGPIO;		//038h
	volatile U32 GUID;		//03Ch
	volatile U32 GSNPSID;		//040h
	volatile U32 GHWCFG1;		//044h
	volatile U32 GHWCFG2;		//048h
	volatile U32 GHWCFG3;		//04Ch
	volatile U32 GHWCFG4;		//050h
	volatile U32 GLPMCFG;		//054h
	volatile U32 GPWRDN;		//058h
	volatile U32 GDFIFOCFG;		//05Ch
	volatile U32 GADPCTL;		//060h

	volatile U32 RESERVED00[39];	//064h-0FFh
	volatile U32 HPTXFSIZ;		//100h
	volatile U32 DPTXFSIZ[15];	//104h-13Ch
	volatile U32 RESERVED01[176];	//140h-3FFh
//Host Mode CSR Map (400h-7FFh)
	volatile U32 HCFG;		//400h
	volatile U32 HFIR;		//404h
	volatile U32 HFNUM;		//408h      	
	volatile U32 RESERVED02;	//40Ch      	
	volatile U32 HPTXSTS;		//410h      	
	volatile U32 HAINT;		//414h      	
	volatile U32 HAINTMSK;		//418h      	
	volatile U32 RESERVED03[9];	//41Ch-43Ch	
	volatile U32 HPRT;		//440h		
	volatile U32 RESERVED04[47];	//444h-4FCh	
	volatile U32 HCSR16[128];	//500h-6FCh	
//16 channel*(6개+2개)=128, 128*4=512=200h,
//{Reserved[2],HCDMA,HCTSIZ,HCINTMSK,HCINT,HCSPLT,HCCHAR}
	volatile U32 RESERVED05[64];	//700h-7FCh
//Device Mode CSR Map (800h-BFFh), 800h-ACh : Device Logical IN Endpoint-Specific Registers
	volatile U32 DCFG;		//800h	      
	volatile U32 DCTL;		//804h        
	volatile U32 DSTS;		//808h        
	volatile U32 RESERVED06;	//80Ch        
	volatile U32 DIEPMSK;		//810h        
	volatile U32 DOEPMSK;		//814h        
	volatile U32 DAINT;		//818h        
	volatile U32 DAINTMSK;		//81Ch        
	volatile U32 DTKNQR1;		//820h        
	volatile U32 DTKNQR2;		//824h        
	volatile U32 DVBUSDIS;		//828h        
	volatile U32 DVBUSPULSE;	//82Ch        
	volatile U32 DTKNQR3;		//830h        
	volatile U32 DTKNQR4;		//834h        
	volatile U32 DEACHINT;		//838h	     
	volatile U32 DEACHINTMSK;	//83Ch       
	volatile U32 DIEPEACHMSK[16];	//840h-87Ch  
	volatile U32 DOEPEACHMSK[16];	//880h-8C0h  

	volatile U32 RESERVED07[16];	//8C0h-8FCh
	volatile U32 DIESR16[128];	//900h-AFCh
//16 channel*(4개+4개)=128, 128*4=512=200h,
//{Reserved,DTXFSTS,DIEPDMA,DIEPTSIZ,Reserved,DIEPINT,Reserved,DIEPCTL}
	volatile U32 DOESR16[128];	//B00h-CFCh
//16 channel*(4개+4개)=128, 128*4=512=200h,
//{Reserved[2],DOEPDMA,DOEPTSIZ,Reserved,DOEPINT,Reserved,DOEPCTL}
	volatile U32 RESERVED08[64];	//D00h-DFCh
//Power and Clock Gating Register
	volatile U32 PCGCCTL;		//E00h
	volatile U32 RESERVED09[127];	//E04h-FFCh
//ep fifo Register
	volatile U32 EP00FIFO[1024];	//1000h
	volatile U32 EP01FIFO[1024];	//2000h
	volatile U32 EP02FIFO[1024];	//3000h
	volatile U32 EP03FIFO[1024];	//4000h
	volatile U32 EP04FIFO[1024];	//5000h
	volatile U32 EP05FIFO[1024];	//6000h
	volatile U32 EP06FIFO[1024];	//7000h
	volatile U32 EP07FIFO[1024];	//8000h
	volatile U32 EP08FIFO[1024];	//9000h
	volatile U32 EP09FIFO[1024];	//A000h
	volatile U32 EP10FIFO[1024];	//B000h
	volatile U32 EP11FIFO[1024];	//C000h
	volatile U32 EP12FIFO[1024];	//D000h
	volatile U32 EP13FIFO[1024];	//E000h
	volatile U32 EP14FIFO[1024];	//F000h
	volatile U32 EP15FIFO[1024];	//10000h
	volatile U32 RESERVED10[15360];	//11000h-1FFFCh
	volatile U32 DEBUGFIFO[32768];	//20000h-3FFFCh
//User Register
//	volatile U32 TESTPARM0;		//1840h
//	volatile U32 TESTPARM1;		//1844h
//	volatile U32 TESTPARM2;		//1848h
//	volatile U32 TESTPARM3;		//184Ch
//	volatile U32 TESTPARM4;		//1850h
//	volatile U32 TESTPARM5;		//1854h
//	volatile U32 TESTPARM6;		//1858h
//	volatile U32 TESTPARM7;		//185Ch
//	volatile U32 TESTPARM8;		//1860h
//	volatile U32 TESTPARM9;		//1864h
//	volatile U32 TESTPARM10;	//1868h
} NX_USB20OTG_RegisterSet;

typedef struct
{
    volatile U32 OTG_APB00[39];
} NX_USB20OTG_APB_RegisterSet;


#ifdef	__cplusplus
}
#endif


#endif // __NX_USB20OTG_H__
