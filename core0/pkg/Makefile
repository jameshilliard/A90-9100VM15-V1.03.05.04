RGSRC=../
include $(RGSRC)/envir.mak

# XXX Currently libc build from the rg/Makefile. This should be fixed.
PKG_LIBC+=$(if $(CONFIG_ULIBC),$(if $(ULIBC_IN_TOOLCHAIN),,ulibc))
PKG_LIBC+=$(if $(CONFIG_GLIBC),$(if $(GLIBC_IN_TOOLCHAIN),,glibc))
#SUBDIRS+=$(PKG_LIBC)
#LINK_DIRS+=$(PKG_LIBC)
JPKG_EXPORTED_DIR+=$(PKG_LIBC)

SUBDIRS+=$(if $(CONFIG_UCLIBCXX),uclibc++)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_UCLIBCXX),uclibc++)
SUBDIRS+=$(if $(CONFIG_RG_OPENSSL_COMMON),openssl)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_OPENSSL_COMMON),openssl)
LINK_DIRS+=$(if $(CONFIG_RG_OPENSSL_COMMON),openssl)
SUBDIRS+=$(if $(CONFIG_RG_SYSLOG_COMMON),syslog)
SUBDIRS+=util
SUBDIRS+=$(if $(CONFIG_RG_ZLIB),zlib)
SUBDIRS+=$(if $(CONFIG_RG_LANG),language)
SUBDIRS+=$(if $(CONFIG_RG_LICENSE),license)
SUBDIRS+=include
SUBDIRS+=$(if $(CONFIG_RG_DOC_ENABLED),doc)
LINK_DIRS+=$(if $(CONFIG_RG_DOC_ENABLED),doc)
SUBDIRS+=$(if $(CONFIG_RG_LZMA),lzma)
SUBDIRS+=build
SUBDIRS+=$(if $(CONFIG_RG_TZ),tz)
SUBDIRS+=$(if $(CONFIG_RG_MGT),mgt)
ifeq ($(CONFIG_RG_ONE_MODULE),)
SUBDIRS+=$(if $(CONFIG_RG_KERNEL),kernel)
endif
SUBDIRS+=$(if $(CONFIG_RG_LIBMAD),av/libmad)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_LIBMAD),av/libmad)
LINK_DIRS+=$(if $(CONFIG_RG_LIBMAD),av/libmad)
SUBDIRS+=$(if $(CONFIG_IPTABLES),iptables)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_IPTABLES),iptables)
LINK_DIRS+=$(if $(CONFIG_IPTABLES),iptables)
SUBDIRS+=$(if $(CONFIG_RG_IPROUTE2_UTILS),iproute2)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_IPROUTE2_UTILS),iproute2)
LINK_DIRS+=$(if $(CONFIG_RG_IPROUTE2_UTILS),iproute2)
SUBDIRS+=$(if $(CONFIG_BRIDGE_UTILS),bridge-utils)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_BRIDGE_UTILS),bridge-utils)
SUBDIRS+=$(if $(CONFIG_RG_BZIP2)$(CONFIG_BOOTLDR_UBOOT),boot)
SUBDIRS+=$(if $(CONFIG_RG_BUSYBOX),busybox)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_BUSYBOX),busybox)
LINK_DIRS+=$(if $(CONFIG_RG_BUSYBOX),busybox)

SUBDIRS+=$(if $(CONFIG_CRAMFS_FS_COMMON),cramfs)

SUBDIRS+=$(if $(CONFIG_ROMFS_FS),genromfs)
SUBDIRS+=$(if $(CONFIG_RG_TODC),tod)
SUBDIRS+=$(if $(CONFIG_RG_DNS),dns)
SUBDIRS+=$(if $(CONFIG_RG_HTTPS),httpd)
SUBDIRS+=$(if $(CONFIG_LOCAL_WBM_LIB)$(CONFIG_RG_WBM)$(CONFIG_RG_SSI),web_mng)
SUBDIRS+=$(if $(CONFIG_RG_LIBIMAGE)$(CONFIG_RG_LIBJPEG),image)
SUBDIRS+=$(if $(CONFIG_RG_KOS),kos)
SUBDIRS+=$(if $(CONFIG_RG_MSS),mss)
SUBDIRS+=$(if $(CONFIG_RG_ELVIS),elvis)
SUBDIRS+=$(if $(CONFIG_RG_CABLEHOME_COMMON),cablehome)
SUBDIRS+=$(if $(CONFIG_RG_KERBEROS),heimdal)
SUBDIRS+=$(if $(CONFIG_RG_UCD_SNMP),ucd-snmp)
SUBDIRS+=$(if $(CONFIG_RG_NETKIT),netkit)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_NETKIT),netkit)
LINK_DIRS+=$(if $(CONFIG_RG_NETKIT),netkit)
SUBDIRS+=$(if $(CONFIG_RG_NETTOOLS_ARP),net-tools)
JPKG_EXPORTED_DIR_SRC+=$(if $(CONFIG_RG_NETTOOLS_ARP),net-tools)
LINK_DIRS+=$(if $(CONFIG_RG_NETTOOLS_ARP),net-tools)
SUBDIRS+=$(if $(CONFIG_RG_PERM_STORAGE),perm_storage)
SUBDIRS+=$(if $(CONFIG_FLASH),flash)
SUBDIRS+=$(if $(CONFIG_RG_LIBPCAP),libpcap)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_LIBPCAP),libpcap)
LINK_DIRS+=$(if $(CONFIG_RG_LIBPCAP),libpcap)
SUBDIRS+=$(if $(CONFIG_RG_TERMCAP),termcap)
SUBDIRS+=$(if $(CONFIG_RG_TCPDUMP),tcpdump)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_TCPDUMP),tcpdump)
LINK_DIRS+=$(if $(CONFIG_RG_TCPDUMP),tcpdump)
SUBDIRS+=$(if $(CONFIG_RG_RMT_UPDATE),rmt-update)
SUBDIRS+=$(if $(CONFIG_RG_WGET),wget)
SUBDIRS+=$(if $(CONFIG_RG_BLUETOOTH),bluetooth)
SUBDIRS+=$(if $(CONFIG_RG_KAFFE),kaffe)
JPKG_EXPORTED_DIR_SRC+=$(if $(CONFIG_RG_KAFFE),kaffe)
LINK_DIRS+=$(if $(CONFIG_RG_KAFFE),kaffe)
SUBDIRS+=$(if $(CONFIG_RG_VOIP),voip)
SUBDIRS+=$(if $(CONFIG_IXJ),voip/ixj)
SUBDIRS+=$(if $(CONFIG_GMP),gmp)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_GMP),gmp)
LINK_DIRS+=$(if $(CONFIG_GMP),gmp)
SUBDIRS+=$(if $(CONFIG_IPSEC),ipsec)
SUBDIRS+=$(if $(CONFIG_FREESWAN_COMMON),freeswan)
JPKG_EXPORTED_DIR_SRC+=$(if $(CONFIG_FREESWAN_COMMON),freeswan)
LINK_DIRS+=$(if $(CONFIG_FREESWAN_COMMON),freeswan)
SUBDIRS+=$(if $(CONFIG_RG_X509),x509)
SUBDIRS+=$(if $(CONFIG_PCBOX),lilo)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_PCBOX),lilo)
LINK_DIRS+=$(if $(CONFIG_PCBOX),lilo)
SUBDIRS+=$(if $(CONFIG_ATM),atm)
JPKG_EXPORTED_DIR_SRC+=$(if $(CONFIG_ATM),atm)
LINK_DIRS+=$(if $(CONFIG_ATM),atm)
SUBDIRS+=$(if $(CONFIG_RG_PPP),ppp)
SUBDIRS+=$(if $(CONFIG_RG_PPPOE_RELAY),pppoe-server)
SUBDIRS+=$(if $(CONFIG_PCMCIA),pcmcia)
SUBDIRS+=$(if $(CONFIG_RG_GDBSERVER),gdb/gdb/gdbserver)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_GDBSERVER),gdb)
LINK_DIRS+=$(if $(CONFIG_RG_GDBSERVER),gdb/gdb/gdbserver)
SUBDIRS+=$(if $(CONFIG_RG_RIP),rip)
SUBDIRS+=$(if $(CONFIG_RG_IGMP_PROXY),igmp)
SUBDIRS+=$(if $(CONFIG_RG_MCAST_RELAY),igmp)
SUBDIRS+=$(if $(CONFIG_RG_XML),expat)
SUBDIRS+=$(if $(CONFIG_RG_UPNP),upnp)
SUBDIRS+=$(if $(CONFIG_RG_XML),xml)
SUBDIRS+=$(if $(CONFIG_RG_CLI),cli)
SUBDIRS+=$(if $(CONFIG_RG_RMT_MNG),rmt_mng)
SUBDIRS+=$(if $(CONFIG_RG_TELNETS),telnet)
SUBDIRS+=$(if $(CONFIG_RG_SSH),openssh)
SUBDIRS+=$(if $(CONFIG_RG_BOOTLDR),bootldr)
SUBDIRS+=$(if $(CONFIG_RG_BRIDGE),bridge)
SUBDIRS+=$(if $(CONFIG_RG_USB_SLAVE),usb_slave)
SUBDIRS+=$(if $(CONFIG_VALGRIND),valgrind)
JPKG_EXPORTED_DIR_SRC+=$(if $(CONFIG_VALGRIND),valgrind)
LINK_DIRS+=$(if $(CONFIG_VALGRIND),valgrind)
SUBDIRS+=$(if $(CONFIG_RG_DISK_MNG),disk_mng)
SUBDIRS+=$(if $(CONFIG_RG_LPD),lpd)
SUBDIRS+=$(if $(CONFIG_RG_FILESERVER),file_server)
SUBDIRS+=$(if $(CONFIG_RG_PRINT_SERVER),print_server)
SUBDIRS+=$(if $(CONFIG_RG_ATTR),attr)
SUBDIRS+=$(if $(CONFIG_RG_ACL),acl)
SUBDIRS+=$(if $(CONFIG_RG_SAMBA),samba)
JPKG_EXPORTED_DIR_SRC+=$(if $(CONFIG_RG_SAMBA),samba)
LINK_DIRS+=$(if $(CONFIG_RG_SAMBA),samba)
SUBDIRS+=$(if $(CONFIG_RG_E2FSPROGS),e2fsprogs)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_E2FSPROGS),e2fsprogs)
LINK_DIRS+=$(if $(CONFIG_RG_E2FSPROGS),e2fsprogs)
SUBDIRS+=$(if $(CONFIG_RG_DOSFSTOOLS),dosfstools)
JPKG_EXPORTED_DIR_SRC+=$(if $(CONFIG_RG_DOSFSTOOLS),dosfstools)
LINK_DIRS+=$(if $(CONFIG_RG_DOSFSTOOLS),dosfstools)
SUBDIRS+=$(if $(CONFIG_RG_UTIL_LINUX),util-linux)
LINK_DIRS+=$(if $(CONFIG_RG_UTIL_LINUX),util-linux)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_UTIL_LINUX),util-linux)
SUBDIRS+=$(if $(CONFIG_RG_RAID),mdadm)
SUBDIRS+=$(if $(CONFIG_RG_DDNS),ddns)
SUBDIRS+=$(if $(CONFIG_RG_MAIL_SERVER),mail)
SUBDIRS+=$(if $(CONFIG_RG_ENTFY),mail_client)
SUBDIRS+=$(if $(CONFIG_RG_8021X),802.1x)
SUBDIRS+=$(if $(CONFIG_RG_WPA),wpa)
SUBDIRS+=$(if $(CONFIG_RG_RADIUS),radius)
SUBDIRS+=$(if $(CONFIG_HOTPLUG),hotplug)
SUBDIRS+=$(if $(CONFIG_RG_FLEX),flex)
SUBDIRS+=$(if $(CONFIG_RG_USAGI),usagi)
SUBDIRS+=$(if $(CONFIG_RG_L2TPC)$(CONFIG_RG_L2TPS),l2tp)
SUBDIRS+=$(if $(CONFIG_RG_VLAN_8021Q),vlan)
SUBDIRS+=$(if $(CONFIG_RG_TFTP_SERVER),tftps)
SUBDIRS+=$(if $(CONFIG_ZC_AUTO_CONFIG_NON_PNP),auto_conf)
SUBDIRS+=$(if $(CONFIG_RG_ROUTE_MULTIWAN)$(CONFIG_RG_OS_VXWORKS),route)
SUBDIRS+=$(if $(CONFIG_RGTV),av rgtv)
SUBDIRS+=$(if $(CONFIG_RG_FTP_SERVER),ftp)
SUBDIRS+=$(if $(CONFIG_RG_WEB_SERVER),web_server)
SUBDIRS+=$(if $(CONFIG_RG_STAR),star)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_STAR),star)
LINK_DIRS+=$(if $(CONFIG_RG_STAR),star)
SUBDIRS+=$(if $(CONFIG_RG_BACKUP),backup)
SUBDIRS+=$(if $(CONFIG_RG_SAMPLES_COMMON),samples)
JPKG_EXPORTED_DIR_SRC+=samples
LINK_DIRS+=samples
SUBDIRS+=$(if $(CONFIG_RG_FIREWALL)$(CONFIG_RG_NAT)$(CONFIG_RG_RNAT),firewall)
SUBDIRS+=$(if $(CONFIG_RG_VENDOR),vendor)
SUBDIRS+=$(if $(CONFIG_RG_RGLOADER_COMMON),rgloader)
SUBDIRS+=$(if $(CONFIG_RG_TEST_TOOLS),test_tools)
SUBDIRS+=$(if $(CONFIG_RG_DHCP_COMMON),dhcp)
SUBDIRS+=$(if $(CONFIG_RG_XML2),libxml)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_XML2),libxml)
SUBDIRS+=$(if $(CONFIG_RG_XMLSEC),xmlsec)
JPKG_EXPORTED_DIR+=$(if $(CONFIG_RG_XMLSEC),xmlsec)
SUBDIRS+=$(if $(CONFIG_RG_DSLHOME),dslhome)
SUBDIRS+=$(if $(CONFIG_HNAP),hnap)
SUBDIRS+=$(if $(CONFIG_RG_QOS),qos)
SUBDIRS+=$(if $(CONFIG_RG_TOOLS),tools)
SUBDIRS+=$(if $(CONFIG_RG_WEBCAM),webcam)
SUBDIRS+=$(if $(CONFIG_RG_FASTPATH),fastpath)
SUBDIRS+=$(if $(CONFIG_RG_TUNNELS),tunnels)
SUBDIRS+=$(if $(CONFIG_RG_SSL_VPN),ssl_vpn)
SUBDIRS+=$(if $(CONFIG_RG_JAVA),java)
SUBDIRS+=$(if $(CONFIG_RG_IPERF),iperf)
SUBDIRS+=$(if $(CONFIG_RG_JNET_SERVER)$(CONFIG_RG_JNET_CLIENT),jnet)
SUBDIRS+=$(if $(CONFIG_RG_WIRELESS_TOOLS),wireless_tools)
JPKG_EXPORTED_DIR_SRC+=$(if $(CONFIG_RG_GNUDIP),gnudip)
SUBDIRS+=$(if $(CONFIG_RG_HOSTAPD),hostapd)

# IFX infineon vdsl driver
SUBDIRS+=$(if $(CONFIG_RG_INFINEON_VDSL), ifx)

#Debug pages IE CPU_UTIL
SUBDIRS+=$(if $(CONFIG_DEBUG_PAGES), debugpages)

# main _must_ compile after all libraries and tasks.
SUBDIRS+=$(if $(CONFIG_RG_MAIN),main)

# autotest is not part of openrg, but it depends on rg_default.c
# from main therefor it _must_ compile after main.
SUBDIRS+=$(if $(CONFIG_RG_AUTOTEST),autotest)

SUBDIRS+=$(if $(CONFIG_RG_JPKG),jpkg)

# tools are currently relevant for UML only
SUBDIRS+=$(if $(CONFIG_RG_JPKG_UML)$(CONFIG_RG_JPKG_SRC),tools)

ifneq ($(CONFIG_RG_ONE_MODULE),)
SUBDIRS+=$(if $(CONFIG_RG_KERNEL),kernel)
endif

# Build must be the last subdir in order for make ramdisk to work
# XXX We add libc because it is built from rg/Makefile. This shouldbe fixed
RAMDISK_SUBDIRS:=$(PKG_LIBC) $(filter-out build,$(SUBDIRS)) build

include $(RGMK)

