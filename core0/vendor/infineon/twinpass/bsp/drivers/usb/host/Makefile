RGSRC=../../../../../../..
include $(RGSRC)/envir.mak

ifdef CONFIG_USB_DWC3884_HCD
  MOD_TARGET=dwc3884.o
  OX_OBJS_dwc3884.o=dwc3884-hcd.o
  CFLAGS_dwc3884.o+=-I$(BUILDDIR)/os/linux/drivers/usb/host
  RAMDISK_MODULES_FILES+=$(MOD_TARGET)

  INTERNAL_HEADERS+=dwc3884.h
endif

include $(RGMK)
