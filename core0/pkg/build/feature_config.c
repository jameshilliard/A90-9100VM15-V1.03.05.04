/****************************************************************************
 *
 * rg/pkg/build/feature_config.c
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
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>

#include "config_opt.h"
#include "create_config.h"

static void assert_dep(char *dependent, ...)
{
    va_list ap;
    char *token;

    if (!token_get(dependent))
	return;

    va_start(ap, dependent);
    while ((token = va_arg(ap, char *)))
    {
	if (!token_get(token))
	{
	    fprintf(stderr, "Failed dependency check: "
		"%s is dependent on %s.\n", dependent, token);
	    exit(-1);
	}
    }
    va_end(ap);
}

static int is_gui_selected(void)
{
    option_t *p;
    
    for (p = openrg_config_options; p->token; p++)
    {
	/* Return 1 if found any selected theme */
	if (p->value && p->type&OPT_THEME)
	    return 1;
    }
    return 0;
}

static void select_all_themes(void)
{
    option_t *p;
    
    for (p = openrg_config_options; p->token; p++)
    {
	/* temporary hack until B24373 is resolved */
	if (!strcmp(p->token, "CONFIG_GUI_BELKIN"))
	    continue;

	if (p->type&OPT_THEME)
	    token_set_y(p->token);
    }
}

static int is_dsp_configurable(void)
{
    return token_get("CONFIG_VINETIC") || token_get("CONFIG_IXP425_DSR") ||
	token_get("CONFIG_COMCERTO_COMMON");
}

void _set_big_endian(char *file, int line, int is_big)
{
    if (is_big)
	token_set_y("CONFIG_CPU_BIG_ENDIAN");
    else
	token_set_y("CONFIG_CPU_LITTLE_ENDIAN");
    _token_set(file, line, SET_PRIO_TOKEN_SET, "TARGET_ENDIANESS",
	is_big ? "BIG" : "LITTLE");
}

void package_features(void)
{
    /* OpenRG Package types */

    if (token_get("PACKAGE_OPENRG_SOHO_GATEWAY"))
    {
	token_set_y("MODULE_RG_FOUNDATION");
	token_set_y("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("MODULE_RG_SNMP");
	token_set_y("MODULE_RG_UPNP");
	token_set_y("MODULE_RG_ADVANCED_ROUTING");
	token_set_y("MODULE_RG_FIREWALL_AND_SECURITY");
        token_set_y("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	token_set_y("MODULE_RG_PPTP");
	token_set_y("MODULE_RG_IPSEC");
	token_set_y("MODULE_RG_L2TP");
        token_set_y("MODULE_RG_FILESERVER");
        token_set_y("MODULE_RG_PRINTSERVER");
    	token_set_y("CONFIG_RG_SMB");
    }

    if (token_get("PACKAGE_OPENRG_VPN_GATEWAY"))
    {
	token_set_y("MODULE_RG_FOUNDATION");
	token_set_y("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("MODULE_RG_SNMP");
	token_set_y("MODULE_RG_UPNP");
	token_set_y("MODULE_RG_ADVANCED_ROUTING");
	token_set_y("MODULE_RG_FIREWALL_AND_SECURITY");
        token_set_y("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	token_set_y("MODULE_RG_PPTP");
	token_set_y("MODULE_RG_IPSEC");
	token_set_y("MODULE_RG_L2TP");
    }

    if (token_get("PACKAGE_OPENRG_SECURITY_GATEWAY"))
    {
	token_set_y("MODULE_RG_FOUNDATION");
	token_set_y("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("MODULE_RG_SNMP");
	token_set_y("MODULE_RG_UPNP");
	token_set_y("MODULE_RG_ADVANCED_ROUTING");
	token_set_y("MODULE_RG_FIREWALL_AND_SECURITY");
        token_set_y("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* The following come by default 
	 * Static routing pages
	 * Multiple WAN devices
	 * ISP failover configuration comes with PPPoE existance
	 */
    }
    
    if (token_get("PACKAGE_OPENRG_BASIC_GATEWAY"))
    {
	token_set_y("MODULE_RG_FOUNDATION");
	token_set_y("MODULE_RG_ADVANCED_MANAGEMENT");
    }
}

void general_features(void)
{
    if (token_get("CONFIG_RG_MTD_DEFAULT_PARTITION") &&
	!token_get("CONFIG_RG_MTD"))
    {
	token_set_m("CONFIG_RG_MTD");
    }

    if (token_get("MODULE_RG_SNMP"))
    {
	token_set_y("CONFIG_RG_UCD_SNMP");
	token_set_y("CONFIG_RG_UCD_SNMP_V3");
    }

    if (token_get("MODULE_RG_UPNP"))
	token_set_y("CONFIG_RG_IGD");

    if (token_get("MODULE_RG_ADVANCED_ROUTING"))
    {
	token_set_y("CONFIG_RG_RIP");
	token_set_y("CONFIG_RG_DNS_CONCURRENT");
	if (!token_get("CONFIG_RG_OS_VXWORKS"))
	{
	    token_set_y("CONFIG_RG_DNS_DOMAIN_ROUTING");
	    token_set_y("CONFIG_RG_TUNNELS");
	}
    }

    if (token_get("CONFIG_RG_TUNNELS"))
    {
	token_set_y("CONFIG_INET_TUNNEL");
	token_set_y("CONFIG_NET_IPIP");
	token_set_y("CONFIG_NET_IPGRE");
    }

    if (token_get("CONFIG_RG_DNS_DOMAIN_ROUTING"))
	token_set_y("CONFIG_RG_DNS_ROUTE");

    if (token_get("MODULE_RG_FIREWALL_AND_SECURITY"))
    {
	token_set_y("CONFIG_RG_RNAT");
	token_set_y("CONFIG_RG_FIREWALL");

	/* Time of Day Client, SNTP Client */
	token_set_y("CONFIG_RG_TODC");
    }

    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
    {
	if (!token_get("CONFIG_RG_VENDOR_WLAN_SEC"))
	{
	    token_set_y("CONFIG_RG_8021X_MD5");
	    token_set_y("CONFIG_RG_8021X_TLS");
	    token_set_y("CONFIG_RG_8021X_TTLS");
	    token_set_y("CONFIG_RG_8021X_RADIUS");
	    token_set_y("CONFIG_RG_WPA");
	    token_set_y("CONFIG_RG_WEP");
	}
	if (!token_get_str("CONFIG_RG_SSID_NAME"))
	    token_set("CONFIG_RG_SSID_NAME", "openrg");
    }

    if (token_get("MODULE_RG_CABLEHOME"))
    {
	token_set_y("CONFIG_RG_CABLEHOME_11");
	token_set_y("CONFIG_RG_CABLEHOME");
	token_set_y("CONFIG_RG_CABLEHOME_EMBEDDED");
    }

    if (token_get("MODULE_RG_BLUETOOTH"))
	token_set_y("CONFIG_RG_BLUETOOTH");

    token_set_y("CONFIG_RG_LICENSE");
    
    if (token_get("MODULE_RG_FOUNDATION") || token_get("CONFIG_RG_RGLOADER"))
    {
	if (!token_get("CONFIG_RG_OS_VXWORKS"))
	    token_set_y("CONFIG_RG_IPROUTE2");
    }
	
    if (token_get("MODULE_RG_FOUNDATION"))
    {
	token_set_y("CONFIG_RG_FOUNDATION_CORE");

	token_set_y("CONFIG_RG_CHECK_BAD_REBOOTS");

    	/* DHCP Server */
	token_set_y("CONFIG_RG_DHCPS");
	token_set_y("CONFIG_AUTO_LEARN_DNS");

    	/* DHCP Relay */
    	token_set_y("CONFIG_RG_DHCPR");

	/* Telnet Server */
	token_set_y("CONFIG_RG_TELNETS");

	/* Bridging */
	token_set_m("CONFIG_RG_BRIDGE");
	
	/* NAT/NAPT */	
	token_set_y("CONFIG_RG_NAT");

	/* ALG support */
	token_set_y("CONFIG_RG_ALG");
	token_set_y("CONFIG_RG_ALG_SIP");
	token_set_y("CONFIG_RG_ALG_MGCP");
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

	token_set_m("CONFIG_RG_PPPOE_RELAY");

	/* Firewall */
	token_set_m("CONFIG_RG_MSS");
	
//#if 0 /* WSTL needed by CONFIG_WESTELL_MGMUT_FTM*/
	/* Remote management */
	token_set_y("CONFIG_RG_RMT_MNG");
	token_set_y("CONFIG_RG_OSS_RMT");
//#endif
	
	/* various */ 
	if (!token_get("CONFIG_RG_OS_VXWORKS"))
	{
	    token_set_y("CONFIG_RG_STATIC_ROUTE");
	    if (!token_get("CONFIG_RG_CABLEHOME_11") && !token_get("CONFIG_ATHEROS_ART_TOOLS"))
		token_set_m("CONFIG_RG_IGMP_PROXY");
	}	
	
	/* Remove CONFIG_BCM963XX_COMMON restriction when KGDB is implemented
	 */
	if (!token_get("CONFIG_KS8695_COMMON") &&
	    !token_get("CONFIG_BCM963XX_COMMON"))
	{
	    token_set_y("CONFIG_RG_KGDB");
	}
	token_set_y("CONFIG_RG_PING");
	token_set_y("CONFIG_RG_TRACEROUTE");

	/* enable full service list to platforms with no memory limitations */
	if (!token_get("CONFIG_RG_ADV_SERVICES_LEVEL"))
	    token_set("CONFIG_RG_ADV_SERVICES_LEVEL", "2");

	/* Multicast Relay */
	if (!token_get("CONFIG_RG_CABLEHOME_11") && 
	    token_get("CONFIG_RG_OS_VXWORKS"))
	{
	    token_set_y("CONFIG_RG_MCAST_RELAY");
	}

	/* Set DNS+DHCP to no-cablehome distributions (B11715) */
	if (!token_get("CONFIG_RG_CABLEHOME_11"))
	{
	    token_set_y("CONFIG_RG_ALG_DNS");
	    token_set_y("CONFIG_RG_ALG_DHCP");
	}

	/* WBM */
	token_set_y("CONFIG_RG_WBM");
	token_set_y("CONFIG_RG_TZ_COMMON");

	/* Auto IP Configuration */
	token_set_y("CONFIG_ZC_IP_AUTO_DETECTION");

	/* HTTP interception */
#if 0 // XXX seems to crash
	token_set_y("CONFIG_RG_UNCONFIGURED_WAN_INTERCEPTION");
#endif

#if 0 /* WSTL */
	/* Jnet client */
	token_set_y("CONFIG_RG_JNET_CLIENT");
#endif
    }

    if (token_get("CONFIG_RG_FOUNDATION_CORE"))
    {
	token_set_y("CONFIG_OPENRG");

	/* Set general flag for languages support */
	token_set_y("CONFIG_RG_LANG");
	/* Defines which languages to be compiled. The languages name should 
	 * be separated with spaces, ex: "DEF es ja"
	 * If left out, all languages in csv files will be compiled.
	 * For example:
	 * token_set("CONFIG_RG_LANGUAGES", "DEF es ja"); */

	token_set_m("CONFIG_RG_KOS");
	token_set_y("CONFIG_RG_KOS_KNET");
	
	/* Command Line Interface (CLI) */
	token_set_y("CONFIG_RG_CLI");
	token_set_y("CONFIG_RG_RGLOADER_CLI_CMD");
	token_set_y("CONFIG_RG_LOGIN");
	if (!token_get("CONFIG_RG_OS_VXWORKS"))
	    token_set_y("CONFIG_RGLOADER_CLI");
	token_set_y("CONFIG_RG_CONFIG_STRINGS");

	/* DNS Server */ 
	token_set_y("CONFIG_RG_DNS");
	token_set_y("CONFIG_RG_ERESOLV");

	/* Time of Day Client, SNTP Client */
	token_set_y("CONFIG_RG_TODC");

#if 1 /* WSTL (CR12193 enable Dynamic DNS) */
	/* Dynamic DNS */
	token_set_y("CONFIG_RG_DDNS");
#endif

	/* DHCP Client */
    	token_set_y("CONFIG_RG_DHCPC");

	if (!token_get("CONFIG_HW_CABLE_WAN"))
	{
	    /* In Cable modems upgrade is done via DOCSIS */
	    token_set_y("CONFIG_RG_LAN_UPGRADE");
	    token_set_y("CONFIG_RG_WAN_UPGRADE");
	    token_set_y("CONFIG_RG_RMT_UPDATE");
	}

	if (!token_get("CONFIG_RG_OS_VXWORKS"))
	    token_set_y("CONFIG_RG_SYSINIT");
	
	if (!token_get("CONFIG_I386_BOCHS"))
	    token_set_y("CONFIG_RG_PERM_STORAGE");
	
	token_set_y("CONFIG_RG_CHRDEV");

	/* various */ 
	if (!token_get("CONFIG_RG_OS_VXWORKS"))
	    token_set_y("CONFIG_RG_BUSYBOX");

	/* MAC cloning */
	token_set_y("CONFIG_RG_MAC_CLONING");

	/* IP V4 */
	token_set_m("CONFIG_RG_IPV4");
	
	/* The following are implicit in the system and are not mentioned
	 * here explicitly:
	 * Spanning tree protocol
	 * TFTP
	 * Serial access
	 * Autolearning of MAC addresses
	 * Multiple user support
	 * Multilingual support
	 * Connection wizard setup
	 * Connection diagnostics
	 */
    }

    if (token_get("MODULE_RG_ADVANCED_MANAGEMENT"))
    {
	/* OpenRG Advanced Management */

	/* Secured transmission Secured Login; HTTP-S; Telnet-S */
	token_set_y("CONFIG_RG_SSL");

#if 0 /* WSTL */
	/* Email notification */
	token_set_y("CONFIG_RG_ENTFY");
#endif
	
	/* Event Logging */
    	token_set_y("CONFIG_RG_EVENT_LOGGING");

#if 0
	/* SSH */
	token_set_y("CONFIG_RG_SSH");
#endif
    }

    if (token_get("MODULE_RG_PPTP"))
    {
	token_set_y("CONFIG_RG_PPTPC");
	token_set_y("CONFIG_RG_PPTPS");
	token_set_y("CONFIG_RG_PPP_MPPE");
	token_set_m("CONFIG_RG_PPP");
    }

    if (token_get("MODULE_RG_PPP"))
    {
	token_set_m("CONFIG_RG_PPP");
	token_set_y("CONFIG_RG_PPPOE");
	token_set_y("CONFIG_RG_PPPOS");

	if (!token_get("CONFIG_RG_OS_VXWORKS"))
	{
	    token_set_y("CONFIG_RG_PPP_DEFLATE");
	    token_set_y("CONFIG_RG_PPP_BSDCOMP");
	}

	if (token_get("CONFIG_ATM") && !token_get("CONFIG_RG_OS_LINUX_26"))
	    token_set_m("CONFIG_RG_PPPOA");

	if (!token_get_str("CONFIG_RG_PPP_ON_DEMAND_DEFAULT_MAX_IDLE_TIME"))
	{
	    token_set("CONFIG_RG_PPP_ON_DEMAND_DEFAULT_MAX_IDLE_TIME",
		"1200");
	}
	if (!token_get_str("CONFIG_RG_PPP_DEFAULT_BSD_COMPRESSION"))
	{
	    /* XXX Originally the value here was '0' (PPP_COMP_REJECT),
	     * but the generated rg_config.h file printed it as '1'
	     * (PPP_COMP_ALLOW), due to a bug in create_config.c.
	     */
	    /* from include/enums.h PPP_COMP_ALLOW is 1 */
	    token_set("CONFIG_RG_PPP_DEFAULT_BSD_COMPRESSION", "1");
	}
	if (!token_get_str("CONFIG_RG_PPP_DEFAULT_DEFLATE_COMPRESSION"))
	{
	    /* XXX Originally the value here was '0' (PPP_COMP_REJECT),
	     * but the generated rg_config.h file printed it as '1'
	     * (PPP_COMP_ALLOW), due to a bug in create_config.c.
	     */
	    /* from include/enums.h PPP_COMP_ALLOW is 1 */
	    token_set("CONFIG_RG_PPP_DEFAULT_DEFLATE_COMPRESSION", "1");
	}
    }

    if (token_get("MODULE_RG_DSL"))
    {
	token_set_y("CONFIG_ATM");
	token_set_y("CONFIG_ATM_PVC_SCAN");

	if (!token_get("CONFIG_ARCH_SOLOS"))
	    token_set_y("CONFIG_RG_OAM_PING");

	token_set_y("CONFIG_ATM_SKB");
	token_set_y("CONFIG_ATM_BR2684");
	if (token_get("CONFIG_RG_PPP"))
	    token_set_m("CONFIG_RG_PPPOA");
	if (!token_get("CONFIG_RG_OS_LINUX_26"))
	    token_set_y("CONFIG_ATM_CLIP");
    }


    if (token_get("MODULE_RG_IPV6"))
	token_set_y("CONFIG_RG_IPV6");

    if (token_get("MODULE_RG_VLAN"))
    {
	if (!token_get("CONFIG_HW_SWITCH_LAN") &&
	    !token_get("CONFIG_HW_SWITCH_WAN"))
	{
	    token_set_y("CONFIG_RG_VLAN_BRIDGE");
	}

	if (!token_get("CONFIG_RG_OS_VXWORKS"))
	    token_set_y("CONFIG_RG_8021Q_IF");
    }
    
    if (token_get("MODULE_RG_PRINTSERVER"))
    {
	token_set_y("CONFIG_RG_PRINT_SERVER");
	token_set_y("CONFIG_RG_IPP");
	token_set_y("CONFIG_RG_LPD");
    }

    if (token_get("MODULE_RG_ADVANCED_OFFICE_SERVERS"))
    {
	enable_module("CONFIG_RG_MAIL_SERVER");
	enable_module("CONFIG_RG_WEB_SERVER");
	enable_module("CONFIG_RG_FTP_SERVER");
	token_set_y("CONFIG_RG_DISK_MNG");
    }

    if (token_get("MODULE_RG_FILESERVER"))
    {
	enable_module("CONFIG_RG_FS_BACKUP");
	 // enable_module("CONFIG_RG_RAID");  DISABLE RAID 
	token_set_y("CONFIG_RG_DISK_MNG");
	token_set_y("CONFIG_RG_FILESERVER");
	token_set_y("CONFIG_RG_SAMBA");

	if (token_get("MODULE_RG_PRINTSERVER"))
	    token_set_y("CONFIG_RG_SAMBA_PRINTING");

	if (token_get("MODULE_RG_ADVANCED_OFFICE_SERVERS"))
	{
	    token_set_y("CONFIG_RG_WINS_SERVER");
	    token_set_y("CONFIG_RG_FILESERVER_ACLS");
	}

	/* ACL kernel support */
	if (token_get("CONFIG_RG_FILESERVER_ACLS"))
	{
	    token_set_y("CONFIG_FS_POSIX_ACL");

	    /* ACLs for EXT2 */
	    token_set_y("CONFIG_EXT2_FS_POSIX_ACL");
	    token_set_y("CONFIG_EXT2_FS_XATTR");
	    token_set_y("CONFIG_EXT2_FS_XATTR_SHARING");

	    /* ACLs for EXT3 */
	    token_set_y("CONFIG_EXT3_FS_POSIX_ACL");
	    token_set_y("CONFIG_EXT3_FS_XATTR");
	    token_set_y("CONFIG_EXT3_FS_XATTR_SHARING");

	    /* Filesystem Meta Information Block Cache */
	    token_set_y("CONFIG_FS_MBCACHE");
	}
    }

    if (token_get("CONFIG_RG_DISK_MNG"))
    {
	token_set_y("CONFIG_RG_STORAGE");
	token_set_y("CONFIG_EXT3_FS");
	token_set_y("CONFIG_JBD");

	if (token_get("CONFIG_RG_JPKG") ||
	    (!token_get("CONFIG_HW_FIREWIRE_STORAGE") && 
	    !token_get("CONFIG_HW_USB_STORAGE") && !token_get("CONFIG_RG_UML")))
	{
	    token_set_y("CONFIG_FILESERVER_KERNEL_CONFIG");
	}

	if (token_get("CONFIG_RG_RAID"))
	{
	    token_set_y("CONFIG_MD");
	    token_set_y("CONFIG_BLK_DEV_MD");
	    token_set_y("CONFIG_MD_RAID0");
	    token_set_y("CONFIG_MD_RAID1");
	    token_set_y("CONFIG_MD_RAID5");
	}
    }

    if (token_get("MODULE_RG_IPSEC"))
	token_set_y("CONFIG_FREESWAN");

    if (token_get("MODULE_RG_L2TP"))
    {
	token_set_y("CONFIG_RG_L2TPC");
	token_set_y("CONFIG_RG_L2TPS");
	token_set_y("CONFIG_RG_PPP_MPPE");
	token_set_m("CONFIG_RG_PPP");
    }

    if (token_get("MODULE_RG_ATA"))
	token_set_y("CONFIG_RG_ATA");

    if (token_get("MODULE_RG_PBX"))
	token_set_y("CONFIG_RG_PBX");

    if (token_get("MODULE_RG_VOIP_RV_SIP"))
	token_set_y("CONFIG_RG_VOIP_RV_SIP");

    if (token_get("MODULE_RG_VOIP_RV_H323"))
	token_set_y("CONFIG_RG_VOIP_RV_H323");

    if (token_get("MODULE_RG_VOIP_RV_MGCP"))
	token_set_y("CONFIG_RG_VOIP_RV_MGCP");

    if (token_get("MODULE_RG_VOIP_ASTERISK_SIP"))
	token_set_y("CONFIG_RG_VOIP_ASTERISK_SIP");

    if (token_get("MODULE_RG_VOIP_ASTERISK_H323"))
	token_set_y("CONFIG_RG_VOIP_ASTERISK_H323");

    if (token_get("MODULE_RG_VOIP_ASTERISK_MGCP_GW"))
	token_set_y("CONFIG_RG_VOIP_ASTERISK_MGCP_GW");

    if (token_get("MODULE_RG_VOIP_ASTERISK_MGCP_CALL_AGENT"))
	token_set_y("CONFIG_RG_VOIP_ASTERISK_MGCP_CALL_AGENT");

    if (token_get("MODULE_RG_VOIP_OSIP"))
	token_set_y("CONFIG_RG_VOIP_OSIP");
    
    if (token_get("MODULE_RG_URL_FILTERING"))
    {
        token_set_y("CONFIG_RG_SURFCONTROL");
	token_set_y("CONFIG_RG_URL_KEYWORD_FILTER");
    }
    
    if (token_get("MODULE_RG_QOS"))
        token_set_y("CONFIG_RG_QOS");

    if (token_get("MODULE_RG_ROUTE_MULTIWAN"))
	token_set_y("CONFIG_RG_ROUTE_MULTIWAN");
    
    if (token_get("MODULE_RG_DSLHOME"))
    {
        token_set_y("CONFIG_RG_DSLHOME");
        token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");  // WESTELL ADDITION

	/* Enable vouchers for large Flash */
	if (token_get("CONFIG_RG_JPKG") || 
	    token_get("CONFIG_RG_ADV_SERVICES_LEVEL") > 1)
	{
	    token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");
	}
    }

    if (token_get("MODULE_RG_TR_098"))
    {
	assert_dep("MODULE_RG_TR_098", "CONFIG_RG_DSLHOME", "CONFIG_RG_QOS", 
	    NULL);
	token_set_y("CONFIG_RG_TR_098");
    }

    if (token_get("CONFIG_RG_DSLHOME_VOUCHERS"))
    {
	assert_dep("CONFIG_RG_DSLHOME_VOUCHERS", "CONFIG_RG_DSLHOME", NULL);
	token_set_y("CONFIG_RG_OPTION_MANAGER");
    }

    if (token_get("CONFIG_RG_OPTION_MANAGER"))
    {
	token_set_y("CONFIG_RG_XMLSEC");
	token_set_y("CONFIG_RG_XML2");
    }

    if (token_get("MODULE_RG_MAIL_FILTER"))
        token_set_y("CONFIG_RG_MAIL_FILTER");
    
    if (token_get("MODULE_RG_ZERO_CONFIGURATION_NETWORKING"))
	token_set_y("CONFIG_ZERO_CONFIGURATION");
    
    if (token_get("MODULE_RG_TR_064"))
        token_set_y("CONFIG_RG_TR_064");

    if (token_get("MODULE_RG_JVM"))
        token_set_y("CONFIG_RG_KAFFE");

    if (token_get("MODULE_RG_SSL_VPN"))
    {
        token_set_y("CONFIG_RG_SSL_VPN");
	token_set_y("CONFIG_RG_XML");
	/* Web-CIFS adds samba to the distribution, which takes a lot of space
	 * on the flash. Therefore, in distributions with a small flash size, we
	 * avoid adding this feature */
	if (!token_get("CONFIG_RG_SSL_VPN_SMALL_FLASH"))
	    token_set_y("CONFIG_RG_WEB_CIFS");
    }

    /* This is the temporary location for new features which are yet not
     * categorized into a marketing priced module - features should NOT 
     * live here forever!! 
     */
    if (token_get("CONFIG_NEW_NONE_PRICED_FEATURES"))
    {
	token_set_y("CONFIG_RG_PROXY_ARP");
	token_set_y("CONFIG_RG_TFTP_UPGRADE");
	token_set_y("CONFIG_RG_SSI_PAGES");
    }

    if (token_get("CONFIG_RG_GDBSERVER"))
	token_set_y("CONFIG_RG_THREADS");

    if (token_get("CONFIG_RG_IPROUTE2"))
    {
	/* OpenRG is an advanced router by default, just making sure */
	token_set_y("CONFIG_IP_ADVANCED_ROUTER");
	/* Equal cost multipath */
	token_set_y("CONFIG_IP_ROUTE_MULTIPATH");
	/* Verbose route monitoring */
	token_set_y("CONFIG_IP_ROUTE_VERBOSE");
	/* Policy routing */
	token_set_y("CONFIG_IP_MULTIPLE_TABLES");
	/* Use TOS value as routing key */
	token_set_y("CONFIG_IP_ROUTE_TOS");

	if (token_get("CONFIG_RG_ROUTE_MULTIWAN"))
	{
	    /* Routing according DSCP field in IP header */
	    token_set_y("CONFIG_RG_DSCP_POLICY_ROUTING");
	    /* Load balancing */
	    token_set_y("CONFIG_RG_LOAD_BALANCING");
	    /* Fail over */
	    token_set_y("CONFIG_RG_FAILOVER");
	}

	token_set_y("CONFIG_ULIBC_DO_C99_MATH");
    }

    if (token_get("CONFIG_RG_IPROUTE2") && token_get("CONFIG_RG_QOS"))
    {
	token_set_y("CONFIG_TOS_DSCP_RFC2474");
	/* Network packet filtering */
	token_set_y("CONFIG_NETFILTER");
	/* QoS and/or fair queueing */
	token_set_y("CONFIG_NET_SCHED");

	/* HTB packet scheduler */
	token_set_y("CONFIG_NET_SCH_HTB");
	/* Ingress queueing discipline */
	token_set_y("CONFIG_NET_SCH_INGRESS");
	/* The simplest PRIO pseudoscheduler */
	token_set_y("CONFIG_NET_SCH_PRIO");
	/* The simplest RED pseudoscheduler */
	token_set_y("CONFIG_NET_SCH_RED");
	/* The simplest SFQ pseudoscheduler */
	token_set_y("CONFIG_NET_SCH_SFQ");
	/* Diffserv field marker */
	token_set_y("CONFIG_NET_SCH_DSMARK");
	/* QoS support */
	token_set_y("CONFIG_NET_QOS");
	/* Rate estimator */
	token_set_y("CONFIG_NET_ESTIMATOR");
	/* Packet classifier API */
	token_set_y("CONFIG_NET_CLS");
	/* Traffic policing */
	token_set_y("CONFIG_NET_CLS_POLICE");
	/* TC index classifier */
	token_set_y("CONFIG_NET_CLS_TCINDEX");
	/* Firewall based classifier */
	token_set_y("CONFIG_NET_CLS_FW");
	/* U32 classifier */
	token_set_y("CONFIG_NET_CLS_U32");
	/* Special RSVP classifier */
	token_set_y("CONFIG_NET_CLS_RSVP");
	/* Special RSVP classifier for IPv6 */
	token_set_y("CONFIG_NET_CLS_RSVP6");

	/* ATM packet pseudoscheduler */
	if (token_get("CONFIG_ATM"))
	    token_set_y("CONFIG_NET_SCH_ATM");

	/* Use netfilter MARK value as routing key */
	token_set_y("CONFIG_IP_ROUTE_FWMARK");

	/* ingress qdisc support */
	token_set_m("CONFIG_RG_QOS_INGRESS");

	if (token_get("CONFIG_RG_VLAN_BRIDGE") ||
	    token_get("CONFIG_RG_VLAN_8021Q"))
	{
	    token_set_y("CONFIG_RG_VLAN_DSCP");
	}
    }
    
    /* ============= END of Module definition section ============= */

    if (token_get("CONFIG_RG_WEB_CIFS"))
	token_set_y("CONFIG_RG_SAMBA");

    if (token_get("CONFIG_RG_MAIL_SERVER"))
	token_set_y("CONFIG_RG_THREADS");

    if (token_get("CONFIG_LSP_DIST"))
	token_set_y("CONFIG_RG_BUSYBOX");

    if (token_get("CONFIG_IPTABLES"))
    {
	token_set_y("CONFIG_NETFILTER");
	token_set_y("CONFIG_IP_NF_FILTER");
	token_set_y("CONFIG_IP_NF_IPTABLES");
	token_set_y("CONFIG_IP_NF_CONNTRACK");
	token_set_y("CONFIG_IP_NF_NAT_NEEDED");
	token_set_y("CONFIG_IP_NF_NAT");
	token_set_y("CONFIG_IP_NF_NAT_LOCAL");
	token_set_y("CONFIG_IP_NF_TARGET_MASQUERADE");
	token_set_y("CONFIG_IP_NF_CT_ACCT");
	token_set_y("CONFIG_IP_NF_CONNTRACK_MARK");
	token_set_y("CONFIG_NETFILTER_XTABLES");
	token_set_y("CONFIG_NETFILTER_XT_TARGET_CLASSIFY");
	token_set_y("CONFIG_NETFILTER_XT_TARGET_CONNMARK");
	token_set_y("CONFIG_NETFILTER_XT_TARGET_MARK");
	token_set_y("CONFIG_NETFILTER_XT_TARGET_NFQUEUE");
	token_set_y("CONFIG_NETFILTER_XT_TARGET_NOTRACK");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_COMMENT");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_CONNBYTES");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_CONNMARK");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_CONNTRACK");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_DCCP");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_HELPER");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_LENGTH");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_LIMIT");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_MAC");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_MARK");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_PKTTYPE");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_REALM");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_SCTP");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_STATE");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_STRING");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_TCPMSS");
	token_set_y("CONFIG_NETFILTER_XT_MATCH_PHYSDEV");
	token_set_y("CONFIG_NET_CLS_ROUTE");
	token_set_y("CONFIG_BRIDGE_NETFILTER");
	token_set_y("CONFIG_TEXTSEARCH");
    }
    
    if (!token_get_str("CONFIG_RG_CONSOLE_DEVICE"))
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
    
    if (token_get("CONFIG_RG_RADIUS_LOGIN_AUTH") ||
	token_get("CONFIG_RG_8021X_RADIUS"))
    {
	token_set_y("CONFIG_RG_RADIUS");
    }

    if (token_get("CONFIG_RG_PRINT_SERVER"))
    {
	if (token_get("CONFIG_RG_UML"))
	    token_set_y("CONFIG_RG_PRINT_SERVER_PRINTER_NULL");
	else
	    token_set_y("CONFIG_USB_PRINTER");

	if (!token_get("CONFIG_RG_PRINT_SERVER_SPOOL"))
	    token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "0");
    }

    if (!token_get("CONFIG_RG_DEV") && token_get("CONFIG_RG_KOS_KNET"))
	token_set_m("CONFIG_RG_ONE_MODULE");
    /* debug. kernel larger and slower */
    if (token_get("CONFIG_RG_DEV"))
    {
	if (!token_get("CONFIG_ARMNOMMU"))
	    token_set_y("CONFIG_FRAME_POINTER");
	token_set_y("CONFIG_KALLSYMS");
	token_set_y("CONFIG_DEBUG_SLAB");
	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    token_set_y("CONFIG_DEBUG_SPINLOCK_SLEEP");
	    token_set_y("CONFIG_DEBUG_INFO");
	}
	token_set_y("CONFIG_RG_IPROUTE2_UTILS");
	token_set_y("CONFIG_RG_WIRELESS_TOOLS"); 		
    }

    if (IS_DIST("JPKG_I386"))
    {
	token_set("ARCH", "i386");
	token_set("CONFIG_X86_L1_CACHE_SHIFT", "4");
	token_set("LIBC_ARCH", "i386");
	token_set_y("CONFIG_M386");
	token_set_y("CONFIG_FLASH_IN_FILE");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_DYN_LINK"); /* XXX we need also to support static */
	set_big_endian(0);
	token_set_y("CONFIG_HW_CLOCK");
	token_set_y("CONFIG_PROC_FS");
	token_set_y("CONFIG_EXT2_FS");
	token_set_y("CONFIG_BINFMT_ELF");
    }

    if (IS_DIST("JPKG_UML"))
    {
	token_set("ARCH", "um");
	token_set("CONFIG_X86_L1_CACHE_SHIFT", "4");
	token_set_y("CONFIG_RG_GDBSERVER");
	token_set_m("CONFIG_RG_KRGLDR");
	token_set("LIBC_ARCH", "i386");
	token_set_y("CONFIG_M386");
	token_set_y("CONFIG_FLASH_IN_FILE");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_DYN_LINK"); /* XXX we need also to support static */
	set_big_endian(0);
	token_set_y("CONFIG_HW_CLOCK");
	token_set_y("CONFIG_PROC_FS");
	token_set_y("CONFIG_EXT2_FS");
	token_set_y("CONFIG_BINFMT_ELF");
	
	/* XXX hardware specific?
	 * taken from hw_config.c */
 	token_set("CONFIG_KERNEL_STACK_ORDER", "2");
	token_set_y("CONFIG_RAMFS");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set("CONFIG_RAMDISK_SIZE", "65536");
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
	token_set_y("CONFIG_BLK_DEV_UBD");
	token_set_y("CONFIG_BLK_DEV_NBD");
	token_set("CONFIG_BLK_DEV_RAM_SIZE", "8192");
	token_set_y("CONFIG_RG_INITFS_RAMDISK");
	token_set_y("CONFIG_UML_NET");
	token_set_y("CONFIG_UML_NET_ETHERTAP");
	token_set_y("CONFIG_UML_NET_TUNTAP");
	token_set_y("CONFIG_UML_NET_SLIP");
	token_set_y("CONFIG_UML_NET_SLIRP");
	token_set_y("CONFIG_UML_NET_DAEMON");
	token_set_y("CONFIG_UML_NET_MCAST");
	token_set_y("CONFIG_DUMMY");
	token_set_y("CONFIG_TUN");
	token_set_y("CONFIG_USERMODE");
	token_set_y("CONFIG_UID16");
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_BSD_PROCESS_ACCT");
	token_set_y("CONFIG_HOSTFS");
	token_set_y("CONFIG_HPPFS");
	token_set_y("CONFIG_MCONSOLE");
	token_set_y("CONFIG_MAGIC_SYSRQ");
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
   }

    /* Platforms */
    if (token_get("CONFIG_RG_UML"))
    {
	token_set("BOARD", "UML");
	token_set_m("CONFIG_RG_KRGLDR");
	token_set_y("CONFIG_RG_GDBSERVER");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	if (token_get("CONFIG_VALGRIND"))
	    token_set_y("CONFIG_UML_BIG_FLASH");

	if ((os && !strcmp(os, "LINUX_26")))
	{
	    token_set_y("CONFIG_UID16");
	    token_set_y("CONFIG_SYSVIPC");
	    token_set_y("CONFIG_CLEAN_COMPILE");
	    token_set_y("CONFIG_BROKEN_ON_SMP");
	    token_set_y("CONFIG_BASE_FULL");
	    token_set_y("CONFIG_FUTEX");
	    token_set_y("CONFIG_EPOLL");
	    token_set_y("CONFIG_X86_XADD");
	    token_set_y("CONFIG_BINFMT_MISC");
	    token_set_y("CONFIG_STANDALONE");
	    token_set_y("CONFIG_PREVENT_FIRMWARE_BUILD");
	    token_set("CONFIG_BLK_DEV_RAM_COUNT", "16");
	    token_set("CONFIG_NET_IPGRE", "m");
	    token_set_y("CONFIG_NET_IPGRE_BROADCAST");
	    token_set_y("CONFIG_ARCH_HAS_SC_SIGNALS");
	    token_set("CONFIG_TOP_ADDR", "0xc0000000");
	    token_set_y("CONFIG_ARCH_REUSE_HOST_VSYSCALL_AREA");
	    token_set_y("CONFIG_LD_SCRIPT_STATIC");
	    token_set_y("CONFIG_UML_REAL_TIME_CLOCK");
	    token_set_y("CONFIG_NOCONFIG_CHAN");
	    token_set_y("CONFIG_BLK_DEV_COW_COMMON");
	    token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	    token_set_y("CONFIG_QUOTA");
	    token_set_y("CONFIG_QUOTACTL");
	    token_set_y("CONFIG_RG_TTYP");
            token_set("CONFIG_X86_L1_CACHE_SHIFT", "4");

	    /* linux-2.6.16.14 */
	    token_set_y("CONFIG_SWAP");
	    token_set_y("CONFIG_IRQ_RELEASE_METHOD");
	    token_set("UML_CONFIG_STUB_DATA", "0xbffff000");
	    token_set("CONFIG_STUB_DATA", "0xbffff000");
	    token_set("CONFIG_STUB_START", "0xbfffe000");
	    token_set("CONFIG_STUB_CODE", "0xbfffe000");

	    token_set_y("CONFIG_M386");
	    token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	    token_set_y("CONFIG_X86_PPRO_FENCE");
	    token_set_y("CONFIG_X86_F00F_BUG");
	    token_set_y("CONFIG_SEMAPHORE_SLEEPERS");
	    token_set_y("CONFIG_SELECT_MEMORY_MODEL");
	    token_set_y("CONFIG_FLATMEM_MANUAL");
	    token_set_y("CONFIG_FLATMEM");
	    token_set_y("CONFIG_FLAT_NODE_MEM_MAP");
	    token_set_y("CONFIG_FORCED_INLINING");
	    token_set_y("CONFIG_UID16");
	    token_set_y("CONFIG_CC_OPTIMIZE_FOR_SIZE");
	    token_set_y("CONFIG_DEBUG_MUTEXES");
	    token_set_y("CONFIG_DEFAULT_AS");
	    token_set_y("CONFIG_DETECT_SOFTLOCKUP");
	    token_set_y("CONFIG_INET_DIAG");
	    token_set_y("CONFIG_INET_TCP_DIAG");
	    token_set_y("CONFIG_IP_FIB_HASH");
	    token_set_y("CONFIG_SLAB");
	    token_set_y("CONFIG_TCP_CONG_BIC");

	    token_set("CONFIG_DEFAULT_IOSCHED", "\"anticipatory\"");
	    token_set("CONFIG_X86_L1_CACHE_SHIFT", "4");
	    token_set("CONFIG_SPLIT_PTLOCK_CPUS", "4");
	}
    }

    if (token_get("CONFIG_PCBOX") || token_get("CONFIG_RG_UML"))
    {
	token_set("LIBC_ARCH", "i386");
	token_set_y("CONFIG_M386");
	token_set_y("CONFIG_FLASH_IN_FILE");
	token_set_y("CONFIG_SYSCTL");
	token_set_y("CONFIG_HAS_MMU");
	if (!token_get("CONFIG_RG_RGLOADER"))
	    token_set_y("CONFIG_DYN_LINK");
	set_big_endian(0);
	token_set_y("CONFIG_BINFMT_ELF");
	token_set_y("CONFIG_HW_CLOCK");
	token_set_y("CONFIG_PROC_FS");
	token_set_y("CONFIG_EXT2_FS");
    }

    if (token_get("CONFIG_PCBOX"))
    {
	token_set("BOARD", "PCBOX");
	token_set_y("CONFIG_RG_FLASHDISK");
	token_set("ARCH", "i386");
	token_set_y("CONFIG_SERIAL");
	token_set_y("CONFIG_SERIAL_CONSOLE");
	token_set("CONFIG_IMG_PRODUCT", "\"elf\"");
	if (!token_get("CONFIG_GLIBC") && !token_get("CONFIG_VALGRIND"))
	    token_set("CONFIG_RAM_LIMIT", "16");
	token_set_y("CONFIG_APM"); /* XXX: Why? */
	token_set_y("CONFIG_APM_POWER_OFF"); /* XXX: Why? */
	token_set_y("CONFIG_KEYBOARD"); /* Debug only? */
	token_set_y("CONFIG_M386");
	token_set_y("CONFIG_BLK_DEV_IDE");
	token_set_y("CONFIG_BLK_DEV_IDEDISK");
	token_set_y("CONFIG_BLK_DEV_IDEPCI");
	token_set_y("CONFIG_BLK_DEV_IDEDMA");
	token_set_y("CONFIG_BLK_DEV_VIA82CXXX");
	token_set_y("CONFIG_IDEDMA_AUTO");
	token_set_y("CONFIG_NET_ISA"); /* XXX: We don't have any ISA card */
	token_set_y("CONFIG_PCI");
	token_set_y("CONFIG_PCI_BIOS");
	token_set_y("CONFIG_PCI_DIRECT");
	token_set_y("CONFIG_PCI_GOANY");
	token_set_y("CONFIG_PCI_OLD_PROC");
	token_set_y("CONFIG_PCI_QUIRKS");
	token_set_y("CONFIG_RTC");
	token_set("CONFIG_FLASH_FSTYPE", "\"ext2\"");
	token_set("CONFIG_RAMDISK_SIZE", "256");
	token_set("CONFIG_UNIX98_PTY_COUNT", "256");
	token_set_y("CONFIG_UNIX98_PTYS");
	token_set_y("CONFIG_BLK_DEV_LOOP");
	token_set_y("CONFIG_RG_KMONTE");
	token_set_y("CONFIG_SAVE_REAL_MODE_CONF");
	token_set_y("CONFIG_RG_INITFS_RAMDISK");
	token_set("CONFIG_RG_KERNEL_COMP_METHOD", "gzip");
	token_set_m("CONFIG_ZAPTEL");
	token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "2");
	
	if ((os && !strcmp(os, "LINUX_24")))
	{
	    /* from os/linux-2.4/arch/i386/config.in */
	    token_set_y("CONFIG_BLK_DEV_IDEDMA_PCI");
	    /* from CONFIG_M386 section in os/linux-2.4/arch/i386/config.in
	     * i386 section */
	    token_set_y("CONFIG_X86");
	    token_set_y("CONFIG_X86_PPRO_FENCE");
	    token_set("CONFIG_X86_L1_CACHE_SHIFT", "4");
	    token_set_y("CONFIG_IDE");
	    token_set("CONFIG_INITRD_ADDR", "0x400000");
	    token_set_y("CONFIG_PM");
	    token_set_y("CONFIG_MSDOS_PARTITION");
	    if (token_get("CONFIG_RGTV"))
	    { /* Currently, rgtv requires large footprint in the storage */
		token_set("CONFIG_BLK_DEV_RAM_SIZE", "52768");
		token_set("CONFIG_RAMDISK_SIZE", "52768");
	    }
	    else if (token_get("CONFIG_VALGRIND"))
	    {
		token_set("CONFIG_BLK_DEV_RAM_SIZE", "32768");
		token_set("CONFIG_RAMDISK_SIZE", "32768");
	    }
	    else
	    {
		token_set("CONFIG_BLK_DEV_RAM_SIZE", "9216");
		token_set("CONFIG_RAMDISK_SIZE", "9216");
	    }
	    token_set_y("CONFIG_RAMFS");
	    token_set_y("CONFIG_SIMPLE_RAMDISK");
	    if (token_get("CONFIG_RG_KGDB"))
	    {
		token_set("CONFIG_CMDLINE", 
		    "root=/dev/ram0 rw console=ttyS0,38400n8 "
		    "gdbttyS=1 gdbbaud=115200");
	    }
	    else
	    {
		token_set("CONFIG_CMDLINE", 
		    "root=/dev/ram0 rw console=ttyS0,38400n8");
	    }
	    /* IDE support for Cytix CS5530 chipset */
	    token_set_y("CONFIG_BLK_DEV_CS5530");
	}
    }

    if (!token_get("CONFIG_RELEASE")) 
    {
	token_set_y("CONFIG_DEF_KEYS");
    }

    if (token_get("CONFIG_MPC8272ADS"))
    {
	/* Generic PPC configs */

	token_set_y("CONFIG_PPC");

	/* Platform options */

	token_set_y("CONFIG_CPM2");
	token_set_y("CONFIG_8272");
	token_set_y("CONFIG_8260");
	token_set_y("CONFIG_PQ2ADS");
	token_set_y("CONFIG_ADS8272");
	token_set_y("CONFIG_HIGHMEM");

	/* MTD */

	token_set_m("CONFIG_RG_MTD");
	token_set_y("CONFIG_RG_MTD_DEFAULT_PARTITION");
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_I4");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_4");
	token_set("CONFIG_MTD_PHYSMAP_BANKWIDTH", "4");
	token_set("CONFIG_MTD_PHYSMAP_LEN", "0x800000");
	token_set("CONFIG_MTD_PHYSMAP_START", "0xFF800000");
	
	/* Serial drivers */

	token_set_y("CONFIG_SERIAL_CORE");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
	token_set_y("CONFIG_SERIAL_CPM");
	token_set_y("CONFIG_SERIAL_CPM_SCC1");
	token_set_y("CONFIG_SERIAL_CPM_SCC4");
	token_set_y("CONFIG_SERIAL_CPM_CONSOLE");

	/* BUS */

	token_set_y("CONFIG_PCI_8260");

	/* RamDisk/CRAMFS/FS options */

	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");

	if (!token_get("CONFIG_LSP_DIST"))
	    token_set_y("CONFIG_RG_MODFS_CRAMFS");

	token_set("CONFIG_SPLIT_PTLOCK_CPUS", "4");

	token_set_y("CONFIG_HZ_250");
	token_set("CONFIG_HZ", "250");
	token_set_y("CONFIG_FLATMEM");
	token_set_y("CONFIG_FLAT_NODE_MEM_MAP");

	if (token_get("CONFIG_LSP_DIST"))
	    token_set_y("CONFIG_LSP_FLASH_LAYOUT");
    }

    if (token_get("CONFIG_EP8248"))
    {
	static char cmd_line[255];

	/* Build */

	set_big_endian(1);
	token_set("ARCH", "ppc");
	token_set("LIBC_ARCH", "powerpc");
	token_set_y("CONFIG_CPU_BIG_ENDIAN");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_ULIBC_SHARED");
	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_BOOTLDR_UBOOT");

	/* Command line */
	
	sprintf(cmd_line, "console=%s,115200 root=/dev/ram0 rw",
	    token_get_str("CONFIG_RG_CONSOLE_DEVICE")); 
	token_set("CONFIG_CMDLINE", cmd_line);
	
	/* Advanced setup */

	token_set("CONFIG_TASK_SIZE", "0x80000000");
	token_set("CONFIG_KERNEL_START", "0xc0000000");
	token_set("CONFIG_LOWMEM_SIZE", "0x30000000");
	token_set("CONFIG_HIGHMEM_START", "0xfe000000");

	/* Platform options */

	token_set_y("CONFIG_CPM2");
	token_set_y("CONFIG_8272");
	token_set_y("CONFIG_EP8248");
	token_set_y("CONFIG_8260");

	/* Processor */

	token_set_y("CONFIG_PPC_STD_MMU");
	token_set_y("CONFIG_6xx");

	/* Code maturity level options */

	token_set_y("CONFIG_PPC32");
	token_set_y("CONFIG_PPC");
	token_set_y("CONFIG_HAVE_DEC_LOCK");
	token_set_y("CONFIG_RWSEM_XCHGADD_ALGORITHM");
	token_set_y("CONFIG_PPC_FPU");

	/* MTD */
	
	token_set_m("CONFIG_RG_MTD");
	token_set_y("CONFIG_RG_MTD_DEFAULT_PARTITION");
	token_set_y("CONFIG_MTD");
	token_set("CONFIG_MTD_PHYSMAP_LEN", "0x800000");
	token_set("CONFIG_MTD_PHYSMAP_START", "0xFF800000");
	token_set_y("CONFIG_MTD_CFI_I2");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_4");

	/* Serial drivers */

	token_set_y("CONFIG_SERIAL_CORE");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
	token_set_y("CONFIG_SERIAL_CPM");
	token_set_y("CONFIG_SERIAL_CPM_SMC1");
	token_set_y("CONFIG_SERIAL_CPM_CONSOLE");

	/* General_setup */

	token_set("CONFIG_CC_ALIGN_JUMPS", "0");
	token_set("CONFIG_CC_ALIGN_LOOPS", "0");
	token_set("CONFIG_CC_ALIGN_LABELS", "0");
	token_set("CONFIG_CC_ALIGN_FUNCTIONS", "0");

	/* RamDisk/CRAMFS/FS options */

	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");

	if (!token_get("CONFIG_LSP_DIST"))
	    token_set_y("CONFIG_RG_MODFS_CRAMFS");

	/* Flash layout */

	if (token_get("CONFIG_LSP_DIST"))
	    token_set_y("CONFIG_LSP_FLASH_LAYOUT");
    }
    
    if (token_get("CONFIG_MPC8349_ITX"))
    {
	/* Generic PPC configs */
	token_set_y("CONFIG_PPC");
	token_set("CONFIG_BOOTLDR_UBOOT_COMP", "none");

	/* General ARCH */
	token_set_y("CONFIG_GENERIC_NVRAM");
	token_set_y("CONFIG_SCHED_NO_NO_OMIT_FRAME_POINTER");
	
	/* Processor */
	token_set_y("CONFIG_PPC_GEN550");

	/* Platform options */
	token_set_y("CONFIG_MPC8349_ITX");
	token_set_y("CONFIG_83xx");
	token_set_y("CONFIG_PPC_83xx");
	token_set_y("CONFIG_MPC834x");
	
	token_set_y("CONFIG_HZ_250");
	token_set("CONFIG_HZ", "250");
	token_set_y("CONFIG_FLATMEM");
	token_set_y("CONFIG_FLAT_NODE_MEM_MAP");
	
	/* Exists in Linux-2.6.12 but PPC compilation fails if it is on,
	 * taken from Freescale Linux-2.6.13 .config */
	token_set_y("CONFIG_SECCOMP");
	
	token_set_y("CONFIG_ISA_DMA_API");

	/* Bus */
	token_set_y("CONFIG_MPC83xx_PCI2");

	/* UART */

	token_set_y("CONFIG_SERIAL_CORE");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
	
	token_set_y("CONFIG_SERIAL_8250");
	token_set_y("CONFIG_SERIAL_8250_CONSOLE");
	token_set("CONFIG_SERIAL_8250_NR_UARTS", "4");
	token_set("CONFIG_SERIAL_8250_RUNTIME_UARTS", "4");

	token_set("CONFIG_SPLIT_PTLOCK_CPUS", "4");

	/* MTD */

	token_set_y("CONFIG_RG_MTD");
	token_set_y("CONFIG_RG_MTD_DEFAULT_PARTITION");
	token_set_y("CONFIG_MTD");
	token_set("CONFIG_MTD_PHYSMAP_BANKWIDTH", "2");
	token_set("CONFIG_MTD_PHYSMAP_LEN", "0x800000");
	token_set("CONFIG_MTD_PHYSMAP_START", "0xFE800000");
        token_set_y("CONFIG_MTD_CFI_I1");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_2");
	

	/* RamDisk/CRAMFS/FS options */

	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");

	if (token_get("CONFIG_LSP_DIST"))
	    token_set_y("CONFIG_LSP_FLASH_LAYOUT");
	else
	    token_set_y("CONFIG_RG_MODFS_CRAMFS");
    }

    if (token_get("CONFIG_PPC"))
    {
	/* Note: These may be not the generic PPC configs,
	 * but the only PPC we have now are FreeScale(s).
	 * TODO: divide this section into PPC generic, etc..
	 * (e.g. CONFIG_6xx or CONFIG_CPU_BIG_ENDIAN may not be 
	 * correct for any PPC)
	 */
	static char cmd_line[255];

	/* Build */

	set_big_endian(1);
	token_set("ARCH", "ppc");
	token_set("LIBC_ARCH", "powerpc");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_ULIBC_SHARED");
	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_BOOTLDR_UBOOT");

	/* Advanced setup */

	token_set("CONFIG_TASK_SIZE", "0x80000000");
	token_set("CONFIG_KERNEL_START", "0xc0000000");
	token_set("CONFIG_LOWMEM_SIZE", "0x30000000");
	token_set("CONFIG_HIGHMEM_START", "0xfe000000");

	/* General ARCH */

	token_set_y("CONFIG_RWSEM_XCHGADD_ALGORITHM");
	token_set_y("CONFIG_PPC32");
	
	/* Processor */

	token_set_y("CONFIG_PPC_STD_MMU");
	token_set_y("CONFIG_6xx");
	token_set_y("CONFIG_PPC_FPU");

	/* Bus */

	token_set_y("CONFIG_PCI");

	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    /* General */

	    token_set_y("CONFIG_OBSOLETE_INTERMODULE");

	    /* Bus */

	    token_set_y("CONFIG_PCI_DOMAINS");
	    token_set_y("CONFIG_PPC_INDIRECT_PCI");

	    /* GCC setup */

	    token_set("CONFIG_CC_ALIGN_JUMPS", "0");
	    token_set("CONFIG_CC_ALIGN_LOOPS", "0");
	    token_set("CONFIG_CC_ALIGN_LABELS", "0");
	    token_set("CONFIG_CC_ALIGN_FUNCTIONS", "0");
	}

	/* Command line */

	sprintf(cmd_line, "console=%s,115200 root=/dev/ram0 rw",
	    token_get_str("CONFIG_RG_CONSOLE_DEVICE")); 

	token_set("CONFIG_CMDLINE", cmd_line);
    }

    if (token_get("CONFIG_BCM947_COMMON"))
    {
	set_big_endian(0);
	token_set("CONFIG_BLK_DEV_RAM_SIZE", "8000");
	token_set_y("CONFIG_BLK_DEV_LOOP");
	token_set("CONFIG_CMDLINE", 
	    "console=ttyS0,115200 root=/dev/ram0 rw");
	token_set_y("CONFIG_MIPS");
	token_set_y("CONFIG_NONCOHERENT_IO");
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_MIPS_BRCM");
	token_set_y("CONFIG_BCM947XX");
	token_set_y("CONFIG_BCM947");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_BINFMT_ELF");
	token_set_y("CONFIG_SERIAL");
	token_set_y("CONFIG_SERIAL_CONSOLE");
	token_set_y("CONFIG_SYSCTL");
	token_set("CONFIG_UNIX98_PTY_COUNT", "256");
	token_set_y("CONFIG_UNIX98_PTYS");
	token_set("ENDIANESS_SUFFIX", "el30");
	token_set("ARCH", "mips");
	token_set("LIBC_ARCH", "mips");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	token_set_y("CONFIG_BCM4310");
	token_set_y("CONFIG_PCI");
	token_set_y("CONFIG_NEW_TIME_C");
	token_set_y("CONFIG_NEW_IRQ");
	token_set_y("CONFIG_CPU_MIPS32");
	token_set_y("CONFIG_CPU_HAS_PREFETCH");
	token_set_y("CONFIG_CPU_HAS_LLSC");
	token_set_y("CONFIG_CPU_HAS_SYNC");
	token_set_y("CONFIG_KCORE_ELF");
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_ADV_OPTIONS");
	token_set_y("CONFIG_MTD_CFI_NOSWAP");
	token_set_y("CONFIG_MTD_CFI_GEOMETRY");
	token_set_y("CONFIG_MTD_CFI_B2");
	token_set_y("CONFIG_MTD_CFI_I1");
	token_set_y("CONFIG_MTD_CFI_INTELEXT");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");
	token_set_y("CONFIG_MTD_BCM947XX");
	token_set_m("CONFIG_IL");
	token_set_y("CONFIG_IL_47XX");
	token_set_m("CONFIG_WL");
	token_set_y("CONFIG_WL_AP");
	token_set_y("CONFIG_NET_PCI");
	token_set_y("CONFIG_NET_RADIO");
	token_set_m("CONFIG_HERMES");
	token_set_y("CONFIG_RAMFS");
	token_set_y("CONFIG_PARTITION_ADVANCED");
	/*token_set("CONFIG_NET_WIRELESS");
	#token_set_m("CONFIG_USB");
	#token_set("CONFIG_USB_DEVICEFS");
	#token_set_m("CONFIG_USB_OHCI");*/
	token_set_y("CONFIG_CROSSCOMPILE");
	token_set_y("CONFIG_MAGIC_SYSRQ");
	token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	token_set_y("CONFIG_CRAMFS_FS");
	token_set_y("CONFIG_RG_MODFS");
	/* Uncompress is done by CFE/PMON which does not support other methods
	 */
	token_set("CONFIG_RG_KERNEL_COMP_METHOD", "gzip");
    }
    
    if (token_get("CONFIG_BCM963XX_COMMON"))
    {
	static char cmd_line[255];

	/* Linux/Build generic */
	set_big_endian(1);
	token_set("ARCH", "mips");
	token_set("LIBC_ARCH", "mips");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_MIPS32");

	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    token_set_y("CONFIG_DMA_NONCOHERENT");
	    token_set_y("CONFIG_TRAD_SIGNALS");
	    token_set("CONFIG_MIPS_L1_CACHE_SHIFT", "5");
	    token_set_y("CONFIG_PAGE_SIZE_4KB");
	    token_set("CONFIG_LOG_BUF_SHIFT", "14");
	    token_set_y("CONFIG_RG_TTYP");
	    token_set_y("CONFIG_CC_OPTIMIZE_FOR_SIZE");
	    token_set("CONFIG_SPLIT_PTLOCK_CPUS", "4");
	    token_set_y("CONFIG_32BIT");
	    token_set_y("CONFIG_BUILD_ELF32");
	    token_set_y("CONFIG_CPU_MIPS32_R1");
	    token_set_y("CONFIG_FLATMEM");
	    token_set_y("CONFIG_FLAT_NODE_MEM_MAP");
	    token_set_y("CONFIG_BOOT_ELF32");
	    token_set_y("CONFIG_LINUX_SKB_REUSE");
	}

	if (token_get("CONFIG_RG_OS_LINUX_24"))
	{
	    token_set_y("CONFIG_BINFMT_ELF");
	    token_set_y("CONFIG_SYSCTL");
	    token_set("CONFIG_UNIX98_PTY_COUNT", "256");
	    token_set_y("CONFIG_UNIX98_PTYS");
	    token_set_y("CONFIG_NONCOHERENT_IO");
	    token_set_y("CONFIG_NEW_TIME_C");
	    token_set_y("CONFIG_NEW_IRQ");
	    token_set_y("CONFIG_SERIAL");
	    token_set_y("CONFIG_SERIAL_CONSOLE");

	    token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	    token_set_y("CONFIG_RG_MODFS");
	    token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	    token_set_y("CONFIG_CRAMFS_FS");
	    token_set("CONFIG_BLK_DEV_RAM_SIZE", "8000");
	    if (!IS_DIST("GTWX5803"))
		token_set("CONFIG_RAMDISK_SIZE", "512");
	    token_set_y("CONFIG_RAMFS");

	    /* RamDisk/CRAMFS/FS options */
	    token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	    token_set_y("CONFIG_RG_MODFS");
	    token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	    token_set_y("CONFIG_CRAMFS_FS");
	}
	
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	token_set_y("CONFIG_CPU_MIPS32");
	token_set_y("CONFIG_CPU_HAS_PREFETCH");
	token_set_y("CONFIG_CPU_HAS_LLSC");
	token_set_y("CONFIG_CPU_HAS_SYNC");
	token_set_y("CONFIG_KCORE_ELF");
	token_set_y("CONFIG_CROSSCOMPILE");
	token_set_y("CONFIG_MAGIC_SYSRQ");
	
	token_set_y("CONFIG_MIPS");
	token_set_y("CONFIG_MIPS_BRCM");
	token_set_y("CONFIG_BCM963XX");
	
	if (!token_get("CONFIG_RG_RGLOADER"))
	{
	    token_set_y("CONFIG_DYN_LINK");
	    /* Loop devices */
	    token_set_y("CONFIG_BLK_DEV_LOOP");
	}

	if (!token_get_str("CONFIG_SDRAM_SIZE"))
	{
	    token_set("CONFIG_SDRAM_SIZE",
		token_get_str("CONFIG_BCM963XX_SDRAM_SIZE"));
	}
	
	/* Command line */
	
	if (token_get("CONFIG_RG_OS_LINUX_24"))
	{
	    sprintf(cmd_line, "console=ttyS0,115200 root=/dev/ram0 rw %s",
		token_get("CONFIG_RG_RGLOADER") ? "quiet" : ""); 
	    token_set("CONFIG_CMDLINE", cmd_line);
	}
	if (token_get("CONFIG_RG_OS_LINUX_26"))
	    token_set("CONFIG_CMDLINE", "console=ttyS0,115200");
	
	/* MTD */
	
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");
	/* New MTD configs (Linux-2.6) */
	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_1");
	    token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_2");
	    token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_4");
	    token_set_y("CONFIG_MTD_CFI_I1");
	    token_set_y("CONFIG_MTD_CFI_I2");
	    token_set_y("CONFIG_MTD_CFI_UTIL");
	    token_set_y("CONFIG_OBSOLETE_INTERMODULE");
	}
    }
    
    if (token_get("CONFIG_MACH_ADI_FUSIV"))
    {
	static char cmd_line[255];

	/* CPU */
	token_set_y("CONFIG_CPU_LX4189");
	

	/* Serial */
	token_set_y("CONFIG_MAGIC_SYSRQ");
	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    /* Command line */
	    token_set("CONFIG_CMDLINE", "console=ttyS0,9600");

	    token_set_y("CONFIG_SERIAL_8250");
	    token_set_y("CONFIG_SERIAL_8250_CONSOLE");
	    token_set("CONFIG_SERIAL_8250_NR_UARTS", "1");
	    token_set_y("CONFIG_SERIAL_CORE");
	    token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
	}
	if (token_get("CONFIG_RG_OS_LINUX_24"))
	{
	    /* Command line */
	    sprintf(cmd_line, "console=ttyS0,9600 root=/dev/ram0 rw %s",
		token_get("CONFIG_RG_RGLOADER") ? "quiet" : ""); 
	    token_set("CONFIG_CMDLINE", cmd_line);

	    token_set_y("CONFIG_SERIAL");
	    token_set_y("CONFIG_SERIAL_CONSOLE");
	}

    	/* Flash/MTD */
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");

	/* New MTD configs (Linux-2.6) */
	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_1");
	    token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_2");
	    token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_4");
	    token_set_y("CONFIG_MTD_CFI_I1");
	    token_set_y("CONFIG_MTD_CFI_I2");
	    token_set_y("CONFIG_MTD_CFI_UTIL");
	}

	/* PCI */
	token_set_y("CONFIG_PCI");
	token_set_y("CONFIG_PCI_AUTO");
	/* Looks like CONFIG_NEW_PCI is not present in Linux-2.6  */
	if (token_get("CONFIG_RG_OS_LINUX_24"))
	    token_set_y("CONFIG_NEW_PCI");
    }
    
    if (token_get("CONFIG_CPU_LX4189"))
    {
	token_set_y("CONFIG_HAS_MMU");
	
	/* Linux/Build generic */
	token_set("ARCH", "mips");
	token_set("LIBC_ARCH", "mips");

	if (token_get("CONFIG_RG_OS_LINUX_24"))
	{
	    token_set_y("CONFIG_BINFMT_ELF");
	    token_set_y("CONFIG_SYSCTL");
	    token_set_y("CONFIG_CPU_HAS_PREFETCH");
	    token_set_y("CONFIG_EXPERIMENTAL");
	    token_set_y("CONFIG_CPU_HAS_WB");
	    token_set_y("CONFIG_KCORE_ELF");
	    token_set_y("CONFIG_CROSSCOMPILE");

	    /* RamDisk/CRAMFS/FS options */
	    token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	    token_set_y("CONFIG_CRAMFS_FS");
	    token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");

	    if (!token_get("CONFIG_RG_RGLOADER"))
	    {
		/* Loop devices */
		token_set_y("CONFIG_BLK_DEV_LOOP");
	    }
	    token_set("CONFIG_BLK_DEV_RAM_SIZE", "8000");
	    token_set("CONFIG_RAMDISK_SIZE", "1024");
	    token_set_y("CONFIG_RAMFS");

	    token_set_y("CONFIG_NONCOHERENT_IO");
	    token_set_y("CONFIG_NEW_TIME_C");
	    token_set_y("CONFIG_NEW_IRQ");
	}

	/* The following standard Linux MIPS configs are not set for Lexra:
	 * CONFIG_CPU_MIPS32
	 * CONFIG_CPU_HAS_LLSC
	 * CONFIG_UNIX98_PTY_COUNT (256)
	 * CONFIG_UNIX98_PTYS
	 * CONFIG_CPU_HAS_SYNC
	 */

	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    token_set_y("CONFIG_DMA_NONCOHERENT");
	    token_set_y("CONFIG_HAVE_DEC_LOCK");
	    token_set_y("CONFIG_TRAD_SIGNALS");
	    token_set("CONFIG_MIPS_L1_CACHE_SHIFT", "5");
	    token_set_y("CONFIG_PAGE_SIZE_4KB");
	    token_set("CONFIG_LOG_BUF_SHIFT", "14");
	    token_set_y("CONFIG_RG_TTYP");
	}

	token_set_y("CONFIG_CPU_BIG_ENDIAN");
	token_set("TARGET_ENDIANESS", "BIG");

	/* MIPS specific settings */
	token_set_y("CONFIG_MIPS_PATENTFREE");
	token_set_y("CONFIG_MIPS32");
	token_set_y("CONFIG_MIPS");
	token_set_y("CONFIG_BOOT_ELF32");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	
	token_set_y("CONFIG_CROSSCOMPILE");
#if 0
	/* Wireless Recheck */
	token_set_m("CONFIG_WL");
	token_set_y("CONFIG_WL_AP");
	token_set_y("CONFIG_NET_PCI");
	token_set_y("CONFIG_NET_RADIO");
	token_set_m("CONFIG_HERMES");
	token_set("CONFIG_NET_WIRELESS");
#endif
    }

    if (token_get("CONFIG_KS8695_COMMON"))
    {
	token_set_y("CONFIG_SOFT_FLOAT");
    }
    
    if (token_get("CONFIG_ARMNOMMU"))
    {
	token_set_y("CONFIG_DEBUG_USER");
	token_set("ARCH", "armnommu");
	token_set("LIBC_ARCH", "arm");
	token_set_y("CONFIG_ARM");
	token_set("CONFIG_RAM_LIMIT", "16");
	token_set_y("CONFIG_CPU_32");
	token_set_y("CONFIG_NO_UNALIGNED_ACCESS");
	token_set_y("CONFIG_BINUTILS_NEW");
	token_set_y("CONFIG_SOFT_FLOAT");
	if (token_get("CONFIG_RG_OS_LINUX"))
	{	
	    token_set_y("CONFIG_KERNEL_ELF");
	    token_set_y("CONFIG_BINFMT_FLAT");
	    token_set_y("CONFIG_MALLOC_CONSERVE_RAM");
	    token_set_y("CONFIG_SMALL_STACK");
	    token_set_y("CONFIG_TEXT_SECTIONS");
	    token_set_y("CONFIG_NO_FRAME_POINTER");
	    if (!token_get("CONFIG_LSP_DIST"))
		token_set_y("CONFIG_EXTRA_ALLOC");
	}
	token_set("CONFIG_RG_KERNEL_COMP_METHOD", "gzip");
    }

    if (token_get("CONFIG_CX821XX_COMMON"))
    {
	set_big_endian(0);
	token_set_y("CONFIG_CONEXANT_COMMON");
	token_set("CONFIG_ARCH_MACHINE", "cx821xx");
	token_set("CONFIG_NOMMU_PAGECACHE_THRESHOLD", "200");
	token_set("CONFIG_FLASH_POS", "0x00400000");
	token_set_y("CONFIG_SMALL_FLASH");
	token_set_y("CONFIG_RG_FLASH_LINEAR");
	token_set_y("CONFIG_RG_RMT_UPD_LOW_MEM");
	token_set_y("CONFIG_BLK_DEV_LOOP");
	token_set("CONFIG_EXP_BUS_SHL", "0");
	token_set("CONFIG_BOOT_RAM_POS", "0xf00000");
	token_set("CONFIG_BOOT_SP_POS", "0xf00000");
	token_set("CONFIG_SDRAM_BASE", "0x800000");
	token_set("CONFIG_SDRAM_SIZE", "0x800000");
	token_set("CONFIG_PHYS_SDRAM_SIZE", "0x800000");
	token_set("CONFIG_BOOT_FREE_MEM_START", "0xc00000");
	token_set("CONFIG_BOOT_FREE_MEM_END", "0xd00000");
	token_set("CONFIG_RAM_LIMIT", "8");
	/* Taken from CNX BSP */
	token_set_y("MAGIC_ROM_PTR");
	token_set_y("CONFIG_ARCH_CNXT");
	token_set_y("CONFIG_ARCH_CX821XX");
	token_set_y("CONFIG_CPU_ARM940T");
	token_set_y("CONFIG_SERIAL_CNXT");
	token_set_y("CONFIG_CPU_ARM940_CPU_IDLE");
	token_set_y("CONFIG_CPU_ARM940_I_CACHE_ON");
	token_set_y("CONFIG_CPU_ARM940_D_CACHE_ON");
	token_set_y("CONFIG_CPU_ARM940_WRITETHROUGH");
	token_set_y("CONFIG_NO_PGT_CACHE");
	token_set("CONFIG_RAMDISK_SIZE", "256");
	token_set("CONFIG_CRAMFS_RAM_ADDR", "0xf00000");
	token_set_y("CONFIG_CRAMFS_FS");
	token_set_y("CONFIG_CRAMFS_LINEAR");
	if (token_get("CONFIG_KNCX821XX_PCB"))
	    token_set_y("CONFIG_CX821XX_MXIC_FLASH");
    }

    if (token_get("CONFIG_KS8695_COMMON"))
    {
	static char cmd_line[255];

 	token_set("ARCH", "arm");
 	token_set("LIBC_ARCH", "arm");
 	token_set_y("CONFIG_HAS_MMU");
 	token_set_y("CONFIG_ARM");
	token_set_y("CONFIG_CPU_32");
 	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	token_set_y("CONFIG_CPU_LITTLE_ENDIAN");
	token_set("TARGET_ENDIANESS", "LITTLE");
 	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_CPU_32v4");
 	token_set_y("CONFIG_RAMFS"); 
	token_set("CONFIG_BLK_DEV_RAM_SIZE", "16384");
	token_set_y("CONFIG_ARCH_KS8695");
	token_set_y("CONFIG_CPU_ARM922T");
	token_set_y("CONFIG_CPU_ARM922_CPU_IDLE");
	token_set_y("CONFIG_CPU_ARM922_I_CACHE_ON");
	token_set_y("CONFIG_CPU_ARM922_D_CACHE_ON");

	token_set_y("CONFIG_NO_FRAME_POINTER");
	token_set_y("CONFIG_FPE_FASTFPE");
 	token_set_y("CONFIG_KCORE_ELF");
 	token_set_y("CONFIG_BINFMT_ELF");
 	token_set_y("CONFIG_SYSCTL");

	if (!token_get("CONFIG_RG_RGLOADER"))
	    token_set_y("CONFIG_DYN_LINK");

	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	token_set_y("CONFIG_CRAMFS_FS");
	token_set_y("CONFIG_RG_MODFS");
	
	token_set_y("CONFIG_PCI");
	/* ? Replace with CONFIG_RG_... ? */
	token_set_y("CONFIG_PCI_KS8695P"); 
	token_set_y("CONFIG_ARCH_KS8695P");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyAM0");

	sprintf(cmd_line, "console=%s,38400 root=/dev/ram0 rw%s",
	    token_get_str("CONFIG_RG_CONSOLE_DEVICE"), 
	    token_get("CONFIG_RG_KGDB") ? " nohalt" : "");
		
	token_set("CONFIG_CMDLINE", cmd_line);

	token_set_y("CONFIG_SERIAL_CORE");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");

	/* To be replaced by matching CONFIG_RG_... */
	token_set_y("CONFIG_SERIAL_KS8695");
	token_set_y("CONFIG_SERIAL_KS8695_CONSOLE");

	if (!token_get("CONFIG_LSP_DIST"))
	    token_set_m("CONFIG_RG_KRGLDR");	

	/* token_set_y("CONFIG_PLD"); needed for altera ether00 support */

	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");

	token_set_y("CONFIG_MTD_PHYSMAP");
	token_set("CONFIG_MTD_PHYSMAP_START", "0x2800000");
	token_set("CONFIG_MTD_PHYSMAP_LEN", "0x400000");
	token_set("CONFIG_MTD_PHYSMAP_BUSWIDTH", "1");

	/* Debugging configurations */
 	token_set_y("CONFIG_ALIGNMENT_TRAP");
 	token_set("CONFIG_ALIGNMENT_TRAP_MODE", "2");
	token_set_y("CONFIG_DEBUG_USER");
    }
    
    if (token_get("CONFIG_TI_404_COMMON"))
    {
	token_set("ARCH", "mips");
	token_set("LIBC_ARCH", "mips");
    }

    if (token_get("CONFIG_RG_RMT_MNG") ||
	(token_get("CONFIG_RG_DSLHOME") && !token_get("CONFIG_RG_JNET_SERVER")))
    {
	token_set_y("CONFIG_RG_SSI");
    }

    if (token_get("CONFIG_RG_OSS_RMT"))
    {
	token_set_y("CONFIG_RG_WAN_UPGRADE");
	token_set_y("CONFIG_RG_SSI");
    }

    if (token_get("CONFIG_RG_PPPOS_CLI"))
	token_set_y("CONFIG_RG_PPPOS");

    if (token_get("CONFIG_RG_PPPOS"))
	token_set_y("CONFIG_RG_SERIAL");

    if (token_get("CONFIG_RG_SSI_PAGES"))
	token_set_y("CONFIG_RG_SSI");

    if (token_get("CONFIG_RG_SURFCONTROL") || 
	token_get("CONFIG_RG_URL_KEYWORD_FILTER"))
    {
	token_set_y("CONFIG_RG_HTTP_PROXY");
    }
    
    if (token_get("CONFIG_RG_TFTP_UPGRADE"))
    {
	token_set_y("CONFIG_RG_TFTP_SERVER");
	token_set_y("CONFIG_RG_RMT_UPDATE");
    }

    if (token_get("OPENRG_DEBUG"))
    {
	token_set_y("CONFIG_NFS_FS");
	if (!token_get("CONFIG_VALGRIND"))
	    token_set_y("OPENRG_DEBUG_EXEC_SH");
	token_set_y("OPENRG_DEBUG_MODULES");
	token_set_y("CONFIG_RG_NO_OPT");
#if 0 /* Uncomment for file/executable debug */
	token_set_y("OPENRG_DEBUG_FILE_OPEN");
#endif
    }

    if (token_get("CONFIG_NFS_FS"))
    {
	token_set_y("CONFIG_LOCKD");
	token_set_y("CONFIG_SUNRPC");
    }

    if (token_get("CONFIG_RG_PERM_STORAGE"))
	token_set_y("CONFIG_GEN_RG_DEFAULT");

    /*  The following 3 CONFIGS control the system behavior in regards to
     *  freeing cached pages. This configuration makes the system try to work
     *  constantly at no less then HIGH*PAGE_SIZE free memory if possible. 
     */

    if (token_get("CONFIG_FREEPAGES_THRESHOLD"))
    {
	token_set("CONFIG_FREEPAGES_MIN", "32");
	token_set("CONFIG_FREEPAGES_LOW", "256");
	token_set("CONFIG_FREEPAGES_HIGH", "384");
    }

    if (token_get("CONFIG_RG_VOATM_ELCP") || token_get("CONFIG_RG_VOATM_CAS"))
	token_set_y("CONFIG_RG_VOATM");
    
    if (token_get("CONFIG_RG_VOATM"))
	token_set_y("CONFIG_RG_TELEPHONY");

    if (token_get("CONFIG_FREESWAN"))
    {
	token_set_y("CONFIG_GMP");
	token_set_y("CONFIG_IPSEC");
	token_set_y("CONFIG_IPSEC_DRIVER");
	token_set_y("CONFIG_IPSEC_IPIP");
	token_set_y("CONFIG_IPSEC_AH");
	token_set_y("CONFIG_IPSEC_AUTH_HMAC_MD5");
	token_set_y("CONFIG_IPSEC_AUTH_HMAC_SHA1");
	token_set_y("CONFIG_IPSEC_ESP");
	token_set_y("CONFIG_IPSEC_ENC_1DES");
	token_set_y("CONFIG_IPSEC_ENC_3DES");
	token_set_y("CONFIG_IPSEC_ENC_NULL");
	token_set_y("CONFIG_IPSEC_IPCOMP");
	token_set_y("CONFIG_IPSEC_NAT_TRAVERSAL");
	token_set_y("CONFIG_VPN");
	token_set_y("CONFIG_IPSEC_DEBUG");
	token_set_y("CONFIG_PLUTO_DEBUG");
	token_set_y("CONFIG_RG_FREESWAN");
#if 0 /* WSTL */
	token_set_y("CONFIG_RG_X509");
#endif
	token_set_m("CONFIG_RG_NETBIOS_RT");
	token_set_y("CONFIG_RG_IPSEC_NO_SECRET_FILE");
	if (!strcmp(os, "LINUX_24") || !strcmp(os, "LINUX_26"))
	    token_set_y("CONFIG_RG_IPSEC_IKE_ALG");
	if ((!strcmp(os, "LINUX_24") || !strcmp(os, "LINUX_26")) &&
	    !token_get("CONFIG_IPSEC_USE_IXP4XX_CRYPTO"))
	{
	    token_set_y("CONFIG_RG_IPSEC_ESP_ALG");
	    token_set_y("CONFIG_IPSEC_ALG");
	    if (token_get("CONFIG_IPSEC_ENC_1DES"))
		token_set_m("CONFIG_IPSEC_ALG_1DES");
	    if (token_get("CONFIG_IPSEC_ENC_3DES"))
		token_set_m("CONFIG_IPSEC_ALG_3DES");
	    if (token_get("CONFIG_IPSEC_ENC_NULL"))
		token_set_m("CONFIG_IPSEC_ALG_NULL");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	    token_set_m("CONFIG_IPSEC_ALG_AES");
	    if (token_get("CONFIG_IPSEC_AUTH_HMAC_MD5"))
		token_set_m("CONFIG_IPSEC_ALG_MD5");
	    if (token_get("CONFIG_IPSEC_AUTH_HMAC_SHA1"))
		token_set_m("CONFIG_IPSEC_ALG_SHA1");
	}
    }

    if (token_get("CONFIG_RG_PPTPS"))
    {
	token_set_y("CONFIG_RG_PPTP");
	token_set_y("CONFIG_LIBDES");
	token_set_y("CONFIG_VPN");
    }

    if (token_get("CONFIG_RG_PPTPC"))
    {
	token_set_y("CONFIG_RG_PPTP");
	token_set_y("CONFIG_LIBDES");
	token_set_y("CONFIG_VPN");
    }

    if (token_get("CONFIG_SL2312_COMMON_RG"))
    {
	token_set_y("CONFIG_SL2312_COMMON");
	token_set("CONFIG_RAMDISK_SIZE", "512");
	
 	token_set_y("CONFIG_BLK_DEV_IDE");
 	token_set_y("CONFIG_BLK_DEV_SL2312_IDE");
 	token_set_y("CONFIG_BLK_DEV_IDEPCI");
 	token_set_y("CONFIG_BLK_DEV_IDEDMA");
 	token_set_y("CONFIG_BLK_DEV_IDEDMA_PCI");
 	token_set_y("CONFIG_BLK_DEV_SL2312_IDE_CHANNEL_1");
 	token_set_y("CONFIG_BLK_DEV_IDEDISK");
 	token_set_y("CONFIG_BLK_DEV_IDECD");
 	token_set_y("CONFIG_BLK_DEV_IDE_MODES");
 	token_set_y("CONFIG_IDE");
	
	token_set_y("CONFIG_PCI");
 	token_set_y("CONFIG_PCI_NAMES");
 	token_set_y("CONFIG_NET_PCI");
	token_set_y("CONFIG_PCI_RESET");
    }
    
    if (token_get("CONFIG_RG_HSS"))
	token_set_y("CONFIG_IXP425_CSR_HSS");

    if (token_get("CONFIG_IXP425_COMMON_RGLOADER"))
    {
	/* Override HW settings to save RAM for network boot feature */
	token_set("CONFIG_SDRAM_SIZE", "16");
	/* Enable boot of image from network */
	token_set_y("CONFIG_RG_NETWORK_BOOT");
	token_set_y("CONFIG_IXP425_COMMON");
	token_set("CONFIG_FORCE_MAX_ZONEORDER", "12");
	token_set_y("CONFIG_CRAMFS_FS");
	token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	token_set_y("CONFIG_PCI_RESET");
    }
 
    if (token_get("CONFIG_SIBYTE_SB1250"))
    {
	/* Linux configs */
	/* General MIPS */
	token_set("ARCH", "mips");
	token_set_y("CONFIG_MIPS");
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_MODULES");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	token_set_y("CONFIG_NEW_TIME_C");
	token_set_y("CONFIG_BOOT_ELF32");
	/* Sibyte */
	token_set_y("CONFIG_SMP");
	token_set("CONFIG_NR_CPUS", "2");
	token_set_y("CONFIG_CPU_SB1");
	token_set_y("CONFIG_CPU_SB1_PASS_1");
	token_set_y("CONFIG_CPU_HAS_PREFETCH");
	token_set_y("CONFIG_CPU_HAS_LLSC");
	token_set_y("CONFIG_CPU_HAS_SYNC");
	/* Current toolchain doesn't support -msb1-pass1-workarounds,
	 * only required by modules, we should understand why
	token_set_y("CONFIG_SB1_PASS_1_WORKAROUNDS");
	*/
	token_set_y("CONFIG_SIBYTE_SB1xxx_SOC");
	token_set_y("CONFIG_SIBYTE_CFE");
	token_set_y("CONFIG_NET_SB1250_MAC"); /* network */
	token_set_y("CONFIG_SERIAL_NONSTANDARD");
	token_set_y("CONFIG_SIBYTE_SB1250_DUART"); /* serial */
	token_set_y("CONFIG_SIBYTE_SB1250_DUART_CONSOLE");
	token_set_y("CONFIG_TRAD_SIGNALS");
	token_set_y("CONFIG_PCI");
	token_set_y("CONFIG_NO_ISA");
	token_set_y("CONFIG_SWAP_IO_SPACE");
	token_set("CONFIG_MIPS_L1_CACHE_SHIFT", "5");
	token_set_y("CONFIG_PAGE_SIZE_4KB");
	token_set("CONFIG_SPLIT_PTLOCK_CPUS", "4");
	token_set_y("CONFIG_DMA_COHERENT");
	token_set_y("CONFIG_FLATMEM");
	token_set_y("CONFIG_FLAT_NODE_MEM_MAP");
	token_set_y("CONFIG_OBSOLETE_INTERMODULE");
#if 0 /* TODO: our GCC is not capable of building 64bit code,
       * neither ulibc supports MIPS 64 bit */
	token_set_y("CONFIG_64BIT");
	token_set_y("CONFIG_BUILD_ELF64");
#else
	token_set_y("CONFIG_32BIT");
	token_set_y("CONFIG_BUILD_ELF32");
#endif
	/* general */
	token_set_y("CONFIG_CROSSCOMPILE");
	token_set_y("CONFIG_SYSVIPC");
	token_set_y("CONFIG_SYSCTL");
	token_set_y("CONFIG_KCORE_ELF");
	token_set_y("CONFIG_BINFMT_ELF");
	token_set_y("CONFIG_UNIX98_PTYS");
	token_set("CONFIG_UNIX98_PTY_COUNT", "256");
	token_set_y("CONFIG_NET");
	token_set_y("CONFIG_PACKET");
	token_set_y("CONFIG_PACKET_MMAP");
	//token_set_y("CONFIG_NETLINK_DEV");
	token_set_y("CONFIG_UNIX");
	token_set_y("CONFIG_INET");
	token_set_y("CONFIG_NETDEVICES");
	token_set_y("CONFIG_NET_ETHERNET");

	/* maybe not used by kernel !!! */
	token_set("CONFIG_CMDLINE", "console=ttyS0,115200");

	/* OpenRG configs */
	set_big_endian(1);
	token_set("LIBC_ARCH", "mips");
	token_set_y("CONFIG_HAS_MMU");
    }
    
    if (token_get("CONFIG_IXP425_COMMON_RG"))
    {
	token_set_y("CONFIG_IXP425_COMMON");
	token_set("CONFIG_FORCE_MAX_ZONEORDER", "12");
	
	/* Ramdisk configuration */
	if (!token_get("CONFIG_SIMPLE_RAMDISK"))
	{
	    token_set_y("CONFIG_CRAMFS_FS");
	    token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	    token_set("CONFIG_RAMDISK_SIZE", "512");
	}
    }
    
    if (token_get("CONFIG_IXP425_COMMON_LSP"))
    {
	token_set_y("CONFIG_IXP425_COMMON");
	token_set_y("CONFIG_IXP425_CSR_FULL");
	token_set("CONFIG_FORCE_MAX_ZONEORDER", "13");
	if (token_get("CONFIG_SIMPLE_RAMDISK"))
	{
	    token_set("CONFIG_RAMDISK_SIZE", "8192");
	}
	else
	{
	    token_set_y("CONFIG_RG_BOOTLDR_REDBOOT");
	    token_set_y("CONFIG_MTD_REDBOOT_PARTS");
	    token_set_y("CONFIG_CRAMFS_FS");
	}
	token_set_m("CONFIG_JFFS2_FS");
	/* See B11020
	token_set_y("CONFIG_IXP425_DSR");
	token_set_m("CONFIG_IXP425_CODELET_DSP_ENG");
	*/
    }

    if (token_get("CONFIG_IXP425_DSR"))
    {
	token_set_y("CONFIG_IXP425_CSR_HSS");
	token_set_y("CONFIG_DIVDI3");

	if (token_get("CONFIG_IXP425_DSR_DEMO"))
	{
	    token_set_y("CONFIG_IXP425_CODELETS");
	    token_set_y("CONFIG_IXP425_CODELET_DSP_ENG");
	}
    }
    
    if (token_get("CONFIG_SL2312_COMMON"))
    {
 	static char cmd_line[255];
 	
	token_set_m("CONFIG_RG_KRGLDR");

	set_big_endian(0);
 	token_set("ARCH", "arm");
 	token_set("LIBC_ARCH", "arm");
 	token_set_y("CONFIG_HAS_MMU");
 	token_set_y("CONFIG_ARM");
 	token_set("CONFIG_ALIGNMENT_TRAP_MODE", "2");
 	token_set_y("CONFIG_UID16");
 	token_set_y("CONFIG_CPU_32");
 	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
 	token_set_y("CONFIG_EXPERIMENTAL");
 	token_set_y("CONFIG_CPU_32v4");
 	token_set_y("CONFIG_BLK_DEV_LOOP"); 
 	token_set_y("CONFIG_RAMFS"); 
 	/* floating point emulator, check */
 	token_set_y("CONFIG_FPE_NWFPE");
 	/* adds /proc/kcore in elf format for dbg */
 	token_set_y("CONFIG_KCORE_ELF");
 	/* support a.out format */
 	token_set_y("CONFIG_BINFMT_AOUT");
 	/* handle unaligned access (TODO remove) */
 	token_set_y("CONFIG_ALIGNMENT_TRAP");
 	/* debug purposes - when user mode crashes gives some info */
 	token_set_y("CONFIG_DEBUG_USER");

 	if (!token_get("CONFIG_RG_RGLOADER"))
	    token_set_y("CONFIG_RG_NETTOOLS_ARP");
 		
 	sprintf(cmd_line, "%sconsole=%s,115200 root=/dev/ram0 rw %s",
	    token_get("CONFIG_SL2312_COMMON_RGLOADER") ? "quiet " : "", 
 	    token_get_str("CONFIG_RG_CONSOLE_DEVICE"),
	    token_get("CONFIG_RG_KGDB") ? "nohalt" : "");
 		
 	token_set("CONFIG_CMDLINE", cmd_line);
 	token_set("CONFIG_ARCH_MACHINE", "sl2312");
 	token_set_y("CONFIG_SERIAL_CONSOLE");
	token_set_y("CONFIG_SERIAL_CORE");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
 
 	token_set_y("CONFIG_MTD");

 	token_set_y("CONFIG_SL2312_SHARE_PIN");
 
 	token_set_y("CONFIG_BINFMT_ELF");
 	token_set_y("CONFIG_DEVPTS_FS");
 	token_set_y("CONFIG_SYSCTL");
 	token_set_y("CONFIG_UNIX98_PTYS");
 	token_set("CONFIG_UNIX98_PTY_COUNT", "256");
 	token_set("CONFIG_BLK_DEV_RAM_SIZE", "16384");

 	if (token_get("CONFIG_RG_DEV"))
 	{
 	    /* debug. kernel larger and slower */
 	    token_set_y("CONFIG_FRAME_POINTER");
 	}
 	token_set_y("CONFIG_SL2312_LPC");
 	token_set_y("CONFIG_SL2312_LPC_IT8712");
 	token_set_y("CONFIG_CPU_FA52X");
 	token_set_y("CONFIG_CPU_FA52X_I_CACHE_ON");
 	token_set_y("CONFIG_CPU_FA52X_D_CACHE_ON");
 	token_set_y("CONFIG_IT8712_GPIO");
 	token_set_y("CONFIG_SERIAL_IT8712");
 	token_set_y("CONFIG_SERIAL_IT8712_CONSOLE");
 	token_set("CONFIG_KERNEL_START", "0xc0000000");
	token_set_y("CONFIG_IDEDMA_AUTO");
	token_set_y("CONFIG_CRAMFS_FS");
	token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	token_set_y("CONFIG_RG_FACTORY_SETTINGS");
    }
    
    if (token_get("CONFIG_IXP425_CSR_FULL"))
    {
    	token_set_y("CONFIG_IXP425_CSR_USB");
	token_set_y("CONFIG_IXP425_CSR_UART");
	token_set_y("CONFIG_IXP425_CSR_ATM");
	token_set_y("CONFIG_IXP425_CSR_HSS");
	token_set_y("CONFIG_IXP425_CSR_CRYPTO_ACC");
	token_set_y("CONFIG_IXP425_CSR_PERF_PROF_ACC");
	token_set_m("CONFIG_IXP425_CODELETS");
	token_set_m("CONFIG_IXP425_CODELET_ETH");
	token_set_m("CONFIG_IXP425_CODELET_USB");
	token_set_m("CONFIG_IXP425_CODELET_ATM");
	token_set_m("CONFIG_IXP425_CODELET_HSS");
	token_set_m("CONFIG_IXP425_CODELET_DMA");
	token_set_m("CONFIG_IXP425_CODELET_CRYPTO");
	token_set_m("CONFIG_IXP425_CODELET_TIMERS");
	token_set_m("CONFIG_IXP425_CODELET_PERF_PROF");
    }

    if (token_get("CONFIG_IXP425_COMMON"))
    {
	static char cmd_line[255];
	
	set_big_endian(1);
	token_set("ARCH", "arm");
	token_set("LIBC_ARCH", "arm");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_ARM");
	token_set_y("CONFIG_STRONGARM");
	token_set_y("CONFIG_NO_UNALIGNED_ACCESS");
	token_set("CONFIG_ALIGNMENT_TRAP_MODE", "2");
	token_set_y("CONFIG_CPU_32");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_CPU_32v4");
	token_set_y("CONFIG_CPU_XSCALE");
	token_set_y("CONFIG_RAMFS");
	token_set_y("CONFIG_RG_MODFS");
	/* smaller size on expense of efficiency */
	token_set_y("CONFIG_ARM_THUMB");
	token_set("CONFIG_ZBOOT_ROM_TEXT", "0");
	token_set("CONFIG_ZBOOT_ROM_BSS", "0");
	/* adds /proc/kcore in elf format for dbg */
	token_set_y("CONFIG_KCORE_ELF");
	/* handle unaligned access (TODO remove) */
	token_set_y("CONFIG_ALIGNMENT_TRAP");
	/* debug purposes - when user mode crashes gives some info */
	token_set_y("CONFIG_DEBUG_USER");
	token_set_y("CONFIG_IXP425_OS_TIMER1");
	if (!token_get("CONFIG_RG_RGLOADER"))
	    token_set_y("CONFIG_DYN_LINK");
	
	if (!IS_DIST("BRUCE") ||
	    !token_get("CONFIG_RG_RGLOADER") ||
	    token_get("CONFIG_RG_JPKG"))
	{
	    token_set_y("CONFIG_PCI");
	}

	if (!token_get("CONFIG_RG_RGLOADER") || token_get("CONFIG_RG_JPKG"))
	{
	    token_set_y("CONFIG_RG_NETTOOLS_ARP");
	    token_set_y("CONFIG_PCI_NAMES");
	    token_set_y("CONFIG_NET_PCI");
	}
	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
		
	sprintf(cmd_line, "%sconsole=%s,115200 root=/dev/ram0 rw%s %s",
	    token_get("CONFIG_IXP425_COMMON_RGLOADER") ? "quiet " : "", 
	    token_get_str("CONFIG_RG_CONSOLE_DEVICE"), 
	    token_get("CONFIG_RG_KGDB") ? " nohalt" : "",
	    token_get("CONFIG_MD") ? "raid=noautodetect" : "");
		
	token_set("CONFIG_CMDLINE", cmd_line);
	token_set_y("CONFIG_ARCH_IXP425");
	token_set("CONFIG_ARCH_MACHINE", "ixp425");
	token_set_m("CONFIG_IXP425_CSR");
	token_set_y("CONFIG_IXP425_CSR_ETH_ACC");
	token_set_y("CONFIG_IXP425_CSR_ETH_DB");
	token_set_y("CONFIG_IXP425_CSR_ETH_MII");
	token_set_y("CONFIG_SERIAL_CONSOLE");
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_INTELEXT"); 
	token_set_y("CONFIG_BINFMT_ELF");
	token_set_y("CONFIG_SERIAL");
	token_set_y("CONFIG_SYSCTL");
	token_set_y("CONFIG_UNIX98_PTYS");
	token_set("CONFIG_UNIX98_PTY_COUNT", "256");
	token_set("CONFIG_BLK_DEV_RAM_SIZE", "16384");
	token_set("ENDIANESS_SUFFIX", "saeb");
	if (token_get("CONFIG_GLIBC"))
	    token_set_y("CONFIG_DYN_LINK");  
	token_set_y("CONFIG_IXP425");

	if (!token_get_str("CONFIG_SDRAM_SIZE"))
	{
	    token_set("CONFIG_SDRAM_SIZE",
		token_get_str("CONFIG_IXP425_SDRAM_SIZE"));
	}
	token_set_m("CONFIG_RG_KRGLDR");
    }

    /* IXP425 VPN HW Acceleration */
    if (token_get("CONFIG_IPSEC_USE_IXP4XX_CRYPTO"))
	token_set_y("CONFIG_IXP425_CSR_CRYPTO_ACC");

    if (token_get("CONFIG_INCAIP_COMMON"))
    {
	static char cmd_line[255];
	
	token_set("BOARD", "Infineon");
	token_set("FIRM", "Infineon");

	token_set("ARCH", "mips");
	token_set("LIBC_ARCH", "mips");
	token_set("ENDIANESS_SUFFIX", "eb");
	set_big_endian(1);
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_INCAIP_EVALBOARD");
	token_set_y("CONFIG_INCAIP");
	token_set_y("CONFIG_INCA_IP");
	token_set_y("CONFIG_MIPS");
	token_set_y("CONFIG_MIPS32");
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	token_set_y("CONFIG_NEW_IRQ");
	token_set_y("CONFIG_NONCOHERENT_IO");
	token_set_y("CONFIG_NEW_TIME_C");
	token_set_y("CONFIG_RAMFS");
	/* TODO Fine-tune watchdog so that it won't prevent rgloader
	 * to load images */
	//token_set_y("CONFIG_INCAIP_WATCHDOG");

	/* CPU selection */
	token_set_y("CONFIG_CPU_MIPS32");
	token_set_y("CONFIG_CPU_HAS_LLSC");
	token_set_y("CONFIG_CPU_HAS_SYNC");

	/* MIPS initrd options */
	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	token_set_y("CONFIG_CRAMFS_FS");

	/* General setup */
	token_set_y("CONFIG_SYSCTL");
	token_set_y("CONFIG_KCORE_ELF");
	token_set_y("CONFIG_BINFMT_ELF");

	/* MTD */
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");
	token_set_m("CONFIG_MTD_INCAIP");
	token_set_y("CONFIG_MTD_CONCAT");

	/* Syncronous Serial Controller (SSC). /dev/ssc1, dev/ssc2 */
	token_set_m("CONFIG_INCAIPSSC");

	token_set_y("CONFIG_UNIX98_PTYS");
	token_set("CONFIG_UNIX98_PTY_COUNT", "256");

	/* Serial drivers (must be statically linked to the kernel) */
	token_set_y("CONFIG_SERIAL_INCAIPASC");
	token_set_y("CONFIG_SERIAL_INCAIPASC_CONSOLE");
	token_set("CONFIG_INCAIPASC_DEFAULT_BAUDRATE", "115200");
	token_set_y("CONFIG_SERIAL_CORE");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set_m("CONFIG_RG_KRGLDR");

	/* I2C support */
#if 0
	token_set_y("CONFIG_I2C");
	token_set_y("CONFIG_I2C_PROC");
#endif

	/* Kernel hacking */
	token_set_y("CONFIG_CROSSCOMPILE");
	token_set_y("CONFIG_MAGIC_SYSRQ");

	/* Block devices */
	token_set("CONFIG_BLK_DEV_RAM_SIZE", "8192");

	sprintf(cmd_line, "%sconsole=ttyS0,115200 root=/dev/ram0 rw",
	    token_get("CONFIG_RG_RGLOADER") ? "quiet " : "");
  	token_set("CONFIG_CMDLINE", cmd_line);

	token_set_y("CONFIG_INCAIP_MUX");
	token_set_y("CONFIG_DEBUG_USER");

	/* Network interface configs */
	token_set_m("CONFIG_INCAIP_SWITCH");
	if (!token_get("CONFIG_RG_RGLOADER"))
	    token_set_m("CONFIG_INCAIP_SWITCH_API");
	token_set_m("CONFIG_INCAIP_ETHERNET");

	if (!token_get("CONFIG_INCAIP_ALLTEK"))
	{
	    /* PWM device */
	    token_set_m("CONFIG_INCAIPPWM");
	}
    }

    if (token_get("CONFIG_INCAIP_MUX"))
    {
	/* Inca-IP Multiplexer Setup */
	token_set_y("CONFIG_INCAIP_MUX_KEY0");
	token_set_y("CONFIG_INCAIP_MUX_KEY1");
	token_set_y("CONFIG_INCAIP_MUX_KEY2");
	token_set_y("CONFIG_INCAIP_MUX_KEY3");
	token_set_y("CONFIG_INCAIP_MUX_KEY4");
	token_set_y("CONFIG_INCAIP_MUX_KEY5");
	token_set_y("CONFIG_INCAIP_MUX_LED0");
	token_set_y("CONFIG_INCAIP_MUX_LED1");
	token_set_y("CONFIG_INCAIP_MUX_LED2");
	token_set_y("CONFIG_INCAIP_MUX_LED3");
	token_set_y("CONFIG_INCAIP_MUX_LED4");
	token_set_y("CONFIG_INCAIP_MUX_LED5");
	token_set_y("CONFIG_INCAIP_MUX_LED6");
	token_set_y("CONFIG_INCAIP_MUX_LED7");
	token_set_y("CONFIG_INCAIP_MUX_LED8");
	token_set_y("CONFIG_INCAIP_MUX_RXD");
	token_set_y("CONFIG_INCAIP_MUX_TXD");
	token_set_y("CONFIG_INCAIP_MUX_SSC1");
	token_set_y("CONFIG_INCAIP_MUX_FSC");
	token_set_y("CONFIG_INCAIP_MUX_DCL");
	token_set_y("CONFIG_INCAIP_MUX_DU");
	token_set_y("CONFIG_INCAIP_MUX_DD");

	if (token_get("CONFIG_INCAIP_ALLTEK"))
	{
	    token_set_y("CONFIG_INCAIP_MUX_P1_16");
	    token_set_y("CONFIG_INCAIP_MUX_P1_17");
	    token_set_y("CONFIG_INCAIP_MUX_P2_9");
	    token_set_y("CONFIG_INCAIP_MUX_P2_10");
	    token_set_y("CONFIG_INCAIP_MUX_P2_11");
	    token_set_y("CONFIG_INCAIP_MUX_P2_12");
	    token_set_y("CONFIG_INCAIP_MUX_P2_13");
	    token_set_y("CONFIG_INCAIP_MUX_P2_14");
	}
	else
	{
	    token_set_y("CONFIG_INCAIP_MUX_KEY6");
	    token_set_y("CONFIG_INCAIP_MUX_KEY7");
	    token_set_y("CONFIG_INCAIP_MUX_SSC2");
	    token_set_y("CONFIG_INCAIP_MUX_PWM1");
	    token_set_y("CONFIG_INCAIP_MUX_PWM2");
	}

	token_set_y("CONFIG_INCAIP_MUX_P2_15");
    }

    if (token_get("CONFIG_CX8620X_COMMON_RGLOADER"))
    {
	/* Override HW settings to save RAM for network boot feature */
	token_set("CONFIG_CX8620X_SDRAM_SIZE", "16");
	/* Enable boot of image from network */
	token_set_y("CONFIG_RG_NETWORK_BOOT");
	token_set_y("CONFIG_CX8620X_COMMON");
	token_set("CONFIG_FORCE_MAX_ZONEORDER", "12");
    }
 
    if (token_get("CONFIG_CX8620X_COMMON"))	
    {
	static char cmd_line[255];
	
	set_big_endian(0);
	token_set_y("CONFIG_CONEXANT_COMMON");
	token_set_y("CONFIG_CX8620X");
	token_set_y("CONFIG_ARCH_CX8620X");
	token_set("CONFIG_ARCH_MACHINE", "cx8620x");

	token_set("ARCH", "arm");
	token_set("LIBC_ARCH", "arm");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_ARM");
	token_set_y("CONFIG_CPU_32");
	token_set_y("CONFIG_CPU_32v5");
	token_set_y("CONFIG_CPU_ARM926T");
	
	token_set_y("CONFIG_NO_UNALIGNED_ACCESS");
	token_set("CONFIG_ALIGNMENT_TRAP_MODE", "2"); // ?
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	//token_set_y("CONFIG_EXPERIMENTAL");
	//token_set_y("CONFIG_SOFT_FLOAT");

	token_set("CONFIG_BLK_DEV_RAM_SIZE", "8192");
	token_set_y("CONFIG_RAMFS");

	token_set_y("CONFIG_CRAMFS_FS");
	token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	if (!token_get("CONFIG_LSP_DIST"))
	{
	    token_set_y("CONFIG_RG_MODFS");
	    token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	}

	token_set("CONFIG_SDRAM_SIZE",
	    token_get_str("CONFIG_CX8620X_SDRAM_SIZE"));

	token_set("CONFIG_ZBOOT_ROM_TEXT", "0");
	token_set("CONFIG_ZBOOT_ROM_BSS", "0");
	/* adds /proc/kcore in elf format for dbg */
	token_set_y("CONFIG_KCORE_ELF");
	/* handle unaligned access (TODO remove) */
	token_set_y("CONFIG_ALIGNMENT_TRAP");
	/* debug purposes - when user mode crashes gives some info */
	token_set_y("CONFIG_DEBUG_USER");
	
	if (!token_get("CONFIG_RG_RGLOADER"))
	{
	    /* XXX: Move PCI_RESET to RGLOADER after B14803 is fixed */
	    token_set_y("CONFIG_DYN_LINK");
	    token_set_y("CONFIG_PCI");
	    token_set_y("CONFIG_PCI_RESET");
	}

#if 0
	if (!token_get("CONFIG_RG_RGLOADER"))
	{
	    token_set_y("CONFIG_DYN_LINK");
	    token_set_y("CONFIG_RG_NETTOOLS_ARP");
	    token_set_y("CONFIG_PCI_NAMES");
	    token_set_y("CONFIG_NET_PCI");
	}
#endif

	sprintf(cmd_line, "console=%s,115200 root=/dev/ram0 rw%s",
	    token_get_str("CONFIG_RG_CONSOLE_DEVICE"), 
	    token_get("CONFIG_RG_KGDB") ? " nohalt" : "");
		
	token_set("CONFIG_CMDLINE", cmd_line);
	token_set_y("CONFIG_SERIAL_CONSOLE");
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CONCAT");
	token_set_y("CONFIG_MTD_CFI_INTELEXT");
	token_set_y("CONFIG_BINFMT_ELF");
	token_set_y("CONFIG_SERIAL");
	token_set_y("CONFIG_SYSCTL");	
	token_set_y("CONFIG_UNIX98_PTYS");
	token_set("CONFIG_UNIX98_PTY_COUNT", "256");
	if (!token_get("CONFIG_LSP_DIST"))
	    token_set_m("CONFIG_RG_KRGLDR");	
	token_set_y("CONFIG_MAGIC_SYSRQ");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
    }

    if (token_get("CONFIG_COMCERTO_COMMON"))	
    {
	static char cmd_line[255];

	/* Needed to build vendor/mindspeed folder */
	token_set_y("CONFIG_MINDSPEED_COMMON");

	/* Needed to build vendor/mindspeed/comcerto folder */
	token_set_y("CONFIG_COMCERTO");
	token_set_y("CONFIG_COMCERTO_SINGLE_IMAGE");
	token_set("CONFIG_ARCH_MACHINE", "comcerto");
	
	/* Processor */
	set_big_endian(0);
	token_set("ARCH", "arm");
	token_set_y("CONFIG_ARM");
	token_set_y("CONFIG_UID16");
	token_set("LIBC_ARCH", "arm");
	token_set_y("CONFIG_CPU_ARM920T");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_CPU_32");
	token_set_y("CONFIG_CPU_32v4");
	token_set_y("CONFIG_CPU_ABRT_EV4T");
	token_set_y("CONFIG_CPU_CACHE_V4WT");
	token_set_y("CONFIG_CPU_CACHE_VIVT");
	token_set_y("CONFIG_CPU_COPY_V4WB");
	token_set_y("CONFIG_CPU_TLB_V4WBI");
	token_set("CONFIG_SPLIT_PTLOCK_CPUS", "4096");
	token_set_y("CONFIG_FLATMEM");
	token_set_y("CONFIG_FLAT_NODE_MEM_MAP");

	token_set_y("CONFIG_ARCH_COMCERTO");
	token_set_y("CONFIG_ARCH_M828XX");
	token_set_y("CONFIG_CSP_16M");
	token_set_y("CONFIG_M828XX_PROC_FS");

	/* MSP Processor*/
	token_set("MSP_CODE_LOCATION", "/lib/modules/msp.axf");
	token_set("MSP_NODE_LOCATION", "/dev/msp");

	/* Flash Memory */
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_COMCERTO");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_1");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_2");
	token_set_y("CONFIG_MTD_MAP_BANK_WIDTH_4");
	token_set_y("CONFIG_MTD_CONCAT");
	token_set_y("CONFIG_MTD_CFI_INTELEXT");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");
	token_set_y("CONFIG_MTD_CFI_I1");
	token_set_y("CONFIG_MTD_CFI_I2");
	token_set_y("CONFIG_MTD_CFI_UTIL");
	token_set("CONFIG_MTD_CFI_AMDSTD_RETRY", "2");
	token_set_y("CONFIG_OBSOLETE_INTERMODULE");

	/* SDRAM memory */
	token_set("CONFIG_SDRAM_SIZE", "112"); /* Needed by OpenRG */

	/* PCI bus */
	token_set_y("CONFIG_PCI"); 
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");

	/* Serial ports */
	token_set_y("CONFIG_SERIAL_8250");
	token_set_y("CONFIG_SERIAL_8250_COMCERTO");
	token_set("CONFIG_SERIAL_8250_NR_UARTS", "2");
	token_set("CONFIG_SERIAL_8250_RUNTIME_UARTS", "2");

	/* SPI */
	token_set_y("CONFIG_SPI");
	token_set_y("CONFIG_COMCERTO_SPI");
	token_set_y("CONFIG_SPI_SI3220");

	/* Console */
	token_set_y("CONFIG_SERIAL_CORE");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
	token_set_y("CONFIG_SERIAL_8250_CONSOLE");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");

	/* SmartMedia*/
#if 0
	token_set_y("CONFIG_MTD_NAND");
	token_set_y("CONFIG_MTD_NAND_COMCERTO");
#endif

	/* File System */
	token_set_y("CONFIG_RG_INITFS_RAMFS");
	token_set_y("CONFIG_RG_MAINFS_CRAMFS");
	token_set_y("CONFIG_RG_MODFS_CRAMFS");

	/* Kernel args */
	sprintf(cmd_line, "console=%s,115200 root=/dev/ram0 rw%s",
	    token_get_str("CONFIG_RG_CONSOLE_DEVICE"),
	    token_get("CONFIG_RG_KGDB") ? " nohalt" : "");
	token_set("CONFIG_CMDLINE", cmd_line);

	/* Various Options */
	token_set("CONFIG_ZBOOT_ROM_TEXT", "0");
	token_set("CONFIG_ZBOOT_ROM_BSS", "0");
	/* adds /proc/kcore in elf format for dbg */
	token_set_y("CONFIG_KCORE_ELF");
	token_set_y("CONFIG_SYSCTL");
	token_set_y("CONFIG_UNIX98_PTYS");
	/* handle unaligned access */
	token_set_y("CONFIG_ALIGNMENT_TRAP");
	/* debug purposes - when user mode crashes gives some info */
	token_set_y("CONFIG_DEBUG_USER");
	token_set_y("CONFIG_BINFMT_ELF");
	token_set("CONFIG_UNIX98_PTY_COUNT", "256");
	token_set_y("CONFIG_MAGIC_SYSRQ");
	
	if (!token_get("CONFIG_RG_RGLOADER"))
	    token_set_y("CONFIG_DYN_LINK");

	if (token_get("CONFIG_COMCERTO_MALINDI"))
	{
	    token_set_y("CONFIG_EVM_MALINDI");
	    token_set("CONFIG_LOCALVERSION","malindi");
	    token_set("BOARD", "Malindi");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "4");
	    
	}
	else if (token_get("CONFIG_COMCERTO_NAIROBI"))
	{
	    token_set_y("CONFIG_EVM_SUPERMOMBASA");
	    token_set("CONFIG_LOCALVERSION","nairobi");
	    token_set("BOARD", "Nairobi");
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "2");
	}
    }
    
    if (token_get("CONFIG_RG_IPPHONE") || token_get("CONFIG_RG_ATA") ||
	token_get("CONFIG_RG_PBX"))
    {
	token_set_y("CONFIG_RG_VOIP");

	if (!token_get("CONFIG_RG_JPKG") &&
	    !token_get_str("CONFIG_HW_NUMBER_OF_FXS_PORTS"))
	{
	    token_set("CONFIG_HW_NUMBER_OF_FXS_PORTS", "0");
	}
    }

    if (token_get("CONFIG_RG_PBX"))
    {
	token_set_y("CONFIG_RG_SAMPLES");
	token_set_y("CONFIG_RG_LIBMAD");
	if (!token_get("CONFIG_RG_UML"))
	    token_set_y("CONFIG_RG_DISK_MNG");
    }

    if (token_get("CONFIG_RG_VOIP_RV_SIP"))
    {
	token_set_y("CONFIG_RG_VOIP_RV");
	token_set_y("CONFIG_RG_VOIP_SIP");
    }

    if (token_get("CONFIG_RG_VOIP_RV_H323"))
    {
	token_set_y("CONFIG_RG_VOIP_RV");
	token_set_y("CONFIG_RG_VOIP_H323");
    }

    if (token_get("CONFIG_RG_VOIP_RV_MGCP"))
    {
	token_set_y("CONFIG_RG_VOIP_RV");
	token_set_y("CONFIG_RG_VOIP_MGCP");
    }

    if (token_get("CONFIG_RG_VOIP_OSIP"))
	token_set_y("CONFIG_RG_VOIP_SIP");

    if (token_get("CONFIG_RG_VOIP_ASTERISK_SIP"))
    {
	token_set_y("CONFIG_RG_VOIP_ASTERISK");
	token_set_y("CONFIG_RG_VOIP_SIP");
    }

    if (token_get("CONFIG_RG_VOIP_ASTERISK_H323"))
    {
	token_set_y("CONFIG_RG_VOIP_ASTERISK");
	token_set_y("CONFIG_RG_VOIP_H323");
	token_set_y("CONFIG_RG_CXX");
    }

    if (token_get("CONFIG_RG_VOIP_ASTERISK_MGCP_GW"))
    {
	token_set_y("CONFIG_RG_VOIP_ASTERISK");
	token_set_y("CONFIG_RG_VOIP_MGCP");
    }

    if (token_get("CONFIG_RG_VOIP_ASTERISK_MGCP_CALL_AGENT"))
    {
	token_set_y("CONFIG_RG_VOIP_ASTERISK");
	token_set_y("CONFIG_RG_VOIP_MGCP_CALL_AGENT");
    }

    if (token_get("CONFIG_RG_VOIP_SIP"))
	token_set_y("CONFIG_RG_VOIP_USE_SIP_ALG");

    if (token_get("CONFIG_RG_VOIP_RV") || token_get("CONFIG_RG_VOIP_OSIP") ||
	token_get("CONFIG_RG_VOIP_ASTERISK"))
    {
	token_set_y("CONFIG_RG_VOIP");
    }
    
    if (token_get("CONFIG_RG_VOIP"))
    {
	token_set_y("CONFIG_RG_THREADS");
	token_set_m("CONFIG_RG_JRTP");
    }

    if (token_get("CONFIG_RG_VOIP_ASTERISK"))
    {
	token_set_y("CONFIG_RG_THREADS");
	token_set_y("CONFIG_RG_TERMCAP");
	token_set_y("CONFIG_ULIBC_DO_C99_MATH");
	token_set_y("CONFIG_ULIBC_SHARED");
    }

    if (token_get("CONFIG_INCAIP_WATCHDOG"))
    {
	token_set_y("CONFIG_INCAIP_WATCHDOG_HEARTBEAT");
	token_set("CONFIG_INCAIP_WATCHDOG_TIMEOUT", "4");
    }

    if (token_get("CONFIG_INFINEON_TWINPASS"))
    {
	static char cmd_line[255];

	token_set("BOARD", "TWINPASS");
	token_set("FIRM", "INFINEON");
	token_set_y("CONFIG_BOOTLDR_UBOOT");

	/* Linux configs */
	/* Generic */	
	token_set_y("CONFIG_MODULES");
	token_set_y("CONFIG_NEW_IRQ");
	token_set_y("CONFIG_NONCOHERENT_IO");
	token_set_y("CONFIG_NEW_TIME_C");
	token_set_y("CONFIG_PAGE_SIZE_4KB");
	token_set_y("CONFIG_RAMFS");
	
	if (token_get("CONFIG_PCI"))
	{
	    token_set_y("CONFIG_PCI_AUTO");
	    token_set_y("CONFIG_DANUBE_PCI");
	    token_set_y("CONFIG_DANUBE_PCI_HW_SWAP");
	}

	token_set_y("CONFIG_SYSCTL");
	token_set_y("CONFIG_KCORE_ELF");
	token_set_y("CONFIG_BINFMT_ELF");
	
	// not needed probably
	token_set_y("CONFIG_BLK_DEV_LOOP");
	
	token_set_y("CONFIG_UNIX98_PTYS");
	token_set("CONFIG_UNIX98_PTY_COUNT", "32");
	token_set_y("CONFIG_NET_HW_FLOWCONTROL");
	token_set_y("CONFIG_CROSSCOMPILE");
	token_set_y("CONFIG_MAGIC_SYSRQ");

	/* MIPS Twinpass aka Danube */
	token_set("ARCH", "mips");
	token_set_y("CONFIG_MIPS");
	token_set_y("CONFIG_MIPS32");
	token_set_y("CONFIG_CPU_MIPS32");
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_CPU_HAS_PREFETCH");
	token_set_y("CONFIG_CPU_HAS_LLSC");
	token_set_y("CONFIG_CPU_HAS_SYNC");
	token_set_y("CONFIG_DANUBE");
	token_set_y("CONFIG_BOARD_TWINPASS_E");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	
	//token_set_y("CONFIG_DANUBE_MPS");
	//token_set_y("CONFIG_DANUBE_MPS_PROC_DEBUG");

	/* UART */
	token_set_y("CONFIG_SERIAL_IFX_ASC");
	token_set_y("CONFIG_SERIAL_IFX_ASC_CONSOLE");
	token_set("CONFIG_IFX_ASC_DEFAULT_BAUDRATE", "115200");
	token_set_y("CONFIG_SERIAL_CORE_CONSOLE");
	/* XXX - For some reason core 1 can only use ttyS1 - the regular serial
	 * (u-boot also uses ttyS1). Core 0 (OpenRG) must use ttyS0.
	 * We should fix it so that both u-boot and OpenRG prints out to the main
	 * serial (ttyS1).
	 */
	if (token_get("CONFIG_RG_DANUBE_DUALCORE"))
	{
		token_set_y("CONFIG_IFX_ASC_CONSOLE_ASC0");
		token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	}
	else
	{
		token_set_y("CONFIG_IFX_ASC_CONSOLE_ASC1");
		token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	}

	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	token_set_y("CONFIG_CRAMFS_FS");
	token_set_y("CONFIG_RG_MODFS");

	sprintf(cmd_line, "console=%s,115200",
	    token_get_str("CONFIG_RG_CONSOLE_DEVICE"));
  	token_set("CONFIG_CMDLINE", cmd_line);

	/* OpenRG configs */
	set_big_endian(1);
	token_set("LIBC_ARCH", "mips");
	token_set_y("CONFIG_HAS_MMU");	
    }
    
    if (token_get("CONFIG_ADM5120_COMMON"))
    {
	static char cmd_line[255];
	
	token_set("BOARD", "ADM5120");
	token_set("FIRM", "ADMTEK");

	set_big_endian(0);
	token_set("ARCH", "mips");
	token_set("LIBC_ARCH", "mips");
	token_set_y("CONFIG_HAS_MMU");
	token_set_y("CONFIG_MIPS_AM5120");
	token_set_y("CONFIG_MIPS");
	token_set_y("CONFIG_MIPS32");
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	token_set_y("CONFIG_NEW_IRQ");
	token_set_y("CONFIG_NONCOHERENT_IO");
	token_set_y("CONFIG_NEW_TIME_C");
	token_set("MAX_DMA_ADDRESS", "0x1000000");
	token_set("IO_SPACE_LIMIT", "0xffffffff");
	token_set("CONFIG_SDRAM_SIZE","0xffff00");
	token_set_y("CONFIG_RAMFS");

	/* CPU selection */
	token_set_y("CONFIG_CPU_MIPS32");
	token_set_y("CONFIG_CPU_HAS_LLSC");
	token_set_y("CONFIG_CPU_HAS_SYNC");
	token_set_y("CONFIG_CPU_HAS_PREFETCH");
	token_set_y("CONFIG_VTAG_ICACHE");

	token_set_y("CONFIG_RG_FOOTPRINT_REDUCTION");
	token_set_y("CONFIG_COPY_CRAMFS_TO_RAM");
	token_set_y("CONFIG_CRAMFS_FS");
	if (!token_get("CONFIG_RG_RGLOADER"))
	{
	    if (IS_DIST("ADM5120_ATA"))
		token_set_y("CONFIG_RG_NONCOMPRESSED_USERMODE_IMAGE");
	    token_set_y("CONFIG_RG_MODFS");
	}

	/* General setup */
	token_set_y("CONFIG_SYSCTL");
	token_set_y("CONFIG_KCORE_ELF");
	token_set_y("CONFIG_BINFMT_ELF");

	/* MTD */
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");
	token_set_m("CONFIG_ADM5120_MTD");

	token_set_y("CONFIG_UNIX98_PTYS");
	token_set("CONFIG_UNIX98_PTY_COUNT", "256");

	/* switch/ethernet */
	token_set_m("CONFIG_ADM5120_SWITCH");

	/* Serial drivers (must be statically linked to the kernel) */
	token_set_y("CONFIG_ADM5120_SERIAL");
	token_set_y("CONFIG_SERIAL_CONSOLE");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	if (!token_get("CONFIG_LSP_DIST"))
	    token_set_m("CONFIG_RG_KRGLDR");

	/* Kernel hacking */
	token_set_y("CONFIG_CROSSCOMPILE");
	token_set_y("CONFIG_MAGIC_SYSRQ");

	/* Block devices */
	token_set("CONFIG_BLK_DEV_RAM_SIZE", "16384");

	sprintf(cmd_line, "%sconsole=ttyS0,115200 root=/dev/ram0 rw",
	    token_get("CONFIG_RG_RGLOADER") ? "quiet " : "");
  	token_set("CONFIG_CMDLINE", cmd_line);

	token_set_y("CONFIG_DEBUG_USER");
    }

    if (token_get("CONFIG_RG_CABLEHOME") || token_get("CONFIG_RG_DSL_CH"))
    {
	token_set_y("CONFIG_RG_TODC");
	token_set_y("CONFIG_RG_PING");
	token_set_y("CONFIG_RG_SYSLOG_REMOTE");
	token_set_y("CONFIG_RG_SSL");
	token_set_y("CONFIG_RG_ALG_H323");
	token_set_y("CONFIG_RG_ALG_MSNMS");
	token_set_y("CONFIG_RG_ALG_AIM");
	token_set_y("CONFIG_RG_ALG_PPTP");
	token_set_y("CONFIG_RG_ALG_IPSEC");
	token_set_y("CONFIG_RG_ALG_L2TP");
	token_set_y("CONFIG_RG_CH_EVT_RPT");
    }

    if (token_get("CONFIG_RG_CABLEHOME"))
    {
	/* Configuration for CableHome that we don't want for DSL-CH */
	if (token_get("CONFIG_RG_CABLEHOME_10"))
	    token_set_y("CONFIG_RG_RMT_FW_CONF");
	if (!token_get("CONFIG_RG_CABLEHOME_EMBEDDED"))
	{
	    token_set_y("CONFIG_RG_STANDALONE_PS");
	    token_set_y("CONFIG_RG_RMT_UPDATE");
	}

	token_set_y("CONFIG_BIDIR_NAT");
	token_set_y("CONFIG_RG_CH_FW");
	token_set_y("CONFIG_RG_CONN_SPEED");
	token_set_y("CONFIG_RG_KERBEROS");
	token_set_y("CONFIG_RG_CERT");
	token_set_y("CONFIG_RG_XML");
	token_set_m("CONFIG_RG_PKTCBL");
	token_set_y("CABLEHOME_TEP_BUG");
	token_set_m("CONFIG_RG_BRIDGE");
	token_set_m("CONFIG_RG_USFS");
	token_set_m("CONFIG_RG_BROUTE");
	token_set_m("CONFIG_RG_CHWAN");
    }

    if (token_get("CONFIG_RG_WAN_UPGRADE") || 
	token_get("CONFIG_RG_RMT_FW_CONF") ||
	token_get("CONFIG_RGLOADER_CLI") ||
	token_get("CONFIG_RG_DDNS"))
    {
	token_set_y("CONFIG_RG_WGET");
    }

    if (token_get("CONFIG_MTD")) 
    {
	token_set_y("CONFIG_MTD_PARTITIONS");
	token_set_y("CONFIG_MTD_BLOCK");
	token_set_y("CONFIG_MTD_CFI");
	token_set_y("CONFIG_MTD_GEN_PROBE");
    }

    if (token_get("CONFIG_RG_KGDB"))
    {
	if (token_get("CONFIG_ARM"))
	{
	    token_set_y("CONFIG_KGDB");
	    token_set_y("CONFIG_KGDB_SERIAL");
	    if (token_get("CONFIG_IXP425_COMMON"))
		token_set_y("CONFIG_DEBUG_LL");
	    token_set_y("CONFIG_MAGIC_SYSRQ");
	}
	else if (token_get("CONFIG_PCBOX") && !token_get("CONFIG_RG_UML"))
	{
	    if (strcmp(os, "LINUX_24")) 
		token_set_y("CONFIG_GDB");
	    else
	    {
		token_set_y("CONFIG_X86_REMOTE_DEBUG");
		token_set_y("CONFIG_KGDB_THREAD");
		token_set_y("CONFIG_KERNEL_ASSERTS");
		token_set_y("CONFIG_MAGIC_SYSRQ");
	    }
	}
	/* Remove for INCAIP since no kgdb stub exists */
	else if (token_get("CONFIG_MIPS"))
	{
	    token_set_y("CONFIG_KGDB");
	    /* Mips moved to CONFIG_KGDB from 2.4.21.mips.
	     * This config is kept for compatability with platforms which still
	     * need it.
	     */
	    token_set_y("CONFIG_REMOTE_DEBUG");
	}
    }

    /* IPV6 support */
    if (token_get("CONFIG_RG_IPV6"))
    {
  	token_set_y("CONFIG_IPV6");
	if (token_get("CONFIG_RG_OS_LINUX_24"))
	{
	    token_set_y("CONFIG_IPV6_RESTRICTED_DOUBLE_BIND");
	    token_set_y("CONFIG_IPV6_6TO4_NEXTHOP");
	}
	token_set_y("CONFIG_RG_FLEX");
	token_set_y("CONFIG_RG_USAGI");
    }

    if (token_get("CONFIG_RG_PERM_STORAGE") &&
	!token_get("CONFIG_RG_FLASHDISK") && !token_get("CONFIG_MTD") &&
	!token_get("CONFIG_RG_UML") && !IS_DIST("JPKG_UML") &&
	!token_get("CONFIG_VX_TFFS"))
    {
	token_set_m("CONFIG_FLASH");
    }

    /* DOS gcc cannot optimize */
    if (!token_get("CONFIG_GCC_CANNOT_OPTIMIZE"))
	token_set_y("CONFIG_GCC_CAN_INLINE");
    
    if (token_get("CONFIG_RG_DHCPC") || token_get("CONFIG_RG_DHCPS") ||
	token_get("CONFIG_RG_DHCPR"))
    {
	token_set_m("CONFIG_RG_DHCP_PKTFIL");
    }

    if (token_get("CONFIG_RG_DHCPS"))
	token_set_y("CONFIG_RG_PING");
    
    if (token_get("CONFIG_RG_8021X_TLS") || token_get("CONFIG_RG_8021X_TTLS") ||
	token_get("CONFIG_RG_8021X_MD5") || token_get("CONFIG_RG_8021X_RADIUS"))
    {
	token_set_y("CONFIG_RG_8021X");
    }

    if (token_get("CONFIG_RG_8021X"))
	token_set_m("CONFIG_RG_8021X_PKTFIL");
    if (token_get("CONFIG_FREESWAN") ||
       	token_get("CONFIG_RG_8021X_TLS") ||
       	token_get("CONFIG_RG_8021X_TTLS"))
    {
	token_set_y("CONFIG_RG_X509");
    }
    
    if (token_get("CONFIG_RG_SSL") || token_get("CONFIG_RG_X509"))
	token_set_y("CONFIG_RG_CERT");
    if (token_get("CONFIG_RG_RMT_FW_CONF") || 
	token_get("CONFIG_RG_UCD_SNMP_V3"))
    {
	token_set_y("CONFIG_RG_CRYPTO");
    }

    if (token_get("CONFIG_RG_CERT") || token_get("CONFIG_RG_CRYPTO"))
	token_set_y("CONFIG_RG_OPENSSL");

    if (token_get("CONFIG_RG_OPENSSL"))
	token_set_y("CONFIG_RG_CRYPTO");

    if (token_get("CONFIG_RG_IGD") || token_get("CONFIG_RG_TR_064"))
	token_set_y("CONFIG_RG_UPNP");

    if (token_get("CONFIG_RG_UPNP") || token_get("CONFIG_RG_DSLHOME"))
    {
	token_set_y("CONFIG_RG_XML");
	token_set_y("CONFIG_RG_MGT_IGD");
    }

    if (token_get("CONFIG_OPENRG") && token_get("CONFIG_RG_WBM"))
	token_set_y("CONFIG_RG_SESSION_MEMORY");

    if (token_get("CONFIG_OPENRG") &&
	(token_get("CONFIG_RG_WBM") || token_get("CONFIG_RG_SSI")))
    {
	token_set_y("CONFIG_RG_HTTPS");
	token_set_y("CONFIG_RG_LIBIMAGE_DIM");
	/* Add all themes if no theme was added previously */
	if (!is_gui_selected())
	    select_all_themes();
    }

    if (token_get("CONFIG_RG_LIBIMAGE_DIM"))
	token_set_y("CONFIG_RG_LIBIMAGE");

    if (token_get("CONFIG_RG_SSI"))
	token_set_y("CONFIG_RG_HTTPS");

    if (token_get("CONFIG_RG_HTTPS"))
	token_set_y("CONFIG_HTTP_CGI_PROCESS");

    if (token_get("CONFIG_RG_ALG_MSNMS"))
	token_set_y("CONFIG_RG_ALG_SIP");

    if (token_get("CONFIG_RG_PPP_DEFLATE"))
    {
	token_set_y("CONFIG_ZLIB_INFLATE");
	token_set_y("CONFIG_ZLIB_DEFLATE");
    }

    if (token_get("CONFIG_RG_TZ_COMMON") || token_get("CONFIG_RG_TZ_FULL"))
    {
	token_set_y("CONFIG_RG_AUTO_DST");
	if (!token_get("CONFIG_RG_TZ_YEARS"))
	    token_set("CONFIG_RG_TZ_YEARS", "5");
    }

    /* If a firewall feature is requested, enable one of the possible firewall
     * modules.
     */
    if (token_get("CONFIG_RG_FIREWALL") || token_get("CONFIG_RG_NAT") ||
	token_get("CONFIG_RG_RNAT"))
    {
	token_set_y("CONFIG_RG_GENERIC_PROXY");
	if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
	    token_set_y("CONFIG_RG_HTTP_AUTH");
	token_set_y("CONFIG_RG_ALG_USERSPACE");
	token_set_m("CONFIG_RG_JFW");
	token_set_y("CONFIG_RG_NETOBJ");
    }

    if (token_get("CONFIG_ZERO_CONFIGURATION"))
	token_set_y("CONFIG_ZC_AUTO_CONFIG_NON_PNP");

    if (token_get("CONFIG_RG_WPA"))
	token_set_y("CONFIG_RG_WPA_WBM");

    if (token_get("CONFIG_RG_8021X"))
	token_set_y("CONFIG_RG_8021X_WBM");

    if (token_get("CONFIG_RG_8021Q_IF"))
	token_set_y("CONFIG_RG_VLAN_8021Q");
        
    if (token_get("CONFIG_RG_VLAN_8021Q"))
	token_set_y("CONFIG_VLAN_8021Q");

    if (token_get("CONFIG_FILESERVER_KERNEL_CONFIG"))
    {
	/* USB controller VIA VT6202: EHCI + 2 x UHCI */
	if (token_get("CONFIG_RG_OS_LINUX_24"))
	{
	    token_set_y("CONFIG_RG_USB");
	    token_set_m("CONFIG_USB_OHCI");
	    token_set_m("CONFIG_USB_UHCI");
	    token_set_m("CONFIG_USB_EHCI_HCD");
	}
	else
	{
	    token_set_y("CONFIG_USB");
	    token_set_y("CONFIG_USB_OHCI_HCD");
	    token_set_y("CONFIG_USB_UHCI_HCD");
	    token_set_y("CONFIG_USB_EHCI_HCD");
	}

	token_set_y("CONFIG_USB_DEVICEFS");

	if (strcmp(token_get_str("CONFIG_FILESERVER_KERNEL_CONFIG"), "USB"))
	{
	    /* Firewire VIA VT6307: IEEE1394 */
	    token_set_y("CONFIG_IEEE1394");
	    token_set_y("CONFIG_IEEE1394_OHCI1394");

	    if (token_get("CONFIG_RG_OS_LINUX_24"))
	    {
		/* Currently SBP2 is only supported as module */
		token_set_m("CONFIG_IEEE1394_SBP2");
	    }
	    else
		token_set_y("CONFIG_IEEE1394_SBP2");
	}

	/* Storage support for file server */
	/* XXX should not be a module until B15165 is fixed */
	token_set_y("CONFIG_USB_STORAGE");
	
	/* Hot plug is needed as both Firewire and USB storage devices are
	 * hot pluggable
	 */
	token_set_y("CONFIG_HOTPLUG");
    }

    if (token_get("CONFIG_RG_USB_SLAVE"))
    {
	token_set_m("CONFIG_RG_RNDIS");
	token_set_y("CONFIG_USB_RNDIS");
	token_set_y("CONFIG_RG_USB");
    }

    if (token_get("CONFIG_IXP425_ATM"))
    {
	token_set_y("CONFIG_ATM");
	token_set_y("CONFIG_IXP425_CSR_ATM");
    }

    if (token_get("CONFIG_PRISM2_PCI"))
    {
	token_set_y("CONFIG_INTERSIL_COMMON");
	token_set_y("CONFIG_NET_RADIO");
	token_set_y("CONFIG_NET_WIRELESS");
	token_set_y("CONFIG_PRISM2_LOAD_FIRMWARE");
    }

    if (token_get("CONFIG_ISL38XX"))
    {
	token_set_y("CONFIG_INTERSIL_COMMON");
	token_set_y("CONFIG_NET_RADIO");
	token_set_y("CONFIG_NET_WIRELESS");
	token_set_y("INTERSIL_EVENTS");
    }

    if (token_get("CONFIG_ISL_SOFTMAC"))
    {
	token_set_y("CONFIG_INTERSIL_COMMON");
	token_set_y("CONFIG_NET_RADIO");
	token_set_y("CONFIG_NET_WIRELESS");
    }

    if (token_get("CONFIG_RALINK_RT2560") || token_get("CONFIG_AGN100") || 
	token_get("CONFIG_RALINK_RT2561"))
    {
	token_set_y("CONFIG_NET_RADIO");
	token_set_y("CONFIG_NET_WIRELESS");
    }

    if (token_get("CONFIG_HW_80211G_BCM43XX"))
    {
	token_set_y("CONFIG_NET_RADIO");
	token_set_y("CONFIG_NET_WIRELESS");
	token_set("CONFIG_BCM43XX_MODE", "AP");
    }

    if (token_get("CONFIG_RG_RGLOADER"))
	token_set_y("CONFIG_RG_BOOTSTRAP");
    
    if (token_get("CONFIG_RG_FOOTPRINT_REDUCTION"))
    {
	token_set_y("CONFIG_RG_KERNEL_NEEDED_SYMBOLS");
	if (token_get("CONFIG_RG_RGLOADER"))
	{
	    token_set_y("CONFIG_SMALL_FLASH");
	    token_set_y("CONFIG_RG_NONCOMPRESSED_USERMODE_IMAGE");
	}
    }

    if (token_get("CONFIG_RG_OS_LINUX_26"))
    {
	token_set_y("CONFIG_RG_LARGE_FILE_SUPPORT");
	token_set_y("CONFIG_RG_MAINFS");

	if (token_get("CONFIG_RG_INITFS_RAMFS"))
	{
	    token_set("CONFIG_INITRAMFS_SOURCE", "$(RAMDISK_MOUNT_POINT)");
	    token_set("CONFIG_INITRAMFS_ROOT_UID", "$(shell id -u)");
	    token_set("CONFIG_INITRAMFS_ROOT_GID", "$(shell id -g)");
	}

	if (token_get("CONFIG_RG_MAINFS_CRAMFS"))
	    token_set_y("CONFIG_CRAMFS_FS");

	if (token_get("CONFIG_RG_MODFS_CRAMFS"))
	{
	    token_set_y("CONFIG_CRAMFS_FS");
	    token_set_y("CONFIG_RG_MODFS");
	}
	
	if (token_get("CONFIG_CRAMFS_FS"))
	{
	    if (!token_get("CONFIG_RG_JPKG_SRC"))
		token_set_y("CONFIG_CRAMFS_FS_COMMON");

	    token_set_y("CONFIG_CRAMFS");
	    token_set_y("CONFIG_ZLIB_INFLATE");

	    /* Optimized CRAMFS compression, a little slower. */
	    token_set_y("CONFIG_CRAMFS_DYN_BLOCKSIZE");

	    if (!token_get_str("CONFIG_RG_CRAMFS_COMP_METHOD"))
	    {
		if (token_get("CONFIG_RG_NONCOMPRESSED_USERMODE_IMAGE"))
		    token_set("CONFIG_RG_CRAMFS_COMP_METHOD", "none");
		else
		    token_set("CONFIG_RG_CRAMFS_COMP_METHOD", "lzma");
	    }

	    if (token_get("CONFIG_CRAMFS_DYN_BLOCKSIZE"))
	    {
		if (!strcmp(token_get_str("CONFIG_RG_CRAMFS_COMP_METHOD"),
		    "lzma"))
		{
		    token_set("CONFIG_CRAMFS_BLKSZ", "65536");
		}
		else
		    token_set("CONFIG_CRAMFS_BLKSZ", "32768");
	    }
	}	
    }
    
    if (token_get("CONFIG_RG_OS_LINUX_24"))
    {
	if (token_get("CONFIG_SIMPLE_RAMDISK"))
	    token_set_y("CONFIG_RG_INITFS_RAMDISK");

	if (!token_get("CONFIG_RG_INITFS_RAMDISK"))
	    token_set_y("CONFIG_RG_ROOTFS_TMPFS");

	if (token_get("CONFIG_RG_MODFS"))
	{
	    token_set_y("CONFIG_ROMFS_FS");
	    token_set_y("CONFIG_RG_MODFS_ROMFS");
	}

	if (token_get("CONFIG_RG_ROOTFS_TMPFS"))
	{
	    token_set_y("CONFIG_TMPFS");
	    token_set_y("CONFIG_RG_INITFS_CRAMFS");
	}

	token_set_y("CONFIG_RG_LARGE_FILE_SUPPORT");

	if (token_get("CONFIG_RG_INITFS_RAMDISK"))
	{
	    token_set_y("CONFIG_BLK_DEV_INITRD");
	    token_set_y("CONFIG_BLK_DEV_RAM");
	    token_set_y("CONFIG_RG_ROOTFS_RAMDISK");
	    if (!token_get("CONFIG_RG_UML"))
		token_set_y("CONFIG_EMBEDDED_RAMDISK");
	    if (!token_get("CONFIG_RG_RGLOADER") ||
		token_get("CONFIG_RG_JPKG"))
	    {
		token_set_y("CONFIG_EXT2_FS");
	    }
	}

	if (token_get("CONFIG_CRAMFS_FS"))
	{
	    token_set_y("CONFIG_CRAMFS_FS_COMMON");

	    token_set_y("CONFIG_CRAMFS");
	    token_set_y("CONFIG_ZLIB_INFLATE");

	    if (token_get("CONFIG_COPY_CRAMFS_TO_RAM"))
	    {
		/* Optimized CRAMFS compression, a little slower. */
		token_set_y("CONFIG_CRAMFS_DYN_BLOCKSIZE");
	    }

	    if (!token_get_str("CONFIG_RG_CRAMFS_COMP_METHOD"))
	    {
		if (token_get("CONFIG_RG_NONCOMPRESSED_USERMODE_IMAGE"))
		    token_set("CONFIG_RG_CRAMFS_COMP_METHOD", "none");
		else
		    token_set("CONFIG_RG_CRAMFS_COMP_METHOD", "lzma");
	    }

	    if (token_get("CONFIG_CRAMFS_DYN_BLOCKSIZE"))
	    {
		if (!strcmp(token_get_str("CONFIG_RG_CRAMFS_COMP_METHOD"),
		    "lzma"))
		{
		    token_set("CONFIG_CRAMFS_BLKSZ", "65536");
		}
		else
		    token_set("CONFIG_CRAMFS_BLKSZ", "32768");
	    }
	}
    }

    {
	char *kernel_comp_method;
	
	/* Figure out the kernel compression method needed config. */
	kernel_comp_method = 
	    token_get_str("CONFIG_RG_KERNEL_COMP_METHOD") ? : "";
	
	if (!strcmp(kernel_comp_method, "lzma"))
	    token_set_y("CONFIG_RG_LZMA_COMPRESSED_KERNEL");
	if (!strcmp(kernel_comp_method, "gzip"))
	    token_set_y("CONFIG_RG_GZIP_COMPRESSED_KERNEL");
	if (!strcmp(kernel_comp_method, "bzip2"))
	    token_set_y("CONFIG_RG_BZIP2_COMPRESSED_KERNEL");

	if (token_get("CONFIG_RG_BZIP2_COMPRESSED_KERNEL"))
	    token_set_y("CONFIG_RG_BZIP2");
    }
    
    if (token_get("CONFIG_HW_USB_HOST_UHCI"))
    {
	 if (!token_get("CONFIG_RG_OS_LINUX_26"))
	 {
	     token_set_y("CONFIG_RG_USB");
	     token_set("CONFIG_USB_UHCI",
		 token_get_str("CONFIG_HW_USB_HOST_UHCI"));
	 }
	 else
	 {
	     token_set_y("CONFIG_USB");
	     token_set("CONFIG_USB_UHCI_HCD",
		 token_get_str("CONFIG_HW_USB_HOST_UHCI"));
	 }
	token_set_y("CONFIG_USB_DEVICEFS");
    }

    if (token_get("CONFIG_HW_USB_HOST_OHCI"))
    {
	if (!token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    token_set_y("CONFIG_RG_USB");
	    token_set("CONFIG_USB_OHCI",
		token_get_str("CONFIG_HW_USB_HOST_OHCI"));
	}
	else
	{
	    token_set_y("CONFIG_USB");
	    token_set("CONFIG_USB_OHCI_HCD",
		token_get_str("CONFIG_HW_USB_HOST_OHCI"));
	}
	token_set_y("CONFIG_USB_DEVICEFS");
    }

    if (token_get("CONFIG_HW_USB_HOST_EHCI"))
    {
	if (!token_get("CONFIG_RG_OS_LINUX_26"))
	    token_set_y("CONFIG_RG_USB");
	else
	    token_set_y("CONFIG_USB");
	token_set_y("CONFIG_USB_DEVICEFS");
	token_set("CONFIG_USB_EHCI_HCD",
	    token_get_str("CONFIG_HW_USB_HOST_EHCI"));
    }

    if (token_get("CONFIG_RG_WEBCAM"))
    {
        token_set_y("CONFIG_RG_LIBJPEG");
        token_set_y("CONFIG_HW_CAMERA_USB_PWC");
        token_set_y("CONFIG_HW_CAMERA_USB_OV511");
        token_set_y("CONFIG_HW_CAMERA_USB_SPCA5XX");
    }

    if (token_get("CONFIG_HW_FIREWIRE"))
    {
	token_set("CONFIG_IEEE1394", token_get_str("CONFIG_HW_FIREWIRE"));
	token_set("CONFIG_IEEE1394_OHCI1394",
	    token_get_str("CONFIG_HW_FIREWIRE"));
    }

    if (token_get("CONFIG_HW_USB_STORAGE"))
    {
	token_set("CONFIG_USB_STORAGE", token_get_str("CONFIG_HW_USB_STORAGE"));
	if (!token_get("CONFIG_LSP_DIST"))
	    token_set_y("CONFIG_HOTPLUG");
    }

    /* Currently SBP2 is only supported as module */
    if (token_get("CONFIG_HW_FIREWIRE_STORAGE"))
    {
	token_set_m("CONFIG_IEEE1394_SBP2");
	if (!token_get("CONFIG_LSP_DIST"))
	    token_set_y("CONFIG_HOTPLUG");
    }

    if (token_get("CONFIG_RG_EVENT_LOGGING"))
    {
    	token_set_y("CONFIG_RG_SYSLOG");
	token_set_m("CONFIG_RG_LOG_DEV");
	token_set("CONFIG_LOG_FILES_MIN_SIZE", "12288");
	token_set("CONFIG_LOG_FILES_MAX_SIZE", "16384");
	if (!token_get("CONFIG_RG_OS_VXWORKS"))
	{
	    token_set_y("CONFIG_RG_KERN_LOG");
	    token_set_y("CONFIG_SYSLOG_UNIXSOCK_SUPPORT");
	}
	token_set_y("CONFIG_RG_SYSLOG_REMOTE"); 
    }
    if (token_get("CONFIG_RG_FW_ICSA"))
    {
	/* 128 KB */
	token_set("CONFIG_RG_LOG_DEV_BUF_SIZE", "131072");
	token_set("CONFIG_RG_SYSLOG_FW_DEF_SIZE", "131072");
	/* Secured transmission Secured Login; HTTP-S; Telnet-S */
	token_set_y("CONFIG_RG_SSL");
	/* For Local Admin */
	token_set_y("CONFIG_RG_PPPOS_CLI");	
    }
    else if (token_get("CONFIG_RG_SYSLOG") &&
	!token_get("CONFIG_RG_LOG_DEV_BUF_SIZE"))
    {
	/* 100 KB */
	token_set("CONFIG_RG_LOG_DEV_BUF_SIZE", "102400");
	token_set("CONFIG_RG_SYSLOG_FW_DEF_SIZE", "102400");
    }

    if (!token_get("CONFIG_RG_BUILD_LOCAL_TARGETS_ONLY") &&
	(token_get("CONFIG_RG_JPKG") ||
	token_get("CONFIG_GLIBC") || 
	(token_get("CONFIG_ULIBC") && token_get("ULIBC_IN_TOOLCHAIN"))))
    {
	/* There is a bug (B28967) in glibc/ulibc, which is fixed in pkg/ulibc.
	 * Workaround this bug in other libcs (glibc, or ulibc from toolchain) 
	 * by using code from this glibc directory.
	 */
	token_set_y("CONFIG_RG_SYSLOG_GLIBC");
    }
    if (token_get("CONFIG_RG_SYSLOG") || token_get("CONFIG_RG_SYSLOG_GLIBC"))
	token_set_y("CONFIG_RG_SYSLOG_COMMON");

    /* OV511 Video Camera */
    if (token_get("CONFIG_HW_CAMERA_USB_OV511"))
    {
	token_set("CONFIG_USB_OV511", 
	    token_get_str("CONFIG_HW_CAMERA_USB_OV511"));
    }

    /* PWC Video Camera */
    if (token_get("CONFIG_HW_CAMERA_USB_PWC"))
        token_set("CONFIG_USB_PWC", token_get_str("CONFIG_HW_CAMERA_USB_PWC"));

    /* SPCA5XX Video Camera */
    if (token_get("CONFIG_HW_CAMERA_USB_SPCA5XX"))
    {
	token_set("CONFIG_USB_SPCA5XX",
	    token_get_str("CONFIG_HW_CAMERA_USB_SPCA5XX"));
    }

    /* Video Device */
    if (token_get("CONFIG_HW_CAMERA_USB_OV511") ||
        token_get("CONFIG_HW_CAMERA_USB_PWC") ||
        token_get("CONFIG_HW_CAMERA_USB_SPCA5XX"))
    {
	token_set_y("CONFIG_VIDEO_DEV");
	token_set_y("CONFIG_VIDEO_PROC_FS");
    }

    /* PCMCIA/Cardbus support */
    if (token_get("CONFIG_HW_PCMCIA_CARDBUS"))
    {
	token_set_y("CONFIG_HOTPLUG");
	token_set_y("CONFIG_CARDBUS");
	token_set("CONFIG_PCMCIA", token_get_str("CONFIG_HW_PCMCIA_CARDBUS"));
    }

    /* Firewire IEEE1394 and USB storage require SCSI */
    if (token_get("CONFIG_IEEE1394_SBP2") || token_get("CONFIG_USB_STORAGE"))
    {
	token_set_y("CONFIG_SCSI");
	token_set_y("CONFIG_BLK_DEV_SD");
	token_set("CONFIG_SD_EXTRA_DEVS", "32");
	token_set_y("CONFIG_RG_STORAGE");
    }

    if (token_get("CONFIG_RG_BLUETOOTH"))
    {
	if (token_get("CONFIG_RG_OS_LINUX_24"))
	{
	    token_set_y("CONFIG_BLUEZ");
	    token_set_y("CONFIG_BLUEZ_L2CAP");
	    token_set_y("CONFIG_BLUEZ_RFCOMM");
	    token_set_y("CONFIG_BLUEZ_SCO");
	    token_set_y("CONFIG_BLUEZ_BNEP");
	    token_set_y("CONFIG_BLUEZ_BNEP_MC_FILTER");
	    token_set_y("CONFIG_BLUEZ_BNEP_PROTO_FILTER");

	    if (token_get("CONFIG_HW_USB_HOST_OHCI") ||
		token_get("CONFIG_HW_USB_HOST_UHCI"))
	    {
		token_set_m("CONFIG_BLUEZ_HCIUSB");
	    }
	}
	
	if (token_get("CONFIG_RG_OS_LINUX_26"))
	{
	    token_set_y("CONFIG_BT");
	    token_set_y("CONFIG_BT_L2CAP");
	    token_set_y("CONFIG_BT_RFCOMM");
	    token_set_y("CONFIG_BT_SCO");
	    token_set_y("CONFIG_BT_BNEP");
	    token_set_y("CONFIG_BT_BNEP_MC_FILTER");
	    token_set_y("CONFIG_BT_BNEP_PROTO_FILTER");
	}

	token_set_y("CONFIG_RG_BLUETOOTH_PAN");
	token_set_y("CONFIG_HOTPLUG");
    }

    if (token_get("CONFIG_RG_STORAGE"))
    {
	token_set_y("CONFIG_EXT2_FS");
	token_set_y("CONFIG_FAT_FS");
	token_set_y("CONFIG_MSDOS_FS");
	token_set_y("CONFIG_VFAT_FS");
	token_set("CONFIG_FAT_DEFAULT_CODEPAGE", "437");
	if (!token_get_str("CONFIG_RG_CODEPAGE"))
	    token_set("CONFIG_RG_CODEPAGE", "\"437\"");
	token_set_y("CONFIG_MSDOS_PARTITION");
	if (!token_get("CONFIG_LSP_DIST"))
	    token_set_y("CONFIG_RG_STORAGE_UTILS");
    }

    if (token_get_str("CONFIG_RG_CODEPAGE"))
    {
	token_set_y("CONFIG_NLS");
	token_set_y("CONFIG_NLS_CODEPAGE_437");
	token_set_y("CONFIG_NLS_CODEPAGE_850");
	token_set_y("CONFIG_NLS_UTF8");
	token_set("CONFIG_NLS_DEFAULT", token_get_str("CONFIG_RG_CODEPAGE"));
    }
    else
	token_set("CONFIG_RG_CODEPAGE", "\"\"");

    /* Software reset/restore_defaults */
    if (token_get("CONFIG_HW_BUTTONS"))
	token_set_m("CONFIG_RG_HW_BUTTONS");

    /* limit maximum size of EXT2 partition for running check disk
     * on machines with 32MB RAM or less */
    if (token_get("CONFIG_RG_FILESERVER"))
    {
	char *size;

	token_set("CONFIG_RG_MAX_FAT_CHECK_SIZE", "1");
	if ((size = token_get_str("CONFIG_SDRAM_SIZE")) && atoi(size) <= 32)
	    token_set("CONFIG_RG_MAX_EXT2_CHECK_SIZE", "80");
    }

    if (token_get("CONFIG_RG_RGLOADER"))
    {
	token_set_m("CONFIG_RG_KOS");
	
	/* We need it for insmod */
	token_set_y("CONFIG_RG_BUSYBOX");
	
	/* XXX - shouldnt be required for RGLoader */
	token_set_y("CONFIG_RG_LANG");
	token_set("CONFIG_RG_LANGUAGES", "DEF");
	token_set("CONFIG_RG_DIST_LANG", "eng");
    }

    if (token_get("CONFIG_RG_RGLOADER") || 
	token_get("CONFIG_RG_RGLOADER_CLI_CMD"))
    {
	token_set_y("CONFIG_RGLOADER_CLI");
	token_set_y("CONFIG_RG_CLI");
	if (!token_get("CONFIG_I386_BOCHS"))
	    token_set_y("CONFIG_RG_PERM_STORAGE");
        token_set_y("CONFIG_RG_PING");
	if (token_get("CONFIG_RG_OS_VXWORKS"))
	    token_set_y("CONFIG_RG_CLI_SERVER");
	else
	    token_set_y("CONFIG_RG_CLI_SERIAL");

	token_set_y("CONFIG_RG_WGET");
    }

    if (is_dsp_configurable())
    {
	token_set_y("CONFIG_RG_AUDIO_MGT");
    }

    /* Set the default language used for this distribution */
    if (!token_get_str("CONFIG_RG_DIST_LANG"))
	token_set("CONFIG_RG_DIST_LANG", "eng");
	
    if (!token_get_str("RG_PROD_STR"))
	token_set("RG_PROD_STR", "OpenRG");

    if (token_get("CONFIG_RG_FLASH_ONLY_ON_BOOT"))
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");

    if (token_get("CONFIG_ULIBC") && token_get("CONFIG_DYN_LINK"))
	token_set_y("CONFIG_ULIBC_SHARED");

    if (token_get("CONFIG_RG_JFW") || token_get("CONFIG_RG_PPTPC"))
	token_set_m("CONFIG_RG_FRAG_CACHE");

    if (token_get("CONFIG_ATHEROS_AR531X_MIPS"))
	token_set("LIBC_ARCH", "mips");
    
    if (token_get("CONFIG_I386_BOCHS") && token_get("CONFIG_RG_OS_VXWORKS"))
    {
	token_set("LIBC_ARCH", "i386");
	token_set("ARCH", "i386");
    }
    
    if (token_get("CONFIG_RG_PPP") || token_get("CONFIG_RG_8021X") ||
	token_get("CONFIG_RG_RADIUS"))
    {
	token_set_y("CONFIG_RG_AUTH");
    }
    if (token_get("CONFIG_PCBOX"))
	token_set_y("CONFIG_RG_BOOTLDR");
    if (!token_get("CONFIG_LSP_DIST") && hw)
    {
	token_set_y("CONFIG_RG_TZ");
	token_set_y("CONFIG_RG_MGT");

	/* decide inside openssl what exactly we want:
	 * since PPP needs 4 header files from openssl */
	token_set_y("CONFIG_RG_OPENSSL_COMMON");
    }
    if (token_get("CONFIG_RG_OPENSSL_COMMON"))
    {
	token_set_y("CONFIG_RG_OPENSSL_MD5");
	if (!token_get("CONFIG_RG_RGLOADER"))
	{
	    token_set_y("CONFIG_RG_OPENSSL_SHA");
	    token_set_y("CONFIG_RG_OPENSSL_DES");
	    token_set_y("CONFIG_RG_OPENSSL_MD4");
	}
    }
    if (token_get("CONFIG_RG_TARGET_LINUX"))
	token_set_y("CONFIG_RG_LZMA");
    if (token_get("CONFIG_RG_CABLEHOME") || token_get("CONFIG_RG_DSL_CH"))
	token_set_y("CONFIG_RG_CABLEHOME_COMMON");
    if (token_get("CONFIG_RG_TCPDUMP"))
    {
	token_set_y("CONFIG_RG_LIBPCAP");
	token_set_y("CONFIG_RG_TERMCAP");
    }
    if (token_get("CONFIG_RG_DHCPC") || token_get("CONFIG_RG_DHCPS") ||
	token_get("CONFIG_RG_DHCPR"))
    {
	token_set_y("CONFIG_RG_DHCP_COMMON");
    }
    if (token_get("CONFIG_RG_RGLOADER") || 
	token_get("CONFIG_RG_RGLOADER_CLI_CMD"))
    {
	token_set_y("CONFIG_RG_RGLOADER_COMMON");
    }
    if (token_get("CONFIG_FREESWAN") || token_get("CONFIG_LIBDES"))
	token_set_y("CONFIG_FREESWAN_COMMON");
    if (token_get("CONFIG_RG_STORAGE_UTILS"))
    {
	token_set_y("CONFIG_RG_E2FSPROGS");
	token_set_y("CONFIG_RG_DOSFSTOOLS ");
	token_set_y("CONFIG_RG_UTIL_LINUX");
    }
    if (token_get("CONFIG_RG_PING") || token_get("CONFIG_RG_CONN_SPEED") ||
	token_get("CONFIG_RG_TRACEROUTE"))
    {
	token_set_y("CONFIG_RG_TEST_TOOLS");
    }
    if (token_get("CONFIG_RG_FILESERVER_ACLS"))
    {
	token_set_y("CONFIG_RG_ATTR");
	token_set_y("CONFIG_RG_ACL");
    }
    if (token_get("CONFIG_RG_FS_BACKUP"))
    {
	token_set_y("CONFIG_RG_STAR");
	token_set_y("CONFIG_RG_BACKUP");
    }
    if (token_get("CONFIG_RG_SAMPLES") || token_get("CONFIG_RG_TUTORIAL") 
	|| token_get("CONFIG_RG_TUTORIAL_ADVANCED"))
    {
	token_set_y("CONFIG_RG_SAMPLES_COMMON");
    }

    if (token_get("CONFIG_RG_IPERF"))
	token_set_y("CONFIG_RG_CXX");

    if (token_get("CONFIG_RG_CXX") && token_get("CONFIG_ULIBC"))
	token_set_y("CONFIG_UCLIBCXX");

    if (token_get("CONFIG_RG_RGLOADER"))
	token_set_y("CONFIG_RG_TELNETS");

    if (token_get("CONFIG_RG_SSH"))
    {
	token_set_y("CONFIG_RG_CLI_SERVER");
	token_set_y("CONFIG_RG_OPENSSL_BLOWFISH");
	token_set_y("CONFIG_RG_OPENSSL_CAST");
    }

    if (token_get("CONFIG_RG_JPKG_BIN") &&
	token_get("CONFIG_HW_80211N_AIRGO_AGN100"))
    {
	token_set_y("CONFIG_RG_JPKG_BUILD_LIBC");
    }

    token_set("RMT_UPG_SITE", "update.jungo.com");

    token_set("RMT_UPG_URL", "http%s://%s/",
	token_get("CONFIG_RG_SSL") ? "s" : "", token_get_str("RMT_UPG_SITE"));
    
    if (token_get("CONFIG_RG_JNET_CLIENT"))
    {
	/* XXX Remove this when B31487 is fixed */
	token_set("RMT_UPG_URL", "http://%s/", token_get_str("RMT_UPG_SITE"));
    }

    if (!token_get("CONFIG_RG_BUILD_LOCAL_TARGETS_ONLY"))
    {
	token_set_y("CONFIG_RG_ZLIB");
	token_set_y("CONFIG_RG_KERNEL");
	token_set_y("CONFIG_RG_MAIN");
	token_set_y("CONFIG_RG_VENDOR");
    }
    
    if (token_get("CONFIG_RG_MTD_DEFAULT_PARTITION"))
    {
	if (!token_get("CONFIG_RG_MTD") ||
	    !token_get("CONFIG_MTD_PHYSMAP_START") ||
	    !token_get("CONFIG_MTD_PHYSMAP_LEN"))
	{
	    conf_err("ERROR: Incorrect MTD configuration\n");
	}
    }

    if (token_get("CONFIG_RG_CERT") && 
	token_get("CONFIG_RG_ADV_SERVICES_LEVEL") == 1)
    {
	token_set_y("CONFIG_RG_CERT_SHORT_LIST");
    }
}    

/* Features that are always defined */
void openrg_features(void)
{
    token_set_y("CONFIG_EXPORT_BINARIES"); /* Make distribution export objects
					    * and binaries in addition to src
					    */
    if (token_get("CONFIG_INSURE"))
	token_set_y("CONFIG_GLIBC");

    if (token_get("CONFIG_VALGRIND"))
    {
	token_set_y("CONFIG_GLIBC");
	token_set_y("OPENRG_DEBUG");
    }
   
    if (!token_get("CONFIG_GLIBC") && !token_get("CONFIG_RG_NOT_UNIX"))
	token_set_y("CONFIG_ULIBC");

    if (!token_get("CONFIG_RG_NOT_UNIX"))
	token_set_y("CONFIG_UNIX");

    /* Config freepages limits manually - check CONFIG_FREEPAGES_HIGH for 
     * values.
     */
    token_set_y("CONFIG_FREEPAGES_THRESHOLD");

    token_set_y("CONFIG_RG_FACTORY_SETTINGS");

    token_set_y("CONFIG_SYN_COOKIES");

    /* OpenRG char device */
    token_set("OPENRG_CHRDEV_NAME", "rg_chrdev");
    token_set("OPENRG_CHRDEV_MAJOR", "240");

    if (is_evaluation && !token_get("CONFIG_RG_BUILD_LOCAL_TARGETS_ONLY"))
	token_set_y("CONFIG_LICENSE_AGREEMENT"); 
}

option_t *option_token_get_nofail(option_t *array, char *token)
{
    int i;
    for (i = 0; array[i].token && strcmp(array[i].token, token); i++);
    return array[i].token ? array+i : NULL;
}

option_t *option_token_get(option_t *array, char *token)
{
    option_t *opt = option_token_get_nofail(array, token);
    if (!opt)
	conf_err("ERROR: Can't set %s: No such option\n", token);
    return opt;
}

void _token_set(char *file, int line, set_prio_t set_prio, char *token,
    const char *value_, ...)
{
    option_t *opt;
    char value[4096];
    va_list ap;

    va_start(ap, value_);
    vsnprintf(value, sizeof(value), value_, ap);
    va_end(ap);

    if (strlen(value) > sizeof(value) - 2)
    {
	fprintf(stderr, 
	    "In %s:%d Error setting token \"%s\" - value too long\n",
	    file, line, token);
	exit(-1);
    }

    opt = option_token_get(openrg_config_options, token);

    if (set_prio < opt->set_prio)
    {
	/* The option was already set with higher set priority than 'set_prio',
	 * so don't do anything */
	return;
    }

    if (!strcmp(value, "m"))
    {
	if (!opt->type & OPT_MODULE_EXPAND)
	{
	    conf_err("ERROR: Can't set %s to \"m\". %s is not defined"
		" as OPT_MODULE_EXPAND\n", token, token);
	}
	/* replace m to y when developing. */
	if (opt->type & OPT_STATIC_ON_DEVELOP && token_get("CONFIG_RG_DEV"))
	    sprintf(value, "y");
    }

    if (set_prio == opt->set_prio && opt->value && strcmp(opt->value, value))
    {
	/* Attempt to change a token value that has already been set before,
	 * with the same set priority as 'set_prio' */
	fprintf(stderr, "Replacing %s=%s (from %s:%d) with %s=%s from "
	    "(%s:%d)\n", token, opt->value, opt->file, opt->line, token,
	    value, file, line);
    }

    opt->file = file;
    opt->line = line;
    
    if (opt->value)
	free(opt->value); /* Free old value */
    opt->value = strdup(value);
    opt->set_prio = set_prio;
}

void _token_set_y(char *file, int line, char *token)
{
    _token_set(file, line, SET_PRIO_TOKEN_SET, token, "y");
}

void _token_set_m(char *file, int line, char *token)
{
    if (token_get("CONFIG_RG_OS_VXWORKS"))
	_token_set(file, line, SET_PRIO_TOKEN_SET, token, "y");
    else
	_token_set(file, line, SET_PRIO_TOKEN_SET, token, "m");
}

char *token_getz(char *token)
{
    return token_get_str(token) ?: "";
}

char *token_get_str(char *token)
{
    option_t *opt = option_token_get(openrg_config_options, token);
    return opt->value;
}

int token_is_y(char *token)
{
    char *val;

    if (!(val = token_get_str(token)))
	return 0;
    return !strcmp(val, "y");
}

/* handle both decimal and hex numbers */
/* We allow "", "1234", "0x1234" */
int str_is_number_value(char *val)
{
    int i;
    if (!*val)
	return 1;
    if (val[0]=='0' && val[1]=='x')
    {
	for (i=2; val[i] && isxdigit(val[i]); i++);
	return i>2 && !val[i];
    }
    for (i=0; val[i] && isxdigit(val[i]); i++);
    return i>0 && !val[i];
}

/* handle both decimal and hex numbers */
int str_to_number(char *val)
{
    int i = 0;
    if (!*val)
	return 0;
    if (val[0]=='0' && val[1]=='x')
	sscanf(val, "%x", &i);
    else
	sscanf(val, "%d", &i);
    return i;
}

int token_get(char *token)
{
    char *val;
    option_t *o = option_token_get(openrg_config_options, token);
    
    /* sanity check for internal errors - accessing strings as integer */
    if (o->type & (OPT_STR | OPT_C_STR))
    {
	conf_err("ERROR: token %s is a string type (expected integer)\n",
	    token);
    }
    if (!(val = token_get_str(token)))
	return 0;
    /* is it a number? */
    if (o->type & OPT_NUMBER)
	return str_to_number(val);
    /* otherwise its a y/m/n */
    return !strcmp(val, "y") || !strcmp(val, "m");
}

