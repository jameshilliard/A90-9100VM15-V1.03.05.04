RGSRC=../
include $(RGSRC)/envir.mak

SUBDIRS+=$(if $(CONFIG_CONEXANT_COMMON),conexant)
SUBDIRS+=$(if $(CONFIG_MINDSPEED_COMMON),mindspeed)
SUBDIRS+=$(if $(CONFIG_IDT_COMMON),idt)
SUBDIRS+=$(if $(CONFIG_ZOOM_CM),zoom)
SUBDIRS+=$(if $(CONFIG_RG_VOATM_ELCP),tdsoft)
SUBDIRS+=$(if $(CONFIG_KINPO_COMMON),kinpo)
SUBDIRS+=$(if $(CONFIG_ARCH_SN8947),samsung)
SUBDIRS+=$(if $(CONFIG_MIPS_BRCM),broadcom)
SUBDIRS+=$(if $(CONFIG_SIBYTE_SWARM),broadcom/bcm91125e)
SUBDIRS+=$(if $(CONFIG_SIBYTE_SENTOSA),broadcom/bcm91125e)
SUBDIRS+=$(if $(CONFIG_SIBYTE_COLORADO),broadcom/bcm91125e)
SUBDIRS+=$(if $(CONFIG_IXP425_COMMON),intel)
SUBDIRS+=$(if $(CONFIG_SL2312_COMMON),storlink)
SUBDIRS+=$(if $(CONFIG_INTERSIL_COMMON),intersil)
SUBDIRS+=$(if $(CONFIG_TI_404_MIPS),ti)
SUBDIRS+=$(if $(CONFIG_ARCH_IXP425_NAPA),sohoware)
SUBDIRS+=$(if $(CONFIG_ARCH_IXP425_BAMBOO),pci)
SUBDIRS+=$(if $(CONFIG_ARCH_IXP425_JEEVES),us_robotics)
SUBDIRS+=$(if $(CONFIG_ARCH_IXP425_WAV54G),cybertan)
SUBDIRS+=$(if $(CONFIG_ARCH_IXP425_HG21),welltech)
SUBDIRS+=$(if $(CONFIG_ZCOM_XG1000),zcom)
SUBDIRS+=$(if $(CONFIG_GEMTEK_COMMON),gemtek)
SUBDIRS+=$(if $(CONFIG_INCAIP),infineon/incaip)
SUBDIRS+=$(if $(CONFIG_VINETIC),infineon/vinetic)
SUBDIRS+=$(if $(CONFIG_INFINEON_TWINPASS),infineon/twinpass)
SUBDIRS+=$(if $(CONFIG_ADM5120_COMMON),admtek/adm5120)
SUBDIRS+=$(if $(CONFIG_ATHEROS_AR531X_MIPS)$(CONFIG_HW_80211G_ATHEROS_AR521X)$(CONFIG_HW_80211N_ATHEROS_AR50WL)$(CONFIG_ATHEROS_ART_TOOLS),atheros)
SUBDIRS+=$(if $(CONFIG_ARCH_WRT108G),cybertan/wrt108g)
SUBDIRS+=$(if $(CONFIG_RG_VOIP_RV),radvision)
JPKG_EXPORTED_DIR_SRC+=$(if $(CONFIG_RG_VOIP_RV),radvision)
LINK_DIRS+=$(if $(CONFIG_RG_VOIP_RV),radvision)
SUBDIRS+=$(if $(CONFIG_RG_UML),jungo)
SUBDIRS+=$(if $(CONFIG_ARCH_IXP425_BRUCE),jabil/bruce)
SUBDIRS+=$(if $(CONFIG_ARCH_IXP425_JIWIS800),jstream/jiwis8xx)
SUBDIRS+=$(if $(CONFIG_ARCH_IXP425_JIWIS832),jstream/jiwis8xx)
SUBDIRS+=$(if $(CONFIG_RALINK_RT2560),ralink)
SUBDIRS+=$(if $(CONFIG_RALINK_RT2561),ralink)
SUBDIRS+=$(if $(CONFIG_AGN100),airgo)
SUBDIRS+=$(if $(CONFIG_MPC8272ADS)$(CONFIG_EP8248)$(CONFIG_MPC8349_ITX),freescale)
SUBDIRS+=$(if $(CONFIG_HW_ST_20190),st)
SUBDIRS+=$(if $(CONFIG_CPU_LX4189),lexra/lx4189)
SUBDIRS+=$(if $(CONFIG_ADI_6843),adi/ad6843)
SUBDIRS+=$(if $(CONFIG_ZSP400),lsi)
SUBDIRS+=$(if $(CONFIG_KS8695),micrel)
SUBDIRS+=$(if $(CONFIG_HW_SWITCH_BCM53XX),broadcom/bcm53xx)
SUBDIRS+=$(if $(CONFIG_MARVELL_88E60XX),marvell/mv88e60xx)
SUBDIRS+=$(if $(CONFIG_WESTELL_ULTRALINE3),westell/ultraline3)
SUBDIRS+=$(if $(CONFIG_ENTROPIC_CLINK),entropic)
SUBDIRS+=$(if $(CONFIG_RG_IKANOS_VDSL)$(CONFIG_MACH_ADI_FUSIV),ikanos)
# Infineon VDSL xcvr source files
SUBDIRS+=$(if $(CONFIG_RG_INFINEON_VDSL),infineon/vinax_vdsl2)
#Infineon tantos switch
SUBDIRS+=$(if $(CONFIG_TANTOS_PSB6973),infineon/mgt/dev)
#IFX modules
SUBDIRS+=infineon/modules
SUBDIRS+=infineon/hw_qos

# Open Source stuff added by Westell
SUBDIRS+=$(if $(CONFIG_OPENSOURCE_HTTPTINY),opensource/http-tiny)

# Westell stuff
SUBDIRS+=$(if $(CONFIG_WSTL_ARPMON),westell/arpmon)
SUBDIRS+=$(if $(CONFIG_WSTL_FIREBALL),westell/fireball)
SUBDIRS+=$(if $(CONFIG_WSTL_WBA),westell/wba)
SUBDIRS+=$(if $(CONFIG_WSTL_MEDIASERVER),westell/media_server_control)
SUBDIRS+=$(if $(CONFIG_WSTL_BOOTLOADER_IMG),westell/perm_storage/bootimages)
SUBDIRS+=$(if $(CONFIG_WSTL_MEDIASERVER),twonky)
SUBDIRS+=westell/mgt

include $(RGMK)
