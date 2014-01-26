RGSRC=../../..
include $(RGSRC)/envir.mak

#rg_modules has to compile before bsp for ppa_openrg_al to be linked into ppa
ifndef CONFIG_RG_GPL
SUBDIRS+=mgt
endif
SUBDIRS+=rg_modules bsp modules

EXPORT_HEADERS_DIR=vendor
EXPORT_HEADERS+=$(if $(CONFIG_RG_PERM_STORAGE), flash_layout.c__c/flash_layout.c)
EXPORT_HEADERS+=$(if $(CONFIG_RG_FASTPATH_PLAT_TWINPASS), bsp/include/asm-mips/danube/danube_ppa_api.h__danube/danube_ppa_api.h bsp/include/asm-mips/danube/danube_ppa_ppe_hal.h__danube/danube_ppa_ppe_hal.h bsp/include/asm-mips/danube/danube_ppa_ppe_d3_hal.h__danube/danube_ppa_ppe_d3_hal.h bsp/include/asm-mips/danube/danube_ppa_hook.h__danube/danube_ppa_hook.h)
EXPORT_HEADERS+=$(if $(CONFIG_INFINEON_PPE_D4), bsp/include/asm-mips/danube/danube_ppa_ppe_d4_hal.h__danube/danube_ppa_ppe_d4_hal.h)

include $(RGMK)
