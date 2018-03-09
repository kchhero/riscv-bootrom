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
// File		: fatfs.c
// Description	:
// Author	: Hans
// History	:
// 		2014.01.12 First implementation
==============================================================================*/

#include <nx_type.h>
#include "fatfs.h"	/* FatFs configurations and declarations */
#include "diskio.h"	/* Declarations of low level disk I/O functions */
#include "libarm.h"
#include "fnptr.h"


/*--------------------------------------------------------------------------
   Module Private Definitions
---------------------------------------------------------------------------*/

#define	ENTER_FF(fs)
#define LEAVE_FF(fs, res)	return res


#define	ABORT(fs, res)		{ fp->flag |= FA__ERROR; LEAVE_FF(fs, res); }

#ifndef NULL
#define	NULL	0
#endif


/*--------------------------------------------------------------------------
   Private Work Area
---------------------------------------------------------------------------*/

#define	NAMEBUF(sp,lp)	U8 sp[12]
#define INITBUF(dj,sp,lp)	dj.fn = sp



/*--------------------------------------------------------------------------
   Private Functions
---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* String functions                                                      */
/*-----------------------------------------------------------------------*/
#if 0
/* Copy memory to memory */
static
void mem_cpy(void* dst, const void* src, int cnt)
{
	char *d = (char*)dst;
	const char *s = (const char *)src;
	while (cnt--)
		*d++ = *s++;
}

/* Fill memory */
static
void mem_set(void* dst, int val, int cnt)
{
	char *d = (char*)dst;
	while (cnt--)
		*d++ = (char)val;
}

/* Compare memory to memory */
static
int mem_cmp(const void* dst, const void* src, int cnt)
{
	const char *d = (const char *)dst, *s = (const char *)src;
	int r = 0;
	while (cnt-- && (r = *d++ - *s++) == 0) ;
	return r;
}
#endif
/* Check if chr is contained in the string */
static
int chk_chr(const char* str, int chr) {
	while (*str && *str != chr) str++;
	return *str;
}



/*-----------------------------------------------------------------------*/
/* Change window offset                                                  */
/*-----------------------------------------------------------------------*/

FRESULT move_window(
	FATFS *fs,	/* File system object */
	U32 sector	/* Sector number to make apperance in the fs->win[] */
)			/* Move to zero only writes back dirty window */
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U32 wsect;

	wsect = fs->winsect;
	if (wsect != sector) {	/* Changed current window */
		if (sector) {
			if (pbl0fn->disk_read(fs->drive, fs->win, sector,
						1, fs->diskhandle) != RES_OK)
				return FR_DISK_ERR;
			fs->winsect = sector;
		}
	}

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Get a cluster status                                                  */
/*-----------------------------------------------------------------------*/

U32 get_cluster(	/* 0xFFFFFFFF:Disk error,
			   1:Interal error, Else:Cluster status */
	FATFS *fs,	/* File system object */
	U32 clst	/* Cluster# to get the link information */
)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	U16 wc, bc;
	U32 fsect;

	if (clst < 2 || clst >= fs->max_clust)	/* Check cluster address range */
		return 1;

	fsect = fs->fatbase;
	switch (fs->fs_type) {
	case FS_FAT12:
		bc = (U16)clst * 3 / 2;
		if (pbl0fn->move_window(fs, fsect + (bc / SS(fs))))
			break;
		wc = fs->win[bc & (SS(fs) - 1)];
		bc++;
		if (pbl0fn->move_window(fs, fsect + (bc / SS(fs))))
			break;
		wc |= (U16)fs->win[bc & (SS(fs) - 1)] << 8;
		return (clst & 1) ? (wc >> 4) : (wc & 0xFFF);

	case FS_FAT16:
		if (pbl0fn->move_window(fs, fsect + (clst / (SS(fs) / 2))))
			break;
		return LD_WORD(&fs->win[((U16)clst * 2) & (SS(fs) - 1)]);

	case FS_FAT32:
		if (pbl0fn->move_window(fs, fsect + (clst / (SS(fs) / 4))))
			break;
		return LD_DWORD(&fs->win[((U16)clst * 4) & (SS(fs) - 1)]) &
			0x0FFFFFFF;
	}

	return 0xFFFFFFFF;	/* An error occured at the disk I/O layer */
}

/*-----------------------------------------------------------------------*/
/* Get sector# from cluster#                                             */
/*-----------------------------------------------------------------------*/

U32 clust2sect (	/* !=0: sector number, 0: failed - invalid cluster# */
	FATFS *fs,	/* File system object */
	U32 clst	/* Cluster# to be converted */
)
{
	clst -= 2;
	if (clst >= (fs->max_clust - 2))
		return 0;		/* Invalid cluster# */
	return clst * fs->csize + fs->database;
}




/*-----------------------------------------------------------------------*/
/* Seek directory index                                                  */
/*-----------------------------------------------------------------------*/

FRESULT dir_seek (
	DIR *dj,	/* Pointer to directory object */
	U16 idx		/* Directory index number */
)
{
	U32 clst;
	U16 ic;

	dj->index = idx;
	clst = dj->sclust;
	/* Check start cluster range */
	if (clst == 1 || clst >= dj->fs->max_clust)
		return FR_INT_ERR;

	if (clst == 0) {	/* Static table */
		if (idx >= dj->fs->n_rootdir)	/* Index is out of range */
			return FR_INT_ERR;
		dj->sect = dj->fs->dirbase + idx / (SS(dj->fs) / 32);
	}
	else {				/* Dynamic table */
		ic = SS(dj->fs) / 32 * dj->fs->csize;	/* Indexes per cluster */
		while (idx >= ic) {	/* Follow cluster chain */
			/* Get next cluster */
			clst = get_cluster(dj->fs, clst);
			if (clst == 0xFFFFFFFF)
				return FR_DISK_ERR;	/* Disk error */
			/* Reached to end of table or int error */
			if (clst < 2 || clst >= dj->fs->max_clust)
				return FR_INT_ERR;
			idx -= ic;
		}
		dj->clust = clst;
		dj->sect = clust2sect(dj->fs, clst) + idx / (SS(dj->fs) / 32);
	}
	dj->dir = dj->fs->win + (idx % (SS(dj->fs) / 32)) * 32;

	return FR_OK;	/* Seek succeeded */
}




/*-----------------------------------------------------------------------*/
/* Move directory index next                                             */
/*-----------------------------------------------------------------------*/

FRESULT dir_next (	/* FR_OK:Succeeded,
			   FR_NO_FILE:End of table,
			   FR_DENIED:EOT and could not streach */
	DIR *dj,	/* Pointer to directory object */
	CBOOL streach	/* FALSE: Do not streach table,
			   TRUE: Streach table if needed */
)
{
	U32 clst;
	U16 i;

	streach = streach;	/* unused parameter */

	i = dj->index + 1;
	if (!i || !dj->sect)	/* Report EOT when index has reached 65535 */
		return FR_NO_FILE;

	if (!(i % (SS(dj->fs) / 32))) {	/* Sector changed? */
		dj->sect++;		/* Next sector */

		if (dj->sclust == 0) {	/* Static table */
			/* Report EOT when end of table */
//			if (i >= dj->fs->n_rootdir)
			if (i > dj->fs->n_rootdir)
				return FR_NO_FILE;
		} else {				/* Dynamic table */
			/* Cluster changed? */
			if (((i / (SS(dj->fs) / 32)) &
					(dj->fs->csize - 1)) == 0) {
				/* Get next cluster */
				clst = get_cluster(dj->fs, dj->clust);
				if (clst <= 1)
					return FR_INT_ERR;
				if (clst == 0xFFFFFFFF)
					return FR_DISK_ERR;
				/* When it reached end of dinamic table */
				if (clst >= dj->fs->max_clust)
					/* Report EOT */
					return FR_NO_FILE;
				/* Initialize data for new cluster */
				dj->clust = clst;
				dj->sect = clust2sect(dj->fs, clst);
			}
		}
	}

	dj->index = i;
	dj->dir = dj->fs->win + (i % (SS(dj->fs) / 32)) * 32;

	return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Find an object in the directory                                       */
/*-----------------------------------------------------------------------*/

FRESULT dir_find (
	DIR *dj	/* Pointer to the directory object linked to the file name */
)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	FRESULT res;
	U8 a, c, *dir;


	res = pbl0fn->dir_seek(dj, 0);			/* Rewind directory object */
	if (res != FR_OK) return res;

	do {
		res = pbl0fn->move_window(dj->fs, dj->sect);
		if (res != FR_OK)
			break;
		/* Ptr to the directory entry of current index */
		dir = dj->dir;
		c = dir[DIR_Name];
		if (c == 0) {
			res = FR_NO_FILE;
			break;
		}	/* Reached to end of table */
		a = dir[DIR_Attr] & AM_MASK;
		 /* Is it a valid entry? */
		if (c != 0xE5 && c != '.' &&
				!(a & AM_VOL) &&
//				!mem_cmp(dir, dj->fn, 11))
				!pbl0fn->memcmp(dir, dj->fn, 11))
			break;
		res = pbl0fn->dir_next(dj, CFALSE);	/* Next entry */
	} while (res == FR_OK);

	return res;
}

/*-----------------------------------------------------------------------*/
/* Pick a segment and create the object name in directory form           */
/*-----------------------------------------------------------------------*/

FRESULT create_name (
	DIR *dj,		/* Pointer to the directory object */
	const char **path	/* Pointer to pointer to the segment
				   in the path string */
)
{
	U8 c, d, b, *sfn;
	int ni, si, i;
	const char *p;

	/* Create file name in directory form */
	sfn = dj->fn;
//	mem_set(sfn, ' ', 11);
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	pbl0fn->memset(sfn, ' ', 11);
	si = i = b = 0;
	ni = 8;
	p = *path;
	for (;;) {
		c = p[si++];
		if (c < ' ' || c == '/' || c == '\\')
			break;	/* Break on end of segment */
		if (c == '.' || i >= ni) {
			if (ni != 8 || c != '.')
				return FR_INVALID_NAME;
			i = 8;
			ni = 11;
			b <<= 2;
			continue;
		}
		/* If there is any extended char, eliminate NT flag */
		if (c >= 0x80)
			b |= 3;
		if (IsDBCS1(c)) {		/* If it is DBC 1st byte */
			d = p[si++];			/* Get 2nd byte */
			if (!IsDBCS2(d) || i >= ni - 1)	/* Reject invalid DBC */
				return FR_INVALID_NAME;
			sfn[i++] = c;
			sfn[i++] = d;
		} else {
			/* Reject unallowable chrs for SFN */
			if (chk_chr(" +,;[=]\"*:<>\?|\x7F", c))
				return FR_INVALID_NAME;
			if (IsUpper(c)) {
				b |= 2;
			} else {
				if (IsLower(c)) {
					b |= 1;
					c -= 0x20;
				}
			}
			sfn[i++] = c;
		}
	}
	*path = &p[si];		/* Rerurn pointer to the next segment */
	c = (c < ' ') ? 4 : 0;	/* Set last segment flag if end of path */

	if (!i)
		return FR_INVALID_NAME;		/* Reject null string */
	if (sfn[0] == 0xE5)
		/* When first char collides with 0xE5, replace it with 0x05 */
		sfn[0] = 0x05;

	if (ni == 8)
		b <<= 2;
	if ((b & 0x03) == 0x01)
		c |= 0x10;	/* NT flag (Extension has only small capital) */
	if ((b & 0x0C) == 0x04)
		c |= 0x08;	/* NT flag (Filename has only small capital) */

	sfn[11] = c;		/* Store NT flag, File name is created */

	return FR_OK;
}


/*-----------------------------------------------------------------------*/
/* Follow a file path                                                    */
/*-----------------------------------------------------------------------*/

FRESULT follow_path (		/* FR_OK(0): successful, !=0: error code */
	DIR *dj,		/* Directory object to return last directory
				   and found object */
	const char *path	/* Full-path String
				   to find a file or directory */
)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	FRESULT res;
	U8 *dir, last;

	if (*path == '/' || *path == '\\' )
		path++;	/* Strip heading separator */

	dj->sclust =		/* Set start directory (root dir) */
		(dj->fs->fs_type == FS_FAT32) ? dj->fs->dirbase : 0;

	if ((U8)*path < ' ') {		/* Null path means the root directory */
		res = pbl0fn->dir_seek(dj, 0);
		dj->dir = NULL;

	} else {					/* Follow path */
		for (;;) {
			res = pbl0fn->create_name(dj, &path);	/* Get a segment */
			if (res != FR_OK)
				break;
			res = pbl0fn->dir_find(dj);		/* Find it */
			last = *(dj->fn + 11) & 4;
			if (res != FR_OK) {	/* Could not find the object */
				if (res == FR_NO_FILE && !last)
					res = FR_NO_PATH;
				break;
			}
			if (last)
				/* Last segment match. Function completed. */
				break;
			/* There is next segment. Follow the sub directory */
			dir = dj->dir;
			/* Cannot follow because it is a file */
			if (!(dir[DIR_Attr] & AM_DIR)) {
				res = FR_NO_PATH;
				break;
			}
			dj->sclust = ((U32)LD_WORD(dir + DIR_FstClusHI) << 16) |
					LD_WORD(dir + DIR_FstClusLO);
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Load boot record and check if it is an FAT boot record                */
/*-----------------------------------------------------------------------*/

U8 check_fs (	/* 0:The FAT boot record,
		   1:Valid boot record but not an FAT,
		   2:Not a boot record, 3:Error */
	FATFS *fs,	/* File system object */
	U32 sect	/* Sector# (lba) to Check
			   if it is an FAT boot record or not */
)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	/* Load boot record */
	if (disk_read(fs->drive, fs->win, sect, 1, fs->diskhandle) != RES_OK)
		return 3;
	/* Check record signature (always placed at offset 510
	 * even if the sector size is >512) */
	if (LD_WORD(&fs->win[BS_55AA]) != 0xAA55)
		return 2;
	/* Check FAT signature */
//	if (!mem_cmp(&fs->win[BS_FilSysType], "FAT", 3))
	if (!pbl0fn->memcmp(&fs->win[BS_FilSysType], "FAT", 3))
		return 0;
//	if (!mem_cmp(&fs->win[BS_FilSysType32], "FAT32", 5) &&
	if (!pbl0fn->memcmp(&fs->win[BS_FilSysType32], "FAT32", 5) &&
			!(fs->win[BPB_ExtFlags] & 0x80))
		return 0;

	return 1;
}




/*-----------------------------------------------------------------------*/
/* Make sure that the file system is valid                               */
/*-----------------------------------------------------------------------*/

FRESULT f_mount (	/* FR_OK(0): successful, !=0: any error occured */
	const char **path,	/* Pointer to pointer to the path name (drive number) */
	FATFS *rfs,		/* Pointer to pointer to the found file system object */
	U8 chk_wp		/* !=0: Check media write protection for write access */
)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	FRESULT res;
	U8 vol, fmt, *tbl;
	DSTATUS stat;
	U32 bsect, fsize, tsect, mclst;
	const char *p = *path;
	FATFS *fs;

	chk_wp = chk_wp;

	/* Get logical drive number from the path name */
	vol = p[0] - '0';		/* Is there a drive number? */
	if (vol <= 9 && p[1] == ':') {
		p += 2;		/* Found a drive number, get and strip it */
		*path = p;	/* Return pointer to the path name */
	} else {
		vol = 0;	/* No drive number is given,
				   use drive number 0 as default */
	}

	/* Check if the logical drive number is valid or not */
	if (vol >= _DRIVES)
		/* Is the drive number valid? */
		return FR_INVALID_DRIVE;
	/* Returen pointer to the corresponding file system object */
	fs = rfs;
	if (!fs)
		/* Is the file system object registered? */
		return FR_NOT_ENABLED;

	/* Lock file system */
	ENTER_FF(fs);

	/* If the logical drive has been mounted */
	if (fs->fs_type) {
		stat = pbl0fn->disk_status(fs->drive);
	/* and physical drive is kept initialized (has not been changed), */
		if (!(stat & STA_NOINIT)) {
			/* The file system object is valid */
//			return FR_OK;
//			rom boot disk will always restart.
		}
	}

	/* The logical drive must be re-mounted.
	 * Following code attempts to mount the volume */

	/* Clear the file system object */
	fs->fs_type = 0;
	/* Bind the logical drive and a physical drive */
	fs->drive = LD2PD(vol);
	/* Initialize low level disk I/O layer */
	stat = pbl0fn->disk_initialize(fs->drive);
	/* Check if the drive is ready */
	if (stat & STA_NOINIT)
		return FR_NOT_READY;

	/* Search FAT partition on the drive */
	/* Check sector 0 as an SFD format */
	fmt = pbl0fn->check_fs(fs, bsect = 0);
	/* Not an FAT boot record, it may be patitioned */
	if (fmt == 1 || fmt == 0) {
		/* Check a partition listed in top of the partition table */
		/* Partition table */
		tbl = &fs->win[MBR_Table + LD2PT(vol) * 16];
		/* Is the partition existing? */
		if (tbl[4]) {
			/* Partition offset in LBA */
			bsect = LD_DWORD(&tbl[8]);
			/* Check the partition */
			fmt = pbl0fn->check_fs(fs, bsect);
		}
	}
	if (fmt == 3)
		return FR_DISK_ERR;
	/* No valid FAT patition is found */
	if (fmt || LD_WORD(fs->win+BPB_BytsPerSec) != SS(fs))
		return FR_NO_FILESYSTEM;

	/* Initialize the file system object */
	/* Number of sectors per FAT */
	fsize = LD_WORD(fs->win+BPB_FATSz16);
	if (!fsize)
		fsize = LD_DWORD(fs->win + BPB_FATSz32);
	fs->sects_fat = fsize;
	/* Number of FAT copies */
	fs->n_fats = fs->win[BPB_NumFATs];
	/* (Number of sectors in FAT area) */
	fsize *= fs->n_fats;
	/* FAT start sector (lba) */
	fs->fatbase = bsect + LD_WORD(fs->win + BPB_RsvdSecCnt);
	/* Number of sectors per cluster */
	fs->csize = fs->win[BPB_SecPerClus];
	/* Nmuber of root directory entries */
	fs->n_rootdir = LD_WORD(fs->win + BPB_RootEntCnt);
	/* Number of sectors on the file system */
	tsect = LD_WORD(fs->win + BPB_TotSec16);
	if (!tsect)
		tsect = LD_DWORD(fs->win + BPB_TotSec32);
	fs->max_clust = mclst = (tsect -	/* Last cluster# + 1 */
		LD_WORD(fs->win + BPB_RsvdSecCnt) -
		fsize - fs->n_rootdir / (SS(fs) / 32)) / (fs->csize + 2);

	fmt = FS_FAT12;		/* Determine the FAT sub type */
	if (mclst >= 0xFF7)
		fmt = FS_FAT16;	/* Number of clusters >= 0xFF5 */
	if (mclst >= 0xFFF7)
		fmt = FS_FAT32;	/* Number of clusters >= 0xFFF5 */

	if (fmt == FS_FAT32)
		/* Root directory start cluster */
		fs->dirbase = LD_DWORD(fs->win+BPB_RootClus);
	else
		/* Root directory start sector (lba) */
		fs->dirbase = fs->fatbase + fsize;
	/* Data start sector (lba) */
	fs->database = fs->fatbase + fsize + fs->n_rootdir / (SS(fs) / 32);

	fs->winsect = 0;
	fs->fs_type = fmt;	/* FAT syb-type */
	fs->id = 0xDEAD;	/* File system mount ID */
	res = FR_OK;

	return res;
}




/*-----------------------------------------------------------------------*/
/* Check if the file/dir object is valid or not                          */
/*-----------------------------------------------------------------------*/

FRESULT validate (	/* FR_OK(0): The object is valid, !=0: Invalid */
	FATFS *fs,	/* Pointer to the file system object */
	U16 id		/* Member id of the target object to be checked */
)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	if (!fs || !fs->fs_type || fs->id != id)
		return FR_INVALID_OBJECT;

	ENTER_FF(fs);		/* Lock file system */

	if (pbl0fn->disk_status(fs->drive) & STA_NOINIT)
		return FR_NOT_READY;

	return FR_OK;
}




/*--------------------------------------------------------------------------
   Public Functions
--------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Open or Create a File                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_open (
	FIL *fp,		/* Pointer to the blank file object */
	const char *path,	/* Pointer to the file name */
	U8 mode,		/* Access mode and file open mode flags */
	FATFS *pfs
)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	FRESULT res;
	DIR dj;
	NAMEBUF(sfn, lfn);
	U8 *dir;

	dj.fs = pfs;
	fp->fs = NULL;		/* Clear file object */
	mode &= FA_READ;
#if 0
	res = f_mount(&path, dj.fs, 0);
	if (res != FR_OK)
		LEAVE_FF(dj.fs, res);
#endif
	INITBUF(dj, sfn, lfn);
	res = pbl0fn->follow_path(&dj, path);	/* Follow the file path */

		if (res != FR_OK)
			LEAVE_FF(dj.fs, res);	/* Follow failed */
		dir = dj.dir;
		/* It is a directory */
		if (!dir || (dir[DIR_Attr] & AM_DIR))
			LEAVE_FF(dj.fs, FR_NO_FILE);
	fp->flag = mode;			/* File access mode */
	fp->org_clust =				/* File start cluster */
		((U32)LD_WORD(dir + DIR_FstClusHI) << 16) |
		LD_WORD(dir + DIR_FstClusLO);
	fp->fsize = LD_DWORD(dir + DIR_FileSize);	/* File size */
	fp->fptr = 0;
	fp->csect = 255;		/* File pointer */
	fp->dsect = 0;
	/* Owner file system object of the file */
	fp->fs = dj.fs;
	fp->id = dj.fs->id;

	LEAVE_FF(dj.fs, FR_OK);
}




/*-----------------------------------------------------------------------*/
/* Read File                                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_read (
	FIL *fp, 	/* Pointer to the file object */
	void *buff,	/* Pointer to data buffer */
	U32 btr,	/* Number of bytes to read */
	U32 *br		/* Pointer to number of bytes read */
)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	FRESULT res;
	U32 clst, sect, remain;
	U32 rcnt, cc;
	U8 *rbuff = buff;

	*br = 0;

	res = pbl0fn->validate(fp->fs, fp->id);	/* Check validity of the object */
	if (res != FR_OK)
		LEAVE_FF(fp->fs, res);
	if (fp->flag & FA__ERROR)		/* Check abort flag */
		LEAVE_FF(fp->fs, FR_INT_ERR);
	if (!(fp->flag & FA_READ)) 		/* Check access mode */
		LEAVE_FF(fp->fs, FR_DENIED);
	remain = fp->fsize - fp->fptr;
	if (btr > remain)
		btr = (U32)remain;	/* Truncate btr by remaining bytes */

	for ( ;  btr;			/* Repeat until all data transferred */
		rbuff += rcnt, fp->fptr += rcnt, *br += rcnt, btr -= rcnt) {
		/* On the sector boundary? */
		if ((fp->fptr % SS(fp->fs)) == 0) {
			/* On the cluster boundary? */
			if (fp->csect >= fp->fs->csize) {
				/* On the top of the file? */
				clst = (fp->fptr == 0) ?
					fp->org_clust :
					pbl0fn->get_cluster(fp->fs, fp->curr_clust);
				if (clst <= 1)
					ABORT(fp->fs, FR_INT_ERR);
				if (clst == 0xFFFFFFFF)
					ABORT(fp->fs, FR_DISK_ERR);
				/* Update current cluster */
				fp->curr_clust = clst;
				/* Reset sector offset in the cluster */
				fp->csect = 0;
			}
			/* Get current sector */
			sect = pbl0fn->clust2sect(fp->fs, fp->curr_clust);
			if (!sect)
				ABORT(fp->fs, FR_INT_ERR);
			sect += fp->csect;
			/* When remaining bytes >= sector size, */
			cc = btr / SS(fp->fs);
			/* Read maximum contiguous sectors directly */
			if (cc) {
				/* Clip at cluster boundary */
				if (fp->csect + cc > fp->fs->csize)
					cc = fp->fs->csize - fp->csect;
				if (pbl0fn->disk_read(fp->fs->drive, rbuff, sect,
					(U8)cc, fp->fs->diskhandle) != RES_OK)
					ABORT(fp->fs, FR_DISK_ERR);
				/* Next sector address in the cluster */
				fp->csect += (U8)cc;
				/* Number of bytes transferred */
				rcnt = SS(fp->fs) * cc;
				continue;
			}
			fp->dsect = sect;
			fp->csect++;	/* Next sector address in the cluster */
		}
		/* Get partial sector data from sector buffer */
		rcnt = SS(fp->fs) - (fp->fptr % SS(fp->fs));
		if (rcnt > btr)
			rcnt = btr;
		if (pbl0fn->move_window(fp->fs, fp->dsect))	/* Move sector window */
			ABORT(fp->fs, FR_DISK_ERR);
		/* Pick partial sector */
//		mem_cpy(rbuff, &fp->fs->win[fp->fptr % SS(fp->fs)], rcnt);
		pbl0fn->memcpy(rbuff, &fp->fs->win[fp->fptr % SS(fp->fs)], rcnt);
	}

	LEAVE_FF(fp->fs, FR_OK);
}


/*-----------------------------------------------------------------------*/
/* Close File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_close (
	FIL *fp		/* Pointer to the file object to be closed */
)
{
	struct nx_bl0_fn *pbl0fn = Getbl0fnPtr();
	FRESULT res;

	res = pbl0fn->validate(fp->fs, fp->id);
	if (res == FR_OK)
		fp->fs = NULL;
	LEAVE_FF(fp->fs, res);
}

