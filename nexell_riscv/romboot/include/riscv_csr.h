/*
 * Copyright (C) 2015 Regents of the University of California
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

#ifndef _ASM_RISCV_CSR_H
#define _ASM_RISCV_CSR_H

#include "nx_const.h"

//Machine Mode Register Flags
#define MSR_IE   _AC(0x00000008, UL) /* Interrupt Enable */
#define MSR_PIE  _AC(0x00000080, UL) /* Previous IE */
#define MSR_PS   _AC(0x00001800, UL) /* Previously Supervisor */

#ifndef CONFIG_64BIT
#define SR_SD   _AC(0x80000000, UL) /* FS/XS dirty */
#else
#define SR_SD   _AC(0x8000000000000000, UL) /* FS/XS dirty */
#endif

/* SPTBR flags */
#if __riscv_xlen == 32
#define SPTBR_PPN     _AC(0x003FFFFF, UL)
#define SPTBR_MODE_32 _AC(0x80000000, UL)
#define SPTBR_MODE    SPTBR_MODE_32
#else
#define SPTBR_PPN     _AC(0x00000FFFFFFFFFFF, UL)
#define SPTBR_MODE_39 _AC(0x8000000000000000, UL)
#define SPTBR_MODE    SPTBR_MODE_39
#endif

#ifndef __ASSEMBLY__

#define csr_swap(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrrw %0, " #csr ", %1"		\
			      : "=r" (__v) : "rK" (__v)		\
			      : "memory");			\
	__v;							\
})

#define csr_read(csr)						\
({								\
	register unsigned long __v;				\
	__asm__ __volatile__ ("csrr %0, " #csr			\
			      : "=r" (__v) :			\
			      : "memory");			\
	__v;							\
})

#define csr_write(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrw " #csr ", %0"		\
			      : : "rK" (__v)			\
			      : "memory");			\
})

#define csr_read_set(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrrs %0, " #csr ", %1"		\
			      : "=r" (__v) : "rK" (__v)		\
			      : "memory");			\
	__v;							\
})

#define csr_set(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrs " #csr ", %0"		\
			      : : "rK" (__v)			\
			      : "memory");			\
})

#define csr_read_clear(csr, val)				\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrrc %0, " #csr ", %1"		\
			      : "=r" (__v) : "rK" (__v)		\
			      : "memory");			\
	__v;							\
})

#define csr_clear(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrc " #csr ", %0"		\
			      : : "rK" (__v)			\
			      : "memory");			\
})

#endif /* __ASSEMBLY__ */

#endif /* _ASM_RISCV_CSR_H */
    
