/*
 * Host AP - driver interaction with MADWIFI 802.11 driver
 * Copyright (c) 2004, Sam Leffler <sam@errno.com>
 * Copyright (c) 2004, Video54 Technologies
 * Copyright (c) 2005, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
/* XXX hack to avoid redefinitions "net/if.h" already included by dev.h,
 * and collides with "linux/if.h" */
#define _LINUX_IF_H
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <endian.h>
#define _BYTE_ORDER __BYTE_ORDER
#define _LITTLE_ENDIAN __LITTLE_ENDIAN
#define _BIG_ENDIAN __BIG_ENDIAN
#include <include/compat.h>
#include <net80211/ieee80211.h>
#ifdef WME_NUM_AC
/* Assume this is built against BSD branch of madwifi driver. */
#define MADWIFI_BSD
#include <net80211/_ieee80211.h>
#endif /* WME_NUM_AC */
#include <net80211/ieee80211_crypto.h>
#include <net80211/ieee80211_ioctl.h>

#ifdef IEEE80211_IOCTL_SETWMMPARAMS
/* Assume this is built against madwifi-ng */
#define MADWIFI_NG
#endif /* IEEE80211_IOCTL_SETWMMPARAMS */

#include <net/if_arp.h>
#include "wireless_copy.h"

#include <netinet/in.h>
#include <netpacket/packet.h>

#include "hostapd.h"
#include "driver.h"
#include "ieee802_1x.h"
#include "eloop.h"
#include "priv_netlink.h"
#include "sta_info.h"
#include "l2_packet.h"
#include "hostap_common.h"

#include "eapol_sm.h"
#include "wpa.h"
#include "radius.h"
#include "ieee802_11.h"
#include "accounting.h"
#include "common.h"


struct madwifi_driver_data {
	struct driver_ops ops;			/* base class */
	struct hostapd_data *hapd;		/* back pointer */

	char	iface[IFNAMSIZ + 1];
	int     ifindex;
	struct l2_packet_data *sock_xmit;	/* raw packet xmit socket */
	struct l2_packet_data *sock_recv;	/* raw packet recv socket */
	int	ioctl_sock;			/* socket for ioctl() use */
	int	wext_sock;			/* socket for wireless events */
	int	we_version;
	u8	acct_mac[ETH_ALEN];
	struct hostap_sta_driver_data acct_data;
};

static const struct driver_ops madwifi_driver_ops;

static int madwifi_sta_deauth(void *priv, u8 *addr, int reason_code);

static int
set80211priv(struct madwifi_driver_data *drv, int op, void *data, int len)
{
	struct iwreq iwr;

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
	if (len < IFNAMSIZ) {
		/*
		 * Argument data fits inline; put it there.
		 */
		memcpy(iwr.u.name, data, len);
	} else {
		/*
		 * Argument data too big for inline transfer; setup a
		 * parameter block instead; the kernel will transfer
		 * the data for the driver.
		 */
		iwr.u.data.pointer = data;
		iwr.u.data.length = len;
	}

	if (ioctl(drv->ioctl_sock, op, &iwr) < 0) {
#ifdef MADWIFI_NG
		int first = IEEE80211_IOCTL_SETPARAM;
		int last = IEEE80211_IOCTL_GETMODE;
		static const char *opnames[] = {
			"ioctl[IEEE80211_IOCTL_SETPARAM]",
			"ioctl[IEEE80211_IOCTL_GETPARAM]",
			"ioctl[IEEE80211_IOCTL_SETKEY]",
			"ioctl[IEEE80211_IOCTL_SETWMMPARAMS]",
			"ioctl[IEEE80211_IOCTL_DELKEY]",
			"ioctl[IEEE80211_IOCTL_GETWMMPARAMS]",
			"ioctl[IEEE80211_IOCTL_SETMLME]",
			"ioctl[IEEE80211_IOCTL_GETCHANINFO]",
			"ioctl[IEEE80211_IOCTL_SETOPTIE]",
			"ioctl[IEEE80211_IOCTL_GETOPTIE]",
			"ioctl[IEEE80211_IOCTL_ADDMAC]",
			"ioctl[SIOCIWFIRSTPRIV+11]",
			"ioctl[IEEE80211_IOCTL_DELMAC]",
			"ioctl[IEEE80211_IOCTL_GETCHANLIST]",
			"ioctl[IEEE80211_IOCTL_SETCHANLIST]",
			"ioctl[SIOCIWFIRSTPRIV+15]",
			"ioctl[IEEE80211_IOCTL_SETMODE]",
			"ioctl[IEEE80211_IOCTL_GETMODE]",
		};
#else /* MADWIFI_NG */
		int first = IEEE80211_IOCTL_SETPARAM;
		int last = IEEE80211_IOCTL_CHANLIST;
		static const char *opnames[] = {
			"ioctl[IEEE80211_IOCTL_SETPARAM]",
			"ioctl[IEEE80211_IOCTL_GETPARAM]",
			"ioctl[IEEE80211_IOCTL_SETKEY]",
			"ioctl[SIOCIWFIRSTPRIV+3]",
			"ioctl[IEEE80211_IOCTL_DELKEY]",
			"ioctl[SIOCIWFIRSTPRIV+5]",
			"ioctl[IEEE80211_IOCTL_SETMLME]",
			"ioctl[SIOCIWFIRSTPRIV+7]",
			"ioctl[IEEE80211_IOCTL_SETOPTIE]",
			"ioctl[IEEE80211_IOCTL_GETOPTIE]",
			"ioctl[IEEE80211_IOCTL_ADDMAC]",
			"ioctl[SIOCIWFIRSTPRIV+11]",
			"ioctl[IEEE80211_IOCTL_DELMAC]",
			"ioctl[SIOCIWFIRSTPRIV+13]",
			"ioctl[IEEE80211_IOCTL_CHANLIST]",
			"ioctl[SIOCIWFIRSTPRIV+15]",
			"ioctl[IEEE80211_IOCTL_GETRSN]",
			"ioctl[SIOCIWFIRSTPRIV+17]",
			"ioctl[IEEE80211_IOCTL_GETKEY]",
		};
#endif /* MADWIFI_NG */
		if (first <= op && op <= last)
			perror(opnames[op - first]);
		else
			perror("ioctl[unknown???]");
		return -1;
	}
	return 0;
}

static int
set80211param(struct madwifi_driver_data *drv, int op, int arg)
{
	struct iwreq iwr;

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
	iwr.u.mode = op;
	memcpy(iwr.u.name+sizeof(__u32), &arg, sizeof(arg));

	if (ioctl(drv->ioctl_sock, IEEE80211_IOCTL_SETPARAM, &iwr) < 0) {
		perror("ioctl[IEEE80211_IOCTL_SETPARAM]");
		return -1;
	}
	return 0;
}

static const char *
ether_sprintf(const u8 *addr)
{
	static char buf[sizeof(MACSTR)];

	if (addr != NULL)
		snprintf(buf, sizeof(buf), MACSTR, MAC2STR(addr));
	else
		snprintf(buf, sizeof(buf), MACSTR, 0,0,0,0,0,0);
	return buf;
}

/*
 * Configure WPA parameters.
 */
static int
madwifi_configure_wpa(struct madwifi_driver_data *drv)
{
	hostapd *hapd = drv->hapd;
	struct hostapd_config *conf = hapd->conf;
	int v;

	switch (conf->wpa_group) {
	case WPA_CIPHER_CCMP:
		v = IEEE80211_CIPHER_AES_CCM;
		break;
	case WPA_CIPHER_TKIP:
		v = IEEE80211_CIPHER_TKIP;
		break;
	case WPA_CIPHER_WEP104:
		v = IEEE80211_CIPHER_WEP;
		break;
	case WPA_CIPHER_WEP40:
		v = IEEE80211_CIPHER_WEP;
		break;
	case WPA_CIPHER_NONE:
		v = IEEE80211_CIPHER_NONE;
		break;
	default:
		printf("Unknown group key cipher %u\n",
			conf->wpa_group);
		return -1;
	}
	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: group key cipher=%d\n", __func__, v);
	if (set80211param(drv, IEEE80211_PARAM_MCASTCIPHER, v)) {
		printf("Unable to set group key cipher to %u\n", v);
		return -1;
	}
	if (v == IEEE80211_CIPHER_WEP) {
		/* key length is done only for specific ciphers */
		v = (conf->wpa_group == WPA_CIPHER_WEP104 ? 13 : 5);
		if (set80211param(drv, IEEE80211_PARAM_MCASTKEYLEN, v)) {
			printf("Unable to set group key length to %u\n", v);
			return -1;
		}
	}

	v = 0;
	if (conf->wpa_pairwise & WPA_CIPHER_CCMP)
		v |= 1<<IEEE80211_CIPHER_AES_CCM;
	if (conf->wpa_pairwise & WPA_CIPHER_TKIP)
		v |= 1<<IEEE80211_CIPHER_TKIP;
	if (conf->wpa_pairwise & WPA_CIPHER_NONE)
		v |= 1<<IEEE80211_CIPHER_NONE;
	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: pairwise key ciphers=0x%x\n", __func__, v);
	if (set80211param(drv, IEEE80211_PARAM_UCASTCIPHERS, v)) {
		printf("Unable to set pairwise key ciphers to 0x%x\n", v);
		return -1;
	}

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: key management algorithms=0x%x\n",
		__func__, conf->wpa_key_mgmt);
	if (set80211param(drv, IEEE80211_PARAM_KEYMGTALGS, conf->wpa_key_mgmt)) {
		printf("Unable to set key management algorithms to 0x%x\n",
			conf->wpa_key_mgmt);
		return -1;
	}

	v = 0;
	if (conf->rsn_preauth)
		v |= BIT(0);
	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: rsn capabilities=0x%x\n", __func__, conf->rsn_preauth);
	if (set80211param(drv, IEEE80211_PARAM_RSNCAPS, v)) {
		printf("Unable to set RSN capabilities to 0x%x\n", v);
		return -1;
	}

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: enable WPA= 0x%x\n", __func__, conf->wpa);
	if (set80211param(drv, IEEE80211_PARAM_WPA, conf->wpa)) {
		printf("Unable to set WPA to %u\n", conf->wpa);
		return -1;
	}
	return 0;
}


static int
madwifi_set_iface_flags(void *priv, int dev_up)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;
	struct ifreq ifr;

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_VERBOSE,
		"%s: dev_up=%d\n", __func__, dev_up);

	if (drv->ioctl_sock < 0)
		return -1;

	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, IFNAMSIZ, "%s", drv->iface);

	if (ioctl(drv->ioctl_sock, SIOCGIFFLAGS, &ifr) != 0) {
		perror("ioctl[SIOCGIFFLAGS]");
		return -1;
	}

	if (dev_up)
		ifr.ifr_flags |= IFF_UP;
	else
		ifr.ifr_flags &= ~IFF_UP;

	if (ioctl(drv->ioctl_sock, SIOCSIFFLAGS, &ifr) != 0) {
		perror("ioctl[SIOCSIFFLAGS]");
		return -1;
	}

	if (dev_up) {
		memset(&ifr, 0, sizeof(ifr));
		snprintf(ifr.ifr_name, IFNAMSIZ, "%s", drv->iface);
		ifr.ifr_mtu = HOSTAPD_MTU;
		if (ioctl(drv->ioctl_sock, SIOCSIFMTU, &ifr) != 0) {
			perror("ioctl[SIOCSIFMTU]");
			printf("Setting MTU failed - trying to survive with "
			       "current value\n");
		}
	}

	return 0;
}

static int
madwifi_set_ieee8021x(void *priv, int enabled)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;
	struct hostapd_config *conf = hapd->conf;

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_VERBOSE,
		"%s: enabled=%d\n", __func__, enabled);

	if (!enabled) {
		/* XXX restore state */
		return set80211param(priv, IEEE80211_PARAM_AUTHMODE,
			IEEE80211_AUTH_AUTO);
	}
	if (!conf->wpa && !conf->ieee802_1x) {
		hostapd_logger(hapd, NULL, HOSTAPD_MODULE_DRIVER,
			HOSTAPD_LEVEL_WARNING, "No 802.1X or WPA enabled!");
		return -1;
	}
	if (conf->wpa && madwifi_configure_wpa(drv) != 0) {
		hostapd_logger(hapd, NULL, HOSTAPD_MODULE_DRIVER,
			HOSTAPD_LEVEL_WARNING, "Error configuring WPA state!");
		return -1;
	}
	if (set80211param(priv, IEEE80211_PARAM_AUTHMODE,
		(conf->wpa ?  IEEE80211_AUTH_WPA : IEEE80211_AUTH_8021X))) {
		hostapd_logger(hapd, NULL, HOSTAPD_MODULE_DRIVER,
			HOSTAPD_LEVEL_WARNING, "Error enabling WPA/802.1X!");
		return -1;
	}
	return madwifi_set_iface_flags(priv, 1);
}

static int
madwifi_set_privacy(void *priv, int enabled)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: enabled=%d\n", __func__, enabled);

	return set80211param(priv, IEEE80211_PARAM_PRIVACY, enabled);
}

static int
madwifi_set_sta_authorized(void *priv, u8 *addr, int authorized)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;
	struct ieee80211req_mlme mlme;

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_VERBOSE,
		"%s: addr=%s authorized=%d\n",
		__func__, ether_sprintf(addr), authorized);

	if (authorized)
		mlme.im_op = IEEE80211_MLME_AUTHORIZE;
	else
		mlme.im_op = IEEE80211_MLME_UNAUTHORIZE;
	mlme.im_reason = 0;
	memcpy(mlme.im_macaddr, addr, IEEE80211_ADDR_LEN);
	return set80211priv(priv, IEEE80211_IOCTL_SETMLME, &mlme,
			    sizeof(mlme));
}

static int
madwifi_del_key(void *priv, unsigned char *addr, int key_idx)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;
	struct ieee80211req_del_key wk;

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: addr=%s key_idx=%d\n",
		__func__, ether_sprintf(addr), key_idx);

	memset(&wk, 0, sizeof(wk));
	if (addr != NULL) {
		memcpy(wk.idk_macaddr, addr, IEEE80211_ADDR_LEN);
		wk.idk_keyix = (u8) IEEE80211_KEYIX_NONE;
	} else {
		wk.idk_keyix = key_idx;
	}

	return set80211priv(priv, IEEE80211_IOCTL_DELKEY, &wk, sizeof(wk));
}

static int
madwifi_set_key(void *priv, const char *alg,
	     unsigned char *addr, int key_idx,
	     u8 *key, size_t key_len)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;
	struct ieee80211req_key wk;
	u_int8_t cipher;

	if (strcmp(alg, "none") == 0)
		return madwifi_del_key(priv, addr, key_idx);

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: alg=%s addr=%s key_idx=%d\n",
		__func__, alg, ether_sprintf(addr), key_idx);

	if (strcmp(alg, "WEP") == 0)
		cipher = IEEE80211_CIPHER_WEP;
	else if (strcmp(alg, "TKIP") == 0)
		cipher = IEEE80211_CIPHER_TKIP;
	else if (strcmp(alg, "CCMP") == 0)
		cipher = IEEE80211_CIPHER_AES_CCM;
	else {
		printf("%s: unknown/unsupported algorithm %s\n",
			__func__, alg);
		return -1;
	}

	if (key_len > sizeof(wk.ik_keydata)) {
		printf("%s: key length %lu too big\n", __func__,
		       (unsigned long) key_len);
		return -3;
	}

	memset(&wk, 0, sizeof(wk));
	wk.ik_type = cipher;
	wk.ik_flags = IEEE80211_KEY_RECV | IEEE80211_KEY_XMIT;
	if (addr == NULL) {
		memset(wk.ik_macaddr, 0xff, IEEE80211_ADDR_LEN);
		wk.ik_keyix = key_idx;
		wk.ik_flags |= IEEE80211_KEY_DEFAULT;
	} else {
		memcpy(wk.ik_macaddr, addr, IEEE80211_ADDR_LEN);
		wk.ik_keyix = IEEE80211_KEYIX_NONE;
	}
	wk.ik_keylen = key_len;
	memcpy(wk.ik_keydata, key, key_len);

	return set80211priv(priv, IEEE80211_IOCTL_SETKEY, &wk, sizeof(wk));
}


static int
madwifi_get_seqnum(void *priv, u8 *addr, int idx, u8 *seq)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;
	struct ieee80211req_key wk;

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: addr=%s idx=%d\n", __func__, ether_sprintf(addr), idx);

	memset(&wk, 0, sizeof(wk));
	if (addr == NULL)
		memset(wk.ik_macaddr, 0xff, IEEE80211_ADDR_LEN);
	else
		memcpy(wk.ik_macaddr, addr, IEEE80211_ADDR_LEN);
	wk.ik_keyix = idx;

	if (set80211priv(priv, IEEE80211_IOCTL_GETKEY, &wk, sizeof(wk))) {
		printf("Failed to get encryption.\n");
		return -1;
	}

#ifdef WORDS_BIGENDIAN
	{
		/*
		 * wk.ik_keytsc is in host byte order (big endian), need to
		 * swap it to match with the byte order used in WPA.
		 */
		int i;
		u8 tmp[WPA_KEY_RSC_LEN];
		memcpy(tmp, &wk.ik_keytsc, sizeof(wk.ik_keytsc));
		for (i = 0; i < WPA_KEY_RSC_LEN; i++) {
			seq[i] = tmp[WPA_KEY_RSC_LEN - i - 1];
		}
	}
#else /* WORDS_BIGENDIAN */
	memcpy(seq, &wk.ik_keytsc, sizeof(wk.ik_keytsc));
#endif /* WORDS_BIGENDIAN */
	return 0;
}


static int 
madwifi_flush(void *priv)
{
#ifdef MADWIFI_BSD
	u8 allsta[IEEE80211_ADDR_LEN];
	memset(allsta, 0xff, IEEE80211_ADDR_LEN);
	return madwifi_sta_deauth(priv, allsta, IEEE80211_REASON_AUTH_LEAVE);
#else /* MADWIFI_BSD */
	return 0;		/* XXX */
#endif /* MADWIFI_BSD */
}


static int
madwifi_read_sta_driver_data(void *priv, struct hostap_sta_driver_data *data,
					u8 *addr)
{
	struct madwifi_driver_data *drv = priv;

#ifdef MADWIFI_BSD
	struct ieee80211req_sta_stats stats;

	memset(data, 0, sizeof(*data));

	/*
	 * Fetch statistics for station from the system.
	 */
	memset(&stats, 0, sizeof(stats));
	memcpy(stats.is_u.macaddr, addr, IEEE80211_ADDR_LEN);
	if (set80211priv(drv,
#ifdef MADWIFI_NG
			 IEEE80211_IOCTL_STA_STATS,
#else /* MADWIFI_NG */
			 IEEE80211_IOCTL_GETSTASTATS,
#endif /* MADWIFI_NG */
			 &stats, sizeof(stats))) {
		if (memcmp(addr, drv->acct_mac, ETH_ALEN) == 0) {
			memcpy(data, &drv->acct_data, sizeof(*data));
			return 0;
		}

		printf("Failed to get station stats information element.\n");
		return -1;
	}

	data->rx_packets = stats.is_stats.ns_rx_data;
	data->rx_bytes = stats.is_stats.ns_rx_bytes;
	data->tx_packets = stats.is_stats.ns_tx_data;
	data->tx_bytes = stats.is_stats.ns_tx_bytes;
	return 0;

#else /* MADWIFI_BSD */

	char buf[1024], line[128], *pos;
	FILE *f;
	unsigned long val;

	memset(data, 0, sizeof(*data));
	snprintf(buf, sizeof(buf), "/proc/net/madwifi/%s/" MACSTR,
		 drv->iface, MAC2STR(addr));

	f = fopen(buf, "r");
	if (!f) {
		if (memcmp(addr, drv->acct_mac, ETH_ALEN) != 0)
			return -1;
		memcpy(data, &drv->acct_data, sizeof(*data));
		return 0;
	}
	/* Need to read proc file with in one piece, so use large enough
	 * buffer. */
	setbuffer(f, buf, sizeof(buf));

	while (fgets(line, sizeof(line), f)) {
		pos = strchr(line, '=');
		if (!pos)
			continue;
		*pos++ = '\0';
		val = strtoul(pos, NULL, 10);
		if (strcmp(line, "rx_packets") == 0)
			data->rx_packets = val;
		else if (strcmp(line, "tx_packets") == 0)
			data->tx_packets = val;
		else if (strcmp(line, "rx_bytes") == 0)
			data->rx_bytes = val;
		else if (strcmp(line, "tx_bytes") == 0)
			data->tx_bytes = val;
	}

	fclose(f);

	return 0;
#endif /* MADWIFI_BSD */
}


static int
madwifi_sta_clear_stats(void *priv, u8 *addr)
{
#if defined(MADWIFI_BSD) && defined(IEEE80211_MLME_CLEAR_STATS)
	struct madwifi_driver_data *drv = priv;
	struct hostapd_data *hapd = drv->hapd;
	struct ieee80211req_mlme mlme;
	
	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL, "%s: addr=%s\n",
		      __func__, ether_sprintf(addr));

	mlme.im_op = IEEE80211_MLME_CLEAR_STATS;
	memcpy(mlme.im_macaddr, addr, IEEE80211_ADDR_LEN);
	return set80211priv(priv, IEEE80211_IOCTL_SETMLME, &mlme,
			    sizeof(mlme));
#else /* MADWIFI_BSD && IEEE80211_MLME_CLEAR_STATS */
	return 0; /* FIX */
#endif /* MADWIFI_BSD && IEEE80211_MLME_CLEAR_STATS */
}


static int
madwifi_set_opt_ie(void *priv, const u8 *ie, size_t ie_len)
{
	/*
	 * Do nothing; we setup parameters at startup that define the
	 * contents of the beacon information element.
	 */
	return 0;
}

static int
madwifi_sta_deauth(void *priv, u8 *addr, int reason_code)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;
	struct ieee80211req_mlme mlme;

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: addr=%s reason_code=%d\n",
		__func__, ether_sprintf(addr), reason_code);

	mlme.im_op = IEEE80211_MLME_DEAUTH;
	mlme.im_reason = reason_code;
	memcpy(mlme.im_macaddr, addr, IEEE80211_ADDR_LEN);
	return set80211priv(priv, IEEE80211_IOCTL_SETMLME, &mlme, sizeof(mlme));
}

static int
madwifi_sta_disassoc(void *priv, u8 *addr, int reason_code)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;
	struct ieee80211req_mlme mlme;

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
		"%s: addr=%s reason_code=%d\n",
		__func__, ether_sprintf(addr), reason_code);

	mlme.im_reason = reason_code;
	memcpy(mlme.im_macaddr, addr, IEEE80211_ADDR_LEN);
	return set80211priv(priv, IEEE80211_IOCTL_SETMLME, &mlme, sizeof(mlme));
}

static int
madwifi_del_sta(struct madwifi_driver_data *drv, u8 addr[IEEE80211_ADDR_LEN])
{
	struct hostapd_data *hapd = drv->hapd;
	struct sta_info *sta;

	hostapd_logger(hapd, addr, HOSTAPD_MODULE_IEEE80211,
		HOSTAPD_LEVEL_INFO, "deassociated");

	sta = ap_get_sta(hapd, addr);
	if (sta != NULL) {
		sta->flags &= ~WLAN_STA_ASSOC;
		wpa_sm_event(hapd, sta, WPA_DISASSOC);
		sta->acct_terminate_cause = RADIUS_ACCT_TERMINATE_CAUSE_USER_REQUEST;
		ieee802_1x_set_port_enabled(hapd, sta, 0);
		ap_free_sta(hapd, sta);
	}
	return 0;
}

static int
madwifi_process_wpa_ie(struct madwifi_driver_data *drv, struct sta_info *sta)
{
	struct hostapd_data *hapd = drv->hapd;
	struct ieee80211req_wpaie ie;
	int ielen, res;

	/*
	 * Fetch negotiated WPA/RSN parameters from the system.
	 */
	memset(&ie, 0, sizeof(ie));
	memcpy(ie.wpa_macaddr, sta->addr, IEEE80211_ADDR_LEN);
	if (set80211priv(drv, IEEE80211_IOCTL_GETWPAIE, &ie, sizeof(ie))) {
		printf("Failed to get WPA/RSN information element.\n");
		return -1;		/* XXX not right */
	}
	ielen = ie.wpa_ie[1];
	if (ielen == 0) {
		printf("No WPA/RSN information element for station!?\n");
		return -1;		/* XXX not right */
	}
	ielen += 2;
	res = wpa_validate_wpa_ie(hapd, sta, ie.wpa_ie, ielen,
			ie.wpa_ie[0] == WLAN_EID_RSN ?
			    HOSTAPD_WPA_VERSION_WPA2 : HOSTAPD_WPA_VERSION_WPA);
	if (res != WPA_IE_OK) {
		printf("WPA/RSN information element rejected? (res %u)\n", res);
		return -1;
	}
	free(sta->wpa_ie);
	sta->wpa_ie = malloc(ielen);
	if (sta->wpa_ie == NULL) {
		printf("No memory to save WPA/RSN information element!\n");
		return -1;
	}
	memcpy(sta->wpa_ie, ie.wpa_ie, ielen);
	sta->wpa_ie_len = ielen;
	return 0;
}

static int
madwifi_new_sta(struct madwifi_driver_data *drv, u8 addr[IEEE80211_ADDR_LEN])
{
	struct hostapd_data *hapd = drv->hapd;
	struct sta_info *sta;
	int new_assoc;

	hostapd_logger(hapd, addr, HOSTAPD_MODULE_IEEE80211,
		HOSTAPD_LEVEL_INFO, "associated");

	sta = ap_get_sta(hapd, addr);
	if (sta) {
		accounting_sta_stop(hapd, sta);
	} else {
		sta = ap_sta_add(hapd, addr);
		if (sta == NULL)
			return -1;
	}

	if (memcmp(addr, drv->acct_mac, ETH_ALEN) == 0) {
		/* Cached accounting data is not valid anymore. */
		memset(drv->acct_mac, 0, ETH_ALEN);
		memset(&drv->acct_data, 0, sizeof(drv->acct_data));
	}
	accounting_sta_get_id(hapd, sta);

	if (hapd->conf->wpa) {
		if (madwifi_process_wpa_ie(drv, sta))
			return -1;
	} else {
		free(sta->wpa_ie);
		sta->wpa_ie = NULL;
		sta->wpa_ie_len = 0;
	}

	/*
	 * Now that the internal station state is setup
	 * kick the authenticator into action.
	 */
	new_assoc = (sta->flags & WLAN_STA_ASSOC) == 0;
	sta->flags |= WLAN_STA_ASSOC;
	wpa_sm_event(hapd, sta, WPA_ASSOC);
	hostapd_new_assoc_sta(hapd, sta, !new_assoc);
	ieee802_1x_notify_port_enabled(sta->eapol_sm, 1);
	return 0;
}

static void
madwifi_wireless_event_wireless_custom(struct madwifi_driver_data *drv,
				       char *custom)
{
	struct hostapd_data *hapd = drv->hapd;

	HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL, "Custom wireless event: '%s'\n",
		      custom);

	if (strncmp(custom, "MLME-MICHAELMICFAILURE.indication", 33) == 0) {
		char *pos;
		u8 addr[ETH_ALEN];
		pos = strstr(custom, "addr=");
		if (pos == NULL) {
			HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
				      "MLME-MICHAELMICFAILURE.indication "
				      "without sender address ignored\n");
			return;
		}
		pos += 5;
		if (hwaddr_aton(pos, addr) == 0) {
			ieee80211_michael_mic_failure(drv->hapd, addr, 1);
		} else {
			HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
				      "MLME-MICHAELMICFAILURE.indication "
				      "with invalid MAC address");
		}
	} else if (strncmp(custom, "STA-TRAFFIC-STAT", 16) == 0) {
		char *key, *value;
		u32 val;
		key = custom;
		while ((key = strchr(key, '\n')) != NULL) {
			key++;
			value = strchr(key, '=');
			if (value == NULL)
				continue;
			*value++ = '\0';
			val = strtoul(value, NULL, 10);
			if (strcmp(key, "mac") == 0)
				hwaddr_aton(value, drv->acct_mac);
			else if (strcmp(key, "rx_packets") == 0)
				drv->acct_data.rx_packets = val;
			else if (strcmp(key, "tx_packets") == 0)
				drv->acct_data.tx_packets = val;
			else if (strcmp(key, "rx_bytes") == 0)
				drv->acct_data.rx_bytes = val;
			else if (strcmp(key, "tx_bytes") == 0)
				drv->acct_data.tx_bytes = val;
			key = value;
		}
	}
}

static void
madwifi_wireless_event_wireless(struct madwifi_driver_data *drv,
					    char *data, int len)
{
	struct hostapd_data *hapd = drv->hapd;
	struct iw_event iwe_buf, *iwe = &iwe_buf;
	char *pos, *end, *custom, *buf;

	pos = data;
	end = data + len;

	while (pos + IW_EV_LCP_LEN <= end) {
		/* Event data may be unaligned, so make a local, aligned copy
		 * before processing. */
		memcpy(&iwe_buf, pos, IW_EV_LCP_LEN);
		HOSTAPD_DEBUG(HOSTAPD_DEBUG_VERBOSE, "Wireless event: "
			      "cmd=0x%x len=%d\n", iwe->cmd, iwe->len);
		if (iwe->len <= IW_EV_LCP_LEN)
			return;

		custom = pos + IW_EV_POINT_LEN;
		if (drv->we_version > 18 &&
		    (iwe->cmd == IWEVMICHAELMICFAILURE ||
		     iwe->cmd == IWEVCUSTOM)) {
			/* WE-19 removed the pointer from struct iw_point */
			char *dpos = (char *) &iwe_buf.u.data.length;
			int dlen = dpos - (char *) &iwe_buf;
			memcpy(dpos, pos + IW_EV_LCP_LEN,
			       sizeof(struct iw_event) - dlen);
		} else {
			memcpy(&iwe_buf, pos, sizeof(struct iw_event));
			custom += IW_EV_POINT_OFF;
		}

		switch (iwe->cmd) {
		case IWEVEXPIRED:
			madwifi_del_sta(drv, (u8 *) iwe->u.addr.sa_data);
			break;
		case IWEVREGISTERED:
			madwifi_new_sta(drv, (u8 *) iwe->u.addr.sa_data);
			break;
		case IWEVCUSTOM:
			if (custom + iwe->u.data.length > end)
				return;
			buf = malloc(iwe->u.data.length + 1);
			if (buf == NULL)
				return;		/* XXX */
			memcpy(buf, custom, iwe->u.data.length);
			buf[iwe->u.data.length] = '\0';
			madwifi_wireless_event_wireless_custom(drv, buf);
			free(buf);
			break;
		}

		pos += iwe->len;
	}
}


static void
madwifi_wireless_event_rtm_newlink(struct madwifi_driver_data *drv,
					       struct nlmsghdr *h, int len)
{
	struct ifinfomsg *ifi;
	int attrlen, nlmsg_len, rta_len;
	struct rtattr * attr;

	if (len < sizeof(*ifi))
		return;

	ifi = NLMSG_DATA(h);

	if (ifi->ifi_index != drv->ifindex)
		return;

	nlmsg_len = NLMSG_ALIGN(sizeof(struct ifinfomsg));

	attrlen = h->nlmsg_len - nlmsg_len;
	if (attrlen < 0)
		return;

	attr = (struct rtattr *) (((char *) ifi) + nlmsg_len);

	rta_len = RTA_ALIGN(sizeof(struct rtattr));
	while (RTA_OK(attr, attrlen)) {
		if (attr->rta_type == IFLA_WIRELESS) {
			madwifi_wireless_event_wireless(
				drv, ((char *) attr) + rta_len,
				attr->rta_len - rta_len);
		}
		attr = RTA_NEXT(attr, attrlen);
	}
}


static void
madwifi_wireless_event_receive(int sock, void *eloop_ctx, void *sock_ctx)
{
	char buf[256];
	int left;
	struct sockaddr_nl from;
	socklen_t fromlen;
	struct nlmsghdr *h;
	struct madwifi_driver_data *drv = eloop_ctx;

	fromlen = sizeof(from);
	left = recvfrom(sock, buf, sizeof(buf), MSG_DONTWAIT,
			(struct sockaddr *) &from, &fromlen);
	if (left < 0) {
		if (errno != EINTR && errno != EAGAIN)
			perror("recvfrom(netlink)");
		return;
	}

	h = (struct nlmsghdr *) buf;
	while (left >= sizeof(*h)) {
		int len, plen;

		len = h->nlmsg_len;
		plen = len - sizeof(*h);
		if (len > left || plen < 0) {
			printf("Malformed netlink message: "
			       "len=%d left=%d plen=%d\n",
			       len, left, plen);
			break;
		}

		switch (h->nlmsg_type) {
		case RTM_NEWLINK:
			madwifi_wireless_event_rtm_newlink(drv, h, plen);
			break;
		}

		len = NLMSG_ALIGN(len);
		left -= len;
		h = (struct nlmsghdr *) ((char *) h + len);
	}

	if (left > 0) {
		printf("%d extra bytes in the end of netlink message\n", left);
	}
}


static int
madwifi_get_we_version(struct madwifi_driver_data *drv)
{
	struct iw_range *range;
	struct iwreq iwr;
	int minlen;
	size_t buflen;

	drv->we_version = 0;

	/*
	 * Use larger buffer than struct iw_range in order to allow the
	 * structure to grow in the future.
	 */
	buflen = sizeof(struct iw_range) + 500;
	range = malloc(buflen);
	if (range == NULL)
		return -1;
	memset(range, 0, buflen);

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) range;
	iwr.u.data.length = buflen;

	minlen = ((char *) &range->enc_capa) - (char *) range +
		sizeof(range->enc_capa);

	if (ioctl(drv->ioctl_sock, SIOCGIWRANGE, &iwr) < 0) {
		perror("ioctl[SIOCGIWRANGE]");
		free(range);
		return -1;
	} else if (iwr.u.data.length >= minlen &&
		   range->we_version_compiled >= 18) {
		wpa_printf(MSG_DEBUG, "SIOCGIWRANGE: WE(compiled)=%d "
			   "WE(source)=%d enc_capa=0x%x",
			   range->we_version_compiled,
			   range->we_version_source,
			   range->enc_capa);
		drv->we_version = range->we_version_compiled;
	}

	free(range);
	return 0;
}


static int
madwifi_wireless_event_init(void *priv)
{
	struct madwifi_driver_data *drv = priv;
	int s;
	struct sockaddr_nl local;

	madwifi_get_we_version(drv);

	drv->wext_sock = -1;

	s = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (s < 0) {
		perror("socket(PF_NETLINK,SOCK_RAW,NETLINK_ROUTE)");
		return -1;
	}

	memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_groups = RTMGRP_LINK;
	if (bind(s, (struct sockaddr *) &local, sizeof(local)) < 0) {
		perror("bind(netlink)");
		close(s);
		return -1;
	}

	eloop_register_read_sock(s, madwifi_wireless_event_receive, drv, NULL);
	drv->wext_sock = s;

	return 0;
}


static void
madwifi_wireless_event_deinit(void *priv)
{
	struct madwifi_driver_data *drv = priv;

	if (drv != NULL) {
		if (drv->wext_sock < 0)
			return;
		eloop_unregister_read_sock(drv->wext_sock);
		close(drv->wext_sock);
	}
}


static int
madwifi_send_eapol(void *priv, u8 *addr, u8 *data, size_t data_len, int encrypt)
{
	struct madwifi_driver_data *drv = priv;
	hostapd *hapd = drv->hapd;
	unsigned char buf[3000];
	unsigned char *bp = buf;
	struct l2_ethhdr *eth;
	size_t len;
	int status;

	/*
	 * Prepend the Etherent header.  If the caller left us
	 * space at the front we could just insert it but since
	 * we don't know we copy to a local buffer.  Given the frequency
	 * and size of frames this probably doesn't matter.
	 */
	len = data_len + sizeof(struct l2_ethhdr);
	if (len > sizeof(buf)) {
		bp = malloc(len);
		if (bp == NULL) {
			printf("EAPOL frame discarded, cannot malloc temp "
			       "buffer of size %lu!\n", (unsigned long) len);
			return -1;
		}
	}
	eth = (struct l2_ethhdr *) bp;
	memcpy(eth->h_dest, addr, ETH_ALEN);
	memcpy(eth->h_source, drv->hapd->own_addr, ETH_ALEN);
	eth->h_proto = htons(ETH_P_EAPOL);
	memcpy(eth+1, data, data_len);

	if (HOSTAPD_DEBUG_COND(HOSTAPD_DEBUG_MSGDUMPS))
		hostapd_hexdump("TX EAPOL", bp, len);

	status = l2_packet_send(drv->sock_xmit, addr, ETH_P_EAPOL, bp, len);

	if (bp != buf)
		free(bp);
	return status;
}

static void
handle_read(void *ctx, const u8 *src_addr, const u8 *buf, size_t len)
{
	struct madwifi_driver_data *drv = ctx;
	hostapd *hapd = drv->hapd;
	struct sta_info *sta;

	sta = ap_get_sta(hapd, src_addr);
	if (!sta || !(sta->flags & WLAN_STA_ASSOC)) {
		printf("Data frame from not associated STA %s\n",
		       ether_sprintf(src_addr));
		/* XXX cannot happen */
		return;
	}
	ieee802_1x_receive(hapd, src_addr, buf + sizeof(struct l2_ethhdr),
			   len - sizeof(struct l2_ethhdr));
}

static int
madwifi_init(struct hostapd_data *hapd)
{
	struct madwifi_driver_data *drv;
	struct ifreq ifr;
	struct iwreq iwr;

	drv = malloc(sizeof(struct madwifi_driver_data));
	if (drv == NULL) {
		printf("Could not allocate memory for madwifi driver data\n");
		goto bad;
	}

	memset(drv, 0, sizeof(*drv));
	drv->ops = madwifi_driver_ops;
	drv->hapd = hapd;
	drv->ioctl_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (drv->ioctl_sock < 0) {
		perror("socket[PF_INET,SOCK_DGRAM]");
		goto bad;
	}
	memcpy(drv->iface, hapd->conf->iface, sizeof(drv->iface));

	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", drv->iface);
	if (ioctl(drv->ioctl_sock, SIOCGIFINDEX, &ifr) != 0) {
		perror("ioctl(SIOCGIFINDEX)");
		goto bad;
	}
	drv->ifindex = ifr.ifr_ifindex;

	drv->sock_xmit = l2_packet_init(drv->iface, NULL, ETH_P_EAPOL,
					handle_read, drv, 1);
	if (drv->sock_xmit == NULL)
		goto bad;
	if (l2_packet_get_own_addr(drv->sock_xmit, hapd->own_addr))
		goto bad;
	if (hapd->conf->bridge[0] != '\0') {
		HOSTAPD_DEBUG(HOSTAPD_DEBUG_MINIMAL,
			"Configure bridge %s for EAPOL traffic.\n",
			hapd->conf->bridge);
		drv->sock_recv = l2_packet_init(hapd->conf->bridge, NULL,
						ETH_P_EAPOL, handle_read, drv,
						1);
		if (drv->sock_recv == NULL)
			goto bad;
	} else
		drv->sock_recv = drv->sock_xmit;

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, drv->iface, IFNAMSIZ);

	iwr.u.mode = IW_MODE_MASTER;

	if (ioctl(drv->ioctl_sock, SIOCSIWMODE, &iwr) < 0) {
		perror("ioctl[SIOCSIWMODE]");
		printf("Could not set interface to master mode!\n");
		goto bad;
	}

	madwifi_set_iface_flags(drv, 0);	/* mark down during setup */
#ifdef CONFIG_NO_AUTH
	madwifi_set_iface_flags(drv, 1);
#endif

	hapd->driver = &drv->ops;
	return 0;
bad:
	if (drv->sock_xmit != NULL)
		l2_packet_deinit(drv->sock_xmit);
	if (drv->ioctl_sock >= 0)
		close(drv->ioctl_sock);
	if (drv != NULL)
		free(drv);
	return -1;
}


static void
madwifi_deinit(void *priv)
{
	struct madwifi_driver_data *drv = priv;

	drv->hapd->driver = NULL;

	(void) madwifi_set_iface_flags(drv, 0);
	if (drv->ioctl_sock >= 0)
		close(drv->ioctl_sock);
	if (drv->sock_recv != NULL && drv->sock_recv != drv->sock_xmit)
		l2_packet_deinit(drv->sock_recv);
	if (drv->sock_xmit != NULL)
		l2_packet_deinit(drv->sock_xmit);
	free(drv);
}

static int
madwifi_set_ssid(void *priv, u8 *buf, int len)
{
	struct madwifi_driver_data *drv = priv;
	struct iwreq iwr;

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
	iwr.u.essid.flags = 1; /* SSID active */
	iwr.u.essid.pointer = (caddr_t) buf;
	iwr.u.essid.length = len + 1;

	if (ioctl(drv->ioctl_sock, SIOCSIWESSID, &iwr) < 0) {
		perror("ioctl[SIOCSIWESSID]");
		printf("len=%d\n", len);
		return -1;
	}
	return 0;
}

static int
madwifi_get_ssid(void *priv, u8 *buf, int len)
{
	struct madwifi_driver_data *drv = priv;
	struct iwreq iwr;
	int ret = 0;

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
	iwr.u.essid.pointer = (caddr_t) buf;
	iwr.u.essid.length = len;

	if (ioctl(drv->ioctl_sock, SIOCGIWESSID, &iwr) < 0) {
		perror("ioctl[SIOCGIWESSID]");
		ret = -1;
	} else
		ret = iwr.u.essid.length;

	return ret;
}

static int
madwifi_set_countermeasures(void *priv, int enabled)
{
	struct madwifi_driver_data *drv = priv;
	wpa_printf(MSG_DEBUG, "%s: enabled=%d", __FUNCTION__, enabled);
	return set80211param(drv, IEEE80211_PARAM_COUNTERMEASURES, enabled);
}

static const struct driver_ops madwifi_driver_ops = {
	.name			= "madwifi",
	.init			= madwifi_init,
	.deinit			= madwifi_deinit,
	.set_ieee8021x		= madwifi_set_ieee8021x,
	.set_privacy		= madwifi_set_privacy,
	.set_encryption		= madwifi_set_key,
	.get_seqnum		= madwifi_get_seqnum,
	.flush			= madwifi_flush,
	.set_generic_elem	= madwifi_set_opt_ie,
	.wireless_event_init	= madwifi_wireless_event_init,
	.wireless_event_deinit	= madwifi_wireless_event_deinit,
	.set_sta_authorized	= madwifi_set_sta_authorized,
	.read_sta_data		= madwifi_read_sta_driver_data,
	.send_eapol		= madwifi_send_eapol,
	.sta_disassoc		= madwifi_sta_disassoc,
	.sta_deauth		= madwifi_sta_deauth,
	.set_ssid		= madwifi_set_ssid,
	.get_ssid		= madwifi_get_ssid,
	.set_countermeasures	= madwifi_set_countermeasures,
	.sta_clear_stats        = madwifi_sta_clear_stats,
};

void madwifi_driver_register(void)
{
	driver_register(madwifi_driver_ops.name, &madwifi_driver_ops);
}
