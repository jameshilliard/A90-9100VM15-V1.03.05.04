RGSRC=../../../../../..
include $(RGSRC)/envir.mak

MOD_$(CONFIG_DANUBE_ETHERNET_2PORTS)+=danube_eth_mod.o
O_OBJS_danube_eth_mod.o=danube_eth.o

MOD_$(CONFIG_DANUBE_ETHERNET_D2)+=danube_eth_d2_mod.o
O_OBJS_danube_eth_d2_mod.o=$(if $(CONFIG_DANUBE_PPA), danube_ppa_eth_d2.o, \
  danube_eth_d2.o)


ifndef CONFIG_INFINEON_PPE_D4

ifndef CONFIG_RG_GPL

MOD_$(CONFIG_DANUBE_PPA)+=danube_ppa_mod.o
O_OBJS_danube_ppa_mod.o=danube_ppa_api.o
OX_OBJS_danube_ppa_mod.o=danube_ppa_hook.o danube_ppa_ppe_hal.o \
  danube_ppa_ppe_d3_hal.o 

endif

else


ifndef CONFIG_INFINEON_PPE_D4_11N

MOD_$(CONFIG_DANUBE_ETHERNET_D4)+=danube_eth_d4_mod.o
OX_OBJS_danube_eth_d4_mod.o=danube_ppa_eth_d4.o danube_ppa_hook.o 

MOD_$(CONFIG_DANUBE_PPA)+=danube_ppa_mod.o
O_OBJS_danube_ppa_mod.o=danube_ppa_api_d4.o
OX_OBJS_danube_ppa_mod.o=danube_ppa_ppe_d4_hal.o

else

MOD_$(CONFIG_DANUBE_ETHERNET_D4)+=danube_eth_d4_mod.o
OX_OBJS_danube_eth_d4_mod.o=danube_ppa_eth_d4_11n.o danube_ppa_hook_11n.o 
MOD_$(CONFIG_DANUBE_PPA)+=danube_ppa_mod.o
O_OBJS_danube_ppa_mod.o=danube_ppa_api_d4_11n.o
OX_OBJS_danube_ppa_mod.o=danube_ppa_ppe_d4_11n_hal.o
MOD_CFLAGS+=-DCONFIG_IFX_TPE_DUAL_CPU -DCONFIG_DANUBE_PPA_D4 -DCONFIG_TWINPASS_E

endif
endif
# XXX - CONFIG_DANUBE_SWITCH is a dummy config (does not exist).
# danube_sw.o is probably used by Infineon only.
MOD_$(CONFIG_DANUBE_SWITCH)+=danube_sw_mod.o
O_OBJS_danube_sw_mod.o=danube_sw.o

# XXX - CONFIG_DANUBE_ETHERNET2 is a dummy config (does not exist).
# danube_eth2.o is probably used by Infineon only.
MOD_$(CONFIG_DANUBE_ETHERNET2)+=danube_eth2_mod.o
O_OBJS_danube_eth2_mod.o=danube_eth2.o

MOD_$(CONFIG_ADM6996_SUPPORT)+=admmod_mod.o
OX_OBJS_admmod_mod.o=admmod.o

ifdef CONFIG_WESTELL_ULTRALINE3
  MOD_CFLAGS+=-DCONFIG_WESTELL
endif

ifdef CONFIG_HW_MOCA_WAN
  MOD_CFLAGS+=-DDUAL_SPI_MDIO
endif

ifdef CONFIG_WSTL_DUAL_SPI_MDIO
   MOD_CFLAGS+=-DDUAL_SPI_MDIO
endif

ifdef CONFIG_WSTL_IGMP_CHANGE
   MOD_CFLAGS+=-DCONFIG_WSTL_IGMP_CHANGE
endif 

ifdef CONFIG_WESTELL_WIRELESS_ONBOARD
MOD_CFLAGS+=-DWESTELL_WIRELESS_ONBOARD
endif

MOD_TARGET+=$(MOD_m) $(MOD_y)
MOD_2_STAT=$(MOD_y)
RAMDISK_MODULES_FILES+=$(MOD_m)

include $(RGMK)
