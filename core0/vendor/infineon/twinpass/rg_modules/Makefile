RGSRC=../../../../
include $(RGSRC)/envir.mak

EXPORT_HEADERS_DIR=vendor

ifdef CONFIG_TWINPASS_64_MEG
ifdef CONFIG_INFINEON_PPE_D4
ifdef CONFIG_HW_80211N_ATHEROS_AR50WL
CPU1_FILE=uImage-cpu1-d4-11n-64
else
CPU1_FILE=uImage-cpu1-d4-64
endif
else
ifdef CONFIG_RG_MOCA_WAN
CPU1_FILE=uImage-cpu1-vz-64
else
ifdef CONFIG_RG_IKANOS_VDSL
CPU1_FILE=uImage-cpu1-vz-64
else
CPU1_FILE=uImage-cpu1-64
endif
endif
endif
else
CPU1_FILE=uImage-cpu1
endif

ifndef CONFIG_RG_GPL

ifdef CONFIG_RG_DANUBE_DUALCORE
MOD_$(CONFIG_RG_DANUBE_DUALCORE)+=load_cpu1.o
OX_OBJS_load_cpu1.o=load_cpu1.o 
L_OBJS_load_cpu1.o=uimage.o
O_FORMAT=$(shell $(OBJDUMP) -i | head -2 | grep elf32)
LDFLAGS_load_cpu1.o+=-T $(PWD_SRC)/uImageld.script --oformat $(O_FORMAT)

O_TARGET+=uimage.o
LDFLAGS_uimage.o+=-r -s -b binary unzipped_kernel.o

#unzip kernel image w/o its header
CD_EXPORTED_FILES+=$(CPU1_FILE) uImageld.script
ARCHCONFIG_FIRST_TASKS+=unzipped_kernel.o uImage_load_params.h
CLEAN+=unzipped_kernel.o uImage_load_params.h
unzipped_kernel.o: $(CPU1_FILE)
	dd if=$(PWD_SRC)/$(CPU1_FILE) bs=64 skip=1 | gunzip > $@
#parse header params into uImage_load_params.h	
LOCAL_TARGET+=header_parser
O_OBJS_header_parser+=header_parser.o
ifndef CONFIG_RG_JPKG_SRC
uImage_load_params.h: header_parser $(CPU1_FILE)
	./header_parser $(PWD_SRC)/$(CPU1_FILE) > $@
endif
endif

ifdef CONFIG_HW_BUTTONS
  EXPORT_HEADERS+=hw_buttons_arch.c__c/hw_buttons_arch.c
endif

ifdef CONFIG_RG_FASTPATH_PLAT_TWINPASS
  MOD_O_TARGET+=fastpath_plat.o
  O_OBJS_fastpath_plat.o+=fastpath_plat_ppa.o
  INTERNAL_HEADERS+=fastpath_plat_ppa.h
endif

MOD_$(CONFIG_DANUBE_PPA)+=danube_ppa_al.o
OX_OBJS_danube_ppa_al.o+=danube_ppa_openrg_al.o

endif # not CONFIG_RG_GPL

ifdef CONFIG_RG_FASTPATH_PLAT_TWINPASS
  EXPORT_HEADERS+=danube_ppa_openrg_al.h__danube/danube_ppa_openrg_al.h
endif

ifdef CONFIG_RG_DANUBE_DUALCORE
CD_EXPORTED_FILES+=uImage_load_params_stub.h
ifndef CONFIG_RG_JPKG_SRC
ifndef CONFIG_RG_GPL
EXPORT_HEADERS+= \
  uImage_load_params.h__infineon/twinpass/rg_modules/uImage_load_params.h
else
EXPORT_HEADERS+= \
  uImage_load_params_stub.h__infineon/twinpass/rg_modules/uImage_load_params.h
endif
endif
endif

MOD_TARGET+=$(MOD_m) $(MOD_y)
MOD_2_STAT+=$(MOD_y)
RAMDISK_MODULES_FILES+=$(MOD_m)

include $(RGMK)

