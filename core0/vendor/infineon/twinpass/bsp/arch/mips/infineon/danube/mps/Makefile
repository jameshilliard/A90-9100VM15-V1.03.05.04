RGSRC=../../../../../../../../..
include $(RGSRC)/envir.mak


ifdef CONFIG_INFINEON_PPE_D4
MOD_$(CONFIG_RG_DANUBE_DUALCORE)+=mps_tpe_linux.o
ifndef CONFIG_INFINEON_PPE_D4_11N
OX_OBJS_mps_tpe_linux.o+=mps_tpe_linux_d4.o mps_tpe_buffer_d4.o mps_tpe_device_d4.o
else
OX_OBJS_mps_tpe_linux.o+=mps_tpe_linux_d4_11n.o mps_tpe_buffer_d4_11n.o mps_tpe_device_d4_11n.o
MOD_CFLAGS+=-DCONFIG_IFX_PPA_D4_DUALCORE -DCONFIG_TWINPASS_E
endif
else
MOD_$(CONFIG_RG_DANUBE_DUALCORE)+=mps_tpe_linux.o
OX_OBJS_mps_tpe_linux.o+=mps_tpe_linux.o mps_tpe_buffer.o mps_tpe_device.o
endif

MOD_CFLAGS+=-DCONFIG_MPS_TIMER_OPTIMIZATION -DCONFIG_MPS_FLOW_CONTROL -DCONFIG_DANUBE_MPS_PROC_DEBUG -DCONFIG_BOARD_TWINPASS_E -fvolatile

INTERNAL_HEADERS=mps_tpe_device.h mps_tpe_kthread.h

ifdef CONFIG_RG_DANUBE_DUALCORE
 MOD_y+=mps_tpe_storage_mod.o
 OX_OBJS_mps_tpe_storage_mod.o+=mps_tpe_storage.o
endif

MOD_TARGET+=$(MOD_m) $(MOD_y)
MOD_2_STAT+=$(MOD_y)
RAMDISK_MODULES_FILES+=$(MOD_m)

include $(RGMK)
