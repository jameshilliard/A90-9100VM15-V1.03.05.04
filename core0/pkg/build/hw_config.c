/****************************************************************************
 *
 * rg/pkg/build/hw_config.c
 * 
 * Copyright (C) Jungo LTD 2004
 * 
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General 
 * Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02111-1307, USA.
 *
 * Developed by Jungo LTD.
 * Residential Gateway Software Division
 * www.jungo.com
 * info@jungo.com
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "config_opt.h"
#include "create_config.h"
#ifdef CONFIG_RG_DO_DEVICES
 #define ralink_rt256x_add(type,wl_name,config) _ralink_rt256x_add(type,wl_name,config) 
static void _ralink_rt256x_add(dev_if_type_t type, char *wl_name,char *config)
#else
 #define ralink_rt256x_add(type,wl_name,config) _ralink_rt256x_add(wl_name,config) 
static void _ralink_rt256x_add(char *wl_name,char *config)
#endif    
{
    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
    {
        token_set_y("CONFIG_80211G_AP_ADVANCED");
	token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
	token_set_y("CONFIG_RG_WPA_WBM");
	token_set_y("CONFIG_RG_8021X_WBM");
	if (!strcmp(config, "CONFIG_RALINK_RT2561"))
	    token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
    }
    token_set_m(config); 
    if (token_get("CONFIG_RG_JPKG"))
	token_set_dev_type(type);
    else
    {
	dev_add(wl_name, type, DEV_IF_NET_INT); 
	dev_can_be_missing(wl_name);
    }
}

static void ralink_rt2560_add(char *wl_name)
{
    ralink_rt256x_add(DEV_IF_RT2560, wl_name, "CONFIG_RALINK_RT2560");
}

static void ralink_rt2561_add(char *wl_name)
{
    ralink_rt256x_add(DEV_IF_RT2561, wl_name, "CONFIG_RALINK_RT2561");
}

void bcm43xx_add(char *wl_name)
{
    if (wl_name)
    {
	dev_add(wl_name, DEV_IF_BCM43XX, DEV_IF_NET_INT);
	dev_can_be_missing(wl_name);
    }
    else
	token_set_dev_type(DEV_IF_BCM43XX);

    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
    {
	token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	token_set_y("CONFIG_RG_8021X_MD5");
	token_set_y("CONFIG_RG_8021X_TLS");
	token_set_y("CONFIG_RG_8021X_TTLS");
	token_set_y("CONFIG_RG_8021X_RADIUS");
	token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
	token_set_y("CONFIG_RG_WPA_WBM");
	token_set_y("CONFIG_RG_WPA_BCM");
    }
}

static void atheros_ar521x_add(char *wl_name, char *vap_name, ...)
{
    va_list ap;

    token_set_y("CONFIG_NET_RADIO");
    token_set_y("CONFIG_NET_WIRELESS");

    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
    {
        token_set_y("CONFIG_80211G_AP_ADVANCED");
	token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
	token_set_y("CONFIG_RG_WPA_WBM");
	token_set_y("CONFIG_RG_8021X_WBM");
	token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
	token_set_y("CONFIG_RG_HOSTAPD");
	token_set_y("CONFIG_DRIVER_MADWIFI");
	token_set_y("CONFIG_RSN_PREAUTH");
    }
    if (token_get("CONFIG_RG_JPKG"))
    {
	token_set_dev_type(DEV_IF_AR521X);
	if (vap_name)
	    token_set_dev_type(DEV_IF_AR521X_VAP);
    }
    else
    {
	dev_add(wl_name, DEV_IF_AR521X, DEV_IF_NET_INT); 
	dev_can_be_missing(wl_name);

	for (va_start(ap, vap_name); vap_name; vap_name = va_arg(ap, char *))
	    dev_add(vap_name, DEV_IF_AR521X_VAP, DEV_IF_NET_INT);
	va_end(ap);
    }
}

static void atheros_ar50wl_add(char *wl_name, char *vap_name, ...)
{
    va_list ap;

    token_set_y("CONFIG_NET_RADIO");
    token_set_y("CONFIG_NET_WIRELESS");

    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
    {
    token_set_y("CONFIG_80211G_AP_ADVANCED");
     token_set_y("CONFIG_80211NG_AP_ADVANCED");
	token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
	token_set_y("CONFIG_RG_WPA_WBM");
	token_set_y("CONFIG_RG_8021X_WBM");
	token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
	token_set_y("CONFIG_RG_HOSTAPD");
	token_set_y("CONFIG_DRIVER_MADWIFI");
	token_set_y("CONFIG_RSN_PREAUTH");
    }
    if (token_get("CONFIG_RG_JPKG"))
    {
	token_set_dev_type(DEV_IF_AR50WL);
	if (vap_name)
	    token_set_dev_type(DEV_IF_AR50WL_VAP);
    }
    else
    {
	dev_add(wl_name, DEV_IF_AR50WL, DEV_IF_NET_INT); 
	dev_can_be_missing(wl_name);

	for (va_start(ap, vap_name); vap_name; vap_name = va_arg(ap, char *))
	    dev_add(vap_name, DEV_IF_AR50WL_VAP, DEV_IF_NET_INT);
	va_end(ap);
    }
}

void isl_softmac_add(void)
{
    token_set_m("CONFIG_ISL_SOFTMAC");
    dev_add("eth0", DEV_IF_ISL_SOFTMAC, DEV_IF_NET_INT);
    dev_can_be_missing("eth0");

    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
    {
	token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	token_set_y("CONFIG_RG_8021X_MD5");
	token_set_y("CONFIG_RG_8021X_TLS");
	token_set_y("CONFIG_RG_8021X_TTLS");
	token_set_y("CONFIG_RG_8021X_RADIUS");
    }
}

void hardware_features(void)
{
    option_t *hw_tok;

    if (!hw)
    {
	token_set("CONFIG_RG_HW", "NO_HW");
	token_set("CONFIG_RG_HW_DESC_STR", "No hardware - local targets only");
	token_set_y("CONFIG_RG_BUILD_LOCAL_TARGETS_ONLY");
	return;
    }

    hw_tok = option_token_get(openrg_hw_options, hw);

    if (!hw_tok->value)
	conf_err("No description available for HW=%s\n", hw);

    token_set("CONFIG_RG_HW", hw);
    token_set("CONFIG_RG_HW_DESC_STR", hw_tok->value);

    if (IS_HW("ALLWELL_RTL_RTL"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_8139TOO");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("rtl1", DEV_IF_RTL8139, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_INT);

	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("WADB100G"))
    {
	token_set_y("CONFIG_BCM963XX_COMMON");
	token_set_y("CONFIG_BCM96348");
	token_set_y("CONFIG_BCM963XX_SERIAL");
	token_set_m("CONFIG_BCM963XX_BOARD");

	token_set_m("CONFIG_BCM963XX_MTD");
        token_set_y("CONFIG_MTD_CFI_AMDSTD");
	
	token_set("CONFIG_BCM963XX_BOARD_ID", "96348GW-10");
	token_set("CONFIG_BCM963XX_CHIP", "6348");
	token_set("CONFIG_BCM963XX_SDRAM_SIZE", "16");

	token_set_y("CONFIG_PCI");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM");
	    token_set_m("CONFIG_BCM963XX_ADSL");
	    token_set_m("CONFIG_BCM963XX_ATM");
	    token_set_y("CONFIG_RG_ATM_QOS");
	    dev_add("bcm_atm0", DEV_IF_BCM963XX_ADSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    dev_add("bcm0", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);

	    if (!token_get("CONFIG_HW_SWITCH_LAN"))
	    {
		token_set_m("CONFIG_BCM963XX_ETH");
		dev_add("bcm1", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);
	    }
	}

	if (token_get("CONFIG_HW_SWITCH_LAN"))
	{
	    token_set_m("CONFIG_HW_SWITCH_BCM53XX");
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm1", DEV_IF_BCM5325A_HW_SWITCH, DEV_IF_NET_INT);
	}
	
	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_BCM963XX_WLAN");
	    token_set_y("CONFIG_NET_RADIO");
	    token_set_y("CONFIG_NET_WIRELESS");
	    bcm43xx_add("wl0");

	    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
	       token_set_y("CONFIG_RG_WPA_BCM");
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_y("CONFIG_USB_RNDIS");
	    token_set_m("CONFIG_BCM963XX_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_y("CONFIG_RG_UIEVENTS");

	token_set("BOARD", "WADB100G");
	token_set("FIRM", "Broadcom");
    }

    if (IS_HW("PUNDIT"))
    {
	token_set_y("CONFIG_PCBOX");
	/* the following should be set for screen(rather then serial) output */
	token_set("CONFIG_RG_CONSOLE_DEVICE", "console");
	/* TODO: add Pundit specific modules */
    }

    if (IS_HW("ASUS6020VI"))
    {
	token_set_y("CONFIG_BCM963XX_COMMON");
	token_set_y("CONFIG_BCM96348");
	token_set_y("CONFIG_BCM963XX_SERIAL");
	token_set_m("CONFIG_BCM963XX_BOARD");
	
	token_set_m("CONFIG_BCM963XX_MTD");
        token_set_y("CONFIG_MTD_CFI_AMDSTD");
	
	token_set("CONFIG_BCM963XX_BOARD_ID", "AAM6020VI");
	token_set("CONFIG_BCM963XX_CHIP", "6348");
	token_set("CONFIG_BCM963XX_SDRAM_SIZE", "16");

	token_set_y("CONFIG_PCI");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM");
	    token_set_m("CONFIG_BCM963XX_ADSL");
	    token_set_m("CONFIG_BCM963XX_ATM");
	    token_set_y("CONFIG_RG_ATM_QOS");
	    dev_add("bcm_atm0", DEV_IF_BCM963XX_ADSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_SWITCH_LAN"))
	{
	    token_set_m("CONFIG_HW_SWITCH_BCM53XX");
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm1", DEV_IF_BCM5325E_HW_SWITCH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm1", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_NET_RADIO");
	    token_set_y("CONFIG_NET_WIRELESS");
	    token_set_y("CONFIG_BCM963XX_WLAN");
	    token_set_y("CONFIG_BCM963XX_WLAN_4318");

	    bcm43xx_add("wl0");
	}

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_y("CONFIG_RG_UIEVENTS");

	token_set("BOARD", "ASUS6020VI");
	token_set("FIRM", "Asus");
    }

    if (IS_HW("WADB102GB"))
    {
	token_set_y("CONFIG_BCM963XX_COMMON");
	token_set_y("CONFIG_BCM96348");
	token_set_y("CONFIG_BCM963XX_SERIAL");
	token_set_m("CONFIG_BCM963XX_BOARD");
	token_set("CONFIG_BCM963XX_BOARD_ID", "WADB102GB");
	token_set("CONFIG_BCM963XX_CHIP", "6348");
	token_set("CONFIG_BCM963XX_SDRAM_SIZE", "16");
	token_set_y("CONFIG_BCM963XX_SIMPLE_FLASH_LAYOUT");

	token_set_y("CONFIG_RG_MTD_DEFAULT_PARTITION");
	token_set("CONFIG_MTD_PHYSMAP_LEN", "0x00400000");
	token_set("CONFIG_MTD_PHYSMAP_START", "0x1FC00000");

	token_set_y("CONFIG_PCI");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM");
	    token_set_m("CONFIG_BCM963XX_ADSL");
	    token_set_m("CONFIG_BCM963XX_ATM");
	    token_set_y("CONFIG_RG_ATM_QOS");
	    dev_add("bcm_atm0", DEV_IF_BCM963XX_ADSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm1", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_SWITCH_LAN"))
	{
	    token_set_m("CONFIG_HW_SWITCH_BCM53XX");
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm1", DEV_IF_BCM5325E_HW_SWITCH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_NET_RADIO");
	    token_set_y("CONFIG_NET_WIRELESS");
	    token_set_y("CONFIG_BCM963XX_WLAN");
	    token_set_y("CONFIG_BCM963XX_WLAN_4318");

	    bcm43xx_add("wl0");
	}

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_y("CONFIG_RG_UIEVENTS");

	token_set("BOARD", "WADB102GB");
	token_set("FIRM", "Belkin");
    }
    
    if (IS_HW("MPC8272ADS"))
    {
	token_set_y("CONFIG_MPC8272ADS");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0" );

	token_set("FIRM", "freescale");
	token_set("BOARD", "MPC8272ADS");

	/* Ethernet drivers */

	if (token_get("CONFIG_HW_ETH_FEC"))
	{
	    /* CPM2 Options */

	    token_set_y("CONFIG_FEC_ENET");
	    token_set_y("CONFIG_USE_MDIO");
            token_set_y("CONFIG_FCC_DM9161");
	    token_set_y("CONFIG_FCC1_ENET");
	    token_set_y("CONFIG_FCC2_ENET");

	    dev_add("eth0", DEV_IF_MPC82XX_ETH, DEV_IF_NET_EXT);
	    dev_add("eth1", DEV_IF_MPC82XX_ETH, DEV_IF_NET_INT);
	}
	else if (token_get("CONFIG_HW_ETH_EEPRO1000"))
	{
	    token_set_y("CONFIG_E1000");
	    
	    dev_add("eth0", DEV_IF_EEPRO1000, DEV_IF_NET_EXT);
	    dev_add("eth1", DEV_IF_EEPRO1000, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_USB_STORAGE") ||
	    token_get("MODULE_RG_BLUETOOTH"))
	{		
	    token_set_y("CONFIG_USB");
	    token_set_y("CONFIG_USB_M82XX_HCD");
	}

	if (token_get("CONFIG_HW_USB_STORAGE"))
	    token_set_y("CONFIG_USB_DEVICEFS");

	if (token_get("MODULE_RG_BLUETOOTH"))
	    token_set_y("CONFIG_BT_HCIUSB");

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth2", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth2");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	{
	    ralink_rt2560_add("ra0");
	    token_set("CONFIG_RALINK_RT2560_TIMECSR", "0x40");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_y("CONFIG_IPSEC_USE_MPC_CRYPTO");
    }

    if (IS_HW("EP8248"))
    {
	token_set_y("CONFIG_EP8248");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0" );

	token_set("FIRM", "Embedded Planet");
	token_set("BOARD", "EP8248");

	/* Ethernet drivers */

	if (token_get("CONFIG_HW_ETH_FEC"))
	{
	    /* CPM2 Options */

	    token_set_y("CONFIG_FEC_ENET");
	    token_set_y("CONFIG_USE_MDIO");
            token_set_y("CONFIG_FCC_LXT971");
	    token_set_y("CONFIG_FCC1_ENET");
	    token_set_y("CONFIG_FCC2_ENET");

	    dev_add("eth0", DEV_IF_MPC82XX_ETH, DEV_IF_NET_INT);
	    dev_add("eth1", DEV_IF_MPC82XX_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_USB_STORAGE"))
	{
	    token_set_y("CONFIG_USB");
	    token_set_y("CONFIG_USB_DEVICEFS");
	    token_set_y("CONFIG_USB_M82XX_HCD");
	}
	
	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}
    }
    
    if (IS_HW("MPC8349ITX"))
    {
	token_set_y("CONFIG_MPC8349_ITX");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");

	token_set("FIRM", "Freescale");
	token_set("BOARD", "mpc8349-itx");

	/* Ethernet drivers */
	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_y("CONFIG_GIANFAR");
	    token_set_y("CONFIG_CICADA_PHY");
	    token_set_y("CONFIG_PHYLIB");
	    
	    dev_add("eth0", DEV_IF_MPC82XX_ETH, DEV_IF_NET_EXT);
	}
	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_y("CONFIG_GIANFAR");
	    token_set_y("CONFIG_CICADA_PHY");
	    token_set_y("CONFIG_PHYLIB");

	    dev_add("eth1", DEV_IF_MPC82XX_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_USB_ETH"))
	{
	    token_set_y("CONFIG_USB_GADGET");
	    token_set_y("CONFIG_USB_ETH");
	    token_set_y("CONFIG_USB_MPC");
	    token_set_y("CONFIG_USB_GADGET_MPC");
	    token_set_y("CONFIG_USB_GADGET_DUALSPEED");
	    token_set_y("CONFIG_USB_ETH_RNDIS");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}
	
	if (token_get("CONFIG_HW_USB_STORAGE"))
	{
	    token_set_y("CONFIG_USB");
	    token_set_y("CONFIG_USB_EHCI_HCD");
	    token_set_y("CONFIG_USB_EHCI_ROOT_HUB_TT");
	    token_set_y("CONFIG_USB_DEVICEFS");
	    token_set_y("CONFIG_FSL_USB20");
	    token_set_y("CONFIG_MPH_USB_SUPPORT");
	    token_set_y("CONFIG_MPH0_USB_ENABLE");
	    token_set_y("CONFIG_MPH0_ULPI");
	    if (!token_get("CONFIG_USB_MPC"))
	    {
		token_set_y("CONFIG_MPH1_USB_ENABLE");
		token_set_y("CONFIG_MPH1_ULPI");
	    }
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	{
	    ralink_rt2560_add("ra0");
	    token_set("CONFIG_RALINK_RT2560_TIMECSR", "0x40");
	}
	
	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_IDE"))
	{
	    token_set_y("CONFIG_SCSI");
	    token_set_y("CONFIG_SCSI_SATA");
	    token_set_y("CONFIG_SCSI_SATA_SIL");
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_m("CONFIG_MPC8349E_SEC2x");

	/* TODO: Do we need some CONFIG_HW_xxx to enable flash support? */
	/* MTD */
#if 0
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");
	token_set_y("CONFIG_MTD_CFI_I1");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_2");
	token_set_y("CONFIG_MTD_CFI_UTIL");
	
	/* TODO: write custom mpc8349-itx MTD driver, as it has 2 8Mb chips.
	 * Currently generic physmap driver is used, only for the second chip */
	token_set_y("CONFIG_MTD_PHYSMAP");
    	token_set("CONFIG_MTD_PHYSMAP_BANKWIDTH", "2");
	token_set("CONFIG_MTD_PHYSMAP_LEN", "0x800000");
	token_set("CONFIG_MTD_PHYSMAP_START", "0xFE800000");
#endif	
    }

    if (IS_HW("ALLWELL_RTL_RTL_ISL38XX"))
    {
	token_set_m("CONFIG_8139TOO");
	if (token_get("CONFIG_RG_RGLOADER"))
	{
	    dev_add("rtl1", DEV_IF_RTL8139, DEV_IF_NET_INT);
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_INT);
	}
	else
	{
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_EXT);
	    dev_add("rtl1", DEV_IF_RTL8139, DEV_IF_NET_INT);
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}
	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("ALLWELL_RTL_EEP"))
    {
	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_8139TOO");
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_EEPRO100");
	    dev_add("eep0", DEV_IF_EEPRO100, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    token_set_m("CONFIG_8139TOO");
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_INT);
	}
	
	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("ALLWELL_ATMNULL_RTL"))
    {
	token_set_m("CONFIG_8139TOO");
	token_set_y("CONFIG_ATM");
	token_set_y("CONFIG_ATM_NULL");
	dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_INT);
	dev_add("atmnull0", DEV_IF_ATM_NULL, DEV_IF_NET_EXT);
	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("PCBOX_EEP_EEP"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_EEPRO100");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("eep0", DEV_IF_EEPRO100, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("eep1", DEV_IF_EEPRO100, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("eep0", DEV_IF_EEPRO100, DEV_IF_NET_INT);

	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("CX82100"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_CNXT_EMAC");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("cnx0", DEV_IF_CX821XX_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("cnx1", DEV_IF_CX821XX_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("cnx1", DEV_IF_CX821XX_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	    token_set_y("CONFIG_USB_RNDIS");

	token_set_y("CONFIG_ARMNOMMU");
	token_set_y("CONFIG_CX821XX_COMMON");
	token_set_y("CONFIG_BD_GOLDENGATE");
	token_set_y("CONFIG_CHIP_CX82100");
	token_set_y("CONFIG_PHY_KS8737");

	token_set("FIRM", "Conexant");
	token_set("BOARD", "CX82100");
    }

    if (IS_HW("ADM5120P"))
    {
	token_set_y("CONFIG_ADM5120_COMMON");

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("adm0", DEV_IF_ADM5120_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("adm1", DEV_IF_ADM5120_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_VINETIC");
	    token_set("CONFIG_VINETIC_LINES_PER_CHIP", "2");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "2");
	}
    }

    if (IS_HW("BCM91125E") || IS_HW("COLORADO"))
    {
	token_set_y("CONFIG_SIBYTE_SB1250");

	/* Used to be CONFIG_SIBYTE_SWARM */
	if (IS_HW("BCM91125E"))
	    token_set_y("CONFIG_SIBYTE_SENTOSA");
	else if (IS_HW("COLORADO"))
	    token_set_y("CONFIG_SIBYTE_COLORADO");

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("eth1", DEV_IF_BRCM91125E_ETH, DEV_IF_NET_INT);
	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("eth0", DEV_IF_BRCM91125E_ETH, DEV_IF_NET_EXT);
	else if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("eth0", DEV_IF_BRCM91125E_ETH, DEV_IF_NET_INT);

    	/* Flash/MTD */
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_INTELEXT");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");
	token_set_m("CONFIG_BCM91125E_MTD");
	/* New MTD configs (Linux-2.6) */
	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_1");
	    token_set_y("CONFIG_MTD_CFI_I1");
	    token_set_y("CONFIG_MTD_CFI_UTIL");
	}
    }

    if (IS_HW("INCAIP_LSP"))
    {
	token_set_y("CONFIG_INCAIP_COMMON");

	token_set_y("CONFIG_RG_VLAN_8021Q");
	token_set_m("CONFIG_INCAIP_SWITCH");
	token_set_m("CONFIG_INCAIP_SWITCH_API");
	token_set_m("CONFIG_INCAIP_ETHERNET");
	token_set_m("CONFIG_INCAIP_KEYPAD");
	token_set_m("CONFIG_INCAIP_DSP");
	token_set_m("CONFIG_VINETIC");
	token_set_m("CONFIG_INCAIP_IOM2");
    }

    if (IS_HW("INCAIP"))
    {
	token_set_y("CONFIG_INCAIP_COMMON");

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_m("CONFIG_INCAIP_LEDMATRIX");

	if (token_get("CONFIG_HW_KEYPAD"))
	    token_set_m("CONFIG_INCAIP_KEYPAD");

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_INCAIP_DSP");
	    token_set_y("CONFIG_RG_IPPHONE");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "1");
	}
	
	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_y("CONFIG_INCA_HW_ENCRYPT");
    }

    if (IS_HW("FLEXTRONICS"))
    {
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 
	token_set_y("CONFIG_INCAIP_COMMON");
	token_set_y("CONFIG_INCAIP_FLEXTRONICS");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_m("CONFIG_INCAIP_LEDMATRIX");

	if (token_get("CONFIG_HW_KEYPAD"))
	    token_set_m("CONFIG_INCAIP_KEYPAD");

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_INCAIP_DSP");
	    token_set_y("CONFIG_RG_IPPHONE");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "1");
	}
	
	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_y("CONFIG_INCA_HW_ENCRYPT");
    }

    if (IS_HW("ALLTEK_VLAN"))
    {
	token_set_y("CONFIG_INCAIP_COMMON");
	token_set_y("CONFIG_INCAIP_ALLTEK");

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_VINETIC");
	    token_set("CONFIG_VINETIC_LINES_PER_CHIP", "2");
	    token_set_m("CONFIG_INCAIP_IOM2");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "2");
	}
	
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_y("CONFIG_RG_VLAN_8021Q");
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("inca0.3", DEV_IF_INCAIP_VLAN, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("inca0.2", DEV_IF_INCAIP_VLAN, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_y("CONFIG_INCA_HW_ENCRYPT");
    }

    if (IS_HW("ALLTEK"))
    {
	token_set_y("CONFIG_INCAIP_COMMON");
	token_set_y("CONFIG_INCAIP_ALLTEK");
	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_VINETIC");
	    token_set("CONFIG_VINETIC_LINES_PER_CHIP", "2");
	    token_set_m("CONFIG_INCAIP_IOM2");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "2");
	}
	
	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_y("CONFIG_INCA_HW_ENCRYPT");
    }

    if (IS_HW("RTA770W") || IS_HW("GTWX5803"))
    {
	token_set_y("CONFIG_BCM963XX_COMMON");
	token_set_y("CONFIG_BCM96345");
	token_set_y("CONFIG_BCM963XX_SERIAL");
	token_set_m("CONFIG_BCM963XX_BOARD");
	token_set_y("CONFIG_BCM963XX_RGL_FLASH_LAYOUT");
	token_set("CONFIG_BCM963XX_CHIP", "6345");
	token_set("CONFIG_BCM963XX_SDRAM_SIZE", "16");

	if (token_get("CONFIG_RG_MTD"))
	{
	    token_set_y("CONFIG_RG_MTD_DEFAULT_PARTITION");
	    token_set("CONFIG_MTD_PHYSMAP_LEN", "0x00400000");
	    token_set("CONFIG_MTD_PHYSMAP_START", "0x1FC00000");
	}
	else
	{
	    token_set_m("CONFIG_BCM963XX_MTD");
	    token_set_y("CONFIG_MTD_CFI_AMDSTD");
	}

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM");
	    token_set_m("CONFIG_BCM963XX_ADSL");
	    token_set_m("CONFIG_BCM963XX_ATM");
	    token_set_y("CONFIG_RG_ATM_QOS");
	    dev_add("bcm_atm0", DEV_IF_BCM963XX_ADSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm0", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_SWITCH_LAN"))
	{
	    token_set_m("CONFIG_HW_SWITCH_BCM53XX");
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm0", DEV_IF_BCM5325A_HW_SWITCH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_BCM963XX_WLAN");
	    token_set_y("CONFIG_NET_RADIO");
	    token_set_y("CONFIG_NET_WIRELESS");
	    bcm43xx_add("wl0");
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_y("CONFIG_USB_RNDIS");
	    token_set_m("CONFIG_BCM963XX_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_y("CONFIG_RG_UIEVENTS");

	if (IS_HW("RTA770W"))
	{
	    token_set("CONFIG_BCM963XX_BOARD_ID", "RTA770W");
	    token_set("BOARD", "RTA770W");
	    token_set("FIRM", "Belkin");
	}
	else
	{
	    token_set("CONFIG_BCM963XX_BOARD_ID", "GTWX5803");
	    token_set("BOARD", "GTWX5803");
	    token_set("FIRM", "Gemtek");
	}
    }

    if (IS_HW("BCM94702"))
    {
	token_set_y("CONFIG_BCM947_COMMON");
	token_set_y("CONFIG_BCM4702");

	/* In order to make an root cramfs based dist use the following 
	 * instead of SIMPLE_RAMDISK
	 *  token_set_y("CONFIG_CRAMFS");
	 *  token_set_y("CONFIG_SIMPLE_CRAMDISK");
	 *  token_set("CONFIG_CMDLINE", 
	 *      "\"root=/dev/mtdblock2 noinitrd console=ttyS0,115200\"");
	 */

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_ET");
	    token_set_y("CONFIG_ET_47XX");
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("bcm1", DEV_IF_BCM4710_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("bcm0", DEV_IF_BCM4710_ETH, DEV_IF_NET_INT);

	token_set("BOARD", "BCM94702");
	token_set("FIRM", "Broadcom");
    }

    if (IS_HW("BCM94704"))
    {
	token_set_y("CONFIG_BCM947_COMMON");
	token_set_y("CONFIG_BCM4704");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_ET");
	    token_set_y("CONFIG_ET_47XX");
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("bcm1", DEV_IF_BCM4710_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("bcm0", DEV_IF_BCM4710_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_NET_RADIO");
	    bcm43xx_add("eth0");
	}

	token_set("BOARD", "BCM94704");
	token_set("FIRM", "Broadcom");
    }

    if (IS_HW("BCM94712"))
    {
	/* This means (among others) copy CRAMFS to RAM, which is much
	 * safer, but PMON/CFE currently has a limit of ~3MB when uncompressing.
	 * If your image is larger than that, either reduce image size or
	 * remove CONFIG_COPY_CRAMFS_TO_RAM for this platform. */
	token_set_y("CONFIG_BCM947_COMMON");
	token_set_y("CONFIG_BCM4710");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_ET");
	    token_set_y("CONFIG_ET_47XX");
	    token_set_y("CONFIG_RG_VLAN_8021Q");
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("bcm0.1", DEV_IF_BCM4710_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("bcm0.0", DEV_IF_BCM4710_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_NET_RADIO");
	    bcm43xx_add("eth0");
	}

	token_set("BOARD", "BCM94712");
	token_set("FIRM", "Broadcom");
    }

    if (IS_HW("WRT54G"))
    {
	token_set_y("CONFIG_BCM947_COMMON");
	token_set_y("CONFIG_BCM947_HW_BUG_HACK");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_ET");
	    token_set_y("CONFIG_ET_47XX");
	    token_set_y("CONFIG_RG_VLAN_8021Q");
	    token_set_y("CONFIG_VLAN_8021Q_FAST");
	    dev_add("bcm0", DEV_IF_BCM4710_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("bcm0.1", DEV_IF_VLAN, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("bcm0.2", DEV_IF_VLAN, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    bcm43xx_add("eth0");
	    token_set_y("CONFIG_NET_RADIO");
	}

	token_set("BOARD", "Cybertan");
	token_set("FIRM", "Cybertan");
    }
    
    if (IS_HW("CENTROID"))
    {
	set_big_endian(0);

	/* Do not change the order of the devices definition.
	 * Storlink has a bug in their ethernet driver which compells us to `up`
	 * eth0 before eth1
	 */
	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("sl0", DEV_IF_SL2312_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("sl1", DEV_IF_SL2312_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("sl1", DEV_IF_SL2312_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN") || token_get("CONFIG_HW_ETH_WAN"))
	    token_set_m("CONFIG_SL2312_ETH");

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	    conf_err("Can't define both CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN");

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add("ra0");

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_USB_STORAGE"))
	{
	    token_set_y("CONFIG_RG_USB");
	    token_set_y("CONFIG_USB_DEVICEFS");
	    token_set_m("CONFIG_USB_OHCI_SL2312");
	}
	
	token_set_y("CONFIG_ARCH_SL2312"); 
 	token_set_y("CONFIG_SL2312_ASIC"); 
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttySI0");
	token_set("CONFIG_SDRAM_SIZE", "64");

	token_set_m("CONFIG_SL2312_FLASH");
	
	if (!token_get("CONFIG_RG_MTD"))
	    token_set_y("CONFIG_MTD_CFI_AMDSTD");
	
	token_set("FIRM", "Storlink");
	token_set("BOARD", "SL2312");
    }

    if (IS_HW("IXDP425"))
    {
	/* Larger memory is available for Richfield (256MB) or
	 * Matecumbe (128MB) but use 64 for max PCI performance
	 * rates (DMA window size = 64MB) */
	token_set_y("CONFIG_ARCH_IXP425_IXDP425"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");

	token_set("FIRM", "Intel");
	token_set("BOARD", "IXDP425");
	token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	    conf_err("Can't define both CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN");

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    /* ADSL Chip Alcatel 20170 on board */
	    token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	    token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");
	    
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}
    }

    if (IS_HW("MATECUMBE"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	/* Larger memory is available for Richfield (256MB) or
	 * Matecumbe (128MB) but use 64 for max PCI performance
	 * rates (DMA window size = 64MB) */
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_ARCH_IXP425_MATECUMBE");
	token_set_y("CONFIG_IXP425_CSR_USB");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	token_set("FIRM", "Intel");
	token_set("BOARD", "IXDP425");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }

    if (IS_HW("KINGSCANYON"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set_y("CONFIG_IXP425_CSR_USB");
	token_set_y("CONFIG_ARCH_IXP425_KINGSCANYON");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}

	token_set("FIRM", "Interface_Masters");
	token_set("BOARD", "KINGSCANYON");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }

    if (IS_HW("ROCKAWAYBEACH"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_ARCH_IXP425_ROCKAWAYBEACH");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set("CONFIG_FILESERVER_KERNEL_CONFIG", "USB");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}

	token_set("FIRM", "Intel");
	token_set("BOARD", "ROCKAWAYBEACH");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }
    
    if (IS_HW("WAV54G"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_WAV54G"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20170");
	token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");

	/* Add VLAN support so Cybertan can add HW DMZ */
	token_set_y("CONFIG_RG_VLAN_8021Q");

	if (token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}
	
	token_set("FIRM", "Cybertan");
	token_set("BOARD", "WAV54G");

	/* Flash CHIP */
	token_set("CONFIG_IXP425_FLASH_E28F640J3", "m");

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
    }

    if (IS_HW("NAPA"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_NAPA"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	token_set("FIRM", "Sohoware");
	token_set("BOARD", "NAPA");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	token_set_y("CONFIG_RG_UIEVENTS");
	token_set_m("CONFIG_RG_KLEDS");
    }
	
    if (IS_HW("COYOTE"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_COYOTE"); 
	if (!token_get("CONFIG_RG_FLASH_LAYOUT_SIZE"))
	    token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	if (!token_get("CONFIG_IXP425_SDRAM_SIZE"))
	    token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	if (token_get("CONFIG_HW_HSS_WAN"))
	    token_set_y("CONFIG_RG_HSS");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    /* ADSL Chip Alcatel 20170 on board */
	    token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20170");
	    token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");
	    
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	    conf_err("Can't define both CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN");

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_EEPRO100_LAN"))
	{
	    token_set_m("CONFIG_EEPRO100");
	    dev_add("eep0", DEV_IF_EEPRO100, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}

	if (token_get("CONFIG_HW_80211G_ISL_SOFTMAC"))
	    isl_softmac_add();

	if (token_get("CONFIG_HW_80211B_PRISM2"))
	{
	    token_set_m("CONFIG_PRISM2_PCI");
	    dev_add("wlan0", DEV_IF_PRISM2, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add("ra0");

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_IXP425_DSR");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "2");
	}
	
	if (token_get("CONFIG_HW_EEPROM"))
	    token_set("CONFIG_PCF8594C2", "m");

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	if (token_get("CONFIG_IXP425_CSR_FULL"))
	{
	    /* ADSL Chip Alcatel 20170 on board */
	    token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20170");
	    token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");
	}

	token_set("FIRM", "Intel");
	token_set("BOARD", "COYOTE");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	if (token_get("CONFIG_HW_IDE"))
	{
	    /* Custom IDE device on expansion BUS */
	    token_set_y("CONFIG_IDE");
	    token_set_y("CONFIG_BLK_DEV_IDE");
	    token_set_y("CONFIG_BLK_DEV_IDEDISK");
	    token_set_y("CONFIG_IDEDISK_MULTI_MODE");
	    token_set_y("CONFIG_RG_IDE_NON_BLOCKING");
	}
    }

    if (IS_HW("MONTEJADE"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_MONTEJADE"); 
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_HW_ST_20190");
	    token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");
	    
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_SWITCH_LAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_RTL8305SB_HW_SWITCH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	    conf_err("Can't define both CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN");

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add("ra0");

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_80211N_AIRGO_AGN100"))
	{
	    token_set_y("CONFIG_80211G_AP_ADVANCED");
	    token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	    token_set_y("CONFIG_RG_WPA_WBM");
	    token_set_y("CONFIG_RG_8021X_WBM");
	    token_set_m("CONFIG_AGN100");
	    token_set_y("CONFIG_NETFILTER");
	    dev_add("wlan0", DEV_IF_AGN100, DEV_IF_NET_INT);
	    dev_can_be_missing("wlan0");
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_IXP425_DSR");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "4");
	}

	token_set("FIRM", "Intel");
	token_set("BOARD", "MONTEJADE");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F640J3","m");
    }
 
    if (IS_HW("JIWIS800") || IS_HW("JIWIS832") || IS_HW("JIWIS832FL"))
    {

	if (IS_HW("JIWIS800"))
	{
	    token_set_y("CONFIG_ARCH_IXP425_JIWIS800"); 
	    token_set("BOARD", "JI-WIS 800");
	}
	else if (IS_HW("JIWIS832"))
	{
	    token_set_y("CONFIG_ARCH_IXP425_JIWIS832");
	    token_set("BOARD", "JI-WIS 832");
	}
	else if (IS_HW("JIWIS832FL"))
	{
	    token_set_y("CONFIG_ARCH_IXP425_JIWIS832");
	    token_set("BOARD", "JI-WIS 832FL");
	}

	token_set("CONFIG_IXP425_SDRAM_SIZE", "128");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_HW_ST_20190");
	    token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");
	    
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_SWITCH_LAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_RTL8305SC_HW_SWITCH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	    conf_err("Can't define both CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN");

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add("ra0");

	if (token_get("CONFIG_HW_80211N_AIRGO_AGN100"))
	{
	    token_set_y("CONFIG_80211G_AP_ADVANCED");
	    token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	    token_set_y("CONFIG_RG_WPA_WBM");
	    token_set_y("CONFIG_RG_8021X_WBM");
	    token_set_m("CONFIG_AGN100");
	    token_set_y("CONFIG_NETFILTER");
	    dev_add("wlan0", DEV_IF_AGN100, DEV_IF_NET_INT);
	    dev_can_be_missing("wlan0");
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}

	if (token_get("CONFIG_HW_DSP"))
	{
	    if (IS_HW("JIWIS800"))
	    {
		token_set_y("CONFIG_IXP425_DSR");
		token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "4");
	    }
	    else if (IS_HW("JIWIS832"))
	    {
		/*
		 * TODO: Add support for JI-WIS 832 in DSR modules
		token_set_y("CONFIG_IXP425_DSR");
		token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "1");
		*/
	    }
	    else if (IS_HW("JIWIS832FL"))
	    {
		/* No VOICE */
	    }
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	token_set("FIRM", "JStream");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3","m");
    }
 
    if (IS_HW("BRUCE"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_BRUCE");
	if (!token_get("CONFIG_RG_FLASH_LAYOUT_SIZE"))
	    token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "128");
	token_set("CONFIG_SDRAM_SIZE", "128");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");

	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	{
	    fprintf(stderr, "Can't define CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN "
		"together\n");
	    exit(1);
	}

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}

	if (token_get("CONFIG_HW_EEPROM"))
	    token_set("CONFIG_PCF8594C2", "m");

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	token_set("FIRM", "Jabil");
	token_set("BOARD", "Bruce");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	if (token_get("CONFIG_HW_IDE"))
	{
	    /* SiI680ACL144 and SiI3512CT128 on PCI BUS */
	    token_set_y("CONFIG_IDE");
	    token_set_y("CONFIG_BLK_DEV_IDE");
	    token_set_y("CONFIG_BLK_DEV_IDEDISK");
	    token_set_y("CONFIG_BLK_DEV_IDECD");
	    token_set_y("CONFIG_BLK_DEV_IDEPCI");
	    token_set_y("CONFIG_BLK_DEV_IDEDMA");
	    token_set_y("CONFIG_BLK_DEV_IDEDMA_PCI");
	    token_set_y("CONFIG_BLK_DEV_OFFBOARD");
	    token_set_y("CONFIG_BLK_DEV_SIIMAGE");
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	/* I2C Bus */
	if (token_get("CONFIG_HW_I2C"))
	{
	    token_set_y("CONFIG_I2C");
	    token_set_y("CONFIG_I2C_ALGOBIT");
	    token_set_y("CONFIG_I2C_IXP425");
	}

	/* HW clock */
	if (token_get("CONFIG_HW_CLOCK"))
	    token_set_m("CONFIG_I2C_DS1374");

	/* Env. monitor clock */
	if (token_get("CONFIG_HW_ENV_MONITOR"))
	    token_set_m("CONFIG_I2C_LM81");

	/* CPLD chip module and API */
	token_set_m("CONFIG_ARCH_IXP425_BRUCE_CPLD");
	
    }

    if (IS_HW("TWINPASS") || IS_HW("ULTRALINE3_TWINPASS"))
    {
	token_set_y("CONFIG_INFINEON_TWINPASS");
	
	if (IS_HW("TWINPASS"))
	{
        if(!token_is_y("CONFIG_INFINEON_PPE_D4"))
        {
	    token_set_y("CONFIG_DANUBE_ETHERNET_D2");
        }
        else
        {
            token_set_m("CONFIG_DANUBE_ETHERNET_D4");
        }
	    token_set_y("CONFIG_ADM6996_SUPPORT");
	    token_set_y("CONFIG_SWITCH_ADM6996I");
	    token_set_y("CONFIG_SWITCH_ADM6996_MDIO");

        token_set_y("CONFIG_WESTELL_MGMUT_FTM"); //12-15-06

	    if (token_get("CONFIG_HW_ETH_LAN"))
			dev_add("eth0", DEV_IF_TWINPASS_ETH, DEV_IF_NET_INT);
	    if (token_get("CONFIG_HW_ETH_WAN"))
		dev_add("eth1", DEV_IF_TWINPASS_ETH, DEV_IF_NET_EXT);

	}
	else if (IS_HW("ULTRALINE3_TWINPASS"))
	{
	    token_set_y("CONFIG_WESTELL_ULTRALINE3");
        token_set_y("CONFIG_WESTELL_MGMUT_FTM");
        if(!token_is_y("CONFIG_INFINEON_PPE_D4"))
        {
            token_set_y("CONFIG_DANUBE_ETHERNET_D2");
        }
        else
        {
            token_set_m("CONFIG_DANUBE_ETHERNET_D4");
        }

        if ( token_get("CONFIG_HW_TURBO_DUAL_MOCA") || token_get("CONFIG_HW_TURBO_M100"))
        {
            token_set_y("CONFIG_TANTOS_PSB6973");
            token_set_y("CONFIG_RG_DEV_IF_PSB6973_HW_SWITCH");
            token_set_y("CONFIG_RG_DEV_IF_PSB6973_HW_WAN_SWITCH");
            token_set_y("CONFIG_WSTL_WATCHDOG_CPLD");
        }
        else
            token_set_y("CONFIG_MARVELL_88E60XX");

	    if (token_get("CONFIG_HW_ETH_LAN")){
            if (token_get("CONFIG_TANTOS_PSB6973")) {
                dev_add("eth0", DEV_IF_PSB6973_HW_SWITCH, DEV_IF_NET_INT);

            }
            else{
                dev_add("eth0", DEV_IF_MV88E60XX_HW_SWITCH, DEV_IF_NET_INT);
            }
                
        }
            
	    if (token_get("CONFIG_HW_ETH_WAN") ||
		token_get("CONFIG_HW_VDSL_WAN")  || token_get("CONFIG_HW_MOCA_WAN") || token_get("CONFIG_HW_WAN_ETHERNET") )
	    {
		/* In Westell's UL3 the ETH and VDSL or ETH and MOCA are connected to the same
		 * WAN switch.
		 */
            if (token_get("CONFIG_TANTOS_PSB6973"))
                dev_add("eth1", DEV_IF_PSB6973_HW_WAN_SWITCH, DEV_IF_NET_EXT);
            else
                dev_add("eth1", DEV_IF_MV88E60XX_HW_WAN_SWITCH, DEV_IF_NET_EXT);
		
	    }

	    dev_hw_switch_hide_port("eth1", 0);
	    if (token_get("CONFIG_HW_VDSL_WAN"))
	    {
            dev_hw_switch_hide_port("eth1", 1);
            /* Set the default force port to VDSL port (1) */
            dev_hw_switch_force_port("eth1", 1);
	    }

        if (token_get("CONFIG_HW_MOCA_WAN")) {
            /*We want the Etheret the default port*/
            if (token_get("CONFIG_HW_WAN_ETHERNET") || token_get("CONFIG_HW_DEF_TO_WAN_ETH")) {
                dev_hw_switch_hide_port("eth1", 1);
                dev_hw_switch_force_port("eth1", 0);
          
            }
            else{
                 dev_hw_switch_hide_port("eth1", 1);
                 /* Set the default force port to the MOCA port (1) */
                 dev_hw_switch_force_port("eth1", -1 );
            }   
        }

        if (token_get("CONFIG_HW_WAN_ETHERNET")){
            //dev_hw_switch_hide_port("eth1", 1); this is not needed for M100 since it should only have one WAN port (besides CPU)
            dev_hw_switch_force_port("eth1", 0);
        }

        if (token_get("CONFIG_HW_MOCA_WAN"))
            dev_add_description("eth1", "Broadband Connection (Ethernet)");
        else
             dev_add_description("eth1", "WAN Ethernet");
	    
	    /* TODO: remove if PCI/wireless is on Core1 */
	    token_set_y("CONFIG_PCI");
	    if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
		ralink_rt2560_add("ra0");

	    if (token_get("CONFIG_HW_MOCA"))
	    {
             if (token_get("CONFIG_HW_TURBO_DUAL_MOCA"))
             {
                 token_set_y("MOCA_TURBO_LAN");
                 if (token_get("MOCA_VERSION_1_1")) 
                 {
                     token_set_y("MOCA_ENTROPIC_TURBO");
                 }
                 else
                 {
                     token_set_y("MOCA_ENTROPIC_V_04");
                 }
             }
             else if (token_get("CONFIG_HW_DUAL_MOCA"))
             {
                 token_set_y("MOCA_ENTROPIC_V_04");
             }

                // CR 11355 - Hide Moca switch from gui
             dev_hw_switch_hide_port("eth0", 4);
             token_set_y("CONFIG_WSTL_MOCA_CHANGE");
             token_set_y("CONFIG_ENTROPIC_CLINK");
             if (token_get("CONFIG_HW_MOCA_WAN")) {
                 //add the LAN
                 dev_add("LAN-en2210", DEV_IF_MOCA_EN2210_MII, DEV_IF_NET_INT);
                 dev_moca_conf_set("LAN-en2210", "eth0");
                 //add the WAN
                 dev_add("WAN-en2210", DEV_IF_MOCA_EN2210_MII, DEV_IF_NET_INT);     
                 dev_moca_conf_set("WAN-en2210", "eth1"); 
             }
             else {
                 dev_add("en2210", DEV_IF_MOCA_EN2210_MII, DEV_IF_NET_INT);
                 dev_moca_conf_set("en2210", "eth0");
             }
            
	    }

	    if (token_get("CONFIG_HW_USB_STORAGE"))
	    {
		token_set_y("CONFIG_GENERIC_ISA_DMA");
		token_set_y("CONFIG_USB");
		token_set_y("CONFIG_USB_STORAGE");
		token_set_y("CONFIG_USB_DEVICEFS");
		token_set_y("CONFIG_USB_EHCI_HCD");
//		token_set_m("CONFIG_USB_DWC3884_HCD");
        token_set_m("CONFIG_USB_DWC_OTG_HCD"); /* new Infineon USB otg driver */
		token_set_y("CONFIG_USB_SERIAL");
		token_set_y("CONFIG_USB_SERIAL_PL2303");
		token_set_y("CONFIG_USB_SERIAL_SPCP8X5");
        /* hardware platform specific USB items */
        if ( token_get("CONFIG_HW_TURBO_M100"))
        {
            token_set_y("CONFIG_WSTL_USBHUBOVERCURRENT_CPLD");
        }
        if ( token_get("CONFIG_HW_DUAL_MOCA"))
        {
            token_set_y("CONFIG_WSTL_USBHOSTOVERCURRENT_GPIO");
        }
	    }
	}

	if (token_get("CONFIG_RG_FASTPATH"))
	{
	    token_set_y("CONFIG_RG_FASTPATH_PLAT_TWINPASS");
	    token_set("CONFIG_RG_FASTPATH_PLAT_PATH",
		"vendor/infineon/twinpass/rg_modules");
	    token_set_m("CONFIG_DANUBE_PPA");

        if(token_is_y("CONFIG_INFINEON_PPE_D4"))
        {
	    token_set_y("CONFIG_DANUBE_PPA_D4");
        }
	}

	if (token_get("CONFIG_HW_80211G_ATHEROS_AR521X"))
	{
	    atheros_ar521x_add("wifi0", "ath0", NULL);
	    dev_set_dependency("ath0", "wifi0");

        if (token_get("CONFIG_HW_80211N_ATHEROS_AR50WL"))
        {
            atheros_ar50wl_add("wifi1", "ath1", NULL);
            dev_set_dependency("ath1", "wifi1");
        }
	}
    else
    {

        if (token_get("CONFIG_HW_80211N_ATHEROS_AR50WL"))
        {
            atheros_ar50wl_add("wifi0", "ath0", NULL);
            dev_set_dependency("ath0", "wifi0");
        }
    }


	if (token_get("CONFIG_HW_VDSL_WAN"))
	{
	    if(!token_is_y("CONFIG_RG_INFINEON_VDSL"))
	    {
    	    /* Note: In our case there is no device for the vdsl it is part of
	         * the HW switch eth1
    	     */
	       token_set_y("CONFIG_RG_IKANOS_VDSL");
        }

        token_set_y("CONFIG_RG_THREADS");
	}

    if (token_get("CONFIG_HW_MOCA_WAN")) {
        token_set_y("CONFIG_RG_MOCA_WAN");
    }

    if (token_get("CONFIG_HW_WAN_ETHERNET")) {
        token_set_y("CONFIG_RG_WAN_ETHERNET");
    }

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

    	/* Flash/MTD */
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_ADV_OPTIONS");
	token_set_y("CONFIG_MTD_CFI_NOSWAP");
	token_set_y("CONFIG_MTD_CFI_GEOMETRY");
	token_set_y("CONFIG_MTD_CFI_B2");
	token_set_y("CONFIG_MTD_CFI_I1");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");
	token_set("CONFIG_MTD_CFI_AMDSTD_RETRY", "0");
	token_set_y("CONFIG_MTD_CFI_UTIL");

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_m("CONFIG_CRYPTO_DANUBE");
	    token_set_y("CONFIG_CRYPTO_DEV_DANUBE");
	    token_set_y("CONFIG_CRYPTO_DEV_DANUBE_DES");
	    token_set_y("CONFIG_CRYPTO_DEV_DANUBE_AES");
	    token_set_y("CONFIG_CRYPTO_DEV_DANUBE_SHA1");
	    token_set_y("CONFIG_CRYPTO_DEV_DANUBE_MD5");
	}
    }

    if (IS_HW("AD6834"))
    {
	int is_linux26 = !strcmp(os, "LINUX_26");

	token_set_y("CONFIG_MACH_ADI_FUSIV");

	token_set_y("CONFIG_CPU_LX4189");
	token_set_y("CONFIG_ADI_6843");
	token_set_y("CONFIG_PCI");
	token_set_y("CONFIG_NEW_PCI");
	token_set_y("CONFIG_PCI_AUTO");

	token_set("CONFIG_SDRAM_SIZE", "32");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8");

	token_set_y("CONFIG_ADI_6843_RG1");
	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    if (!is_linux26)
		token_set_y("CONFIG_ETH_NETPRO_SIERRA");
	    else
		token_set_y("CONFIG_ADI_FUSIV_ETHERNET");
	    
	    dev_add("eth0", DEV_IF_AD6834_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_y("CONFIG_ETH_NETPRO_SIERRA");
	    dev_add("eth1", DEV_IF_AD6834_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM");
	    if (!is_linux26) /* linux-2.6 BSP doesn't use zipped ADSL fw */
		token_set_y("CONFIG_ZLIB_INFLATE");
	    token_set_y("CONFIG_RG_ATM_QOS");
	    dev_add("dsl0", DEV_IF_AD68XX_ADSL, DEV_IF_NET_EXT);
	}
	
	token_set("BOARD", "AD6834");
	token_set("FIRM", "Analog Devices");

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_y("CONFIG_USB_RNDIS");
	    token_set_m("CONFIG_ADI_6843_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	{
	    ralink_rt2560_add("ra0");
	    token_set("CONFIG_RALINK_RT2560_TIMECSR", "0x40");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_DSPDRIVER_218X");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "2");
	}

	if (!is_linux26)
	    token_set_m("CONFIG_ADI_6843_MTD");
    }

    if (IS_HW("BCM96358"))
    {
	token_set_y("CONFIG_BCM963XX_COMMON");
	token_set_y("CONFIG_BCM96358");
	token_set_y("CONFIG_BCM963XX_SERIAL");
	token_set_y("CONFIG_BCM96358_BOARD");
	token_set_y("CONFIG_BCM96358_MTD");
	token_set("CONFIG_BCM963XX_BOARD_ID", "96358GW");
	token_set("CONFIG_BCM963XX_SDRAM_SIZE", "32");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "4");
	token_set("CONFIG_BCM963XX_CHIP", "6358");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM");
	    token_set_m("CONFIG_BCM963XX_ADSL");
	    token_set_m("CONFIG_BCM963XX_ATM");
	    token_set_y("CONFIG_RG_ATM_QOS");
	    dev_add("bcm_atm0", DEV_IF_BCM963XX_ADSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_WAN") && token_get("CONFIG_HW_ETH_LAN"))
	    conf_err("Ethernet device 'bcm0' cannot be WAN and LAN device");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm0", DEV_IF_BCM963XX_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    dev_add("bcm0", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);

	    if (!token_get("CONFIG_HW_SWITCH_LAN"))
	    {
		token_set_m("CONFIG_BCM963XX_ETH");
		dev_add("bcm1", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);
	    }
	}

	if (token_get("CONFIG_HW_SWITCH_LAN"))
	{
	    token_set_m("CONFIG_HW_SWITCH_BCM53XX");
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm1", DEV_IF_BCM5325E_HW_SWITCH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_BCM4318");
	    bcm43xx_add("wl0");
	}

	token_set_y("CONFIG_PCI");
	token_set_y("CONFIG_NEW_PCI");
	token_set_y("CONFIG_PCI_AUTO");

	token_set("BOARD", "BCM96358");
	token_set("FIRM", "Broadcom");
    }
    
    if (IS_HW("HG21"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_HG21"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");
	
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	token_set("FIRM", "Welltech");
	token_set("BOARD", "HG21");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F640J3","m");
	token_set("CONFIG_IXP425_FLASH_USER_PART", "0x00100000");

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
    }
    
    if (IS_HW("BAMBOO"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_BAMBOO"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_m("CONFIG_AT93CXX");
	token_set_m("CONFIG_ADM6996");
	token_set_y("CONFIG_RG_VLAN_8021Q");

	/* CSR HSS support */
	token_set_y("CONFIG_IXP425_CSR_HSS");
	token_set("CONFIG_IXP425_CODELETS", "m");
	token_set("CONFIG_IXP425_CODELET_HSS", "m");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	token_set("FIRM", "Planex");
	token_set("BOARD", "BAMBOO");
    }
	
    if (IS_HW("USR8200"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_CLOCK"))
	    token_set("CONFIG_JEEVES_RTC7301", "m");

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("FIRM", "USR");
	token_set("BOARD", "USR8200");
	token_set_y("CONFIG_ARCH_IXP425_JEEVES");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
    }
    
    if (IS_HW("GTWX5715"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add("ra0");

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	token_set_y("CONFIG_GEMTEK_COMMON");
	token_set_y("CONFIG_GEMTEK_WX5715");
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_GTWX5715"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F640J3", "m");

	token_set("FIRM", "Gemtek");
	token_set("BOARD", "GTWX5715");
    }

    if (IS_HW("TI_404"))
    {
	if (token_get("CONFIG_HW_CABLE_WAN"))
	{
	    dev_add("cbl0", DEV_IF_TI404_CBL, DEV_IF_NET_EXT);
	    dev_set_dependency("cbl0", "cable0");
	    dev_add("cable0", DEV_IF_DOCSIS, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("lan0", DEV_IF_TI404_LAN, DEV_IF_NET_INT);

	set_big_endian(1);
	token_set_y("CONFIG_TI_404_MIPS");
	token_set_y("CONFIG_TI_404_COMMON");
	token_set("CONFIG_ARCH_MACHINE", "ti404");
	token_set("RAM_HIGH_ADRS", "0x94F00000");
	token_set("RAM_LOW_ADRS", "0x94001000");
    }

    if (IS_HW("AR531X_G") || IS_HW("WRT108G") || IS_HW("AR531X_WRT55AG") ||
	IS_HW("AR531X_AG"))
    {
	char *size;
	int is_ag_board = IS_HW("AR531X_WRT55AG") || IS_HW("AR531X_AG");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    dev_add(is_ag_board ? "ae1" : "ae3", DEV_IF_AR531X_ETH,
		DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    dev_add(is_ag_board ? "ae0" : "ae2", DEV_IF_AR531X_ETH,
		DEV_IF_NET_INT);
	}

	if (IS_HW("AR531X_WRT55AG"))
	    token_set_y("CONFIG_PHY_KS8995M");
	else
	    token_set_y("CONFIG_PHY_MARVEL");

	if (token_get("CONFIG_HW_80211G_AR531X"))
	{
	    token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	    token_set_y("CONFIG_RG_WPA_WBM");
	    token_set_y("CONFIG_RG_8021X_WBM");
	    token_set_y("CONFIG_80211G_AP_ADVANCED");
	    token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
	    dev_add("vp256", DEV_IF_AR531X_WLAN_G, DEV_IF_NET_INT);
	}
	if (token_get("CONFIG_HW_80211A_AR531X"))
	    dev_add("vp0", DEV_IF_AR531X_WLAN_A, DEV_IF_NET_INT);

	token_set("ARCH", "mips");
	set_big_endian(1);
	token_set_y("CONFIG_ATHEROS_AR531X_MIPS");
	token_set("RAM_HIGH_ADRS", "0x80680000");
	token_set("RAM_LOW_ADRS", "0x80010000");
	token_set_y("CONFIG_VX_TFFS");
	token_set_y("CONFIG_RG_VX_DEFERRED_TX");

	if (IS_HW("WRT108G"))
	{
	    token_set_y("CONFIG_ARCH_WRT108G");
	    token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
	}
	else
	    token_set_y("CONFIG_ARCH_AR531X");

	if ((size = token_get_str("CONFIG_SDRAM_SIZE")) && atoi(size) <= 8)
	    token_set_y("CONFIG_SMALL_SDRAM");
    }

    if (IS_HW("CX8620XR") || IS_HW("CX8620XD"))
    {
	if (IS_HW("CX8620XR"))
	{
	    token_set("CONFIG_CX8620X_SDRAM_SIZE", "8");
	    token_set_y("CONFIG_RG_BOOTSTRAP");
	    
	    /* Flash chip */
	    token_set_m("CONFIG_CX8620X_FLASH_TE28F160C3");
	    token_set("BOARD", "CX8620XR");
	}
	else
	{
	    token_set("CONFIG_CX8620X_SDRAM_SIZE", "64");
	
	    if (token_get("CONFIG_LSP_DIST"))
		token_set_y("CONFIG_RG_BOOTSTRAP");

	    /* Flash chip */
	    if (!token_get("CONFIG_CX8620X_FLASH_TE28F320C3"))
		token_set_m("CONFIG_CX8620X_FLASH_TE28F640C3");
	    token_set("BOARD", "CX8620XD");
	}
	    
	token_set_m("CONFIG_CX8620X_SWITCH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("cnx1", DEV_IF_CX8620X_SWITCH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("cnx0", DEV_IF_CX8620X_SWITCH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("cnx1", DEV_IF_CX8620X_SWITCH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_USB_HOST_EHCI"))
	    token_set_y("CONFIG_CX8620X_EHCI");
	
	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	if (token_get("CONFIG_HW_80211G_ISL_SOFTMAC"))
	    isl_softmac_add();
	
	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add("ra0");

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	token_set("FIRM", "Conexant");
    }
    if (IS_HW("CX9451X"))
    {
	set_big_endian(0);
	token_set("ARCH", "arm");
	token_set("CONFIG_ARCH_MACHINE", "solos");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");
	token_set("CONFIG_CMDLINE", "console=ttyS0,38400");
#if 0
	/* Conexant does not use this ethernet driver. If we try to set it, it
	 * block the kernel
	 */
	token_set_y("CONFIG_MII");
	token_set_y("CONFIG_ARM_SOLOS_ETHER");
#endif
	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MODFS_CRAMFS");
	token_set("CONFIG_ZBOOT_ROM_TEXT", "0");
	token_set_y("CONFIG_CONEXANT_COMMON");
	token_set_y("CONFIG_CX9451X_COMMON");
	token_set_y("CONFIG_CPU_32");
	/* XXX Remove CONFIG_LOCK_KERNEL after resolving B36439 */
	token_set_y("CONFIG_LOCK_KERNEL");
	token_set_y("CONFIG_FPE_NWFPE");
	token_set_y("CONFIG_ALIGNMENT_TRAP");
	token_set_y("CONFIG_PREVENT_FIRMWARE_BUILD");
	token_set_y("CONFIG_ARCH_SOLOS_GALLUS");
	token_set_y("CONFIG_ARCH_SOLOS_376PIN");
	token_set_y("CONFIG_MACH_SOLOS_GALLUSBU");
	token_set_y("CONFIG_CPU_ARM1026");
	token_set_y("CONFIG_CPU_32v5");
	token_set_y("CONFIG_CPU_ABRT_EV5T");
	token_set_y("CONFIG_CPU_CACHE_VIVT");
	token_set_y("CONFIG_CPU_COPY_V4WB");
	token_set_y("CONFIG_CPU_TLB_V4WBI");
	token_set_y("CONFIG_FW_LOADER");
	token_set_y("CONFIG_STANDALONE");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	token_set_y("CONFIG_UID16");
	token_set_y("CONFIG_UNIX98_PTYS");
	token_set_y("CONFIG_SERIAL_CORE");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
	token_set_y("CONFIG_SERIAL_SOLOS");
	token_set_y("CONFIG_SERIAL_SOLOS_CONSOLE");
	token_set("CONFIG_SERIAL_SOLOS_CONSOLE_BAUD", "38400");
	token_set_y("CONFIG_KGDB_SOLOS_SERIAL"); /* KGDB serial support */
	token_set("LIBC_ARCH", "arm");
	token_set("CONFIG_SPLIT_PTLOCK_CPUS", "4");
	token_set_y("CONFIG_FLATMEM");
	token_set_y("CONFIG_FLAT_NODE_MEM_MAP");

	/* MTD */
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CMDLINE_PARTS");
	token_set_y("CONFIG_MTD_CHAR");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_1");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_2");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_4");
	token_set_y("CONFIG_MTD_CFI_I1");
	token_set_y("CONFIG_MTD_CFI_I2");
	token_set_y("CONFIG_MTD_CFI_INTELEXT");
	token_set_y("CONFIG_MTD_CFI_UTIL");
	token_set_y("CONFIG_MTD_PHYSMAP");
	token_set("CONFIG_MTD_PHYSMAP_START", "0x38000000");
	token_set("CONFIG_MTD_PHYSMAP_LEN", "0x1000000"); /* 16M */
	token_set("CONFIG_MTD_PHYSMAP_BANKWIDTH", "1");
	token_set_y("CONFIG_OBSOLETE_INTERMODULE");
	
	if (!token_get("CONFIG_LSP_DIST"))
	{
	    dev_add("lan0", DEV_IF_SOLOS_LAN, DEV_IF_NET_INT);
	    if (token_get("CONFIG_HW_ETH_WAN"))
		dev_add("dmz0", DEV_IF_SOLOS_DMZ, DEV_IF_NET_EXT);
	}
	if (token_get("CONFIG_HW_USB_STORAGE"))
	{
	    token_set_y("CONFIG_USB");
	    token_set_y("CONFIG_USB_DEVICEFS");
	    token_set_y("CONFIG_USB_EHCI_HCD");
	    token_set_y("CONFIG_USB_EHCI_ROOT_HUB_TT");
	    token_set_y("CONFIG_USB_SOLOS_HCD");
	    token_set_y("CONFIG_SOLOS_USB_HOST0");
	    token_set_y("CONFIG_SOLOS_USB_HOST1");
	}

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_CX9451X_DSP");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "2");
	}

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM");
	    token_set_y("CONFIG_RG_ATM_QOS");
	    dev_add("bunatm", DEV_IF_SOLOS_DSL, DEV_IF_NET_EXT);
	}
    }

    if (IS_HW("COMCERTO"))
    {
	if (token_get("CONFIG_COMCERTO_COMMON"))
	{
	    /* Network interfaces */
	    if (token_get("CONFIG_HW_ETH_LAN"))
		dev_add("eth0", DEV_IF_COMCERTO_ETH, DEV_IF_NET_INT);
	    if (token_get("CONFIG_HW_ETH_WAN"))
		dev_add("eth2", DEV_IF_COMCERTO_ETH, DEV_IF_NET_EXT);

	    /* VED */
	    token_set_y("CONFIG_COMCERTO_VED");
	    token_set_y("CONFIG_MII");
	    dev_add("eth1", DEV_IF_COMCERTO_ETH, DEV_IF_NET_INT);
	}

	/* Ralink WiFi card */
	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add("ra0");
	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	token_set_y("CONFIG_NETFILTER");

	token_set("FIRM", "Mindspeed");
    }

    if (IS_HW("TI_TNETWA100"))
    {
	set_big_endian(1);
	token_set_y("CONFIG_TI_404_MIPS");
	token_set_y("CONFIG_TI_404_COMMON");
	token_set("RAM_HIGH_ADRS", "0x94F00000");
	token_set("RAM_LOW_ADRS", "0x94001000");
    }

    if (IS_HW("CENTAUR"))
    {
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "4"); 
	token_set("CONFIG_SDRAM_SIZE", "32");	
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_y("CONFIG_KS8695");
	    dev_add("eth0", DEV_IF_KS8695_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_y("CONFIG_KS8695");
	    dev_add("eth1", DEV_IF_KS8695_ETH, DEV_IF_NET_INT);
	}
	
	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth2", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth2");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add("ra0");

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_ZSP400");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "4");
	}

	token_set("FIRM", "Micrel");
	token_set("BOARD", "CENTAUR");

	/* Flash chip XXX Should be module */
	token_set_y("CONFIG_KS8695_FLASH_AM29LV033C");
    }

    if (IS_HW("I386_BOCHS"))	
    {
	set_big_endian(0);
	token_set_y("CONFIG_I386_BOCHS");
	token_set("CONFIG_ARCH_MACHINE", "i386");
	if (token_get("CONFIG_RG_OS_VXWORKS"))
	{
	    dev_add("ene0", DEV_IF_NE2K_VX, DEV_IF_NET_INT);
	    dev_add("ene1", DEV_IF_NE2K_VX, DEV_IF_NET_INT);
	    dev_add("ene2", DEV_IF_NE2K_VX, DEV_IF_NET_EXT);
	    token_set_y("CONFIG_VX_KNET_SYMLINK");
	    token_set("RAM_HIGH_ADRS", "0x00008000");
	    token_set("RAM_LOW_ADRS", "0x00108000");
	}
	else
	{
	    token_set_m("CONFIG_NE2000");
	    dev_add("ne0", DEV_IF_NE2000, DEV_IF_NET_INT);
	    dev_add("ne1", DEV_IF_NE2000, DEV_IF_NET_EXT);
	    token_set_y("CONFIG_PCBOX");
	}
    }

    if (IS_HW("UML"))
    {
	token_set("ARCH", "um");

	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "64"); 
	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("eth0", DEV_IF_UML, DEV_IF_NET_EXT);
	if (token_get("CONFIG_HW_ETH_WAN2"))
	    dev_add("eth3", DEV_IF_UML, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM_NULL");
	    dev_add("atmnull0", DEV_IF_ATM_NULL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_HSS_WAN"))
	    token_set_y("CONFIG_RG_HSS");

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("eth1", DEV_IF_UML_HW_SWITCH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("eth2", DEV_IF_UML, DEV_IF_NET_INT);

	token_set_y("CONFIG_RG_UML");

	token_set_y("CONFIG_DEBUGSYM"); /* UML is always for debug ;-) */
	
	token_set("CONFIG_RG_CONSOLE_DEVICE", "console");

	token_set_y("CONFIG_RAMFS");
	token_set("CONFIG_KERNEL_STACK_ORDER", "2");
	token_set_y("CONFIG_MODE_TT");
	token_set_y("CONFIG_MODE_SKAS");
	token_set("CONFIG_NEST_LEVEL", "0");
	token_set("CONFIG_CON_ZERO_CHAN", "fd:0,fd:1");
	token_set("CONFIG_CON_CHAN", "xterm");
	token_set("CONFIG_SSL_CHAN", "pty");
	token_set("CONFIG_KERNEL_HALF_GIGS", "1");
	token_set_y("CONFIG_PT_PROXY");
	token_set_y("CONFIG_STDIO_CONSOLE");
	token_set_y("CONFIG_SSL");
	token_set_y("CONFIG_FD_CHAN");
	token_set_y("CONFIG_NULL_CHAN");
	token_set_y("CONFIG_PORT_CHAN");
	token_set_y("CONFIG_PTY_CHAN");
	token_set_y("CONFIG_TTY_CHAN");
	token_set_y("CONFIG_XTERM_CHAN");
	token_set_y("CONFIG_BLK_DEV_NBD");
	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    token_set_y("CONFIG_RG_INITFS_RAMFS");
	    token_set_y("CONFIG_RG_MAINFS_CRAMFS");
	    token_set_y("CONFIG_RG_MODFS_CRAMFS");
	    token_set_y("CONFIG_UML_X86");

	    token_set_y("CONFIG_FLATMEM");
	}
	else
	{
	    token_set_y("CONFIG_BLK_DEV_UBD");
	    token_set_y("CONFIG_SIMPLE_RAMDISK");
	    token_set("CONFIG_BLK_DEV_RAM_SIZE", "8192");
	    token_set_y("CONFIG_RG_INITFS_RAMDISK");
	}
	token_set_y("CONFIG_UML_NET");
	token_set_y("CONFIG_UML_NET_ETHERTAP");
	token_set_y("CONFIG_UML_NET_TUNTAP");
	token_set_y("CONFIG_UML_NET_SLIP");
	token_set_y("CONFIG_UML_NET_SLIRP");
	token_set_y("CONFIG_UML_NET_DAEMON");
	token_set_y("CONFIG_UML_NET_MCAST");
	token_set_y("CONFIG_DUMMY");
	token_set_y("CONFIG_TUN");
	token_set_y("CONFIG_KALLSYMS");

	token_set("CONFIG_UML_RAM_SIZE",
	    token_get("CONFIG_VALGRIND") ? "128M" : "64M");

	token_set_y("CONFIG_USERMODE");
	token_set_y("CONFIG_UID16");
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_BSD_PROCESS_ACCT");
	token_set_y("CONFIG_HOSTFS");
	token_set_y("CONFIG_HPPFS");
	token_set_y("CONFIG_MCONSOLE");
	token_set_y("CONFIG_MAGIC_SYSRQ");
	if (!token_get("CONFIG_RG_OS_LINUX_26"))
	    token_set_y("CONFIG_PROC_MM");

	token_set_y("CONFIG_PACKET_MMAP");
	token_set_y("CONFIG_QUOTA");
	token_set_y("CONFIG_AUTOFS_FS");
	token_set_y("CONFIG_AUTOFS4_FS");
	token_set_y("CONFIG_REISERFS_FS");

	token_set_y("CONFIG_MTD_BLKMTD");
	token_set_y("CONFIG_ZLIB_INFLATE");
	token_set_y("CONFIG_ZLIB_DEFLATE");

	token_set_y("CONFIG_PT_PROXY");
	token_set_y("CONFIG_RG_THREADS");
	token_set("CONFIG_RG_KERNEL_COMP_METHOD", "gzip");
	token_set("CONFIG_RG_CRAMFS_COMP_METHOD", "lzma");
	token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "8");
    }
    
    if (IS_HW("JPKG"))
    {
	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add("ra0");

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add("ra0");

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	    bcm43xx_add(NULL);
    }
    
    if (token_get("MODULE_RG_IPV6"))
	dev_add("sit0", DEV_IF_IPV6_OVER_IPV4_TUN, DEV_IF_NET_INT);
}

