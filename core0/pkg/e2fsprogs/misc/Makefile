E2FSSRC=..
include $(E2FSSRC)/envir.mak

TARGET=mke2fs
RAMDISK_BIN_FILES+=$(TARGET)
#TARGET+=badblocks tune2fs dumpe2fs e2image fsck mklpf chattr lsattr uuidgen

O_OBJS_mke2fs=mke2fs.o util.o get_device_by_label.o
O_OBJS_badblocks=badblocks.o
O_OBJS_tune2fs=tune2fs.o util.o get_device_by_label.o
O_OBJS_dumpe2fs=dumpe2fs.o
O_OBJS_e2image=e2image.o
O_OBJS_fsck=fsck.o get_device_by_label.o base_device.o fstype.o
O_OBJS_mklpf=mklost+found.o
O_OBJS_chattr=chattr.o
O_OBJS_lsattr=lsattr.o
O_OBJS_uuidgen=uuidgen.o

include $(E2FSMK)
