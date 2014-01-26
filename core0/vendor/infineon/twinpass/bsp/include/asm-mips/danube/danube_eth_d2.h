#ifndef __DANUBE_ETH_D2_H__2006_08_23__12_05__
#define __DANUBE_ETH_D2_H__2006_08_23__12_05__


/******************************************************************************
**
** FILE NAME    : danube_eth_d2.h
** PROJECT      : Danube
** MODULES     	: ETH Interface (MII0 & MII1)
**
** DATE         : 23 AUG 2006
** AUTHOR       : Xu Liang
** DESCRIPTION  : ETH Interface (MII0 & MII1) Driver Header File
** COPYRIGHT    : 	Copyright (c) 2006
**			Infineon Technologies AG
**			Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 23 AUG 2006  Xu Liang        Initiate Version
** 23 OCT 2006  Xu Liang        Add GPL header.
*******************************************************************************/


/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  ioctl Command
 */
#define SET_ETH_SPEED_AUTO              SIOCDEVPRIVATE
#define SET_ETH_SPEED_10                SIOCDEVPRIVATE + 1
#define SET_ETH_SPEED_100               SIOCDEVPRIVATE + 2
#define WESTELL_SPI_MDIO_READ           SIOCDEVPRIVATE + 3
#define WESTELL_SPI_MDIO_WRITE          SIOCDEVPRIVATE + 4
#define WESTELL_SPI_MDIO_RESET          SIOCDEVPRIVATE + 5

#ifdef CONFIG_WSTL_DEV_STATS
#define WESTELL_ETH_STATS               SIOCDEVPRIVATE + 6
#else
#define SET_ETH_REG                     SIOCDEVPRIVATE + 6
#endif

#define VLAN_TOOLS                      SIOCDEVPRIVATE + 7
#define MAC_TABLE_TOOLS                 SIOCDEVPRIVATE + 8
#define SET_VLAN_COS                    SIOCDEVPRIVATE + 9
#define SET_DSCP_COS                    SIOCDEVPRIVATE + 10
#define ENABLE_VLAN_CLASSIFICATION      SIOCDEVPRIVATE + 11
#define DISABLE_VLAN_CLASSIFICATION     SIOCDEVPRIVATE + 12
#define VLAN_CLASS_FIRST                SIOCDEVPRIVATE + 13
#define VLAN_CLASS_SECOND               SIOCDEVPRIVATE + 14
#define ENABLE_DSCP_CLASSIFICATION      SIOCDEVPRIVATE + 15
#define DISABLE_DSCP_CLASSIFICATION     SIOCDEVPRIVATE + 16
#define PASS_UNICAST_PACKETS            SIOCDEVPRIVATE + 17
#define FILTER_UNICAST_PACKETS          SIOCDEVPRIVATE + 18
#define KEEP_BROADCAST_PACKETS          SIOCDEVPRIVATE + 19
#define DROP_BROADCAST_PACKETS          SIOCDEVPRIVATE + 20
#define KEEP_MULTICAST_PACKETS          SIOCDEVPRIVATE + 21
#define DROP_MULTICAST_PACKETS          SIOCDEVPRIVATE + 22
#define SET_ETH_DUPLEX_AUTO             SIOCDEVPRIVATE + 23
#define SET_ETH_DUPLEX_HALF             SIOCDEVPRIVATE + 24
#define SET_ETH_DUPLEX_FULL             SIOCDEVPRIVATE + 25

#ifdef CONFIG_WSTL_DEV_STATS
#define SET_ETH_REG                     SIOCDEVPRIVATE + 26
#endif

#ifdef CONFIG_WSTL_DEV_STATS
struct wstl_user_net_device_stats_t {
    unsigned long rx_packets; /* total packets received */
    unsigned long rx_packets_fastpath; /* total packets via the PPA */
    unsigned long tx_packets; /* total packets transmitted */
    unsigned long tx_packets_fastpath; /* total packets via the PPA */
    unsigned long rx_bytes; /* total bytes received */
    unsigned long tx_bytes; /* total bytes transmitted */
    unsigned long rx_errors; /* bad packets received */
    unsigned long tx_errors; /* packet transmit problems */
    unsigned long rx_dropped; 
    unsigned long tx_dropped; 
    unsigned long rx_multicast; /* multicast packets received */
    unsigned long tx_multicast; /* multicast packets transmitted */
    unsigned long rx_broadcast; /* broadcast packets received */
    unsigned long tx_broadcast; /* broadcast packets transmitted */
    unsigned long rx_unicast; /* unicast packets received */
    unsigned long tx_unicast; /* unicast packets transmitted */
    unsigned long collisions;
  };
#endif
/*
 * ####################################
 *              Data Type
 * ####################################
 */

/*
 *  Data Type Used to Call ioctl
 */
struct vlan_cos_req {
    int     pri;
    int     cos_value;
};

struct dscp_cos_req {
    int     dscp;
    int     cos_value;
};


/*
 * ####################################
 *             Declaration
 * ####################################
 */

#if defined(__KERNEL__)
#endif  //  defined(__KERNEL__)


#endif  //  __DANUBE_ETH_D2_H__2006_08_23__12_05__
