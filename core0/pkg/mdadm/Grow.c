/*
 * mdadm - manage Linux "md" devices aka RAID arrays.
 *
 * Copyright (C) 2001-2004 Neil Brown <neilb@cse.unsw.edu.au>
 *
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    Author: Neil Brown
 *    Email: <neilb@cse.unsw.edu.au>
 *    Paper: Neil Brown
 *           School of Computer Science and Engineering
 *           The University of New South Wales
 *           Sydney, 2052
 *           Australia
 */
#include	"mdadm.h"
#include	"dlink.h"

#if ! defined(__BIG_ENDIAN) && ! defined(__LITTLE_ENDIAN)
#error no endian defined
#endif
#include	"md_u.h"
#include	"md_p.h"

int Grow_Add_device(char *devname, int fd, char *newdev)
{
	/* Add a device to an active array.
	 * Currently, just extend a linear array.
	 * This requires writing a new superblock on the
	 * new device, calling the kernel to add the device,
	 * and if that succeeds, update the superblock on
	 * all other devices.
	 * This means that we need to *find* all other devices.
	 */
	struct mdinfo info;

	void *super = NULL;
	struct stat stb;
	int nfd, fd2;
	int d, nd;
	struct supertype *st = NULL;
	

	if (ioctl(fd, GET_ARRAY_INFO, &info.array) < 0) {
		fprintf(stderr, Name ": cannot get array info for %s\n", devname);
		return 1;
	}

	st = super_by_version(info.array.major_version, info.array.minor_version);
	if (!st) {
		fprintf(stderr, Name ": cannot handle arrays with superblock version %d\n", info.array.major_version);
		return 1;
	}

	if (info.array.level != -1) {
		fprintf(stderr, Name ": can only add devices to linear arrays\n");
		return 1;
	}

	nfd = open(newdev, O_RDWR|O_EXCL);
	if (nfd < 0) {
		fprintf(stderr, Name ": cannot open %s\n", newdev);
		return 1;
	}
	fstat(nfd, &stb);
	if ((stb.st_mode & S_IFMT) != S_IFBLK) {
		fprintf(stderr, Name ": %s is not a block device!\n", newdev);
		close(nfd);
		return 1;
	}
	/* now check out all the devices and make sure we can read the superblock */
	for (d=0 ; d < info.array.raid_disks ; d++) {
		mdu_disk_info_t disk;
		char *dv;

		disk.number = d;
		if (ioctl(fd, GET_DISK_INFO, &disk) < 0) {
			fprintf(stderr, Name ": cannot get device detail for device %d\n",
				d);
			return 1;
		}
		dv = map_dev(disk.major, disk.minor);
		if (!dv) {
			fprintf(stderr, Name ": cannot find device file for device %d\n",
				d);
			return 1;
		}
		fd2 = open(dv, O_RDWR);
		if (!fd2) {
			fprintf(stderr, Name ": cannot open device file %s\n", dv);
			return 1;
		}
		if (super) free(super);
		super= NULL;
		if (st->ss->load_super(st, fd2, &super, NULL)) {
			fprintf(stderr, Name ": cannot find super block on %s\n", dv);
			close(fd2);
			return 1;
		}
		close(fd2);
	}
	/* Ok, looks good. Lets update the superblock and write it out to
	 * newdev.
	 */
	
	info.disk.number = d;
	info.disk.major = major(stb.st_rdev);
	info.disk.minor = minor(stb.st_rdev);
	info.disk.raid_disk = d;
	info.disk.state = (1 << MD_DISK_SYNC) | (1 << MD_DISK_ACTIVE);
	st->ss->update_super(&info, super, "grow", newdev, 0);

	if (st->ss->store_super(st, nfd, super)) {
		fprintf(stderr, Name ": Cannot store new superblock on %s\n", newdev);
		close(nfd);
		return 1;
	}
	close(nfd);

	if (ioctl(fd, ADD_NEW_DISK, &info.disk) != 0) {
		fprintf(stderr, Name ": Cannot add new disk to this array\n");
		return 1;
	}
	/* Well, that seems to have worked.
	 * Now go through and update all superblocks
	 */

	if (ioctl(fd, GET_ARRAY_INFO, &info.array) < 0) {
		fprintf(stderr, Name ": cannot get array info for %s\n", devname);
		return 1;
	}

	nd = d;
	for (d=0 ; d < info.array.raid_disks ; d++) {
		mdu_disk_info_t disk;
		char *dv;

		disk.number = d;
		if (ioctl(fd, GET_DISK_INFO, &disk) < 0) {
			fprintf(stderr, Name ": cannot get device detail for device %d\n",
				d);
			return 1;
		}
		dv = map_dev(disk.major, disk.minor);
		if (!dv) {
			fprintf(stderr, Name ": cannot find device file for device %d\n",
				d);
			return 1;
		}
		fd2 = open(dv, O_RDWR);
		if (fd2 < 0) {
			fprintf(stderr, Name ": cannot open device file %s\n", dv);
			return 1;
		}
		if (st->ss->load_super(st, fd2, &super, NULL)) {
			fprintf(stderr, Name ": cannot find super block on %s\n", dv);
			close(fd);
			return 1;
		}
		info.array.raid_disks = nd+1;
		info.array.nr_disks = nd+1;
		info.array.active_disks = nd+1;
		info.array.working_disks = nd+1;
		info.disk.number = nd;
		info.disk.major = major(stb.st_rdev);
		info.disk.minor = minor(stb.st_rdev);
		info.disk.raid_disk = nd;
		info.disk.state = (1 << MD_DISK_SYNC) | (1 << MD_DISK_ACTIVE);
		st->ss->update_super(&info, super, "grow", dv, 0);
		
		if (st->ss->store_super(st, fd2, super)) {
			fprintf(stderr, Name ": Cannot store new superblock on %s\n", dv);
			close(fd2);
			return 1;
		}
		close(fd2);
	}

	return 0;
}

int Grow_addbitmap(char *devname, int fd, char *file, int chunk, int delay, int write_behind, int force)
{
	/*
	 * First check that array doesn't have a bitmap
	 * Then create the bitmap
	 * Then add it
	 *
	 * For internal bitmaps, we need to check the version,
	 * find all the active devices, and write the bitmap block
	 * to all devices
	 */
	mdu_bitmap_file_t bmf;
	mdu_array_info_t array;
	struct supertype *st;
	int major = BITMAP_MAJOR_HI;
	int vers = md_get_version(fd);
	unsigned long long bitmapsize, array_size;

	if (vers < 9003) {
		major = BITMAP_MAJOR_HOSTENDIAN;
#ifdef __BIG_ENDIAN
		fprintf(stderr, Name ": Warning - bitmaps created on this kernel are not portable\n"
			"  between different architectured.  Consider upgrading the Linux kernel.\n");
#endif
	}

	if (ioctl(fd, GET_BITMAP_FILE, &bmf) != 0) {
		if (errno == ENOMEM) 
			fprintf(stderr, Name ": Memory allocation failure.\n");
		else
			fprintf(stderr, Name ": bitmaps not supported by this kernel.\n");
		return 1;
	}
	if (bmf.pathname[0]) {
		if (strcmp(file,"none")==0) {
			if (ioctl(fd, SET_BITMAP_FILE, -1)!= 0) {
				fprintf(stderr, Name ": failed to remove bitmap %s\n",
					bmf.pathname);
				return 1;
			}
			return 0;
		}
		fprintf(stderr, Name ": %s already has a bitmap (%s)\n",
			devname, bmf.pathname);
		return 1;
	}
	if (ioctl(fd, GET_ARRAY_INFO, &array) != 0) {
		fprintf(stderr, Name ": cannot get array status for %s\n", devname);
		return 1;
	}
	if (array.state & (1<<MD_SB_BITMAP_PRESENT)) {
		if (strcmp(file, "none")==0) {
			array.state &= ~(1<<MD_SB_BITMAP_PRESENT);
			if (ioctl(fd, SET_ARRAY_INFO, &array)!= 0) {
				fprintf(stderr, Name ": failed to remove internal bitmap.\n");
				return 1;
			}
			return 0;
		}
		fprintf(stderr, Name ": Internal bitmap already present on %s\n",
			devname);
		return 1;
	}
	bitmapsize = array.size;
	bitmapsize <<= 1;
#ifdef BLKGETSIZE64
	if (ioctl(fd, BLKGETSIZE64, &array_size) == 0 &&
	    array_size > (0x7fffffffULL<<9)) {
		/* Array is big enough that we cannot trust array.size
		 * try other approaches
		 */
		bitmapsize = get_component_size(fd);
	}
#endif
	if (bitmapsize == 0) {
		fprintf(stderr, Name ": Cannot reliably determine size of array to create bitmap - sorry.\n");
		return 1;
	}

	if (array.level == 10) {
		int ncopies = (array.layout&255)*(array.layout>>8);
		bitmapsize = bitmapsize * array.raid_disks / ncopies;
	}

	st = super_by_version(array.major_version, array.minor_version);
	if (!st) {
		fprintf(stderr, Name ": Cannot understand version %d.%d\n",
			array.major_version, array.minor_version);
		return 1;
	}
	if (strcmp(file, "none") == 0) {
		fprintf(stderr, Name ": no bitmap found on %s\n", devname);
		return 1;
	} else if (strcmp(file, "internal") == 0) {
		int d;
		for (d=0; d< st->max_devs; d++) {
			mdu_disk_info_t disk;
			char *dv;
			disk.number = d;
			if (ioctl(fd, GET_DISK_INFO, &disk) < 0)
				continue;
			if (disk.major == 0 &&
			    disk.minor == 0)
				continue;
			if ((disk.state & (1<<MD_DISK_SYNC))==0)
				continue;
			dv = map_dev(disk.major, disk.minor);
			if (dv) {
				void *super;
				int fd2 = open(dv, O_RDWR);
				if (fd2 < 0)
					continue;
				if (st->ss->load_super(st, fd2, &super, NULL)==0) {
					if (st->ss->add_internal_bitmap(st, super,
								    chunk, delay, write_behind,
									bitmapsize, 0, major))
						st->ss->write_bitmap(st, fd2, super);
					else {
						fprintf(stderr, Name ": failed to create internal bitmap - chunksize problem.\n");
						close(fd2);
						return 1;
					}
				}
				close(fd2);
			}
		}
		array.state |= (1<<MD_SB_BITMAP_PRESENT);
		if (ioctl(fd, SET_ARRAY_INFO, &array)!= 0) {
			fprintf(stderr, Name ": failed to set internal bitmap.\n");
			return 1;
		}
	} else {
		int uuid[4];
		int bitmap_fd;
		int d;
		int max_devs = st->max_devs;
		void *super = NULL;
		if (chunk == UnSet)
			chunk = DEFAULT_BITMAP_CHUNK;

		/* try to load a superblock */
		for (d=0; d<max_devs; d++) {
			mdu_disk_info_t disk;
			char *dv;
			int fd2;
			disk.number = d;
			if (ioctl(fd, GET_DISK_INFO, &disk) < 0)
				continue;
			if ((disk.major==0 && disk.minor==0) ||
			    (disk.state & (1<<MD_DISK_REMOVED)))
				continue;
			dv = map_dev(disk.major, disk.minor);
			if (!dv) continue;
			fd2 = open(dv, O_RDONLY);
			if (fd2 >= 0 &&
			    st->ss->load_super(st, fd2, &super, NULL) == 0) {
				close(fd2);
				st->ss->uuid_from_super(uuid, super);
				break;
			}
			close(fd2);
		}
		if (d == max_devs) {
			fprintf(stderr, Name ": cannot find UUID for array!\n");
			return 1;
		}
		if (CreateBitmap(file, force, (char*)uuid, chunk,
				 delay, write_behind, bitmapsize, major)) {
			return 1;
		}
		bitmap_fd = open(file, O_RDWR);
		if (bitmap_fd < 0) {
			fprintf(stderr, Name ": weird: %s cannot be opened\n",
				file);
			return 1;
		}
		if (ioctl(fd, SET_BITMAP_FILE, bitmap_fd) < 0) {
			fprintf(stderr, Name ": Cannot set bitmap file for %s: %s\n",
				devname, strerror(errno));
			return 1;
		}
	}

	return 0;
}

		
