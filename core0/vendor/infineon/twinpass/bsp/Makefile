RGSRC=../../../..
include $(RGSRC)/envir.mak

SUBDIRS+=arch/mips/infineon/danube/basic arch/mips/infineon/danube/port \
  drivers/char drivers/net drivers/mtd/maps drivers/crypto arch/mips/pci \
  arch/mips/infineon/danube/mps

ifdef CONFIG_USB_DWC3884_HCD
  SUBDIRS+=drivers/usb/host
endif  
ifdef CONFIG_USB_DWC_OTG_HCD
  SUBDIRS+=drivers/dwc_otg
endif  

JPKG_EXPORTED_DIR+=include bsp_patches

EXPORT_HEADERS_DIR=vendor
EXPORT_HEADERS=include/asm-mips/danube/port.h__danube_port.h \
  include/asm-mips/danube/danube_eth_d2.h__danube_eth_d2.h

ifdef IS_BUILDDIR
ARCHCONFIG_FIRST_TASKS=include/asm

include/asm: $(RGSRC)/vendor/infineon/twinpass/bsp/include/asm-mips
	$(MKDIR) include
	$(RG_LN) $< $@
endif

include $(RGMK)
