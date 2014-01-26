/****************************************************************************
 *
 * rg/pkg/build/dist_config.c
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


#include <string.h>
#include "config_opt.h"
#include "create_config.h"
#include <sys/stat.h>
#include <unistd.h>

static int stat_lic_file(char *path)
{
    struct stat s;
    int ret = stat(path, &s);

    printf("Searching for license file in %s: %sfound\n", path,
	ret ? "not " : "");
    return ret;
}

static void set_jnet_server_configs(void)
{
    token_set_y("CONFIG_RG_HTTPS");
    token_set_y("CONFIG_RG_SSL");
    token_set_y("CONFIG_RG_OPENSSL_MD5");
    token_set_y("CONFIG_RG_XML");
    token_set_y("CONFIG_RG_DSLHOME");
    token_set_y("CONFIG_RG_WGET");
    token_set_y("CONFIG_LOCAL_WBM_LIB");
    token_set_y("CONFIG_RG_SESSION_LIBDB");
    token_set_y("HAVE_MYSQL");
    token_set_y("CONFIG_RG_JNET_SERVER");
    token_set("CONFIG_RG_JPKG_DIST", "JPKG_LOCAL_I386");
    token_set("TARGET_MACHINE", "local_i386");
    token_set_y("CONFIG_RG_LANG");
    token_set_y("CONFIG_RG_GNUDIP");
    token_set_y("CONFIG_GLIBC");
    token_set_y("CONFIG_RG_LIBIMAGE_DIM");
}

char *set_dist_license(void)
{
#define DEFAULT_LIC_DIR "pkg/license/licenses/"
#define DEFAULT_LIC_FILE "license.lic"
    char *lic = NULL;

    if (IS_DIST("RTA770W"))
	lic = DEFAULT_LIC_DIR "belkin.lic";
    else if (!stat_lic_file(DEFAULT_LIC_DIR DEFAULT_LIC_FILE))
	lic = DEFAULT_LIC_DIR DEFAULT_LIC_FILE;
    else if (!stat_lic_file(DEFAULT_LIC_FILE))
	lic = DEFAULT_LIC_FILE;

    if (lic)
	token_set("LIC", lic);
    return lic;
}

static void small_flash_default_dist(void)
{
    enable_module("MODULE_RG_FOUNDATION");
    enable_module("MODULE_RG_UPNP");
    enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
    enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
    enable_module("MODULE_RG_ADVANCED_ROUTING");
    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
    enable_module("MODULE_RG_VLAN");
    enable_module("MODULE_RG_PPP");
    enable_module("MODULE_RG_PPTP");
    enable_module("MODULE_RG_L2TP");
    enable_module("MODULE_RG_QOS");
    enable_module("MODULE_RG_ROUTE_MULTIWAN");
    enable_module("MODULE_RG_MAIL_FILTER");
    enable_module("MODULE_RG_URL_FILTERING");
    enable_module("MODULE_RG_DSLHOME");
    enable_module("MODULE_RG_TR_098");
    enable_module("MODULE_RG_TR_064");
    enable_module("MODULE_RG_DSL");
    enable_module("MODULE_RG_SSL_VPN");

    token_set_y("CONFIG_RG_WPA");
    token_set_y("CONFIG_RG_WEP");

    token_set_y("CONFIG_ULIBC_SHARED");
    token_set("CONFIG_RG_ADV_SERVICES_LEVEL", "1");
    token_set_y("CONFIG_RG_SSL_VPN_SMALL_FLASH");
}

static void set_jpkg_dist_configs(char *jpkg_dist)
{
    int is_src = !strcmp(jpkg_dist, "JPKG_SRC");

    if (is_src || !strcmp(jpkg_dist, "JPKG_UML"))
    {
	jpkg_dist_add("UML");
	jpkg_dist_add("UML_GLIBC");
	jpkg_dist_add("UML_26");
	jpkg_dist_add("RGLOADER_UML");

	enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	enable_module("MODULE_RG_PBX");
	token_set_y("CONFIG_HW_DSP");
	token_set_y("CONFIG_RG_VOIP_HW_EMULATION");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_ARMV5B"))
    {
	jpkg_dist_add("MONTEJADE");
	jpkg_dist_add("MONTEJADE_ATM");
	jpkg_dist_add("IXDP425");
	jpkg_dist_add("COYOTE");
	jpkg_dist_add("JIWIS8XX");
	jpkg_dist_add("RGLOADER_MONTEJADE");
	jpkg_dist_add("RGLOADER_COYOTE");

	enable_module("CONFIG_AGN100");
	enable_module("MODULE_RG_VOIP_OSIP");
        enable_module("MODULE_RG_VOIP_RV_H323");
        enable_module("MODULE_RG_VOIP_RV_MGCP");
        enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_ASTERISK_H323");
	enable_module("MODULE_RG_ATA");
        enable_module("CONFIG_HW_80211G_RALINK_RT2561");

	token_set_y("CONFIG_UCLIBCXX");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_LX4189"))
    {
	jpkg_dist_add("AD6834");
        enable_module("CONFIG_HW_80211G_RALINK_RT2561");

	enable_module("MODULE_RG_VOIP_OSIP");
        enable_module("MODULE_RG_VOIP_RV_H323");
        enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_ARM_920T_LE"))
    {
	jpkg_dist_add("CENTROID");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
        enable_module("CONFIG_HW_80211G_RALINK_RT2561");
	/* XXX MODULE_RG_ADVANCED_MANAGEMENT Needs DYN_LINK 
	 * and Dyn link causes a crash on CENTROID.
	 * B30410
	 */
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_ARMV5L"))
    {
	/* Adding centaur will causee cpu version conflict. See B29668 
	jpkg_dist_add("CENTAUR_VGW");
	jpkg_dist_add("CENTAUR");
	jpkg_dist_add("RGLOADER_CENTAUR");
	*/

	jpkg_dist_add("CX8620XR");
	jpkg_dist_add("CX8620XD_SOHO");
	jpkg_dist_add("RGLOADER_CX8620XD");

	jpkg_dist_add("SOLOS_LSP");
	jpkg_dist_add("SOLOS");

        enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	/* Needed for voip compilation */
	token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "2");

        enable_module("CONFIG_HW_80211G_RALINK_RT2561");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_ARMV4L"))
    {
	jpkg_dist_add("MALINDI");
	jpkg_dist_add("MALINDI2");
	
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");
        enable_module("CONFIG_HW_80211G_RALINK_RT2560");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_PPC"))
    {
	/* XXX Restore these dists in B36583 
	jpkg_dist_add("EP8248_26");
	token_set_y("CONFIG_HW_BUTTONS");
	*/
	jpkg_dist_add("MPC8272ADS");
	jpkg_dist_add("MPC8349ITX");

        enable_module("CONFIG_HW_80211G_RALINK_RT2561");
        enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");
        enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	enable_module("MODULE_RG_ATA");
	enable_module("MODULE_RG_PBX");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_SB1250"))
    {
	jpkg_dist_add("BCM91125E");
	jpkg_dist_add("BCM_SB1125");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_TWINPASS"))
    {
	jpkg_dist_add("TWINPASS");
	jpkg_dist_add("ULTRALINE3_TWINPASS");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_MIPSEB"))
    {
	jpkg_dist_add("BCM96358");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_IPSEC");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_BCM9634X"))
    {
	jpkg_dist_add("ASUS6020VI");
	jpkg_dist_add("GTWX5803");
	jpkg_dist_add("WADB100G");
	jpkg_dist_add("WADB102GB");
	jpkg_dist_add("RGLOADER_GTWX5803");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_IPSEC");
    }
    if (is_src || !strcmp(jpkg_dist, "JPKG_LOCAL_I386"))
    {
	/* Can't use JNET_SERVER in JPKG_SRC because it turns on
	 * CONFIG_RG_USE_LOCAL_TOOLCHAIN, which we don't want.
	 * Remove this when B32553 is fixed.
	 */
	if (is_src)
	    set_jnet_server_configs();
	else
	    jpkg_dist_add("JNET_SERVER");
    }
    if (is_src)
    {
	token_set_y("CONFIG_RG_TCPDUMP");
	token_set_y("CONFIG_RG_LIBPCAP");
	token_set_y("CONFIG_RG_IPROUTE2_UTILS");
        token_set_y("CONFIG_RG_JAVA");
        token_set_y("CONFIG_RG_JTA");
        token_set_y("CONFIG_RG_PROPER_JAVA_RDP");
        token_set_y("CONFIG_RG_JVFTP");
        token_set_y("CONFIG_RG_JCIFS");
        token_set_y("CONFIG_RG_SMB_EXPLORER");
        token_set_y("CONFIG_RG_TIGHT_VNC");
	token_set_y("GLIBC_IN_TOOLCHAIN");	
	token_set_y("CONFIG_RG_GDBSERVER");
    }
    else
    {
        token_set_y("CONFIG_RG_JPKG_BIN");
    }

    /* Common additional features: */
    token_set_y("CONFIG_RG_JPKG");
    if (strcmp(jpkg_dist, "JPKG_LOCAL_I386"))
    {
	/* These shouldn't be turbed on in binary local jpkg */
	token_set_y("CONFIG_RG_DOC_ENABLED");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	token_set_y("CONFIG_RG_TOOLS");
	enable_module("MODULE_RG_ZERO_CONFIGURATION_NETWORKING");
    }
    enable_module("MODULE_RG_DSLHOME");
}

void distribution_features()
{
    if (!dist)
	conf_err("ERROR: DIST is not defined\n");

    /* MIPS */
    if (IS_DIST("ADM5120_LSP"))
    {
	hw = "ADM5120P";
	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_FRAME_POINTER");
    }
    else if (IS_DIST("BCM91125E") || IS_DIST("BCM_SB1125"))
    {
    	if (IS_DIST("BCM91125E"))
	    hw = "BCM91125E";
	else if (IS_DIST("BCM_SB1125"))
	    hw = "COLORADO";
	os = "LINUX_26";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_SB1250");

	token_set_y("MODULE_RG_FOUNDATION");
	token_set_y("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("MODULE_RG_ADVANCED_ROUTING");
	token_set_y("MODULE_RG_FIREWALL_AND_SECURITY");
	token_set_y("MODULE_RG_SNMP");
	token_set_y("MODULE_RG_UPNP");
	token_set_y("MODULE_RG_IPSEC");
	token_set_y("MODULE_RG_URL_FILTERING");
	token_set_y("MODULE_RG_QOS");
	token_set_y("MODULE_RG_MAIL_FILTER");
	enable_module("MODULE_RG_SSL_VPN");
	token_set_y("MODULE_RG_ADVANCED_OFFICE_SERVERS");
	token_set_y("MODULE_RG_VLAN");
	token_set_y("MODULE_RG_PPP");
	token_set_y("MODULE_RG_PPTP");
	token_set_y("MODULE_RG_L2TP");
	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	token_set_y("CONFIG_RG_SMB");
	token_set_y("MODULE_RG_IPV6");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");

	enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	enable_module("MODULE_RG_PBX");

	token_set_y("CONFIG_DYN_LINK");

	/* Only Ethernet HW for now */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	if (IS_DIST("BCM_SB1125"))
	{
	    enable_module("MODULE_RG_BLUETOOTH");
	    token_set_y("MODULE_RG_FILESERVER");
	    token_set_y("MODULE_RG_PRINTSERVER");
	}

	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");
	token_set_y("CONFIG_RG_MODFS_CRAMFS");
    }

    else if (IS_DIST("RGLOADER_ADM5120"))
    {
	hw = "ADM5120P";
	os = "LINUX_24";
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_FRAME_POINTER");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("ADM5120_ATA"))
    {
	hw = "ADM5120P";
	os = "LINUX_24";
	
	/* OpenRG Feature set */
	token_set_y("CONFIG_RG_FOUNDATION_CORE");
	token_set_y("CONFIG_RG_CHECK_BAD_REBOOTS");
	/* From MODULE_RG_UPNP take only this */
	token_set_y("CONFIG_AUTO_LEARN_DNS");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VOIP_RV_SIP");
    	token_set_y("CONFIG_RG_EVENT_LOGGING");
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */
	token_set_y("CONFIG_RG_UCD_SNMP"); /* SNMP v1/2 only */
	enable_module("MODULE_RG_ATA");

	/* OpenRG HW support */
	enable_module("CONFIG_HW_DSP");
	token_set_y("CONFIG_HW_ETH_WAN");

	token_set_y("CONFIG_DEF_WAN_ALIAS_IP");
    }
    else if (IS_DIST("ADM5120_VGW") || IS_DIST("ADM5120_VGW_OSIP"))
    {
	hw = "ADM5120P";
	os = "LINUX_24";
	
	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	/* XXX Workaround for B31610, remove when bug is resovled */
#if 0
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
#endif
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_UPNP");
        enable_module("MODULE_RG_VLAN");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_DSP");

	token_set_y("CONFIG_DYN_LINK");

	/* VoIP */
	enable_module("MODULE_RG_ATA");
	if (IS_DIST("ADM5120_VGW_OSIP"))
	    enable_module("MODULE_RG_VOIP_OSIP");
	else
	    enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
    }
    else if (IS_DIST("INCAIP_LSP"))
    {
	hw = "INCAIP_LSP";
	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_FRAME_POINTER");
	token_set_y("CONFIG_VINETIC_TAPIDEMO");
    }
    else if (IS_DIST("RGLOADER_INCAIP"))
    {
	hw = "INCAIP";
	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("INCAIP_IPPHONE"))
    {
	hw = "INCAIP";

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	enable_module("CONFIG_HW_DSP");
	token_set_y("CONFIG_HW_KEYPAD");
	token_set_y("CONFIG_HW_LEDS");

	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_DEF_WAN_ALIAS_IP");
    }
    else if (IS_DIST("RGLOADER_FLEXTRONICS"))
    {
	hw = "FLEXTRONICS";
	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("FLEXTRONICS_IPPHONE"))
    {
	hw = "FLEXTRONICS";

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	enable_module("CONFIG_HW_DSP");
	token_set_y("CONFIG_HW_KEYPAD");
	token_set_y("CONFIG_HW_LEDS");

	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_DEF_WAN_ALIAS_IP");
    }
    else if (IS_DIST("INCAIP_ATA") || IS_DIST("INCAIP_ATA_OSIP"))
    {
	hw = "ALLTEK";

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
    	token_set_y("CONFIG_RG_EVENT_LOGGING");

	/* OpenRG HW support */
	enable_module("CONFIG_HW_DSP");
	token_set_y("CONFIG_HW_ETH_WAN");

	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_DEF_WAN_ALIAS_IP");

	/* VoIP */
	enable_module("MODULE_RG_ATA");
	if (IS_DIST("INCAIP_ATA_OSIP"))
	    enable_module("MODULE_RG_VOIP_OSIP");
	else
	{
	    enable_module("MODULE_RG_VOIP_RV_SIP");
	    enable_module("MODULE_RG_VOIP_RV_MGCP");
	    enable_module("MODULE_RG_VOIP_RV_H323");
	}
    }
    else if (IS_DIST("RGLOADER_ALLTEK") ||
	IS_DIST("RGLOADER_ALLTEK_FULLSOURCE"))
    {
	hw = "ALLTEK";

	token_set_y("CONFIG_RG_RGLOADER");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN");

    }
    else if (IS_DIST("INCAIP_VGW") || IS_DIST("INCAIP_VGW_OSIP"))
    {
	hw = "ALLTEK_VLAN";

    	token_set_y("CONFIG_RG_SMB");

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_PPP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_L2TP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
        enable_module("MODULE_RG_IPV6");
        enable_module("MODULE_RG_URL_FILTERING");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_DYN_LINK");

	/* VoIP */
	enable_module("MODULE_RG_ATA");
	if (IS_DIST("INCAIP_VGW_OSIP"))
	    enable_module("MODULE_RG_VOIP_OSIP");
	else
	{
	    enable_module("MODULE_RG_VOIP_RV_SIP");
	    enable_module("MODULE_RG_VOIP_RV_H323");
	    enable_module("MODULE_RG_VOIP_RV_MGCP");
	}
    }
    else if (IS_DIST("INCAIP_FULLSOURCE"))
    {
	hw = "ALLTEK_VLAN";

    	token_set_y("CONFIG_RG_SMB");

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_PPP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_L2TP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
        enable_module("MODULE_RG_IPV6");
        enable_module("MODULE_RG_URL_FILTERING");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_DYN_LINK");
    }
    else if (IS_DIST("BCM94702"))
    {
	hw = "BCM94702";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("BCM94704"))
    {
	hw = "BCM94704";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_SNMP");
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */
	token_set_y("CONFIG_RG_ENTFY");	/* Email notification */

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", "eth0", NULL);
    }
    else if (IS_DIST("USI_BCM94712"))
    {
	hw = "BCM94712";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
        enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0.0", "eth0", NULL);

	token_set_y("CONFIG_DYN_LINK");
    }
    else if (IS_DIST("SRI_USI_BCM94712"))
    {
	hw = "BCM94712";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_URL_FILTERING");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6003");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_SNMP");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_80211G_BCM43XX");
	
	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0.0", "eth0", NULL);

	/* The Broadcom nas application is dynamically linked */
	token_set_y("CONFIG_DYN_LINK");

	/* Wireless GUI options */
	/* Do NOT show Radius icon in advanced */
	token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
    }
    else if (IS_DIST("RTA770W"))
    {
	hw = "RTA770W";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	/* broadcom nas (wpa application) needs ulibc.so so we need to compile
	 * openrg dynamically */
	token_set_y("CONFIG_DYN_LINK");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_UPNP");
	token_set_y("CONFIG_RG_IGD_XBOX");
	enable_module("MODULE_RG_DSL");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	enable_module("CONFIG_HW_USB_RNDIS");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", "usb0", "wl0", NULL);

	token_set_y("CONFIG_GUI_BELKIN");
	token_set("RG_PROD_STR", "Prodigy infinitum");
	token_set_y("CONFIG_RG_DSL_CH");
	token_set_y("CONFIG_RG_PPP_ON_DEMAND_AS_DEFAULT");
	/* Belkin's requirement - 3 hours of idle time */
	token_set("CONFIG_RG_PPP_ON_DEMAND_DEFAULT_MAX_IDLE_TIME", "10800");
	/* from include/enums.h PPP_COMP_ALLOW is 1 */
	token_set("CONFIG_RG_PPP_DEFAULT_BSD_COMPRESSION", "1");
	/* from include/enums.h PPP_COMP_ALLOW is 1 */
	token_set("CONFIG_RG_PPP_DEFAULT_DEFLATE_COMPRESSION", "1");
	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
	/* For autotest and development purposes, Spanish is the default
	 * language allowing an override */
	if (!token_get_str("CONFIG_RG_DIST_LANG"))
	    token_set("CONFIG_RG_DIST_LANG", "spanish_belkin");
	token_set_y("CONFIG_RG_CFG_SERVER");
	token_set_y("CONFIG_RG_OSS_RMT");
	token_set_y("CONFIG_RG_RMT_MNG");
	token_set_y("CONFIG_RG_NON_ROUTABLE_LAN_DEVICE_IP");
    }
    else if (IS_DIST("RTA770W_EVAL"))
    {
	hw = "RTA770W";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_DSL");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	enable_module("CONFIG_HW_USB_RNDIS");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", "usb0", "wl0", NULL);

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
	/* broadcom nas (wpa application) needs ulibc.so so we need to compile
	 * openrg dynamically */
	token_set_y("CONFIG_DYN_LINK");
    }
    else if (IS_DIST("RGLOADER_RTA770W"))
    {
	hw = "RTA770W";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_BCM963XX_BOOTSTRAP");
	token_set_m("CONFIG_RG_KRGLDR");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("GTWX5803"))
    {
	hw = "GTWX5803";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_BCM9634X");
	
	small_flash_default_dist();

	token_set_m("CONFIG_RG_MTD");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_SWITCH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	enable_module("CONFIG_HW_USB_RNDIS");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", NULL);
	dev_add_to_bridge_if_opt("br0", "wl0", "CONFIG_HW_80211G_BCM43XX");
	dev_add_to_bridge_if_opt("br0", "usb0", "CONFIG_HW_USB_RNDIS");
    }
    else if (IS_DIST("RGLOADER_GTWX5803"))
    {
	hw = "GTWX5803";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_RG_TELNETS");
	token_set_y("CONFIG_BCM963XX_BOOTSTRAP");
	token_set_m("CONFIG_RG_KRGLDR");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
	token_set_m("CONFIG_RG_MTD");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("WRT54G"))
    {
	hw = "WRT54G";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
        enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_SNMP");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	token_set_m("CONFIG_HW_BUTTONS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0.2", "eth0", NULL);

	token_set_y("CONFIG_ARCH_BCM947_CYBERTAN");
	token_set_y("CONFIG_RG_BCM947_NVRAM_CONVERT");
	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_GUI_LINKSYS");
    }
    else if (IS_DIST("SOLOS_LSP") || IS_DIST("SOLOS"))
    {
	hw = "CX9451X";
	os = "LINUX_26";
	token_set_y("CONFIG_ARCH_SOLOS");
	token_set_y("CONFIG_DEBUG_USER");
	token_set_y("CONFIG_KALLSYMS");
	token_set_y("CONFIG_SERIAL_CORE");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
	token_set_y("CONFIG_SERIAL_SOLOS_CONSOLE");
	token_set_y("CONFIG_UNIX98_PTYS");

	if (IS_DIST("SOLOS_LSP"))
	{
	    token_set_y("CONFIG_RG_KGDB");
	    token_set_y("CONFIG_LSP_DIST");
	}
	else
	{
	    enable_module("MODULE_RG_FOUNDATION");
	    enable_module("MODULE_RG_ADVANCED_ROUTING");
	    enable_module("MODULE_RG_VLAN");
	    enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	    enable_module("MODULE_RG_DSL");
	    enable_module("MODULE_RG_PPP");
	    enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	    enable_module("MODULE_RG_URL_FILTERING");
	    enable_module("MODULE_RG_IPSEC");
            token_set_y("CONFIG_IPSEC_USE_SOLOS_CRYPTO");
	    enable_module("MODULE_RG_PPTP");
	    enable_module("MODULE_RG_L2TP");
	    enable_module("MODULE_RG_FILESERVER");
	    enable_module("MODULE_RG_PRINTSERVER");
	    enable_module("MODULE_RG_UPNP");

	    /* Voip */
	    enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	    enable_module("MODULE_RG_PBX");
	    
	    enable_module("MODULE_RG_SNMP");
	    enable_module("MODULE_RG_IPV6");
	    enable_module("MODULE_RG_DSLHOME");
	    enable_module("MODULE_RG_TR_064");
	    enable_module("MODULE_RG_QOS");

	    /* HW configuration */
	    enable_module("CONFIG_HW_DSP");
	    token_set_y("CONFIG_HW_USB_STORAGE");
	    token_set_y("CONFIG_HW_DSL_WAN");
	    token_set_y("CONFIG_HW_ETH_WAN");
	    token_set_y("CONFIG_HW_ETH_LAN");

	    dev_add_bridge("br0", DEV_IF_NET_INT, "lan0", NULL);
	}

	token_set_y("CONFIG_DYN_LINK");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5L");
	/* XXX remove after resolving B36422 */
	token_set("CONFIG_RG_EXTERNAL_TOOLS_PATH",
	    "/usr/local/virata/tools_v10.1c/redhat-9-x86/bin/");
    }
    else if (IS_DIST("CX82100_SCHMID"))
    {
	hw = "CX82100";
	os = "LINUX_22";
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_USB_RNDIS");

	token_set_y("CONFIG_RG_TODC");
    }
    else if (IS_DIST("X86_FRG_TMT")) /* x86 */
    {
	hw = "PCBOX_EEP_EEP";
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_VLAN");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_GLIBC");
    }
    else if (IS_DIST("RGLOADER_X86_TMT"))
    {
	hw = "PCBOX_EEP_EEP";

	token_set_y("CONFIG_RG_RGLOADER");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("ALLWELL_RTL_RTL"))
    {
	hw = "ALLWELL_RTL_RTL";

	token_set_y("PACKAGE_OPENRG_VPN_GATEWAY");
	token_set_y("MODULE_RG_VOIP_ASTERISK_SIP");
	token_set_y("MODULE_RG_VOIP_ASTERISK_H323");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_I386");
	
	/* XXX Workaround for B25309, remove when bug is resovled */
	token_set_y("CONFIG_RG_DEV");
    }
    else if (IS_DIST("ALLWELL_RTL_RTL_SSI"))
    {
	hw = "ALLWELL_RTL_RTL";
	token_set_y("PACKAGE_OPENRG_VPN_GATEWAY");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_RG_SSI_PAGES");
    }
    else if (IS_DIST("ALLWELL_RTL_EEP"))
    {
	hw = "ALLWELL_RTL_EEP";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

    }
    else if (IS_DIST("ALLWELL_RTL_RTL_ALADDIN"))
    {
	hw = "ALLWELL_RTL_RTL";

	token_set_y("PACKAGE_OPENRG_VPN_GATEWAY");
	enable_module("MODULE_RG_MAIL_FILTER");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("ALLWELL_RTL_RTL_WELLTECH"))
    {
	hw = "ALLWELL_RTL_RTL";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_GUI_WELLTECH");
    }
    /* XScale IXP425 based boards */
    else if (IS_DIST("COYOTE_WIRELESS_80211B")) 
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211B_PRISM2");
        enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_IDE");

	enable_module("MODULE_RG_ATA");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);

	token_set_y("CONFIG_IXP425_COMMON_RG");
    }
    else if (IS_DIST("COYOTE_WIRELESS_ISL") ||
	IS_DIST("COYOTE_WIRELESS_SOFTMAC") ||
	IS_DIST("COYOTE_WIRELESS_RALINK"))
    {
	char *wireless_dev = NULL;

	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	enable_module("MODULE_RG_ATA");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
	if (IS_DIST("COYOTE_WIRELESS_ISL"))
	{
	    enable_module("CONFIG_HW_80211G_ISL38XX");
	    wireless_dev = "eth0";
	}
	if (IS_DIST("COYOTE_WIRELESS_SOFTMAC"))
	{
	    enable_module("CONFIG_HW_80211G_ISL_SOFTMAC");
	    wireless_dev = "eth0";
	}
	if (IS_DIST("COYOTE_WIRELESS_RALINK"))
	{
	    enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	    wireless_dev = "ra0";
	}
        enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_IDE");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", wireless_dev,
	    NULL);

	token_set_y("CONFIG_IXP425_COMMON_RG");
    }
    else if (IS_DIST("RGLOADER_USR8200"))
    {
	hw = "USR8200";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("USR8200_EVAL"))
    {
	hw = "USR8200";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_FIREWIRE");
	token_set_y("CONFIG_HW_FIREWIRE_STORAGE");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_RG_DATE");
    }
    else if (IS_DIST("USR8200") || IS_DIST("USR8200_ALADDIN"))
    {
	hw = "USR8200";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_UPNP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");
	enable_module("MODULE_RG_PRINTSERVER");
	token_set_y("CONFIG_RG_SURFCONTROL");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_FIREWIRE");
	token_set_y("CONFIG_HW_FIREWIRE_STORAGE");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");
        token_set_y("CONFIG_HW_LEDS");
        token_set_y("CONFIG_HW_CLOCK");

	token_set_y("CONFIG_RG_FW_ICSA");
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_RG_DATE");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	token_set_y("CONFIG_STOP_ON_INIT_FAIL");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6002");
	token_set_y("CONFIG_RG_DHCPR");

	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_SSL_VPN");
	
	/* Include automatic daylight saving time calculation */
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");

	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_MAIL_FILTER");
    }
    else if (IS_DIST("USR8200_TUTORIAL"))
    {
	hw = "USR8200";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_FIREWIRE");
	token_set_y("CONFIG_HW_FIREWIRE_STORAGE");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");
        token_set_y("CONFIG_HW_LEDS");
        token_set_y("CONFIG_HW_CLOCK");

	token_set_y("CONFIG_RG_DATE");
	token_set_y("CONFIG_RG_TUTORIAL");
	token_set_y("CONFIG_STOP_ON_INIT_FAIL");
	token_set_y("CONFIG_IXP425_COMMON_RG");

	/* Include automatic daylight saving time calculation */
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");
    }
    else if (IS_DIST("BAMBOO") || IS_DIST("BAMBOO_ALADDIN"))
    {
	hw = "BAMBOO"; 

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("CONFIG_RG_PROXY_ARP");
	token_set_y("CONFIG_RG_ENTFY");	/* Email notification */
	token_set_y("CONFIG_RG_UCD_SNMP"); /* SNMP v1/v2 */
	token_set_y("CONFIG_RG_8021X_TLS");
	token_set_y("CONFIG_RG_8021X_RADIUS");
	enable_module("MODULE_RG_ATA");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211B_PRISM2");
        enable_module("CONFIG_HW_DSP");
	token_set_m("CONFIG_HW_USB_HOST_EHCI");
	token_set_m("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");
        token_set_y("CONFIG_HW_CAMERA_USB_OV511");
        token_set_m("CONFIG_HW_PCMCIA_CARDBUS");
	token_set_m("CONFIG_HW_BUTTONS");
	token_set_m("CONFIG_HW_LEDS");

	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_IXP425_COMMON_RG");

	/* PPTP, PPPoE */
        token_set("CONFIG_RG_PPTP_ECHO_INTERVAL", "20");
        token_set("CONFIG_RG_PPTP_ECHO_FAILURE", "3");
        token_set("CONFIG_PPPOE_MAX_RETRANSMIT_TIMEOUT", "64");

	/* DSR support */
	token_set_y("CONFIG_IXP425_DSR");
	token_set_y("CONFIG_RG_OLD_XSCALE_TOOLCHAIN");

	if (IS_DIST("BAMBOO_ALADDIN"))
	{
	    enable_module("MODULE_RG_MAIL_FILTER");
	    enable_module("MODULE_RG_PRINTSERVER");
	    token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	}
    }
    else if (IS_DIST("RGLOADER_CENTROID"))
    {
	hw = "CENTROID";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_SL2312_COMMON");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("CENTROID"))
    {
	hw = "CENTROID";
	
	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_SSL_VPN");

	enable_module("MODULE_RG_IPSEC");
	
	/* XXX this is a workaround until B22301 is resolved */
	token_set_y("CONFIG_RG_THREADS");

	token_set_m("CONFIG_RG_MTD");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	token_set_y("CONFIG_HW_USB_STORAGE");
	
	token_set_y("CONFIG_SL2312_COMMON_RG");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 

	dev_add_bridge("br0", DEV_IF_NET_INT, "sl0", NULL);
	dev_add_to_bridge_if_opt("br0", "ra0", 
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");
    }
    else if (IS_DIST("MATECUMBE"))
    {
	hw = dist;

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_RG_INITFS_RAMDISK");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("IXDP425"))
    {
	hw = "IXDP425";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
	token_set_y("CONFIG_RG_SMB");
	token_set_y("CONFIG_ARM_24_FAST_MODULES");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_SSL_VPN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", NULL);
	dev_add_to_bridge_if_opt("br0", "usb0", "CONFIG_HW_USB_RNDIS");
    }
    else if (IS_DIST("IXDP425_WIRELESS"))
    {
	hw = "IXDP425";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }
    else if (IS_DIST("RGLOADER_KINGSCANYON"))
    {
	hw = "KINGSCANYON";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_KINGSCANYON_LSP"))
    {
	hw = "KINGSCANYON";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("KINGSCANYON_WIRELESS") || IS_DIST("KINGSCANYON"))
    {
	hw = "KINGSCANYON";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	if (IS_DIST("KINGSCANYON_WIRELESS"))
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");
	if (IS_DIST("KINGSCANYON_WIRELESS"))
	    enable_module("CONFIG_HW_80211G_ISL38XX");

	if (IS_DIST("KINGSCANYON_WIRELESS"))
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
	else
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("KINGSCANYON_LSP"))
    {
	hw = "KINGSCANYON";

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
    }
    else if (IS_DIST("RGLOADER_ROCKAWAYBEACH"))
    {
	hw = "ROCKAWAYBEACH";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("ROCKAWAYBEACH") || IS_DIST("ROCKAWAYBEACH_WIRELESS"))
    {
	hw = "ROCKAWAYBEACH";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	if (IS_DIST("ROCKAWAYBEACH_WIRELESS"))
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");
	if (IS_DIST("ROCKAWAYBEACH_WIRELESS"))
	    enable_module("CONFIG_HW_80211G_ISL38XX");

	if (IS_DIST("ROCKAWAYBEACH_WIRELESS"))
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
	else
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("IXDP425_NETKLASS"))
    {
	hw = "IXDP425";

	enable_module("MODULE_RG_FOUNDATION");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_UPNP");
	token_set_y("CONFIG_RG_ENTFY");	/* Email notification */
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */
	token_set_y("CONFIG_RG_8021X");
	token_set_y("CONFIG_RG_8021X_MD5");
	token_set_y("CONFIG_RG_PROXY_ARP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_IXP425_CSR_USB");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set("CONFIG_RAMDISK_SIZE", "8192");
	token_set_y("CONFIG_GLIBC");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	
	/* VPN HW Acceleration */
	token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");

    }
    else if (IS_DIST("WAV54G"))
    {
	hw = "WAV54G";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_L2TP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_ISL38XX");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_m("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_GLIBC");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "eth0", NULL);
    }
    else if (IS_DIST("CX8620XR_LSP") || IS_DIST("CX8620XD_LSP"))
    {
	if (IS_DIST("CX8620XR_LSP"))
	    hw = "CX8620XR";
	else
	    hw = "CX8620XD";

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_IPTABLES");
	token_set_m("CONFIG_BRIDGE");
	token_set_y("CONFIG_BRIDGE_UTILS");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	token_set_m("CONFIG_ISL_SOFTMAC");
	token_set_y("CONFIG_RG_NETKIT");
	
	token_set_y("CONFIG_CX8620X_COMMON");

	token_set_y("CONFIG_HW_USB_HOST_EHCI");
    	token_set_y("CONFIG_HW_USB_STORAGE");
    }
    else if (IS_DIST("CX8620XR"))
    {
	hw = "CX8620XR";
	
	token_set_y("CONFIG_CX8620X_COMMON");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
  	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_SSL_VPN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	token_set("CONFIG_RG_ADV_SERVICES_LEVEL", "1");
	
	dev_add_bridge("br0", DEV_IF_NET_INT, "cnx0", NULL);
	dev_add_to_bridge_if_opt("br0", "ra0", 
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5L");
    }
    else if (IS_DIST("CX8620XD_FILESERVER"))
    {
	hw = "CX8620XD";

	token_set_y("CONFIG_CX8620X_COMMON");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
    	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	
	dev_add_bridge("br0", DEV_IF_NET_INT, "cnx0", NULL);
	dev_add_to_bridge_if_opt("br0", "ra0", 
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");
    }
    else if (IS_DIST("CX8620XD_SOHO"))
    {
	hw = "CX8620XD";

	token_set_y("CONFIG_CX8620X_COMMON");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");

	token_set_y("PACKAGE_OPENRG_SOHO_GATEWAY");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_TR_098");
	enable_module("MODULE_RG_TR_064");
	enable_module("MODULE_RG_MAIL_FILTER");
	enable_module("MODULE_RG_JVM");
	enable_module("MODULE_RG_SSL_VPN");

	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6002");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
    	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5L");
	token_set("CONFIG_RG_ADV_SERVICES_LEVEL", "1");

	dev_add_bridge("br0", DEV_IF_NET_INT, "cnx0", NULL);
	dev_add_to_bridge_if_opt("br0", "ra0",
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");
    }
    else if (IS_DIST("MALINDI") || IS_DIST("MALINDI2"))
    {
	hw = "COMCERTO";
	os = "LINUX_26";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV4L");
	
	/* Comcerto chipset */
	token_set_y("CONFIG_COMCERTO_COMMON");

	/* Board */
	if (IS_DIST("MALINDI"))
	    token_set_y("CONFIG_COMCERTO_MALINDI");
	else if (IS_DIST("MALINDI2"))
	    token_set_y("CONFIG_COMCERTO_NAIROBI");

	token_set_y("CONFIG_RG_SMB");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ZERO_CONFIGURATION_NETWORKING");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section as well */
	enable_module("MODULE_RG_IPSEC");
	/* Cryptographic hardware accelerator */
	token_set_y("CONFIG_CADENCE_IPSEC2");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FILESERVER");
        enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_WAN");
	enable_module("CONFIG_HW_ENCRYPTION");

	/* VoIP */
	enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	enable_module("MODULE_RG_PBX");
	token_set_y("CONFIG_COMCERTO_MATISSE");
	token_set_y("CONFIG_COMCERTO_CAL");

	/* Ralink Wi-Fi card */
	token_set_y("CONFIG_HW_80211G_RALINK_RT2561");

	dev_add_bridge("br0", DEV_IF_NET_INT, "eth0", NULL);

	/* Ralink WiFi card */
	dev_add_to_bridge_if_opt("br0", "ra0",
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ?
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");
    }

    else if (IS_DIST("RGLOADER_CX8620XD"))
    {
	hw = "CX8620XD";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_CX8620X_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5L");
    }
    else if (IS_DIST("IXDP425_CYBERTAN"))
    {
	hw = "IXDP425";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("IXDP425_ATM") || IS_DIST("IXDP425_ATM_WIRELESS"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_DSL");
	if (IS_DIST("IXDP425_ATM_WIRELESS"))
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");
	if (IS_DIST("IXDP425_ATM_WIRELESS"))
	{
	    enable_module("CONFIG_HW_80211G_ISL38XX");
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "eth0", NULL);
	}

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }
    else if (IS_DIST("NAPA"))
    {
	hw = dist;

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_FIREWIRE");
	token_set_y("CONFIG_HW_FIREWIRE_STORAGE");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp1", "usb0", NULL);
    }
    /* Gemtek boards */
    else if (IS_DIST("GTWX5715"))
    {
	hw = "GTWX5715";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_SSL_VPN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_m("CONFIG_HW_BUTTONS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", NULL);
	
	enable_module("CONFIG_HW_80211G_ISL38XX");
	dev_add_to_bridge_if_opt("br0", "eth0", "CONFIG_HW_80211G_ISL38XX");

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	    token_set_y("CONFIG_RG_WPA");
	
	dev_add_to_bridge_if_opt("br0", "ra0", 
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");

	token_set_y("CONFIG_GLIBC");

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
    }
    else if (IS_DIST("HG21"))
    {
	hw = "HG21";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_ISL38XX");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RG_INSMOD_SILENT");
	token_set("CONFIG_JFFS2_FS", "m");
	token_set_y("CONFIG_IXP425_JFFS2_WORKAROUND");
	token_set_y("CONFIG_IXP425_CSR_HSS");
	token_set("CONFIG_IXP425_CODELETS", "m");
	token_set("CONFIG_IXP425_CODELET_HSS", "m");
	token_set_y("CONFIG_GUI_WELLTECH");
	token_set_y("CONFIG_GUI_RG");
	token_set_y("CONFIG_RG_SYSLOG_REMOTE");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "eth0", NULL);
    }
    else if (IS_DIST("IXDP425_FRG"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");	

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("IXDP425_TMT"))
    {
	hw = "IXDP425_TMT";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_m("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_GLIBC");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }
    else if (IS_DIST("RGLOADER_WADB100G"))
    {
    	hw = "WADB100G";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_BCM9634X");
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_BCM963XX_BOOTSTRAP");
	token_set_m("CONFIG_RG_KRGLDR");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("WADB100G"))
    {
    	hw = "WADB100G";
	os = "LINUX_24";
	
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_BCM9634X");

	small_flash_default_dist();

	/* OpenRG HW support */
	
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_SWITCH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	token_set_y("CONFIG_HW_USB_RNDIS");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", "bcm1", NULL);
	dev_add_to_bridge_if_opt("br0", "wl0", "CONFIG_HW_80211G_BCM43XX");
	dev_add_to_bridge_if_opt("br0", "usb0", "CONFIG_HW_USB_RNDIS");
    }
    else if (IS_DIST("ASUS6020VI"))
    {
    	hw = "ASUS6020VI";
	os = "LINUX_24";
	
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_BCM9634X");
	
	small_flash_default_dist();

	/* OpenRG HW support */
	
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_SWITCH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm1", NULL);
	dev_add_to_bridge_if_opt("br0", "wl0", "CONFIG_HW_80211G_BCM43XX");
    }
    else if (IS_DIST("RGLOADER_WADB102GB"))
    {
    	hw = "WADB102GB";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_BCM9634X");
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_m("CONFIG_RG_KRGLDR");
	token_set_y("CONFIG_RG_TELNETS");
	
	token_set_y("CONFIG_RG_MTD");

	token_set("CONFIG_SDRAM_SIZE", "16");
	token_set_y("CONFIG_BCM963XX_BOOTSTRAP");

	/* OpenRG HW support */
	
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("WADB102GB"))
    {
    	hw = "WADB102GB";
	os = "LINUX_24";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_BCM9634X");
	
	small_flash_default_dist();

	/* OpenRG HW support */

	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_SWITCH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm1", NULL);
	dev_add_to_bridge_if_opt("br0", "wl0", "CONFIG_HW_80211G_BCM43XX");
    }
    else if (IS_DIST("MPC8272ADS_LSP") || IS_DIST("MPC8272ADS_LSP_26"))
    {
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_PPC");

	/* Hardware */
	
	hw = "MPC8272ADS";
	token_set_y("CONFIG_HW_ETH_FEC");

	if (IS_DIST("MPC8272ADS_LSP_26"))
	    os = "LINUX_26";
	else
	    os = "LINUX_24";
	
	/* Software */

	token_set_y("CONFIG_LSP_DIST");
    }
    else if (IS_DIST("MPC8272ADS"))
    {
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_PPC");

	hw = "MPC8272ADS";
	os = "LINUX_26";
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_TR_098");
	enable_module("MODULE_RG_TR_064");
	enable_module("MODULE_RG_MAIL_FILTER");
	enable_module("MODULE_RG_SSL_VPN");
	token_set_y("CONFIG_RG_SMB");
	enable_module("MODULE_RG_URL_FILTERING");

	enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	enable_module("MODULE_RG_PBX");

	enable_module("MODULE_RG_PRINTSERVER");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	enable_module("MODULE_RG_FILESERVER");
	
	enable_module("MODULE_RG_BLUETOOTH");

	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	
	/* Include automatic daylight saving time calculation */
	
	token_set_y("CONFIG_RG_DATE");
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");

	/* HW Configuration Section */
	
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_HW_ETH_FEC");

	token_set_y("CONFIG_HW_USB_STORAGE");

	/* Ralink RT2560 */

	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	token_set_y("CONFIG_HW_ENCRYPTION");
	
	dev_add_bridge("br0", DEV_IF_NET_INT, "eth1", NULL);
	dev_add_to_bridge_if_opt("br0", "ra0", 
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");

	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
    }
    else if (IS_DIST("EP8248_LSP_26"))
    {
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_PPC");

	/* Hardware */
	
	hw = "EP8248";
	os = "LINUX_26";
	
	token_set_y("CONFIG_HW_ETH_FEC");

	/* Software */

	token_set_y("CONFIG_LSP_DIST");
    }
    else if (IS_DIST("EP8248_26"))
    {
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_PPC");

	hw = "EP8248";
	os = "LINUX_26";
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_TR_098");
	enable_module("MODULE_RG_TR_064");
	enable_module("MODULE_RG_MAIL_FILTER");
	enable_module("MODULE_RG_SSL_VPN");
	enable_module("MODULE_RG_URL_FILTERING");
#if 0
	if (!IS_DIST("MPC8272ADS_26")) /* Temporary */
	{
	    enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");

	    enable_module("MODULE_RG_PRINTSERVER");
	    token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	}

	enable_module("MODULE_RG_FILESERVER");
#endif	
	token_set_y("CONFIG_RG_NETTOOLS_ARP");

	/* Include automatic daylight saving time calculation */
	
	token_set_y("CONFIG_RG_DATE");
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");

	/* HW Configuration Section */
	
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_HW_ETH_FEC");

//	token_set_y("CONFIG_HW_USB_STORAGE");

	dev_add_bridge("br0", DEV_IF_NET_INT, "eth0", NULL);
    }
    else if (IS_DIST("MPC8349ITX_LSP"))
    {
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_PPC");

	/* Hardware */
	
	os = "LINUX_26";
	hw = "MPC8349ITX";

	/* Software */
	token_set_y("CONFIG_GIANFAR");
	token_set_y("CONFIG_CICADA_PHY");
	token_set_y("CONFIG_PHYLIB");
	token_set_y("CONFIG_LSP_DIST");

	token_set_y("CONFIG_USB_GADGET");
	token_set_y("CONFIG_USB_ETH");
	token_set_y("CONFIG_USB_MPC");
	token_set_y("CONFIG_USB_GADGET_MPC");
	token_set_y("CONFIG_USB_GADGET_DUALSPEED");
	token_set_y("CONFIG_USB_ETH_RNDIS");

	token_set_y("CONFIG_SCSI");
	token_set_y("CONFIG_SCSI_SATA");
	token_set_y("CONFIG_SCSI_SATA_SIL");

	token_set_y("CONFIG_USB");
	token_set_y("CONFIG_USB_EHCI_HCD");
	token_set_y("CONFIG_USB_EHCI_ROOT_HUB_TT");
	token_set_y("CONFIG_USB_DEVICEFS");
	token_set_y("CONFIG_FSL_USB20");
	token_set_y("CONFIG_MPH_USB_SUPPORT");
	token_set_y("CONFIG_MPH0_USB_ENABLE");
	token_set_y("CONFIG_MPH0_ULPI");
    }
    else if (IS_DIST("MPC8349ITX"))
    {
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_PPC");

	os = "LINUX_26";
	hw = "MPC8349ITX";
	token_set_y("CONFIG_HW_USB_ETH");
	token_set_y("CONFIG_HW_IDE");

	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_TR_098");
	enable_module("MODULE_RG_TR_064");
	enable_module("MODULE_RG_MAIL_FILTER");
	enable_module("MODULE_RG_SSL_VPN");
	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_URL_FILTERING");

#if 0 
	/* XXX disabled because the image is too big, but enabled in JPKG_PPC */
	enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	enable_module("MODULE_RG_PBX");
#endif

	enable_module("MODULE_RG_PRINTSERVER");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	enable_module("MODULE_RG_FILESERVER");

	enable_module("MODULE_RG_BLUETOOTH");

	token_set_y("CONFIG_RG_NETTOOLS_ARP");

	/* Include automatic daylight saving time calculation */

	token_set_y("CONFIG_RG_DATE");
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");

	/* HW Configuration Section */

	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_HW_USB_STORAGE");

	/* Ralink RT2561 */
	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	token_set_y("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "eth1", NULL);
	dev_add_to_bridge_if_opt("br0", "ra0", 
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");

	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
    }
    else if (IS_DIST("CENTAUR_LSP"))
    {
	hw = "CENTAUR";
 	os = "LINUX_24";
 
 	token_set_y("CONFIG_KS8695_COMMON");
 
 	/* Software */
 	token_set_y("CONFIG_LSP_DIST");
 	token_set_y("CONFIG_SIMPLE_RAMDISK");
 	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
 	token_set("CONFIG_RAMDISK_SIZE", "1024");
 	token_set_y("CONFIG_HW_ETH_WAN");
 	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("CENTAUR") || IS_DIST("CENTAUR_VGW"))
    {
 	hw = "CENTAUR";
 	os = "LINUX_24";
 
 	enable_module("MODULE_RG_FOUNDATION");
 	enable_module("MODULE_RG_ADVANCED_ROUTING");
 	enable_module("MODULE_RG_VLAN");
 	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
   	enable_module("MODULE_RG_PPP");
 	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
 	enable_module("MODULE_RG_URL_FILTERING");
 	enable_module("MODULE_RG_IPSEC");
 	enable_module("MODULE_RG_PPTP");
 	enable_module("MODULE_RG_L2TP");
 	enable_module("MODULE_RG_UPNP");
 	enable_module("MODULE_RG_SNMP");
 	enable_module("MODULE_RG_IPV6");
 	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_MAIL_FILTER");
	enable_module("MODULE_RG_SSL_VPN");
 
 	/* HW Configuration Section */
 	token_set_y("CONFIG_HW_ETH_WAN");
 	token_set_y("CONFIG_HW_ETH_LAN");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
 
 	token_set("CONFIG_RG_SSID_NAME", "Centaur");
 	dev_add_bridge("br0", DEV_IF_NET_INT, "eth1", "ra0", NULL);

	if (IS_DIST("CENTAUR_VGW"))
	{
	    enable_module("MODULE_RG_VOIP_OSIP");
	    enable_module("MODULE_RG_ATA");
	    token_set_y("CONFIG_HW_DSP");
	}
 
 	/* Software */
 	token_set_y("CONFIG_KS8695_COMMON");
    }
    else if (IS_DIST("RGLOADER_CENTAUR"))
    {
 	hw = "CENTAUR";
 	os = "LINUX_24";
 
 	token_set_y("CONFIG_RG_RGLOADER");
 
 	token_set_y("CONFIG_KS8695_COMMON");
 	token_set_y("CONFIG_SIMPLE_RAMDISK");
 	token_set("CONFIG_RAMDISK_SIZE", "4096");
 	token_set_y("CONFIG_PROC_FS");
 	token_set_y("CONFIG_EXT2_FS");
 	token_set_y("CONFIG_HW_ETH_WAN");
 	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("IXDP425_LSP"))
    {
	/* Hardware */
	hw = "IXDP425";

        token_set_y("CONFIG_IXP425_COMMON_LSP");
        token_set("CONFIG_IXDP425_KGDB_UART", "1");

	/* ADSL Chip Alcatel 20150 on board */
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");

	/* EEPROM */
	token_set("CONFIG_PCF8594C2", "m");

	/* IXP425 Eth driver module */
	token_set("CONFIG_IXP425_ETH", "m");

        /* Flash chip */
	token_set_y("CONFIG_IXP425_FLASH_E28F128J3");

	/* Software */
	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
    }
    else if (IS_DIST("COYOTE_CAVIUM"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("COYOTE_ACTIONTEC_OSIP"))
    {
	hw = "COYOTE";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_VOIP_OSIP");
	enable_module("MODULE_RG_PPP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_DSP");

	enable_module("MODULE_RG_ATA");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "eth0", NULL);
    }
    else if (IS_DIST("COYOTE"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_ARM_24_FAST_MODULES");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_TR_098");
	enable_module("MODULE_RG_TR_064");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_SSL_VPN");

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");

	enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	    
	enable_module("MODULE_RG_MAIL_FILTER");
	
	enable_module("MODULE_RG_DSL");
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_WAN");

	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_USB_RNDIS");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_IDE");
	enable_module("CONFIG_HW_80211G_RALINK_RT2560");

	enable_module("MODULE_RG_ATA");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", NULL);
	dev_add_to_bridge_if_opt("br0", "usb0", "CONFIG_HW_USB_RNDIS");
	dev_add_to_bridge_if_opt("br0", "ra0", 
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");

	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");
	enable_module("MODULE_RG_BLUETOOTH");
    }
    else if (IS_DIST("MONTEJADE") || IS_DIST("MONTEJADE_ATM") ||
	IS_DIST("JIWIS8XX"))
    {
	if (IS_DIST("JIWIS8XX"))
	{
	    hw = "JIWIS800";
	    token_set_m("CONFIG_HW_LEDS");
	    token_set_y("CONFIG_IXP425_COMMON_RG");
	}
	else
	{
	    hw = "MONTEJADE";
	    enable_module("CONFIG_HW_USB_RNDIS");
	}

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
	token_set_y("CONFIG_RG_SMB");
	/* Notice: this config interfere with kernel debugger. Disable it while
	 * debugging.
	 */
	token_set_y("CONFIG_ARM_24_FAST_MODULES");
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	enable_module("MODULE_RG_VOIP_ASTERISK_MGCP_CALL_AGENT");
        enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_TR_098");
	enable_module("MODULE_RG_TR_064");
	enable_module("MODULE_RG_MAIL_FILTER");
	enable_module("MODULE_RG_JVM");
	enable_module("MODULE_RG_SSL_VPN");
	enable_module("MODULE_RG_PBX");

	enable_module("MODULE_RG_BLUETOOTH");

	/* HW Configuration Section */
	token_set_m("CONFIG_HW_BUTTONS");
	if (IS_DIST("MONTEJADE_ATM"))
	{
	    enable_module("MODULE_RG_DSL");
	    token_set_y("CONFIG_HW_DSL_WAN");
	}
	else
	{
	    token_set_y("CONFIG_HW_ETH_WAN");
	}

        token_set_y("CONFIG_HW_SWITCH_LAN");

	token_set("CONFIG_HW_USB_HOST_EHCI", "y");
	token_set("CONFIG_HW_USB_HOST_OHCI", "y");
	token_set("CONFIG_HW_USB_STORAGE", "y");
	enable_module("CONFIG_HW_ENCRYPTION");

	/* DSR support */
	enable_module("CONFIG_HW_DSP");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", NULL);

	dev_add_to_bridge_if_opt("br0", "usb0", "CONFIG_HW_USB_RNDIS");

	dev_add_to_bridge_if_opt("br0", "wlan0",
	    "CONFIG_HW_80211N_AIRGO_AGN100");

	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	dev_add_to_bridge_if_opt("br0", "ra0",
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");

	enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "4194304");

	/* IPv6 can not work with airgo - B21867 */
	if (!token_get("CONFIG_HW_80211N_AIRGO_AGN100"))
	    enable_module("MODULE_RG_IPV6");
    }
    else if (IS_DIST("PCBOX_EEP_EEP_EICON") || IS_DIST("PCBOX_RTL_RTL_EICON"))
    {
	if (IS_DIST("PCBOX_EEP_EEP_EICON"))
	    hw = "PCBOX_EEP_EEP";
	else
	    hw = "ALLWELL_RTL_RTL";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_IPSEC");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_GLIBC");
    }
    else if (IS_DIST("COYOTE_LSP"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
	token_set_y("CONFIG_IXP425_COMMON_LSP");
    }
    else if (IS_DIST("COYOTE_EEP"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_EEPRO100_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_IDE");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eep0", NULL);
    }
    /* JStream */
    else if (IS_DIST("RGLOADER_JIWIS8XX"))
    {
	/* By default DIST=JIWIS8XX will build JIWIS800 hardware (keMontajade)
	 * (see Montejade section)
	 * Other supported HW may be added later,
	 * currently HW=JIWIS800 and HW=JIWIS832 are supported
	 */
	if (!hw)
	    hw = "JIWIS800";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_m("CONFIG_HW_BUTTONS");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
    }
    else if (IS_DIST("COYOTE_ALL"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_DSL");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_IDE");

	enable_module("MODULE_RG_ATA");

	dev_add_bridge("br0", DEV_IF_NET_INT, NULL);
    }
    else if (IS_DIST("COYOTE_ATM_WIRELESS_ISL"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_DSL");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_IDE");

	enable_module("MODULE_RG_ATA");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
    }
    else if (IS_DIST("RGLOADER_BRUCE"))
    {
	hw = "BRUCE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("BRUCE"))
    {
	hw = "BRUCE";

	/* Modules */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_HW_IDE");
	token_set_m("CONFIG_HW_BUTTONS");
	token_set_m("CONFIG_HW_LEDS");
        token_set_y("CONFIG_HW_CLOCK");
        token_set_y("CONFIG_HW_I2C");
        token_set_y("CONFIG_HW_ENV_MONITOR");

	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp1", "eth0", NULL);
    }
    //start of westell
    else if (IS_DIST("TWINPASS") || IS_DIST("ULTRALINE3_TWINPASS") ||
             IS_DIST("WSTL_ULS3_VDSL_IKANOS_64") ||
             IS_DIST("WSTL_ULS3_DUAL_MOCA_64")||
             IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_64")||
             IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64")||
             IS_DIST("WSTL_ULS3_VDSL_INFINEON_64") || 
             IS_DIST("WSTL_ULS3_WAN_ETHERNET_64") || 
             IS_DIST("WSTL_ULS3_WAN_ETHERNET_CO_64") || 
             IS_DIST("WSTL_ULS3_DMOCA_GENERIC_64")||
             IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_64") || 
             IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_64") ||
             IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64") ||
             IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64") ||
             IS_DIST("WSTL_ULS3_TURBO_M100_64") ||
             IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_11N_64") ||
             IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64"))
    {
	if (IS_DIST("WSTL_ULS3_VDSL_IKANOS_64") ||
	    IS_DIST("WSTL_ULS3_DUAL_MOCA_64") ||
	    IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64")||
	    IS_DIST("WSTL_ULS3_VDSL_INFINEON_64") ||
	    IS_DIST("WSTL_ULS3_WAN_ETHERNET_64") ||
	    IS_DIST("WSTL_ULS3_WAN_ETHERNET_CO_64") ||
	    IS_DIST("WSTL_ULS3_DMOCA_GENERIC_64") ||
	    IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_64") ||
	    IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_64") ||
	    IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64") ||
	    IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64") ||
	    IS_DIST("WSTL_ULS3_TURBO_M100_64") ||
	    IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_11N_64") ||
        IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64"))

	    hw = "ULTRALINE3_TWINPASS";
	else
	    hw = dist;
	    
	os = "LINUX_24";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_TWINPASS");
	token_set("LIC_AUTOCONF", "n");

	/* Modules */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
 	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	//enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_UPNP");
    if (!IS_DIST("WSTL_ULS3_TURBO_M100_64") &&
        !IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64") )
       // && !IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64"))
    {
        
         enable_module("MODULE_RG_DSLHOME");
         enable_module("MODULE_RG_TR_098");
         
    }

	enable_module("MODULE_RG_TR_064");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	token_set("CONFIG_RG_ADV_SERVICES_LEVEL", "1");
    if (!IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64"))
    {
        token_set_m("CONFIG_RG_DANUBE_DUALCORE");
        token_set_m("CONFIG_TWINPASS_DUALCORE_WLAN");
        if (IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_11N_64") || IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64"))
        {
             token_set_y("CONFIG_HW_80211N_ATHEROS_AR50WL");
             token_set_y("CONFIG_INFINEON_PPE_D4_11N");
        }
        else
        {
            token_set_y("CONFIG_HW_80211G_ATHEROS_AR521X");
        }
    }
    if (IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_64"))
    {
        token_set_y("CONFIG_WESTELL_WIRELESS_ONBOARD");
		token_set_y("CONFIG_DEBUG_PAGES");
    }
	if (IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64"))
	{
		token_set_y("CONFIG_GENERIC_IKANOS");
	}

	token_set_m("CONFIG_RG_FASTPATH");

	token_set("LIBC_IN_TOOLCHAIN", "n");
	token_set_y("CONFIG_ULIBC");
	token_set_y("CONFIG_DYN_LINK");
    token_set_y("CONFIG_RG_THREADS");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_WAN");

	token_set_y("CONFIG_HW_ENCRYPTION");
#if 0
	enable_module("CONFIG_HW_USB_RNDIS");
	enable_module("CONFIG_HW_80211G_RALINK_RT2560");
#endif
    /* For generating full flash image */
    token_set_y("CONFIG_RG_PROD_IMG");
    token_set_y("CONFIG_WSTL_BOOTLOADER_IMG");

	/* Stuff that's not supported on Infineon reference board
	 */
	if (!IS_DIST("TWINPASS"))
	{
	    token_set_m("CONFIG_HW_BUTTONS");
	    token_set_y("CONFIG_HW_LEDS");
	    token_set_m("CONFIG_RG_HW_SWITCH_QOS");

        /*M100 does not have Moca device*/
        if (!IS_DIST("WSTL_ULS3_TURBO_M100_64") &&
            !IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64"))
	        token_set_y("CONFIG_HW_MOCA");
	}

	dev_add_bridge("br0", DEV_IF_NET_INT, "eth0", NULL);
	dev_add_to_bridge_if_opt("br0", "usb0", "CONFIG_HW_USB_RNDIS");
	dev_add_to_bridge_if_opt("br0", "ra0",
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");
 	dev_add_to_bridge_if_opt("br0", "ath0",
        token_get("CONFIG_HW_80211G_ATHEROS_AR521X") ?
 	    "CONFIG_HW_80211G_ATHEROS_AR521X" :
        "CONFIG_HW_80211N_ATHEROS_AR50WL");

	/* Options for specific Westell targets
	 */

    /*VDSL*/
    if (IS_DIST("ULTRALINE3_TWINPASS") ||
        IS_DIST("WSTL_ULS3_VDSL_IKANOS_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64") ||
	IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64"))
	{
        if (IS_DIST("WSTL_ULS3_VDSL_INFINEON_64") ||
            IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_64") ||
            IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64"))
        {
       
            token_set_y("CONFIG_RG_INFINEON_VDSL");        
        }
        
        token_set_y("CONFIG_HW_VDSL_WAN");
	}

    /*Dual Moca- */
   /* we need to define one for WAN MOCA here*/
    if (IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_64") ||
        IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_11N_64"))
    {
        token_set_y("CONFIG_HW_MOCA_WAN");
        token_set_y("MOCA_VERSION_1_1");
    }


    if (IS_DIST("WSTL_ULS3_DUAL_MOCA_64") || IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_64"))
    {
        token_set_y("CONFIG_HW_MOCA_WAN");
        //If you use this token elsewhere, make sure the defaults get set up
        token_set_y("CONFIG_BASIC_LOG");
    }
    /* Turbo M100- WAN Ethernet only*/
	if (IS_DIST("WSTL_ULS3_TURBO_M100_64"))
    {

         token_set_y("CONFIG_HW_WAN_ETHERNET");
         token_set_y("CONFIG_WSTL_RETAIL");
    }

    /* Define build for Generic WAN MOCA here*/
	if (IS_DIST("WSTL_ULS3_DMOCA_GENERIC_64"))
    {
        token_set_y("CONFIG_HW_MOCA_WAN");
        token_set_y("CONFIG_HW_DEF_TO_WAN_ETH");
        token_set_y("CONFIG_WSTL_GENERIC");
    }

    /* Generic IFX vdsl build. Also used for Generic Ikanos build */
	if(IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_64") ||
           IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64") ||
           IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64"))
	{
        token_set_y("CONFIG_WSTL_GENERIC");	
	}
    /*WAN Ethernet only */
    if (IS_DIST("WSTL_ULS3_WAN_ETHERNET_64") ||
        IS_DIST("WSTL_ULS3_WAN_ETHERNET_CO_64"))
    {
        token_set_y("CONFIG_HW_WAN_ETHERNET");
        if (IS_DIST("WSTL_ULS3_WAN_ETHERNET_CO_64")) {
           token_set_y("CONFIG_WSTL_DUAL_SPI_MDIO"); //dual moca board
        }
        /*Comcast specific changes*/
        token_set_y("CONFIG_WSTL_COMCAST");
    }

   /* Verizon Dual MOCA with USB */
    if (IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_64") || IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64"))
    {
#if 0  /* for now no USB */
        token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("MODULE_RG_FILESERVER");
#endif
        
        token_set_y("CONFIG_WSTL_USBHOSTOVERCURRENT_GPIO"); /* always enable USB power and over current monitoring */
#if 0   /* Start of optional 4homemedia config */
        token_set_y("CONFIG_WSTL_4HOMEMEDIA_HOMECONTROL");
        token_set("CONFIG_MAX_DMA_SIZE", "6144"); /* number of pages */
        token_set("CONFIG_JFFS2_FS", "y");
        token_set("CONFIG_JFFS2_SIZE", "2048"); /* number of Kbytes */
#endif  /* End of 4homemedia config */
    }

    //with 11n card
    if (IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64")) {
        token_set_y("CONFIG_INFINEON_PPE_D4");
    }

  
   /* Turbo Dual MOCA */
    if (IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_64") ||
        IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_11N_64"))
    {
        token_set_y("CONFIG_HW_TURBO_DUAL_MOCA");
        //token_set_y("CONFIG_TANTOS_PSB6973"); //this is done in hw_config.c
        token_set_y("CONFIG_RG_TURBO_SWITCH_IGMP");
        token_set_m("CONFIG_RG_TURBO_SWITCH_IGMP_MODULE");     
        token_set_y("CONFIG_INFINEON_PPE_D4");
        token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("MODULE_RG_FILESERVER");
	/* token_set_y("CONFIG_WSTL_MEDIASERVER"); */
#if 0   /* Start of optional 4homemedia config */
        token_set_y("CONFIG_WSTL_4HOMEMEDIA_HOMECONTROL");
        token_set("CONFIG_MAX_DMA_SIZE", "6144"); /* number of pages */
        token_set("CONFIG_JFFS2_FS", "y");
        token_set("CONFIG_JFFS2_SIZE", "2048"); /* number of Kbytes */
#endif  /* End of 4homemedia config */
    }

    /* Dual Moca builds that can run on both revisions of the board */
    if (IS_DIST("WSTL_ULS3_DUAL_MOCA_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_64") || IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64"))
    {
         token_set_y("CONFIG_HW_DUAL_MOCA");
         token_set_y("CONFIG_WSTL_CPLD_CONFIG");
         token_set("CONFIG_WSTL_CPLD_REVISION", "0");  /* default to Rev 1.0 */
    }

    /* Turbo M100 WAN Eth only */
    if (IS_DIST("WSTL_ULS3_TURBO_M100_64"))
    {
        token_set_y("CONFIG_HW_TURBO_M100");
        //token_set_y("CONFIG_TANTOS_PSB6973"); //this is done in hw_config.c
        token_set_y("CONFIG_RG_TURBO_SWITCH_IGMP");
        token_set_m("CONFIG_RG_TURBO_SWITCH_IGMP_MODULE");     
        token_set_y("CONFIG_INFINEON_PPE_D4");
        token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("MODULE_RG_FILESERVER");
        token_set_y("CONFIG_WSTL_MEDIASERVER");
//        token_set_y("CONFIG_WSTL_4HOMEMEDIA_HOMECONTROL");
//        token_set("CONFIG_MAX_DMA_SIZE", "6144"); /* number of pages */
//        token_set("CONFIG_JFFS2_FS", "y");
//        token_set("CONFIG_JFFS2_SIZE", "2048");
    }

    /* Common ART build for Turbo boards WAN Eth only */
    if (IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64"))
    {
        token_set_y("CONFIG_WESTELL_WIRELESS_ONBOARD");
        token_set_y("CONFIG_ATHEROS_ART_TOOLS");
        token_set_y("CONFIG_HW_TURBO_M100");
        token_set_y("CONFIG_INFINEON_PPE_D4");
        token_set_y("CONFIG_HW_WAN_ETHERNET");
        token_set_y("CONFIG_WSTL_GENERIC");
    }

    /* USB enabled builds. */
	if(IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64"))
	{
	    token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("MODULE_RG_FILESERVER");
	}


    if (IS_DIST("WSTL_ULS3_VDSL_IKANOS_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_64") ||
        IS_DIST("WSTL_ULS3_WAN_ETHERNET_64") ||
        IS_DIST("WSTL_ULS3_WAN_ETHERNET_CO_64") ||
        IS_DIST("WSTL_ULS3_DMOCA_GENERIC_64") || 
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_64") ||
        IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64") ||
        IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64") ||
        IS_DIST("WSTL_ULS3_TURBO_M100_64") ||
        IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_11N_64") ||
        IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64"))
    {
        token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM"); 
        token_set_y("CONFIG_TWINPASS_64_MEG");
    }

   /* need to define a bootloader to use */
    if (IS_DIST("WSTL_ULS3_VDSL_IKANOS_64"))
    {
        token_set_y("CONFIG_BOOT_WSTL_VDSL_64");
    }
    else if (IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64")) 
    {
		token_set_y("CONFIG_BOOT_WSTL_VDSL_64");
    }
    else if (IS_DIST("WSTL_ULS3_VDSL_INFINEON_64") ||
             IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_64") ||
             IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64"))
    {
        token_set_y("CONFIG_BOOT_WSTL_VDSL_64");
    }
    /*For now, we'll use the Dual Moca bootloader for WAN Ethernet only board*/
    else if (IS_DIST("WSTL_ULS3_DUAL_MOCA_64") ||
             IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_64") ||
             IS_DIST("WSTL_ULS3_WAN_ETHERNET_64") ||
             IS_DIST("WSTL_ULS3_WAN_ETHERNET_CO_64") ||
             IS_DIST("WSTL_ULS3_DMOCA_GENERIC_64") ||
             IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64"))
    {
        token_set_y("CONFIG_BOOT_WSTL_DUAL_MOCA_64");
    }
    else if (IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_64") ||
             IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_11N_64"))
    {
        token_set_y("CONFIG_BOOT_WSTL_TURBO_64");
    }
    else if (IS_DIST("WSTL_ULS3_TURBO_M100_64") ||
             IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64"))
    {
        token_set_y("CONFIG_BOOT_WSTL_M100_64");
    }
    else
        /* NOTE: Most of the remaining Westell Distribution types really do not use the bootloader
         *       Just need to define one to make the build process happy.
         */
    {
        token_set_y("CONFIG_BOOT_UBOOT_VDSL");
    }

    /* select flash size and layout */
    /* select new flash layout with 7 Meg app */
    if (IS_DIST("WSTL_ULS3_VDSL_IKANOS_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_64") ||
        IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_64"))
    {
        token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 
        token_set_y("CONFIG_WSTL_FLASH_LAYOUT_8FLASH_7APP");
        token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
    }
    
    /* select flash layout for boards with 16 Meg flash */
    else if (IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_64") ||
        IS_DIST("WSTL_ULS3_TURBO_M100_64") ||
        IS_DIST("WSTL_ULS3_WAN_ETHERNET_CO_64") ||
        IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_11N_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64"))
    {
        token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
        token_set_y("CONFIG_WSTL_FLASH_LAYOUT_16FLASH");
        token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
    }
    /* special flash layout for ART build */
    else if (IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64"))
    {
        token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
        token_set_y("CONFIG_WSTL_FLASH_LAYOUT_16FLASH_ART");
    }
    else
    {
        /* remaining distributions are old ones that had the original 6 Meg App  */
        /* there is no CONFIG_WSTL_FLASH_LAYOUT_XXX for these and no flash layout section */
        token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 
    }
    /* end of flash size and layout selection */

	/* Select a minimal set of Jungo GUI themes.
	 *  (If we don't specify any, then all themes are selected.)
	 */
	token_set_y("CONFIG_GUI_RG2");
	token_set_y("CONFIG_GUI_SMB2");

	/* Things don't build without this selected */
	token_set_y("CONFIG_GUI_RG");

	/* Options for all Westell targets
	 */
    if (IS_DIST("WSTL_ULS3_VDSL_IKANOS_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_64") ||
        IS_DIST("WSTL_ULS3_WAN_ETHERNET_64") ||
        IS_DIST("WSTL_ULS3_WAN_ETHERNET_CO_64") ||
        IS_DIST("WSTL_ULS3_DMOCA_GENERIC_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_64") ||
        IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_64") ||
        IS_DIST("WSTL_ULS3_VDSL_INFINEON_GENERIC_USB_64") ||
        IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64") ||
        IS_DIST("WSTL_ULS3_TURBO_M100_64") ||
        IS_DIST("WSTL_ULS3_TURBO_DUAL_MOCA_11N_64") ||
        IS_DIST("WSTL_ULS3_TURBO_COMMON_ART_64") ||
        IS_DIST("WSTL_ULS3_DUAL_MOCA_USB_11N_64"))
	{
	    /* Use non-Jungo defaults for host name, NTP server, etc.
	     */

	    
		token_set("CONFIG_RG_SSID_NAME", "UltralineS3");
	    token_set_y("CONFIG_WSTL_CONFIG_DEFAULTS");
		token_set_y("CONFIG_REDUCED_DNS_TRAFFIC");

	    /* Westell GUI hacks  and changes*/
	    token_set_y("CONFIG_WESTELL_GUI");
		
	   if (IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64"))
	   {
		   token_set_y("CONFIG_WSTL_TR_HIDDEN_PAGE");
	   }

	   if(!(token_get("CONFIG_HW_TURBO_M100") || token_get("CONFIG_HW_TURBO_DUAL_MOCA"))) {
               token_set_y("CONFIG_BASIC_LOG");
	   }
        /*VZ Themes will not be used on the M100*/
        if (IS_DIST("WSTL_ULS3_TURBO_M100_64")) {
            token_set("RG_PROD_STR", "UltraLine Media Server"); 
            token_set_y("CONFIG_GUI_WSTL_RETAIL");  
             token_set_y("CONFIG_WSTL_4HOMEMEDIA_GUI");

        }
        else{
              /* Themes */
            token_set_y("CONFIG_GUI_WSTL");
            token_set_y("CONFIG_GUI_WSTL_VZ");
            if (token_get("CONFIG_WSTL_COMCAST")) {
                 token_set("RG_PROD_STR", "UltraLine Series3");
            }
            else{
                token_set_y("CONFIG_GUI_VZ_NO_ICONS");
                token_set("RG_PROD_STR", "Wireless Broadband Router");	// CR12349 make GUI refs generic.
            }
               

        }
            
	    
	    /* JFD: To build jungo gui version replace the 3 lines above with the following... *
	    token_set_y("CONFIG_GUI_WSTL");
		if (IS_DIST("WSTL_ULS3_VDSL_IKANOS_GENERIC_64"))   // do jungo gui for these build(s).
		{
		    token_set("RG_PROD_STR", "UltraLine Series3");
		}
		else
		{
		    token_set("RG_PROD_STR", "Wireless Broadband Router");	// CR12349 make GUI refs generic.
	
		    // Themes
	    	token_set_y("CONFIG_GUI_WSTL_VZ");
			token_set_y("CONFIG_GUI_VZ_NO_ICONS"); 
		}
		*/

	    /* Fireball */
	    token_set_y("CONFIG_WSTL_SAVE_RXDEV");
	    token_set_y("CONFIG_WSTL_ARPMON");
        //if (!token_get("CONFIG_WSTL_RETAIL"))
            token_set_y("CONFIG_WSTL_FIREBALL");

	    /* Note that the set of LAN interfaces monitored by Fireball
	     * is currently hard-coded.
	     * 
	     * If the interfaces change, they code will need to be changed.
	     */

	    /* Hide stuff in the GUI that we don't support, or that are
	     * just Jungo advertising.
	     */
	    token_set_y("CONFIG_WSTL_GUI_HIDE");
	    token_set_y("CONFIG_WSTL_GUI_NO_HELP");


        /*Enable Westell logging for diagnostic and reqd purposes */
         token_set_y("CONFIG_WSTL_LOG_DIAGNOSTICS");
         /*Westell interface statistics*/
         token_set_y("CONFIG_WSTL_DEV_STATS");

         /*We don't need this for Comcast*/
         if (!token_get("CONFIG_WSTL_COMCAST") && 
             !token_get("CONFIG_WSTL_RETAIL") &&
             !token_get("CONFIG_ATHEROS_ART_TOOLS"))
         {
	         /*We don't need this for Generic */
    	     if (!token_get("CONFIG_WSTL_GENERIC"))
        	 {
            	 token_set_y("CONFIG_WSTL_WBA");
                 token_set_y("CONFIG_VERIZON_ONLY");
                 /* Enable Verizon specifig DNS resolution procedure */
                 token_set_y("CONFIG_WSTL_VERIZON_DNS_RESOLUTION");
        	 }
        	 if (!token_get("CONFIG_RG_WSTL_WAN_CABLE"))     	 
        	 {
                /* Enable Westell Wan Auto protocl detect */
                token_set_y("CONFIG_WSTL_WAN_PROTOCOL_DETECT");
             
                /* Enable Verizon specifig option 64 changes */
                token_set_y("CONFIG_WSTL_VERIZON_DHCP_OPT_60");
             }    
         }

         //For verizon builds Display the Fake PPPoE Device for the sys monitoring and device pages
         if(token_get("CONFIG_VERIZON_ONLY"))
         {
             token_set_y("CONFIG_PPPSTUB");
         }

         /*Hidden page for TR-69  and TR-64 removes TR69/64 from Remote Administration page*/
         if (!token_get("CONFIG_WSTL_RETAIL") &&
             !token_get("CONFIG_ATHEROS_ART_TOOLS"))
         {
              token_set_y("CONFIG_WSTL_TR_HIDDEN_PAGE");
         }

         if (!token_get("CONFIG_ATHEROS_ART_TOOLS"))
         {
             /*Wstl IGMP changes*/
             token_set_y("CONFIG_WSTL_IGMP_CHANGE");
             token_set_y("CONFIG_WSTL_LOOP_DETECT");
         }
         
		/* Enable Westell parental control GUI (top bar) selection. */
 	    token_set_y("WSTL_QND_PARENTAL");

 
        /*Used for general CR Fixes not covered in other config*/
        token_set_y("CONFIG_WSTL_CR_FIX");

	}

    }// end vz stuff
    else if (IS_DIST("AD6834"))
    {
	hw = "AD6834";

	/* Modules */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_PPP");
        enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_SSL_VPN");

	token_set_y("CONFIG_RG_PERM_STORAGE_VENDOR_HEADER");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	
	token_set("LIBC_IN_TOOLCHAIN", "n");
	token_set_y("CONFIG_DYN_LINK");

	enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
        enable_module("CONFIG_HW_DSP");
	enable_module("MODULE_RG_ATA");

	enable_module("MODULE_RG_BLUETOOTH");

	/* OpenRG HW support */
 	enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_DSL_WAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");
	dev_add_bridge("br0", DEV_IF_NET_INT, "eth0", NULL);
	dev_add_to_bridge_if_opt("br0", "usb0", "CONFIG_HW_USB_RNDIS");
	dev_add_to_bridge_if_opt("br0", "ra0",
	    token_get("CONFIG_HW_80211G_RALINK_RT2561") ? 
	    "CONFIG_HW_80211G_RALINK_RT2561" :
	    "CONFIG_HW_80211G_RALINK_RT2560");

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_LX4189");
    }
    else if (IS_DIST("AD6834_26") || IS_DIST("AD6834_26_FULL"))
    {
	hw = "AD6834";
	os = "LINUX_26";

	/* Modules */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_PPP");
        enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_SSL_VPN");
	
	token_set_y("CONFIG_RG_PERM_STORAGE_VENDOR_HEADER");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	
	token_set_y("CONFIG_DYN_LINK");

	enable_module("MODULE_RG_BLUETOOTH");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_DSL_WAN");
        enable_module("CONFIG_HW_ENCRYPTION");
	dev_add_bridge("br0", DEV_IF_NET_INT, "eth0", NULL);

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_LX4189");

	/* Ramdisk */
	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");
	token_set_y("CONFIG_RG_MODFS_CRAMFS");
    }
    else if (IS_DIST("AD6834_26_LSP"))
    {
	hw = "AD6834";
	os = "LINUX_26";
	token_set_y("CONFIG_DYN_LINK");

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_RG_PERM_STORAGE");

	/* Devices */
	token_set_y("CONFIG_HW_ETH_LAN");

	/* Ramdisk */
	//token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");
	//token_set_y("CONFIG_RG_MODFS_CRAMFS");
	token_set_y("CONFIG_SIMPLE_RAMDISK");

    }
    else if (IS_DIST("JNET_SERVER"))
    {
	set_jnet_server_configs();
	token_set_y("CONFIG_RG_USE_LOCAL_TOOLCHAIN");
	token_set_y("CONFIG_RG_BUILD_LOCAL_TARGETS_ONLY");
    }
    else if (IS_DIST("BCM96358_LSP"))
    {
	hw = "BCM96358";
	os = "LINUX_26";

	token_set_y("CONFIG_LSP_DIST");

	/* Devices */
	/* XXX: Add all devices when drivers are ready.
	 */
	token_set_y("CONFIG_HW_ETH_LAN");
	
	token_set("LIBC_IN_TOOLCHAIN", "n");

	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");
	token_set_y("CONFIG_RG_MODFS_CRAMFS");
    }
    /* Two bcm96358 boards exist, bcm96358GW and bcm96358M, for now their
     * features are the same, but in the future if different features will be 
     * implemented, must configure two dists */
    else if (IS_DIST("BCM96358"))
    {
	hw = "BCM96358";
	os = "LINUX_26";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_MIPSEB");
	
	small_flash_default_dist();
	
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_SWITCH_LAN");
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");

	enable_module("CONFIG_HW_80211G_BCM43XX");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	
	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm1", "wl0", NULL);

	token_set("LIBC_IN_TOOLCHAIN", "y");

	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");
	token_set_y("CONFIG_RG_MODFS_CRAMFS");
    }

    /* REDBOOT distributions for IXP425 based boards */
    else if (IS_DIST("REDBOOT_RICHFIELD"))
    {
	hw = "RICHFIELD";
	os = "ECOS";
	token_set("DIST_TYPE", "BOOTLDR");
    }
    else if (IS_DIST("REDBOOT_MATECUMBE"))
    {
	hw = "MATECUMBE";
	os = "ECOS";
	token_set("DIST_TYPE", "BOOTLDR");
    }
    else if (IS_DIST("REDBOOT_COYOTE"))
    {
	hw = "COYOTE";
	os = "ECOS";
	token_set("DIST_TYPE", "BOOTLDR");
    }
    else if (IS_DIST("REDBOOT_NAPA"))
    {
	hw = "NAPA";
	os = "ECOS";
	token_set("DIST_TYPE", "BOOTLDR");
    }
    else if (IS_DIST("RGLOADER_IXDP425") ||
	IS_DIST("RGLOADER_IXDP425_FULLSOURCE") ||
	IS_DIST("RGLOADER_IXDP425_16MB") || IS_DIST("RGLOADER_IXDP425_LSP"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set("CONFIG_IXDP425_KGDB_UART", "1");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");	

	if (IS_DIST("RGLOADER_IXDP425_16MB"))
	    token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16");

	if (IS_DIST("RGLOADER_IXDP425_LSP"))
	    token_set_y("CONFIG_LSP_FLASH_LAYOUT");
    }
    else if (IS_DIST("RGLOADER_MATECUMBE"))
    {
	hw = "MATECUMBE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_NAPA"))
    {
	hw = "NAPA";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
	token_set_y("CONFIG_IXP425_POST");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_COYOTE"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_COYOTE_64MB_RAM"))
    {
	hw = "COYOTE";
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
    }
    else if (IS_DIST("RGLOADER_COYOTE_64MB_RAM_32MB_FLASH"))
    {
	hw = "COYOTE";
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
	token_set_y("CONFIG_MTD_CONCAT");

	/* HW Configuration Section */
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "32"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_FLASH_CHIPS", "2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_COYOTE_LSP"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_MONTEJADE"))
    {
	hw = "MONTEJADE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
    }
    else if (IS_DIST("RGLOADER_HG21"))
    {
	hw = "HG21";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_BAMBOO"))
    {	
	hw = "BAMBOO";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_GTWX5715"))
    {
	hw = "GTWX5715";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");

	/* Disable VLAN switch on production boards */
	token_set_y("CONFIG_WX5715_VLAN_SWITCH_DISABLE");
    }
    else if (IS_DIST("RGLOADER_ALLWELL_RTL_RTL"))
    {
	hw = "ALLWELL_RTL_RTL";

	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_ALLWELL_RTL_EEP"))
    {
	hw = "ALLWELL_RTL_EEP";

	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_ALLWELL_EEP_EEP"))
    {
	hw = "PCBOX_EEP_EEP";

	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_WAV54G"))
    {
	hw = "WAV54G";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_m("CONFIG_HW_BUTTONS");
    }
    else if (IS_DIST("WRT55AG") || IS_DIST("ATHEROS_AR531X_AG_VX") ||
	IS_DIST("WRT55AG_INT"))
    {
	os = "VXWORKS";

	if (IS_DIST("WRT55AG"))
	    hw = "AR531X_WRT55AG";
	else if (IS_DIST("ATHEROS_AR531X_AG_VX"))
	    hw = "AR531X_AG";
	else
	    hw = "AR531X_G";
		
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
        enable_module("MODULE_RG_URL_FILTERING");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_AR531X");
	if (IS_DIST("WRT55AG") || IS_DIST("ATHEROS_AR531X_AG_VX"))
	    enable_module("CONFIG_HW_80211A_AR531X");
	token_set_m("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_RG_SSI_PAGES");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");
	token_set_y("CONFIG_RG_RMT_MNG");
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */
	token_set_y("CONFIG_RG_ENTFY"); /* Email notification */
	token_set("CONFIG_SDRAM_SIZE", "16");

	token_set_y("CONFIG_RG_WLAN_STA_STATISTICS_WBM");

	/* WLAN */
	token_set_y("CONFIG_80211G_AP_ADVANCED");
	token_set_y("CONFIG_RG_WPA_WBM");
	token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
	token_set_y("CONFIG_RG_8021X_WBM");

	if (!IS_DIST("WRT55AG_INT"))
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ae0", "vp0", "vp256", NULL);
	else
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "vp256", NULL);
    }
    else if (IS_DIST("AR2313_LIZZY") || IS_DIST("BOCHS_LIZZY"))
    {
	os = "VXWORKS";

	/* This distribution is FOUNDATION_CORE plus many features */
	token_set_y("CONFIG_RG_FOUNDATION_CORE");

    	/* DHCP Server */
	token_set_y("CONFIG_RG_DHCPS");

	/* Telnet Server */
	token_set_y("CONFIG_RG_TELNETS");

	/* Bridging */
	token_set_y("CONFIG_RG_BRIDGE");
	
	/* NAT/NAPT */	
	token_set_y("CONFIG_RG_NAT");

	/* ALG support */
	token_set_y("CONFIG_RG_ALG");
	token_set_y("CONFIG_RG_ALG_SIP");
	token_set_y("CONFIG_RG_ALG_H323");
	token_set_y("CONFIG_RG_ALG_AIM");
	token_set_y("CONFIG_RG_ALG_MSNMS");
	token_set_y("CONFIG_RG_ALG_PPTP");
	token_set_y("CONFIG_RG_ALG_IPSEC");
	token_set_y("CONFIG_RG_ALG_L2TP");
	token_set_y("CONFIG_RG_ALG_ICMP");
	token_set_y("CONFIG_RG_ALG_PORT_TRIGGER");
	token_set_y("CONFIG_RG_ALG_FTP");
	token_set_y("CONFIG_RG_ALG_RTSP");

	/* Firewall */
	token_set_y("CONFIG_RG_MSS");
	
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	/* Vlan */
	enable_module("MODULE_RG_VLAN");

	/* Additional features */
    	token_set_y("CONFIG_RG_DHCPR");
	token_set_y("CONFIG_RG_URL_KEYWORD_FILTER");

	if (IS_DIST("AR2313_LIZZY"))
	{
	    /* WLAN */
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	    token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
	    token_set_y("CONFIG_RG_WLAN_REPEATING");
	    token_set_y("CONFIG_RG_WDS_CONN_NOTIFIER");
	    token_set("CONFIG_RG_SSID_NAME", "wireless");
	}

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	if (IS_DIST("AR2313_LIZZY"))
	{
	    enable_module("CONFIG_HW_80211G_AR531X");
	    token_set_y("CONFIG_HW_BUTTONS");
	}

	token_set_y("CONFIG_RG_SSI_PAGES");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");
	token_set_y("CONFIG_RG_RMT_MNG");
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */

	/* Email notification */
	token_set_y("CONFIG_RG_ENTFY");

	/* Event Logging */
    	token_set_y("CONFIG_RG_EVENT_LOGGING");

	token_set("CONFIG_SDRAM_SIZE", "8");

	/* Statistics control */
	token_set_y("CONFIG_RG_STATISTICS_CTRL_WBM");
	token_set_y("CONFIG_RG_WLAN_STA_STATISTICS_WBM");

	if (IS_DIST("AR2313_LIZZY"))
	{
	    hw = "AR531X_G";

	    dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "vp256", NULL);
	}
	else /* BOCHS_LIZZY */
	{
	    hw = "I386_BOCHS";

	    dev_add_bridge("br0", DEV_IF_NET_INT, "ene0", "ene1", NULL);
	}
    }
    else if (IS_DIST("AR2313_ZIPPY") || IS_DIST("BOCHS_ZIPPY"))
    {
	os = "VXWORKS";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	/* Additional features */
    	token_set_y("CONFIG_RG_DHCPR");
	token_set_y("CONFIG_RG_URL_KEYWORD_FILTER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	if (IS_DIST("AR2313_ZIPPY"))
	{
	    enable_module("CONFIG_HW_80211G_AR531X");
	    token_set_y("CONFIG_HW_BUTTONS");
	}

	token_set_y("CONFIG_RG_SSI_PAGES");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");

	if (IS_DIST("AR2313_ZIPPY"))
	{
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	    token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
	    token_set_y("CONFIG_AR531X_DEBUG");
	}

	token_set("CONFIG_SDRAM_SIZE", "16");

	/* Statistics control */
	token_set_y("CONFIG_RG_STATISTICS_CTRL_WBM");
	token_set_y("CONFIG_RG_WLAN_STA_STATISTICS_WBM");

	if (IS_DIST("AR2313_ZIPPY"))
	{
	    hw = "AR531X_G";
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "vp256", NULL);
	} 
	else /* BOCHS_ZIPPY */
	{
	    hw = "I386_BOCHS";
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ene0", "ene1", NULL);
	}
    }
    else if (IS_DIST("ATHEROS_AR531X_VX"))
    {
	os = "VXWORKS";
	hw = "AR531X_G";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	token_set_y("CONFIG_RG_TFTP_UPGRADE");
	token_set_y("CONFIG_RG_TFTP_SERVER_PASSWORD");
	token_set_y("CONFIG_RG_ENTFY");	/* Email notification */
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */
	token_set_y("CONFIG_RG_URL_KEYWORD_FILTER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_80211G_AR531X");
	token_set_y("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
	token_set_y("CONFIG_RG_SSI_PAGES");
	token_set_y("CONFIG_AR531X_DEBUG");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");
	token_set("CONFIG_SDRAM_SIZE", "16");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "vp256", NULL);
    }
    else if (IS_DIST("WRT108G_VX"))
    {
	os = "VXWORKS";
	hw = "WRT108G";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	token_set_y("CONFIG_RG_TFTP_UPGRADE");
	token_set_y("CONFIG_RG_TFTP_SERVER_PASSWORD");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_80211G_AR531X");
	token_set_y("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_RG_RADIUS_WBM_IN_CONN");
	token_set_y("CONFIG_GUI_LINKSYS");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");

	token_set_y("CONFIG_RG_RGLOADER_CLI_CMD");
	dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "ar1", NULL);

	token_set("CONFIG_SDRAM_SIZE", "16");

	/* Features not ready */
#if 0
	token_set_y("CONFIG_RG_IGMP_PROXY");
	
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
#endif
    }
    else if (IS_DIST("TI_404_VX_EVAL") || IS_DIST("T_TI_404_VX") ||
    	IS_DIST("ARRIS_TI_404_VX"))
    {
	os = "VXWORKS";
	hw = "TI_404";

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_CABLE_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

    }
    else if (IS_DIST("T_TI_404_VX_CH") || IS_DIST("ARRIS_TI_404_VX_CH"))
    {
	os = "VXWORKS";
	hw = "TI_404";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_CABLEHOME");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_CABLE_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	if (IS_DIST("SMC_TI_404_VX_CH"))
	{
	    enable_module("MODULE_RG_UPNP");
	    enable_module("MODULE_RG_ADVANCED_ROUTING");
	}

	dev_add_bridge("br0", DEV_IF_NET_INT, "cbl0", "lan0", NULL);
    }
    else if (IS_DIST("TI_TNETC440_VX_CH") || IS_DIST("HITRON_TNETC440_VX_CH") ||
	IS_DIST("HITRON_TNETC440_VX"))
    {
	os = "VXWORKS";
	hw = "TI_404";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
    	token_set_y("CONFIG_RG_EVENT_LOGGING");	/* Event Logging */
	if (!IS_DIST("HITRON_TNETC440_VX"))
	    enable_module("MODULE_RG_CABLEHOME");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_CABLE_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	dev_add_bridge("br0", DEV_IF_NET_INT, "cbl0", "lan0", NULL);

	if (IS_DIST("HITRON_TNETC440_VX_CH") || IS_DIST("HITRON_TNETC440_VX"))
	    token_set_y("CONFIG_HITRON_BSP");
    }
    else if (IS_DIST("TI_404_VX_CH_EVAL"))
    {
	os = "VXWORKS";
	hw = "TI_404";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_CABLEHOME");
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_CABLE_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	dev_add_bridge("br0", DEV_IF_NET_INT, "cbl0", "lan0", NULL);
    }
    else if (IS_DIST("TI_WPA_VX"))
    {
	os = "VXWORKS";
	hw = "TI_TNETWA100";
	token_set_y("CONFIG_RG_WPA");
	token_set_y("CONFIG_RG_8021X_RADIUS");
	token_set_y("CONFIG_RG_OPENSSL");
    }
    else if (IS_DIST("I386_BOCHS_VX"))
    {
	os = "VXWORKS";
	hw = "I386_BOCHS";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ene0", "ene1", NULL);
    }
    else if (IS_DIST("ALLWELL_RTL_RTL_VALGRIND"))
    {
	hw = "ALLWELL_RTL_RTL";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_VALGRIND");
    }
    else if (IS_DIST("ALLWELL_RTL_RTL_ISL38XX"))
    {
	hw = "ALLWELL_RTL_RTL_ISL38XX";

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_80211G_ISL38XX");

	dev_add_bridge("br0", DEV_IF_NET_INT, "rtl1", "eth0", NULL);
    }
    else if (IS_DIST("RGLOADER_UML"))
    {
	hw = "UML";
	os = "LINUX_24";

	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_RG_TELNETS");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
    }
    else if (IS_DIST("JPKG_SRC"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"src\"");
	token_set_y("CONFIG_RG_JPKG_SRC");

	set_jpkg_dist_configs("JPKG_SRC");
    }
    else if (IS_DIST("JPKG_UML"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"uml\"");
        token_set_y("CONFIG_RG_JPKG_UML");

	set_jpkg_dist_configs("JPKG_UML");
    }
    else if (IS_DIST("JPKG_ARMV5B"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"armv5b\"");
	token_set_y("CONFIG_RG_JPKG_ARMV5B");
	    
	token_set_y("CONFIG_GLIBC");
	token_set_y("GLIBC_IN_TOOLCHAIN");

	set_jpkg_dist_configs("JPKG_ARMV5B");
    }
    else if (IS_DIST("JPKG_ARMV5L"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"armv5l\"");
        token_set_y("CONFIG_RG_JPKG_ARMV5L");

	set_jpkg_dist_configs("JPKG_ARMV5L");
    }
    else if (IS_DIST("JPKG_ARMV4L"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"armv4l\"");
        token_set_y("CONFIG_RG_JPKG_ARMV4L");

	set_jpkg_dist_configs("JPKG_ARMV4L");
    }
    else if (IS_DIST("JPKG_PPC"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"ppc\"");
        token_set_y("CONFIG_RG_JPKG_PPC");

	set_jpkg_dist_configs("JPKG_PPC");
    }
    else if (IS_DIST("JPKG_MIPSEB"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"mipseb\"");
        token_set_y("CONFIG_RG_JPKG_MIPSEB");

	set_jpkg_dist_configs("JPKG_MIPSEB");
    }
    else if (IS_DIST("JPKG_BCM9634X"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"bcm9634x\"");
        token_set_y("CONFIG_RG_JPKG_BCM9634X");

	set_jpkg_dist_configs("JPKG_BCM9634X");
    }
    else if (IS_DIST("JPKG_SB1250"))
    {
	hw = "JPKG";
	token_set("JPKG_ARCH", "\"sb1250\"");

	token_set_y("CONFIG_RG_JPKG_SB1250");

	set_jpkg_dist_configs("JPKG_SB1250");
    }
    else if (IS_DIST("JPKG_TWINPASS"))
    {
	hw = "JPKG";
	token_set("JPKG_ARCH", "\"twinpass\"");

	token_set_y("CONFIG_RG_JPKG_TWINPASS");

	set_jpkg_dist_configs("JPKG_TWINPASS");
    }    
    else if (IS_DIST("JPKG_ARM_920T_LE"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"arm-920t-le\"");
        token_set_y("CONFIG_RG_JPKG_ARM_920T_LE");

	set_jpkg_dist_configs("JPKG_ARM_920T_LE");
    }
    else if (IS_DIST("JPKG_LX4189"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"lx4189\"");
	token_set_y("CONFIG_RG_JPKG_LX4189");

	set_jpkg_dist_configs("JPKG_LX4189");
    }
    else if (IS_DIST("JPKG_LOCAL_I386"))
    {
	hw = NULL;
        token_set("JPKG_ARCH", "\"local-i386\"");
	token_set_y("CONFIG_RG_JPKG_LOCAL_I386");

	set_jpkg_dist_configs("JPKG_LOCAL_I386");
    }
    else if (IS_DIST("UML_LSP"))
    {
	hw = "UML";
	os = "LINUX_24";

	token_set_y("CONFIG_RG_UML");

	token_set_y("CONFIG_LSP_DIST");
    }
    else if (IS_DIST("UML_LSP_26"))
    {
	hw = "UML";
	os = "LINUX_26";

	token_set_y("CONFIG_RG_UML");
	
	token_set_y("CONFIG_GLIBC");

	token_set_y("CONFIG_LSP_DIST");
    }
    else if (IS_DIST("UML") || IS_DIST("UML_GLIBC") || IS_DIST("UML_DUAL_WAN")
	|| IS_DIST("UML_ATM") || IS_DIST("UML_26"))
    {
	hw = "UML";

	if (IS_DIST("UML_26"))
	    os = "LINUX_26";

	if (IS_DIST("UML_GLIBC"))
	{
	    token_set_y("CONFIG_GLIBC");
	    token_set_y("GLIBC_IN_TOOLCHAIN");
	}

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_TR_098");
	enable_module("MODULE_RG_TR_064");
	enable_module("MODULE_RG_JVM");
	enable_module("MODULE_RG_SSL_VPN");

	enable_module("MODULE_RG_MAIL_FILTER");
	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	if (IS_DIST("UML_DUAL_WAN"))
	    token_set_y("CONFIG_HW_ETH_WAN2");
	if (IS_DIST("UML_ATM"))
	{
	    enable_module("MODULE_RG_DSL");
	    token_set_y("CONFIG_HW_DSL_WAN");
	}
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_UML");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	dev_add_bridge("br0", DEV_IF_NET_INT, "eth1", "eth2", NULL);
    }
    else if (IS_DIST("UML_VALGRIND") || IS_DIST("UML_ATM_VALGRIND"))
    {
	hw = "UML";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_ROUTE_MULTIWAN");
	enable_module("MODULE_RG_SSL_VPN");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_TR_098");
	enable_module("MODULE_RG_MAIL_FILTER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	if (IS_DIST("UML_ATM_VALGRIND"))
	{
	    enable_module("MODULE_RG_DSL");
	    token_set_y("CONFIG_HW_DSL_WAN");
	}

	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_VALGRIND");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_UML");
    }
    else if (IS_DIST("UML_IPPHONE"))
    {
	hw = "UML";

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_DSP");

	/* VoIP */
	token_set_y("CONFIG_RG_IPPHONE");
	token_set_y("CONFIG_RG_VOIP_HW_EMULATION");
    }
    else if (IS_DIST("UML_IPPHONE_VALGRIND"))
    {
	hw = "UML";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_DSP");

	token_set_y("CONFIG_VALGRIND");

	/* VoIP */
	token_set_y("CONFIG_RG_IPPHONE");
	token_set_y("CONFIG_RG_VOIP_HW_EMULATION");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
    }
    else if (IS_DIST("UML_ATA") || IS_DIST("UML_ATA_OSIP") ||
	IS_DIST("UML_ATA_VALGRIND") || IS_DIST("UML_ATA_OSIP_VALGRIND"))
    {
	hw = "UML";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");

	if (IS_DIST("UML_ATA_OSIP") || IS_DIST("UML_ATA_OSIP_VALGRIND"))
	{
	    enable_module("MODULE_RG_VOIP_OSIP");
	}
	else
	{
	    enable_module("MODULE_RG_VOIP_ASTERISK_SIP");
	    enable_module("MODULE_RG_VOIP_ASTERISK_H323");
	    token_set_y("CONFIG_UCLIBCXX");
	    token_set_y("CONFIG_UML_BIG_FLASH");
	}

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_DSP");

	if (IS_DIST("UML_ATA_VALGRIND") || IS_DIST("UML_ATA_OSIP_VALGRIND"))
	    token_set_y("CONFIG_VALGRIND");

	/* VoIP */
	enable_module("MODULE_RG_ATA");
	token_set_y("CONFIG_RG_VOIP_HW_EMULATION");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
    }
    else if (IS_DIST("RGTV"))
    {
	hw = "PUNDIT";

	/* Basic Linux support */
	token_set_y("CONFIG_LSP_DIST");
	/* RGTV components are incompatible with ulibc */
	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RGTV");
	/* Kernel modules */
	token_set_m("CONFIG_SOUND_ICH");
	token_set_y("CONFIG_VIDEO_DEV");
	token_set_m("CONFIG_SMB_FS");
    	token_set_y("CONFIG_NLS");
    	token_set_y("CONFIG_NLS_DEFAULT");
	token_set_y("CONFIG_BLK_DEV_SIS5513");
	token_set_m("CONFIG_B44");
    }
    else if (IS_DIST("FULL"))
    {
	token_set_y("CONFIG_RG_SMB");

	/* All OpenRG available Modules */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_UPNP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
        enable_module("MODULE_RG_DSL");
        enable_module("MODULE_RG_FILESERVER");
        enable_module("MODULE_RG_PRINTSERVER");
        enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
        enable_module("MODULE_RG_CABLEHOME");
        enable_module("MODULE_RG_VODSL");
    }
    else
	conf_err("invalid DIST=%s\n", dist);

    if (hw && strcmp(hw, "JPKG") && !(*os))
	os = "LINUX_24";

    token_set("CONFIG_RG_DIST", dist);
}
