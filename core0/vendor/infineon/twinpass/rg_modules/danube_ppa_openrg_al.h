/****************************************************************************
 *
 * rg/vendor/infineon/twinpass/rg_modules/danube_ppa_openrg_al.h
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

#ifndef _DANUBE_PPA_OPENRG_AL_H_
#define _DANUBE_PPA_OPENRG_AL_H_

/*
 * OpenRG AL specific definitions.
 */
typedef struct ppa_fp_entry_id_t PPA_SESSION;
typedef struct net_device PPA_NETIF;
typedef spinlock_t PPA_LOCK;
typedef kmem_cache_t PPA_MEM_CACHE;
typedef struct timer_list PPA_TIMER;
typedef char PPA_IFNAME;
typedef uint32_t IPADDR;
#define PPA_IF_NAME_SIZE IFNAMSIZ
#define PPA_ETH_ALEN ETH_ALEN

typedef struct ppa_buf_t PPA_BUF;

#define RET_E_OK 0
#define RET_E_ERR -1
#define RET_E_NOT_AVAIL -1
#define RET_E_NOT_POSSIBLE -2

/*
 * PPA driver generic AL functions.
 */
PPA_SESSION *ppa_get_session(PPA_BUF *);
uint8_t ppa_get_pkt_ip_proto(PPA_BUF *);
uint32_t ppa_get_pkt_src_ip(PPA_BUF *);
uint32_t ppa_get_pkt_dst_ip(PPA_BUF *);
void *ppa_get_pkt_transport_hdr(PPA_BUF *);
uint16_t ppa_get_pkt_src_port(PPA_BUF *);
uint16_t ppa_get_pkt_dst_port(PPA_BUF *);
void ppa_get_pkt_rx_src_mac_addr(PPA_BUF *, uint8_t[ETH_ALEN]);
void ppa_get_pkt_rx_dst_mac_addr(PPA_BUF *, uint8_t[ETH_ALEN]);
PPA_NETIF *ppa_get_pkt_src_if(PPA_BUF *);
PPA_NETIF *ppa_get_pkt_dst_if(PPA_BUF *);

uint32_t ppa_check_is_ppp_netif(PPA_NETIF *);
uint32_t ppa_check_is_pppoe_netif(PPA_NETIF *);
int32_t ppa_if_is_vlan_if(PPA_NETIF *dev, PPA_IFNAME *ifname);
int32_t ppa_is_netif_bridged(PPA_IFNAME *ifname, PPA_NETIF *dev);
int32_t ppa_pppoe_get_dst_mac(PPA_BUF *, PPA_NETIF * netif, uint8_t[ETH_ALEN]);
int32_t ppa_get_dst_mac(PPA_BUF *, PPA_SESSION *, uint8_t[ETH_ALEN]);

PPA_NETIF *ppa_get_netif(char *ifname);
void ppa_get_netif_hwaddr(PPA_NETIF *, uint8_t[ETH_ALEN]);
PPA_IFNAME *ppa_get_netif_name(PPA_NETIF *netif);
uint32_t ppa_is_netif_equal(PPA_NETIF *, PPA_NETIF *);
uint32_t ppa_is_netif_name(PPA_NETIF *, PPA_IFNAME *ifname);
uint32_t ppa_is_netif_name_prefix(PPA_NETIF *, PPA_IFNAME *ifname_prefix,
    int n);
uint16_t ppa_get_pkt_pppoe_session_id(PPA_BUF *);

uint32_t ppa_is_session_equal(PPA_SESSION *, PPA_SESSION *);
uint32_t ppa_get_session_helper(PPA_SESSION *);
uint32_t ppa_get_nat_helper(PPA_SESSION *);
uint32_t ppa_check_is_special_session(PPA_BUF *, PPA_SESSION * session);

int32_t ppa_is_pkt_fragment(PPA_BUF *ppa_buf);
int32_t ppa_is_pkt_host_output(PPA_BUF *);
int32_t ppa_is_pkt_broadcast(PPA_BUF *);
int32_t ppa_is_pkt_multicast(PPA_BUF *);
int32_t ppa_is_pkt_loopback(PPA_BUF *);
int32_t ppa_is_pkt_local(PPA_BUF *);
int32_t ppa_is_pkt_routing(PPA_BUF *);
int32_t ppa_is_pkt_mc_routing(PPA_BUF *);

int32_t ppa_lock_init(PPA_LOCK *p_lock);
void ppa_lock_get(PPA_LOCK *p_lock);
void ppa_lock_release(PPA_LOCK *p_lock);

void *ppa_malloc(uint32_t size);
int32_t ppa_free(void *buf);

int32_t ppa_mem_cache_create(const char *name, uint32_t size,
    PPA_MEM_CACHE **pp_cache);
int32_t ppa_mem_cache_destroy(PPA_MEM_CACHE *p_cache);
void *ppa_mem_cache_alloc(PPA_MEM_CACHE *p_cache);
void ppa_mem_cache_free(void *buf, PPA_MEM_CACHE *p_cache);

void ppa_memcpy(void *dst, const void *src, uint32_t n);
void ppa_memset(void *dst, uint32_t pad, uint32_t n);

int32_t ppa_timer_init(PPA_TIMER *p_timer, void (*callback)(unsigned long));
int32_t ppa_timer_add(PPA_TIMER *p_timer, uint32_t timeout_in_sec);
void ppa_timer_del(PPA_TIMER *p_timer);
uint32_t ppa_get_time_in_sec(void);

#endif
