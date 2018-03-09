/*==============================================================================
//
// Copyright (C) 2009 Nexell Co. All Rights Reserved
// Nexell Co. Proprietary & Confidential
//
// Nexell informs that this code and information is provided "as is" base
// and without warranty of any kind, either expressed or implied, including
// but not limited to the implied warranties of merchantability and/or
// fitness for a particular puporse.
//
// Module	: bl0
// File		: fatfs.h
// Description	:
// Author	: Hans
// History	:
// 		2014.01.12 First implementation
==============================================================================*/
#ifndef _FATFS
#define _FATFS

#define _DRIVES		1
/* Number of volumes (logical drives) to be used. */


#define	_MAX_SS		512
/* Maximum sector size to be handled. (512/1024/2048/4096) */
/* 512 for memroy card and hard disk, 1024 for floppy disk, 2048 for MO disk */


/* End of configuration options. Do not change followings without care.     */
/*--------------------------------------------------------------------------*/



/* Definitions corresponds to multiple sector size */

#define	SS(fs)			512

#define PARTITION_NUMBER	4    // for MBR

#define PARTITION_NORMAL	0x00 // BootIndicator
#define PARTITION_SYSTEM	0x80

#define PARTITION_EMPTY		0x00 // SystemId
#define PARTITION_FAT12		0x01
#define PARTITION_FAT16		0x04
#define PARTITION_EXT		0x05
#define PARTITION_FAT16B	0x06
#define PARTITION_FAT32		0x0B
#define PARTITION_FAT32L	0x0C
#define PARTITION_FAT16L	0x0E
#define PARTITION_EXTL		0x0F

typedef  struct _PARTITION_TABLE {
	U8  BootIndicator;
	U8  StartingHead;
	U16 StartingSector:6;
	U16 StartingCylinder:10;
	U8  SystemId;
	U8  EndingHead;
	U16 EndingSector:6;
	U16 EndingCylinder:10;
	U32 RelativeSector;
	U32 TotalSector;
}PARTITION_TABLE, *PPARTITION_TABLE;

// master boot record(MBR)
#define MASTERBOOT_CODELEN	446
#define MASTERBOOT_SIGNATURE	0xAA55

typedef  struct _MASTERBOOT_RECORD{
	U8		BootCode[MASTERBOOT_CODELEN];
	PARTITION_TABLE	Partition[PARTITION_NUMBER];
	U16		Signature;
}MASTERBOOT_RECORD, *PMASTERBOOT_RECORD;

// partition boot sector
#define RAWPARTITION_JUMPCODELEN	3
#define RAWPARTITION_CREATORIDLEN	8
#define RAWPARTITION_VOLUMELABLELEN	11
#define RAWPARTITION_FILESYSTEMLEN	8
#define RAWPARTITION_RESERVED1LEN	12
#define RAWPARTITION_RESERVED2LEN	420

#define RAWPARTITION_SIGNATURE		0xAA55
#define RAWPARTITION_EFDCSIGNATURE	0x29

typedef  struct _RAWPARTITION {
	U8  JumpCode[RAWPARTITION_JUMPCODELEN]; // FDC part
	U8  CreatorID[RAWPARTITION_CREATORIDLEN];
	U16 SectorSize;
	U8  SectorPerCluster;
	U16 ReservedSector;
	U8  NumberOfFAT;
	U16 NumberOfRootEntry;
	U16 TotalSector;
	U8  MediumId;
	U16 SectorPerFAT;
	U16 SectorPerTrack;
	U16 NumberOfSide;
	U32 NumberOfHiddenSector;              // EFDC part
	U32 TotalSector2; // when TotalSector==0, use this
	union {
		struct { // FAT12/FAT16
			U8  PhysicalDiskNumber;
			U8  Reserved;
			U8  EFDCSignature;
			U32 VolumeSerialNumber;
			U8  VolumeLabel[RAWPARTITION_VOLUMELABLELEN];
			U8  FileSystemType[RAWPARTITION_FILESYSTEMLEN];
		};
		struct { // FAT32
			U32 SectorPerFAT;
			U16 ExtentFlag;
			U16 FileSystemVersion;
			U32 RootEntryCluster;
			U16 FileSystemInfomation;
			U16 BackupBootSector;
			U8  Reserved1[RAWPARTITION_RESERVED1LEN];
			U8  PhysicalDiskNumber;
			U8  Reserved;
			U8  EFDCSignature;
			U32 VolumeSerialNumber;
			U8  VolumeLabel[RAWPARTITION_VOLUMELABLELEN];
			U8  FileSystemType[RAWPARTITION_FILESYSTEMLEN];
		}FAT32;
	};
	U8  Reserved2[RAWPARTITION_RESERVED2LEN];
	U16 Signature;
}RAWPARTITION, *PRAWPARTITION;


/* File system object structure */

typedef struct _FATFS {
	U8	fs_type;	/* FAT sub type */
	U8	drive;		/* Physical drive number */
	U8	csize;		/* Number of sectors per cluster */
	U8	n_fats;		/* Number of FAT copies */
	U8	wflag;		/* win[] dirty flag (1:must be written back) */
	U8	pad1[3];
	U16	id;		/* File system mount ID */
	U16	n_rootdir;	/* Number of root directory entries (0 on FAT32) */
	U32	sects_fat;	/* Sectors per fat */
	U32	max_clust;	/* Maximum cluster# + 1. Number of clusters is max_clust - 2 */
	U32	fatbase;	/* FAT start sector */
	U32	dirbase;	/* Root directory start sector (Cluster# on FAT32) */
	U32	database;	/* Data start sector */
	U32	winsect;	/* Current sector appearing in the win[] */
	U32 *	diskhandle;	/* storage handle */
	U8	win[_MAX_SS];	/* Disk access window for Directory/FAT */
} FATFS;



/* Directory object structure */

typedef struct _DIR {
	U16	id;		/* Owner file system mount ID */
	U16	index;		/* Current index number */
	FATFS*	fs;		/* Pointer to the owner file system object */
	U32	sclust;		/* Table start cluster (0:Static table) */
	U32	clust;		/* Current cluster */
	U32	sect;		/* Current sector */
	U8*	dir;		/* Pointer to the current SFN entry in the win[] */
	U8*	fn;		/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
} DIR;



/* File object structure */

typedef struct _FIL {
	FATFS*	fs;		/* Pointer to the owner file system object */
	U16	id;		/* Owner file system mount ID */
	U8	flag;		/* File status flags */
	U8	csect;		/* Sector address in the cluster */
	U32	fptr;		/* File R/W pointer */
	U32	fsize;		/* File size */
	U32	org_clust;	/* File start cluster */
	U32	curr_clust;	/* Current cluster */
	U32	dsect;		/* Current data sector */
} FIL;



/* File status structure */

typedef struct _FILINFO {
	U32	fsize;		/* File size */
	U16	fdate;		/* Last modified date */
	U16	ftime;		/* Last modified time */
	U8	fattrib;	/* Attribute */
	char	fname[13];	/* Short file name (8.3 format) */
} FILINFO;


/* Character code support macros */

#define IsUpper(c)	(((c)>='A')&&((c)<='Z'))
#define IsLower(c)	(((c)>='a')&&((c)<='z'))
#define IsDigit(c)	(((c)>='0')&&((c)<='9'))


#define IsDBCS1(c)	0
#define IsDBCS2(c)	0




/* Definitions corresponds to multi partition */

#define LD2PD(drv)	(drv)	/* Physical drive# is equal to the logical drive# */
#define LD2PT(drv)	0	/* Always mounts the 1st partition */




/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,		/* 0 */
	FR_DISK_ERR,		/* 1 */
	FR_INT_ERR,		/* 2 */
	FR_NOT_READY,		/* 3 */
	FR_NO_FILE,		/* 4 */
	FR_NO_PATH,		/* 5 */
	FR_INVALID_NAME,	/* 6 */
	FR_DENIED,		/* 7 */
	FR_EXIST,		/* 8 */
	FR_INVALID_OBJECT,	/* 9 */
	FR_WRITE_PROTECTED,	/* 10 */
	FR_INVALID_DRIVE,	/* 11 */
	FR_NOT_ENABLED,		/* 12 */
	FR_NO_FILESYSTEM,	/* 13 */
	FR_MKFS_ABORTED,	/* 14 */
	FR_TIMEOUT		/* 15 */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

/* mount filesystem to disk */
FRESULT f_mount(const char **path, FATFS *rfs, U8 chk_wp);
/* Open or create a file */
FRESULT f_open(FIL*, const char*, U8, FATFS*);
/* Read data from a file */
FRESULT f_read(FIL*, void*, U32, U32*);
/* Close an open file object */
FRESULT f_close(FIL*);

/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access control and file status flags (FIL.flag) */

#define	FA_READ			0x01
#define	FA_OPEN_EXISTING	0x00
#define FA__ERROR		0x80


/* FAT sub type (FATFS.fs_type) */

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute bits for directory entry */

#define	AM_RDO		0x01	/* Read only */
#define	AM_HID		0x02	/* Hidden */
#define	AM_SYS		0x04	/* System */
#define	AM_VOL		0x08	/* Volume label */
#define AM_LFN		0x0F	/* LFN entry */
#define AM_DIR		0x10	/* Directory */
#define AM_ARC		0x20	/* Archive */
#define AM_MASK		0x3F	/* Mask of defined bits */


/* FatFs refers the members in the FAT structures with byte offset instead
/ of structure member because there are incompatibility of the packing option
/ between various compilers. */

#define BS_jmpBoot		0
#define BS_OEMName		3
#define BPB_BytsPerSec		11
#define BPB_SecPerClus		13
#define BPB_RsvdSecCnt		14
#define BPB_NumFATs		16
#define BPB_RootEntCnt		17
#define BPB_TotSec16		19
#define BPB_Media		21
#define BPB_FATSz16		22
#define BPB_SecPerTrk		24
#define BPB_NumHeads		26
#define BPB_HiddSec		28
#define BPB_TotSec32		32
#define BS_55AA			510

#define BS_DrvNum		36
#define BS_BootSig		38
#define BS_VolID		39
#define BS_VolLab		43
#define BS_FilSysType		54

#define BPB_FATSz32		36
#define BPB_ExtFlags		40
#define BPB_FSVer		42
#define BPB_RootClus		44
#define BPB_FSInfo		48
#define BPB_BkBootSec		50
#define BS_DrvNum32		64
#define BS_BootSig32		66
#define BS_VolID32		67
#define BS_VolLab32		71
#define BS_FilSysType32		82

#define	FSI_LeadSig		0
#define	FSI_StrucSig		484
#define	FSI_Free_Count		488
#define	FSI_Nxt_Free		492

#define MBR_Table		446

#define	DIR_Name		0
#define	DIR_Attr		11
#define	DIR_NTres		12
#define	DIR_CrtTime		14
#define	DIR_CrtDate		16
#define	DIR_FstClusHI		20
#define	DIR_WrtTime		22
#define	DIR_WrtDate		24
#define	DIR_FstClusLO		26
#define	DIR_FileSize		28
#define	LDIR_Ord		0
#define	LDIR_Attr		11
#define	LDIR_Type		12
#define	LDIR_Chksum		13
#define	LDIR_FstClusLO		26



/*--------------------------------*/
/* Multi-byte word access macros  */
#define	LD_WORD(ptr)		  (U16)(((U16)*(U8*)((ptr)+1)<<8)|	\
					((U16)*(U8*)((ptr)+0)<<0))
#define	LD_DWORD(ptr)		    (U32)(((U32)*(U8*)((ptr)+3)<<24)|	\
					  ((U32)*(U8*)((ptr)+2)<<16)|	\
					  ((U16)*(U8*)((ptr)+1)<<8)|	\
						*(U8*)((ptr)+0)<<0)
#define	ST_WORD(ptr,val)	*(U8*)((ptr)+0)=(U8)((U16)(val)>>0);	\
				*(U8*)((ptr)+1)=(U8)((U16)(val)>>8)
#define	ST_DWORD(ptr,val)	*(U8*)((ptr)+0)=(U8)((U16)(val)>>0);	\
				*(U8*)((ptr)+1)=(U8)((U16)(val)>>8);	\
				*(U8*)((ptr)+2)=(U8)((U32)(val)>>16);	\
				*(U8*)((ptr)+3)=(U8)((U32)(val)>>24)


#endif /* _FATFS */
