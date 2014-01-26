#include "common_cli.h"

//make sure athx is up on  CPU1 before get mac address
wait_queue_head_t my_waitqueue;
#define MPS_ENTRY_NUM0 1024
static int refcount = 0; /* for wait up condition */
void *got_data;	/* ernest:FIXME! */
static struct iwreq user_ret_data; /* FIXME */

static int vnet_vap_open(struct net_device *dev);
static int vnet_vap_close(struct net_device *dev);
static int vnet_ioctl(struct net_device *net_dev, struct ifreq *rq, int cmd);
static int ioctl_send(struct ioctl_req *ioreq);
static int ioctl_recv(void *pdata, unsigned int len);
static int vnet_change_mtu(struct net_device *net_dev, int new_mtu);
/*static int vnet_set_mac_address(struct net_device *net_dev, void *addr); */
static int vnet_get_mac_address(char *dev_name, unsigned char *addr);
//static int vnet_list_stations(struct net_device *net_dev, struct ifreq *ifr, int cmd);
static int vnet_ieee80211_ioctl(struct net_device *net_dev, struct ifreq *ifr, int cmd);
static int vnet_ioctl_standard_call(struct net_device *dev, struct ifreq *ifr, int cmd);
static int vnet_ioctl_private_call(struct net_device *dev, struct ifreq *ifr, int cmd);


static struct net_device *g_mps_cpu0_netdev;
static struct ioctl_req *cpu1_ioreq = NULL;

typedef struct cpu0_dev
{
	struct net_device *dev;
	char ifname[IFNAMSIZ];
} cpu0_dev_t;

cpu0_dev_t cpu0_device_tbl[4];

int cpu0_dev_num = 0;

//prochao+, 13-03-2007
static struct net_device_stats	net_wifi_dev_stats[4+1];	//one more for the "wifi0"
static struct iw_statistics		net_wireless_stats;		//only one as "wifi0"
//procaho-

#define IFX_MPS_MAGIC2		39

static struct net_device *alloc_netdev(int sizeof_priv, const char *mask,
				       void (*setup)(struct net_device *))
{
	struct net_device *dev;
	int alloc_size;

	/* ensure 32-byte alignment of the private area */
	alloc_size = sizeof (*dev) + sizeof_priv + 31;

	dev = (struct net_device *) kmalloc (alloc_size, GFP_KERNEL);
	if (dev == NULL)
	{
		printk(KERN_ERR "alloc_dev: Unable to allocate device memory.\n");
		return NULL;
	}

	memset(dev, 0, alloc_size);

	if (sizeof_priv)
		dev->priv = (void *) (((long)(dev + 1) + 31) & ~31);

	setup(dev);
	strcpy(dev->name, mask);

	return dev;
}

static struct net_device *cpu0_get_dev_by_index(int ii)
{
	if (ii < 4)
	{
		return cpu0_device_tbl[ii].dev;
	}

	return NULL;
}

#if 0
static void dump_data(unsigned char *data, int len) {
	int i;

	DBG_PRINT("\n================================\n");
	for (i=0;i < len;i++)
	{
		DBG_PRINT("%2X:", *data);
		data++;
	}
	DBG_PRINT("\n================================\n");
}
#endif

static void print_mac(unsigned char *mac) {
	if (mac!=NULL)
	{
		DBG_PRINT("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
}

/*
 * Number of private arguments
 */
static inline int get_priv_size(__u16   args)
{
	int num = args & IW_PRIV_SIZE_MASK;
	int type = (args & IW_PRIV_TYPE_MASK) >> 12;

	return num * priv_type_size[type];
}

static void us_data_cb(void *pdata, unsigned int len)
{
	struct sk_buff *skb = (struct sk_buff *)pdata;
	struct net_device *dev;

	dma_cache_inv((unsigned long)skb, (unsigned long)sizeof(*skb));
	dma_cache_inv((unsigned long)skb->head, (unsigned long)(skb->end - skb->head));
#ifdef	_PRO_DEBUGGING_		//prochao+
	DBG_PRINT(KERN_INFO "%s:%d skb->len=%d\n", __func__, __LINE__, skb->len);
	dump(skb->data, skb->len);
#endif
	/* Take control of skbuff which is a CPU0 allocated buffer anyways */
	skb->next = skb->prev = NULL; /* Just in case */
	skb->list = NULL;
	skb->sk = 0;
	skb->dst = 0;

	/* Setup CPU0 skb header here. Just like any other device driver */
#ifdef	_PRO_DEBUGGING_		//prochao+
	DBG_PRINT(KERN_INFO "%s:%d skb->dev->name=%s\n", __func__, __LINE__, skb->dev->name);
#endif

#if 0
	dev = dev_get_by_name(skb->dev->name);
#else
	dev = cpu0_get_dev_by_index(skb->cb[IFX_MPS_MAGIC2]);
#endif
	if (!dev)
	{
		DBG_PRINT("%s:%d dev name %s not found.\n", __func__, __LINE__, skb->dev->name);
		dev_kfree_skb(skb);
		return;
	}
//    skb->dev = g_mps_cpu0_netdev;
	skb->dev = dev;
	skb->real_dev = NULL;
//    skb->protocol = eth_type_trans(skb, skb->dev);
#ifdef	_PRO_DEBUGGING_		//prochao+
	DBG_PRINT(KERN_INFO "%s:%d skb->protocol=%04x\n", __func__, __LINE__, skb->protocol);
#endif
#if 0
	netif_receive_skb(skb);
#else
	netif_rx(skb);
#endif

}

static void us_message_cb(void *pdata, unsigned int len)
{   
	/* got response from madwifi, wake up! */

	/* if this is ioctl command, copy data to userland */
	refcount = 1;
	ioctl_recv(pdata, len);
	wake_up_interruptible(&my_waitqueue);
}

#ifdef WIRELESS_EVENT_SUPPORT
struct iw_event_twinpass
{
	char            name[IFNAMSIZ];		//yuan-gu, 12/08/2006, moved from last to begin
	__u16           len;					/* Real lenght of this stuff */
	__u16           cmd;					/* Wireless IOCTL */
	union iwreq_data        u;				/* IOCTL fixed payload */
};

static int rtnetlink_fill_iwinfo(struct sk_buff *   skb,
								 struct net_device * dev,
								 int         type,
								 char *          event,
								 int         event_len)
{
	struct ifinfomsg *r;
	struct nlmsghdr  *nlh;
	unsigned char    *b = skb->tail;

	nlh = NLMSG_PUT(skb, 0, 0, type, sizeof(*r));
	r = NLMSG_DATA(nlh);
	r->ifi_family = AF_UNSPEC;
	r->ifi_type = dev->type;
	r->ifi_index = dev->ifindex;
	r->ifi_flags = dev->flags;
	r->ifi_change = 0;	/* Wireless changes don't affect those flags */

	/* Add the wireless events in the netlink packet */
	RTA_PUT(skb, IFLA_WIRELESS,
			event_len, event);

	nlh->nlmsg_len = skb->tail - b;
	return skb->len;

	nlmsg_failure:
	rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}

static void rtmsg_iwinfo_twinpass(struct net_device *   dev,
								  char *          event,
								  int         event_len)
{
	struct sk_buff *skb;
	int size = NLMSG_GOODSIZE;

	skb = alloc_skb(size, GFP_ATOMIC);
	if (!skb)
		return;

	if (rtnetlink_fill_iwinfo(skb, dev, RTM_NEWLINK,
							  event, event_len) < 0)
	{
		kfree_skb(skb);
		return;
	}
	NETLINK_CB(skb).dst_groups = RTMGRP_LINK;
	netlink_broadcast(rtnl, skb, 0, RTMGRP_LINK, GFP_ATOMIC);
}

void wlan_event_cb(void *pdata, unsigned int len)
{
	struct net_device *dev;
	struct iw_event_twinpass *event;
	int event_len;

	// add by ygwei for debug cache 12/07
	dma_cache_inv((unsigned long)pdata, len);

	event = (struct iw_event_twinpass *) pdata;

	DBG_PRINT("%s: device name: %s\n", __func__, event->name);

	/* get device */
	dev = dev_get_by_name(event->name);
	if (dev == NULL)
	{
		DBG_PRINT("%s:%d No such device!\n", __func__, __LINE__);
		return;
	}
	/* remove the interface name field */
	event_len = len - IFNAMSIZ;
	//yuan-gu, 12/08/2006
	pdata += IFNAMSIZ;
	//
	DBG_PRINT("%s: event_len: %d\n", __func__, event_len);

	rtmsg_iwinfo_twinpass(dev, (char *) pdata, event_len);
}
#endif

/* all commands share one ioct_recv() */
static int 
ioctl_recv(void *pdata, unsigned int len) 
{
	struct ioctl_req *ioreq;
	ioreq = (struct ioctl_req *)pdata;

	if (ioreq->cmd == IEEE80211_IOCTL_GETWPAIE || ioreq->cmd == IEEE80211_IOCTL_STA_INFO)
	{
		dma_cache_inv((unsigned long)&(ioreq->ifr), sizeof(struct ifreq));
		if (ioreq->extra_size > 0)
			dma_cache_inv((unsigned long)ioreq->extra, ioreq->extra_size);

		DBG_PRINT("%s:%d retcmd:%x, dest user address:%p src kernel address:%p, datalength:%d\n", __func__, __LINE__,
				ioreq->cmd, user_ret_data.u.data.pointer,  ioreq->extra, ioreq->extra_size); //ernest 2006-12-07 debug
		//dump_data(ioreq->extra, ioreq->extra_size);	
		cpu1_ioreq = ioreq;

		//dump_data(cpu1_ioreq->extra, cpu1_ioreq->extra_size);
	}

	return 0;
}

static int 
vnet_open(struct net_device *dev)
{
/* XXX get mac addr from cpu1 */

	if (vnet_get_mac_address(dev->name, dev->dev_addr) < 0)
	{
		DBG_PRINT("%s:%d get mac address fail!\n", __func__, __LINE__);
	}
	//memcpy(dev->dev_addr, mac, dev->addr_len);
	dev->trans_start = jiffies;

	if (!netif_running(dev))
	{
		DBG_PRINT("%s:%d cannot get mac address, dev is not running\n", __func__, __LINE__);
		return  -EBUSY;
	}

	return 0;
}

static int 
vnet_tx(struct sk_buff *skb, struct net_device *dev)
{

	dma_cache_wback((unsigned long)skb, (unsigned long)sizeof(*skb));
	dma_cache_wback((unsigned long)skb->head,
					(unsigned long)(skb->end - skb->head));
	if (skb->dev)
	{
#ifdef	_PRO_DEBUGGING_		//prochao+
		DBG_PRINT(KERN_INFO "%s:%d skb->dev->name=%s dev->name=%s\n", __func__, __LINE__, skb->dev->name, dev->name);
#endif
	}
	if (ifx_mps_data_send(MPS_SRV_DS_DATA_ID, (void *)skb, sizeof(struct sk_buff)) != IFX_OK)
	{
		DBG_PRINT("%s:%d ifx_mps_data_send() failed.\n", __func__, __LINE__);
		return -1;
	}
	return 0;
}

/* each command needs to call ioctl_send */
static int 
ioctl_send(struct ioctl_req *ioreq)
{
	int ret = 0;

	// Add by ygwei for debug
	//dma_cache_wback_inv((unsigned long)ioreq, sizeof(struct ioctl_req));

	if ((ifx_mps_app_message_send(MPS_SRV_DS_MSG_ID, (void *)ioreq, sizeof(struct ioctl_req))) != IFX_OK)
	{
		DBG_PRINT("%s:%d ifx_mps_app_message_send fail!\n", __func__, __LINE__);
		return -1;
	}

	/* return data is put to user ioctl command buffer */
	wait_event_interruptible(my_waitqueue, refcount!=0);    
	DBG_PRINT("refcount should be=1 waked %d\n", refcount);

	if (cpu1_ioreq != NULL)
	{	//FIXME: 
		ret = cpu1_ioreq->ret;

		if (cpu1_ioreq->cmd  == IEEE80211_IOCTL_GETWPAIE || cpu1_ioreq->cmd == IEEE80211_IOCTL_STA_INFO)
		{
			DBG_PRINT("%s:%d, extra_size:%d:%p:%p\n", __func__, __LINE__,
					cpu1_ioreq->extra_size, user_ret_data.u.data.pointer, cpu1_ioreq->extra);//ernest 2006-12-07 
			if (copy_to_user(user_ret_data.u.data.pointer, cpu1_ioreq->extra, cpu1_ioreq->extra_size))
			{
				DBG_PRINT("%s:%d copy_to_user!!!\n", __func__, __LINE__);
			}
		}
		else
		{
			DBG_PRINT("%s:%d\n", __func__, __LINE__);
			if (copy_to_user(&user_ret_data, &(cpu1_ioreq->ifr), sizeof(struct iwreq)))
			{
				DBG_PRINT("%s:%d copy_to_user_fail!!!\n", __func__, __LINE__);
			}
		} 
		cpu1_ioreq = NULL;
		//ret = cpu1_ioreq->ret;
	}
	else
	{ // Add by ygwei
		ret = ioreq->ret;
	}

	/* reset wakeup flag */
	refcount = 0;

	return ret;
}

static int 
vnet_get_mac_address(char *dev_name, unsigned char *addr)
{
	struct ioctl_req *ioreq = NULL;
	int ret = 0;

	ioreq = (struct ioctl_req *) kmalloc(sizeof(struct ioctl_req), GFP_KERNEL);
	ioreq->cmd = SIOCGIFHWADDR; 
	sprintf(ioreq->ifname, "%s", dev_name);
	sprintf(ioreq->ifr.ifr_name, "%s", dev_name);
	ret = ioctl_send(ioreq);
	print_mac(ioreq->ifr.ifr_hwaddr.sa_data);
	memcpy(addr, ioreq->ifr.ifr_hwaddr.sa_data, MAX_ADDR_LEN);
	kfree(ioreq);
	return ret;
}

static int 
vnet_vap_open(struct net_device *dev)
{
	/* For ath0, ath...*/

	if (vnet_get_mac_address(dev->name, dev->dev_addr) < 0)
	{
		DBG_PRINT("%s:%d get mac address fail!\n", __func__, __LINE__);
	}
	DBG_PRINT("%s:%d %s mac address is done!\n", __func__, __LINE__, dev->name);
	print_mac(dev->dev_addr);
	return 0;
}

static int 
vnet_vap_close(struct net_device *dev)
{
	struct ioctl_req *ioreq = NULL;
	int ret = 0;
	ioreq = (struct ioctl_req *) kmalloc(sizeof(struct ioctl_req), GFP_KERNEL);
	ioreq->cmd = SIOCSIFDOWN; 
	sprintf(ioreq->ifname, "%s", dev->name);
	sprintf(ioreq->ifr.ifr_name, "%s", dev->name);
	ret = ioctl_send(ioreq);
	kfree(ioreq);
	return ret;
}

//prochao+, 12-03-2007, to support the statistics
static struct net_device_stats *
vnet_get_stats(struct net_device *dev)
{
	struct net_device_stats *stats;
	int		dev_i;

	for (dev_i = 0; dev_i < 4; dev_i++)
	{
		if (dev == cpu0_device_tbl[dev_i].dev)
			break;
	}
	if (dev_i == 4)
	{	// not exist, check if "wifi0" or not
		if (strcmp(dev->name, "wifi0") == 0)
		{	//later maybe extended for the dual-band wifi devices
			dev_i = VNET_WIFI0_MAGIC_NUM;	//magic# for the "wifi0" device indication
		}
		else
		{
			printk("\n%s:%d NULL", __func__, __LINE__);
			return NULL;
		}
	}
	//
	stats = (struct net_device_stats *) ifx_mps_get_core1_wifi_stats( dev_i);	// should not be NULL
	//debugging
//	printk("\n%s:%d stats=0x%08X, dev_i=%d", __func__, __LINE__, stats, dev_i);
	if (stats != NULL)
	{
		if (dev_i == VNET_WIFI0_MAGIC_NUM)	//funny!!!
			dev_i = 4;	//set to store the pre-defined storage
		//
		dma_cache_wback_inv((unsigned long) stats, sizeof(struct net_device_stats));
		memcpy(&net_wifi_dev_stats[dev_i], stats, sizeof(struct net_device_stats));
	}

	return &net_wifi_dev_stats[dev_i];
}

//wireless statistics always use the "wifi0"
static struct iw_statistics *
vnet_get_wireless_stats(struct net_device *dev)
{
	struct iw_statistics	*iwstats;
	int		dev_i;

	for (dev_i = 0; dev_i < 4; dev_i++)
	{
		if (dev == cpu0_device_tbl[dev_i].dev)
			break;
	}
	if (dev_i == 4)
	{	// not exist
		printk("\n%s:%d NULL", __func__, __LINE__);
		return NULL;
	}
	//
	iwstats = (struct iw_statistics *) ifx_mps_get_core1_wireless_stats( dev_i);
	//debugging
//	printk("\n%s:%d iwstats=0x%08X, dev_i=%d", __func__, __LINE__, iwstats);
	if (iwstats != NULL)
	{
		dma_cache_wback_inv((unsigned long) iwstats, sizeof(struct iw_statistics));
		memcpy(&net_wireless_stats, iwstats, sizeof(struct iw_statistics));
	}

	return &net_wireless_stats;
}
//prochao-

static int 
vnet_ioctl(struct net_device *net_dev, struct ifreq *ifr, int cmd)
{

	struct ieee80211_clone_params cp;
	struct net_device *vap_dev;
	int error = 0;
	struct ioctl_req *ioreq;
//prochao+, 13-03-2007
	struct iw_statistics	*iwstats;
	struct net_device_stats	*stats;
//prochao-

	ioreq = (struct ioctl_req *)kmalloc(sizeof(struct ioctl_req), GFP_KERNEL);
	//prochao+
	if (ioreq == NULL)
		return -ENOMEM;
	//prochao-

	ioreq->cmd = cmd;
	sprintf(ioreq->ifname,"%s", ifr->ifr_name);

	DBG_PRINT("%s:%d ioctl cmd:%x\n", __FILE__, __LINE__, cmd);
	switch (cmd)
	{
		/*dev_iwstats moving from wireless.c*/
		case    SIOCGIWSTATS:
//prochao+, 13-03-2007
//			struct iw_statistics	*iwstats;

			iwstats = vnet_get_wireless_stats( net_dev);
			if (iwstats != NULL)
			{
				if (copy_to_user(ifr->ifr_data, iwstats, sizeof(struct iw_statistics)))
					error = -EFAULT;
				else
					error = 0;
			}
//prochao-
			kfree( ioreq);
			return error;	//0;	//prochao+-, 13-03-2007
		case    SIOCGATHSTATS: /* tools:athstats 
								diag:ani
								*/
//prochao+, 13-03-2007
#if 1
//			struct net_device_stats	*stats;

			stats = vnet_get_stats( net_dev);
			if (stats != NULL)
			{
				if (copy_to_user(ifr->ifr_data, stats, sizeof(struct net_device_stats)))
					error = -EFAULT;
				else
					error = 0;
			}
#else
			ioreq->extra_size = sizeof(struct ath_stats);
			ioreq->ifr.ifr_data = (char *)kmalloc(ioreq->extra_size, GFP_KERNEL);
			if (!ioreq->ifr.ifr_data)
			{
//				kfree( ioreq->ifr.ifr_data);
				kfree( ioreq);
				return -ENOMEM;
			}

			if (copy_from_user(ioreq->ifr.ifr_data, ifr->ifr_data, ioreq->extra_size))
			{
				DBG_PRINT("copy_from_user:%p:%p!\n", ioreq, ifr->ifr_data);
				kfree( ioreq->ifr.ifr_data);
				kfree( ioreq);
				return -EFAULT;
			}
			error = ioctl_send(ioreq);

			kfree( ioreq->ifr.ifr_data);
#endif
//prochao-, 13-03-2007
			kfree( ioreq);

			return error;
		case    SIOCGATHDIAG:	/* 	dfs:radartool 
								diag:eeprom
									 rfgain
									 ani
									 radar
									 txpow
									 dumpregs
									 key			
							*/
			ioreq->extra_size = sizeof(struct ath_diag);
			ioreq->ifr.ifr_data = (char *)kmalloc(ioreq->extra_size, GFP_KERNEL);
			if (ioreq->ifr.ifr_data == NULL)
			{
//				kfree( ioreq->ifr.ifr_data);
				kfree( ioreq);
				return -ENOMEM;
			}

			if (copy_from_user(ioreq->ifr.ifr_data, ifr->ifr_data, ioreq->extra_size))
			{
				DBG_PRINT("copy_from_user:%p:%p!\n", ioreq, ifr->ifr_data);
				kfree( ioreq->ifr.ifr_data);
				kfree( ioreq);
				return -EFAULT;
			}

			error = ioctl_send(ioreq);

			kfree( ioreq->ifr.ifr_data);
			kfree( ioreq);
			return error;
		case    SIOCGATHPHYERR:	/* radartool */
			ioreq->extra_size = sizeof(cmd);
			ioreq->ifr.ifr_data = (char *)kmalloc(ioreq->extra_size, GFP_KERNEL);
			if (ioreq->ifr.ifr_data == NULL)
			{
//				kfree( ioreq->ifr.ifr_data );
				kfree( ioreq);
				return -ENOMEM;
			}

			if (copy_from_user(ioreq->ifr.ifr_data, ifr->ifr_data, ioreq->extra_size))
			{
				DBG_PRINT("copy_from_user:%p:%p!\n", ioreq, ifr->ifr_data);
				kfree(ioreq->ifr.ifr_data);
				kfree( ioreq);
				return -EFAULT;
			}
			error = ioctl_send(ioreq);
			//prochao+
			kfree( ioreq->ifr.ifr_data);
			kfree( ioreq);
			//prochao-
			return error;
		case    SIOCETHTOOL:	/* man ethtool(8) */
			ioreq->extra_size = sizeof(cmd);
			ioreq->ifr.ifr_data = (char *)kmalloc(ioreq->extra_size, GFP_KERNEL);
			if (ioreq->ifr.ifr_data == NULL)
			{
//				kfree( ioreq->ifr.ifr_data);
				kfree( ioreq);
				return -ENOMEM;
			}

			if (copy_from_user(ioreq->ifr.ifr_data, ifr->ifr_data, ioreq->extra_size))
			{
				DBG_PRINT("copy_from_user fail:%p:%p!\n", ioreq, ifr->ifr_data);
				kfree( ioreq->ifr.ifr_data);
				kfree( ioreq);
				return -EFAULT;
			}
			error = ioctl_send(ioreq);
			kfree(ioreq->ifr.ifr_data);
			kfree( ioreq);
			return error;	//break;
		case SIOC80211IFCREATE:
//prochao+, 13-03-2007, to support the statistics
			if (cpu0_dev_num >= 4)
			{
				kfree( ioreq);
				return -EFAULT;
			}
//prochao-
			ioreq->extra_size = sizeof(struct ieee80211_clone_params);
			ioreq->ifr.ifr_data = (char *)kmalloc(ioreq->extra_size, GFP_KERNEL);
			if (ioreq->ifr.ifr_data == NULL)
			{
//				kfree( ioreq->ifr.ifr_data);
				kfree( ioreq);
				return -ENOMEM;
			}

			if (copy_from_user(ioreq->ifr.ifr_data, ifr->ifr_data, sizeof(cp)))
			{
				DBG_PRINT("copy_from_user fail:%p:%p!\n", ioreq, ifr->ifr_data);
				kfree(ioreq->ifr.ifr_data);
				kfree( ioreq);
				return -EFAULT;
			}

			memcpy(&cp, ifr->ifr_data, sizeof(cp));             
			dma_cache_wback_inv((unsigned long)(&(ioreq->ifr)), sizeof(struct ifreq));
			dma_cache_wback_inv((unsigned long)(ioreq->ifr.ifr_data), sizeof(cp));
			error = ioctl_send(ioreq);

			/* create VAP on  CPU 0*/
			/* only return success can we go ahread */
			vap_dev = alloc_netdev(sizeof(struct net_device), cp.icp_name, ether_setup);
			DBG_PRINT("%s:%d allocate vap:%s!\n", __func__, __LINE__, cp.icp_name);
			if (!vap_dev)
			{
				kfree( ioreq->ifr.ifr_data);
				kfree( ioreq);
				return(-ENODEV);
			}
			/* overrite some functions.*/
			vap_dev->open = vnet_vap_open;
			vap_dev->stop = vnet_vap_close;
			vap_dev->do_ioctl = vnet_ieee80211_ioctl;
			vap_dev->hard_start_xmit = vnet_tx;
			vap_dev->change_mtu = vnet_change_mtu;
			/*vap_dev->set_mac_address = vnet_set_mac_address;*/
			/* Keep vap name */
			memcpy(cpu0_device_tbl[cpu0_dev_num].ifname, cp.icp_name, IFNAMSIZ);
//prochao+, 13-03-2007, to support the statistics
			vap_dev->get_stats = vnet_get_stats;
			vap_dev->get_wireless_stats = vnet_get_wireless_stats;
//prochao-
			cpu0_device_tbl[cpu0_dev_num].dev = vap_dev;
			cpu0_dev_num++;

#if IW_HANDLER_VERSION < 7
			/* TODO: Not implemented yet */
			/*	vap_dev->get_wireless_stats = vnet_ieee80211_iw_getstats;	*/  
#endif
			/* see net-core-wireless.c: let it call dev->do_ioctl()			*/
			/*	vap_dev->wireless_handlers = &vnet_ieee80211_iw_handler_def;*/

			if (register_netdevice(vap_dev))
			{
				DBG_PRINT(KERN_ERR "%s: unable to register device\n", vap_dev->name);
				kfree( ioreq->ifr.ifr_data);
				kfree( ioreq);

				return(-ENODEV);
			}

			kfree( ioreq->ifr.ifr_data);
			kfree( ioreq);

			return error;       
		default:
			kfree( ioreq);
			return -EOPNOTSUPP;
	}

	/*reset the wakeup flag*/
	refcount = 0;

	return error;
}

static int 
vnet_ioctl_private_call(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	/* private iw_handler 				 			*/      
	/*(SIOCIWFIRSTPRIV)0x8BE0 - 					*/
	/*				0x8BFF(SIOCIWLASTPRIV)			*/
	/* actually only 0x8BE0+22						*/
	/* wireless.c: ioctl_private_call				*/

	struct iwreq *          iwr = (struct iwreq *) ifr;
	const struct iw_priv_args *     descr = NULL;
	struct ioctl_req *ioreq = NULL;
	int ret  = 0;//-EINVAL;
	int i = 0;
	int num_private_args = sizeof(ieee80211_priv_args) / sizeof(ieee80211_priv_args[0]);

	ioreq = (struct ioctl_req *)kmalloc(sizeof(struct ioctl_req), GFP_KERNEL);
	//prochao+
	if (ioreq == NULL)
		return -ENOMEM;
	//prochao-
	ioreq->extra = NULL;    
	ioreq->cmd = cmd;   
	sprintf(ioreq->ifname, "%s", ifr->ifr_name);
	sprintf(ioreq->ifr.ifr_name, "%s", ifr->ifr_name);

	DBG_PRINT("%s:%d\n", __func__, __LINE__);
	/* Get the description of the IOCTL */
	//for(i = 0; i < dev->wireless_handlers->num_private_args; i++)
	//		descr = &(dev->wireless_handlers->private_args[i]);
	//		break;
	//


	for (i = 0; i < num_private_args; i++)
	{
		if (cmd == ieee80211_priv_args[i].cmd)
		{
			descr = &(ieee80211_priv_args[i]);
			break;
		}
	}

	DBG_PRINT("cmd:%X, name:%s\n", descr->cmd, descr->name);    
	DBG_PRINT(KERN_DEBUG "%s (WE) : Found private handler for 0x%04X\n", ifr->ifr_name, cmd);
	if (descr == NULL)
	{
		DBG_PRINT(KERN_DEBUG "%s (WE) : Name %s, set %X, get %X\n", dev->name, descr->name, descr->set_args, descr->get_args);
		return -EINVAL;
	}
	/* Compute the size of the set/get arguments */
	if (descr != NULL)
	{
		if (IW_IS_SET(cmd))
		{
			int offset = 0;	/* For sub-ioctls */
			/* Check for sub-ioctl handler */
			if (descr->name[0] == '\0')
				/* Reserve one int for sub-ioctl index */
				offset = sizeof(__u32);

			/* Size of set arguments */
			ioreq->extra_size = get_priv_size(descr->set_args);

			DBG_PRINT("%s:%d extrasize:%d\n", __func__, __LINE__, ioreq->extra_size);
			/* Does it fits in iwr ? */
			if ((descr->set_args & IW_PRIV_SIZE_FIXED) &&
				((ioreq->extra_size + offset) <= IFNAMSIZ))
				ioreq->extra_size = 0;
		}
		else
		{
			/* Size of set arguments */
			ioreq->extra_size = get_priv_size(descr->get_args);
			DBG_PRINT("%s:%d extrasize:%d\n", __func__, __LINE__, ioreq->extra_size);

			/* Does it fits in iwr ? */
			if ((descr->get_args & IW_PRIV_SIZE_FIXED) &&
				(ioreq->extra_size <= IFNAMSIZ))
				ioreq->extra_size = 0;
		}
	}
	/* Check if we have a pointer to user space data or not. */
	if (ioreq->extra_size == 0)
	{
		/* No extra arguments. Trivial to handle */
		ioreq->extra = NULL;
		ioreq->extra_size = 0;

		if (!memcpy(&(ioreq->ifr), ifr, sizeof(struct ifreq)))
		{
			//prochao+
			kfree(ioreq);
			//prochao-
			return -EFAULT;
		}

		dma_cache_wback_inv((unsigned long)&(ioreq->ifr), sizeof(struct ifreq));

		/* ret = handler(dev, &info, &(iwr->u), (char *)&(iwr->u))*/
		/* Prepare the call */
		// Add by ygwei for debug
		ret = ioctl_send(ioreq);
		DBG_PRINT("%s: %d, ret=%d\n", __func__, __LINE__, ret);

		/* Add by ygwei for debug */
		if (ret == 0)
		{
			memcpy(ifr, &(ioreq->ifr), sizeof(struct ifreq));
		}

	}
	else
	{
		int err;

		/* Check what user space is giving us */
		if (IW_IS_SET(cmd))
		{
			/* Check NULL pointer */
			if ((iwr->u.data.pointer == NULL) &&
				(iwr->u.data.length != 0))
			{
				//prochao+
				kfree(ioreq);
				//prochao-
				return -EFAULT;
			}
			/* Does it fits within bounds ? */
			if (iwr->u.data.length > (descr->set_args &
									  IW_PRIV_SIZE_MASK))
			{
				//prochao+
				kfree(ioreq);
				//prochao-
				return -E2BIG;
			}
		}
		else
		{
			/* Check NULL pointer */
			if (iwr->u.data.pointer == NULL)
			{
				//prochao+
				kfree(ioreq);
				//prochao-
				return -EFAULT;
			}
		}

		DBG_PRINT(KERN_DEBUG "%s (WE) : Malloc %d bytes\n", dev->name, ioreq->extra_size);
		/* Always allocate for max space. Easier, and won't last
		 * long... */
		ioreq->extra = kmalloc(ioreq->extra_size, GFP_KERNEL);
		if (ioreq->extra == NULL)
		{
			//prochao+
			kfree(ioreq);
			//prochao-
			return -ENOMEM;
		}
		/* If it is a SET, get all the extra data in here */
		if (IW_IS_SET(cmd) && (iwr->u.data.length != 0))
		{
			err = copy_from_user(ioreq->extra, iwr->u.data.pointer,
								 ioreq->extra_size);

			if (err)
			{
				kfree(ioreq->extra);
				kfree(ioreq);
				DBG_PRINT("%s:%d cmd:%X\n", __func__, __LINE__, cmd);
				return -EFAULT;
			}
			DBG_PRINT(KERN_DEBUG "%s (WE) : Got %d elem\n", dev->name, iwr->u.data.length);
		}

		if (!memcpy(&(ioreq->ifr), ifr, sizeof(struct ifreq)))
		{
			//prochao+
			kfree(ioreq->extra);
			kfree(ioreq);
			//prochao-
			return -EFAULT;
		}

		dma_cache_wback_inv((unsigned long)&(ioreq->ifr), sizeof(struct ifreq));
		if (ioreq->extra_size > 0)
			dma_cache_wback_inv((unsigned long)ioreq->extra, ioreq->extra_size);
		/* ret = handler(dev, &info, &(iwr->u), extra) */
		/* Prepare the call */
		ret = ioctl_send(ioreq);

		/* If we have something to return to the user */
		if (!ret && IW_IS_GET(cmd)) {
			struct iwreq *wrq = (struct iwreq *) ifr; // add by ygwei for debug cache 12/07

			// add by ygwei for cache debug 12/07
			dma_cache_inv((unsigned long)ioreq->extra, ioreq->extra_size);

			//dump_data(ioreq->extra, ioreq->extra_size);

			DBG_PRINT("%s: %d, wrq->u.data.length: %d\n", __func__, __LINE__, wrq->u.data.length);

			// add by ygwei for cache debug 12/07
			wrq->u.data.length = ioreq->extra_size;

			err = copy_to_user(iwr->u.data.pointer, ioreq->extra, ioreq->extra_size);
			if (err)
			{
				//prochao+
				kfree(ioreq->extra);
				kfree(ioreq);
				//prochao-
				ret =  -EFAULT;         
			}
		}
		/* Cleanup - I told you it wasn't that long ;-) */
		kfree(ioreq);
		kfree(ioreq->extra);
	}
	return ret;
}

static int 
vnet_ioctl_standard_call(struct net_device *dev,struct ifreq *ifr, int cmd)
{
	const struct iw_ioctl_description * descr;
	//struct iwreq *			iwr = (struct iwreq *)ifr;
	int         user_size = 0;
	struct iwreq *  iwr = (struct iwreq *)ifr;
	struct ioctl_req *ioreq = NULL;
	int		ret = 0;// -EINVAL;

	ioreq = (struct ioctl_req *)kmalloc(sizeof(struct ioctl_req), GFP_KERNEL);
	if (ioreq == NULL)
		return -ENOMEM;

	ioreq->extra = NULL;    
	descr = &(standard_ioctl[cmd - SIOCIWFIRST]);

	ioreq->cmd = cmd;   
	sprintf(ioreq->ifname, "%s", ifr->ifr_name);
	sprintf(ioreq->ifr.ifr_name, "%s", ifr->ifr_name);
	DBG_PRINT("%s (WE) : Found standard handler for 0x%04X\n", ifr->ifr_name, cmd);
	DBG_PRINT("%s (WE) : Header type : %d, Token type : %d, size : %d, token : %d\n",
			dev->name, descr->header_type, descr->token_type, descr->token_size, descr->max_tokens);
	/* Check if we have a pointer to user space data or not */
	if (descr->header_type != IW_HEADER_TYPE_POINT)
	{

		/* No extra arguments. Trivial to handle */
		// add by ygwei for debug 12/07
		DBG_PRINT("%s: NOT IW_HEADER_TYPE_POINT\n", __func__);
		ioreq->extra = NULL;
		ioreq->extra_size = 0;
		/*determine user_size*/
		switch (descr->header_type)
		{
			case IW_HEADER_TYPE_NULL:
				user_size = 0;
				break;
			case IW_HEADER_TYPE_CHAR:
				user_size = IFNAMSIZ;
				break;
			case IW_HEADER_TYPE_UINT:
				user_size = sizeof(__u32);
				break;
			case IW_HEADER_TYPE_FREQ:
				user_size = sizeof(struct iw_freq);
				break;
			case IW_HEADER_TYPE_ADDR:
				user_size = sizeof(struct sockaddr);
				break;
			case IW_HEADER_TYPE_PARAM:
				user_size = sizeof(struct iw_param);
				break;
			case IW_HEADER_TYPE_QUAL:
				user_size = sizeof(struct iw_quality);
				break;
				/*warning:default: deprecated use of label at end of compound statement */
		}

		/* ifreq is already in kernel, see net-core-dev.c */
		if (!memcpy(&(ioreq->ifr), ifr, sizeof(struct ifreq)))
		{
			DBG_PRINT("%s:%d copy_from_user fail!\n", __func__, __LINE__);
		} //for iwr->u		
		dma_cache_wback_inv((unsigned long)&(ioreq->ifr), sizeof(struct ifreq));
		if (ioreq->extra_size > 0)
			dma_cache_wback_inv((unsigned long)ioreq->extra, ioreq->extra_size);

		/* Prepare the call */
		//ret = handler(dev, &info, &(iwr->u), NULL);
		ret = ioctl_send(ioreq);

			// Add by ygwei for debug 12/07
			if (ret == 0) {
				memcpy(ifr, &(ioreq->ifr), sizeof(struct ifreq));
			}

		//prochao+
		kfree(ioreq);
		//prochao-
		return 0;
	}
	else
	{
		int err;
		if (IW_IS_SET(cmd))
		{
			/* Check NULL pointer */
			if ((iwr->u.data.pointer == NULL) &&
				(iwr->u.data.length != 0))
			{
				//prochao+
				kfree(ioreq);
				//prochao-
				return -EFAULT;
			}
			/* Check if number of token fits within bounds */
			if (iwr->u.data.length > descr->max_tokens)
			{
				//prochao+
				kfree(ioreq);
				//prochao-
				return -E2BIG;
			}
			if (iwr->u.data.length < descr->min_tokens)
			{
				//prochao+
				kfree(ioreq);
				//prochao-
				return -EINVAL;
			}
		}
		else
		{
	// add by ygwei for debug 12/07
	DBG_PRINT("%s: IS_GET_CMD\n", __func__);

			/* Check NULL pointer */
			if (iwr->u.data.pointer == NULL)
			{
				//prochao+
				kfree(ioreq);
				//prochao-
				return -EFAULT;
			}
			/* Save user space buffer size for checking */
			user_size = iwr->u.data.length;
		}

		/* Always allocate for max space. Easier, and won't last
		 * long... */

		ioreq->extra = kmalloc(descr->max_tokens * descr->token_size, GFP_KERNEL);
		ioreq->extra_size = descr->max_tokens * descr->token_size;
		if (ioreq->extra == NULL)
		{
			//prochao+
			kfree(ioreq);
			//prochao-
			return -ENOMEM;
		}

		/* If it is a SET, get all the extra data in here */
		if (IW_IS_SET(cmd) && (iwr->u.data.length != 0))
		{
			err = copy_from_user(ioreq->extra, iwr->u.data.pointer, iwr->u.data.length * descr->token_size);
			if (err)
			{
				kfree(ioreq->extra);
				//prochao+
				kfree(ioreq);
				//prochao-
				return -EFAULT;
			}
		}
		/* wrap extra to ioreq packet, and call the handler */
		if (!memcpy(&(ioreq->ifr), ifr, sizeof(struct ifreq)))
		{
			DBG_PRINT("%s:%d memcpy fail!\n", __func__, __LINE__);
			return -EFAULT;
		} //for iwr->u		
		dma_cache_wback_inv((unsigned long)&(ioreq->ifr), sizeof(struct iw_freq));
		if (ioreq->extra_size > 0)
			dma_cache_wback_inv((unsigned long)ioreq->extra, ioreq->extra_size);
		/* Prepare the call */
		ret = ioctl_send(ioreq);

	// add by ygwei for debug 12/07
	DBG_PRINT("%s: %d: ret: 0x%08x\n", __func__, __LINE__, ret);

		/* if we have something to return to the user */    
		if (!ret && IW_IS_GET(cmd))
		{
			int wb_size;
			if (user_size < iwr->u.data.length)
			{
				kfree(ioreq->extra);
				//prochao+
				kfree(ioreq);
				//prochao-
				return -E2BIG;
			}

			// Add by ygwei for debug cache
			dma_cache_inv((unsigned long)ioreq->extra, iwr->u.data.length * descr->token_size);
			// Add by ygwei for debug cache
			/* First invalidate DMA cache */
			dma_cache_inv((unsigned long)&(ioreq->ifr), sizeof(struct ifreq));

			wb_size = iwr->u.data.length * descr->token_size;

			// Add by ygwei for write back debug
			memcpy(ifr, (void *)&(ioreq->ifr), sizeof(struct ifreq));

			err = copy_to_user(iwr->u.data.pointer, ioreq->extra, wb_size);
			if (err)
			{
				//prochao+
				kfree(ioreq->extra);
				kfree(ioreq);
				//prochao-
				ret =  -EFAULT;                
			}
		}
		/* Cleanup - I told you it wasn't that long ;-) */
		kfree(ioreq->extra);
		//prochao+
		kfree(ioreq);
		//prochao-
	}

	return 0;
}

static int 
vnet_ieee80211_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct ioctl_req *ioreq = NULL;
	int ret = 0;//-EINVAL;
	struct iwreq *iwr = (struct iwreq *)ifr;
//prochao+, 13-03-2007, to support the statistics
	struct iw_statistics	*iwstats;
	struct net_device_stats	*stats;
//prochao-

	ioreq = (struct ioctl_req *)kmalloc(sizeof(struct ioctl_req), GFP_KERNEL);

	//prochao+
	if (ioreq == NULL)
		return -ENOMEM;
	//prochao-
	ioreq->extra = NULL;    
	ioreq->extra_size = 0;
	ioreq->cmd = cmd;
	sprintf(ioreq->ifname, "%s", ifr->ifr_name);
	sprintf(ioreq->ifr.ifr_name, "%s", ifr->ifr_name);
	/* iwconfig, iwpriv without arguments   	*/
	/* standard iw_handler 	0x8B01 - 0x8B2D		*/  
	/* wireless.c: ioctl_standard_call 			*/
	/* ===================ioct_export_private 	*/
	switch (cmd)
	{
		/*************************************************/
		case SIOCSIFMTU:
			DBG_PRINT("%s:%d mtu:%d\n", __func__, __LINE__, ioreq->ifr.ifr_mtu);
			/* MTU must be positive */
			if (ifr->ifr_mtu < 0)
				return -EINVAL;
			if (!netif_device_present(dev))
				return -ENODEV;
			if (copy_from_user(&(ioreq->ifr),ifr, sizeof(struct ifreq)))
			{
				DBG_PRINT("%s:%d copy_from_user\n", __func__, __LINE__);
				return -EFAULT;
			}
			DBG_PRINT("%s:%d mtu:%d\n", __func__, __LINE__, ioreq->ifr.ifr_mtu);
			dma_cache_wback_inv((unsigned long)&(ioreq->ifr), sizeof(struct ifreq));
			dma_cache_wback_inv((unsigned long)&(ioreq->ifr.ifr_mtu), sizeof(int));
			ret = ioctl_send(ioreq);
			return ret;
		case SIOCGIWSTATS:
//prochao+, 13-03-2007, to support the statistics
#if	1
//			struct iw_statistics	*iwstats;

			iwstats = vnet_get_wireless_stats( dev);
			if (iwstats!= NULL)
			{
				if (copy_to_user(ifr->ifr_data, iwstats, sizeof(struct iw_statistics)))
					ret = -EFAULT;
				else
					ret = 0;
			}
#else	//prochao%
			ioreq->extra_size = 0;
			ret = ioctl_send(ioreq);
			/* copy_to_user(stats) */
#endif	//prochao-
			kfree(ioreq);
			return ret;
//prochao+, 13-03-2007, to support the statistics
		case SIOCGATHSTATS:
//			struct net_device_stats	*stats;

			stats = vnet_get_stats( dev);
			if (stats!= NULL)
			{
				if (copy_to_user(ifr->ifr_data, stats, sizeof(struct net_device_stats)))
					ret = -EFAULT;
				else
					ret = 0;
			}
			return ret;
//prochao-
		case SIOC80211IFDESTROY:
			sprintf(ioreq->ifname, "%s", dev->name);
			ioreq->extra_size = 0;
			ret = ioctl_send(ioreq);
#if 0 //ernest 2006-12-08 debug
			atomic_set(&dev->refcnt, 0);
			DBG_PRINT("%s:%d waiting:%s, refcnt:%d\n", __func__, __LINE__, dev->name, atomic_read(&dev->refcnt));
			//unregister_netdev(dev);//destroy vap on cpu0
			unregister_netdevice(dev);//destroy vap on cpu0
			DBG_PRINT("%s:%d\n", __func__, __LINE__);
			free_netdev(dev);
			DBG_PRINT("%s:%d\n", __func__, __LINE__);
#endif
			//prochao+
			kfree(ioreq);
			//prochao-
			return ret;
//		case SIOCG80211STATS:
//		case IEEE80211_IOCTL_STA_STATS:
//		case IEEE80211_IOCTL_GETKEY:
		case IEEE80211_IOCTL_STA_INFO:
			/* keep user space address for return data */
			user_ret_data.u.data.pointer = iwr->u.data.pointer;
			user_ret_data.u.data.length = iwr->u.data.length;
			DBG_PRINT("stainfo %s:%d: dev:%s cmd:%X, len:%d\n", __func__, __LINE__, dev->name, cmd, iwr->u.data.length);
			ioreq->extra = kmalloc(iwr->u.data.length, GFP_KERNEL);
			ioreq->extra_size = iwr->u.data.length;
			if (copy_from_user(ioreq->extra, iwr->u.data.pointer, iwr->u.data.length))
			{
				DBG_PRINT("stainfo %s:%d copy_from_user\n", __func__, __LINE__);
				kfree(ioreq->extra);
				kfree(ioreq);
				return -EFAULT;
			}
			dma_cache_wback_inv((unsigned long)&(ioreq->ifr), sizeof(struct ifreq));
			dma_cache_wback_inv((unsigned long)ioreq->extra, ioreq->extra_size);
			DBG_PRINT("stainfo %s:%d: dev:%s cmd:%X, len:%d\n", __func__, __LINE__, dev->name, cmd, iwr->u.data.length);
			/* Prepare the call */
			ret = ioctl_send(ioreq);
			iwr->u.data.length = ioreq->extra_size; //ernest 2007-02-27 debug
			kfree(ioreq->extra);
			kfree(ioreq);
			return 0;//ret;
		case SIOCG80211STATS:
		case IEEE80211_IOCTL_STA_STATS:
		case IEEE80211_IOCTL_GETKEY:
		case IEEE80211_IOCTL_SCAN_RESULTS:
			DBG_PRINT("%s:%d: dev:%s cmd:%X\n", __func__, __LINE__, dev->name, cmd);
			return 0;
		case IEEE80211_IOCTL_GETWPAIE:
			/* keep user space address for return data */
			user_ret_data.u.data.pointer = iwr->u.data.pointer;
			user_ret_data.u.data.length = iwr->u.data.length;
			DBG_PRINT("%s:%d: dev:%s cmd:%X, user dataptr:%p\n", __func__, __LINE__, dev->name, cmd, user_ret_data.u.data.pointer);
			ioreq->extra = kmalloc(iwr->u.data.length, GFP_KERNEL);
			ioreq->extra_size = iwr->u.data.length;
			if (copy_from_user(ioreq->extra, iwr->u.data.pointer, iwr->u.data.length))
			{
				DBG_PRINT("%s:%d copy_from_user\n", __func__, __LINE__);
				kfree(ioreq->extra);
				kfree(ioreq);
				return -EFAULT;
			}
			dma_cache_wback_inv((unsigned long)&(ioreq->ifr), sizeof(struct ifreq));
			dma_cache_wback_inv((unsigned long)ioreq->extra, ioreq->extra_size);
			/* Prepare the call */
			ret = ioctl_send(ioreq);

			kfree(ioreq->extra);
			kfree(ioreq);
			return 0;//ret;
		case SIOCGIWPRIV:
			{
				struct iwreq *      iwr = (struct iwreq *) ifr;
				const struct iw_priv_args *priv_descr = NULL;
				//	descr = &(standard_ioctl[cmd - SIOCIWFIRST]);
				int num_private_args = sizeof(ieee80211_priv_args) / sizeof(ieee80211_priv_args[0]);
				priv_descr = &(ieee80211_priv_args[cmd - SIOCIWFIRSTPRIV]);
				/* Check if the driver has something to export */
				if ((num_private_args == 0) || (priv_descr == NULL))
				{
					kfree(ioreq);
					return -EOPNOTSUPP;
				}

				/* Check NULL pointer */
				if (iwr->u.data.pointer == NULL)
				{
					kfree(ioreq);
					return -EFAULT;
				}
#ifdef WE_STRICT_WRITE
				/* Check if there is enough buffer up there */
				if (iwr->u.data.length < num_private_args)
				{
					DBG_PRINT(KERN_ERR "%s (WE) : Buffer for request SIOCGIWPRIV too small (%d<%d)\n",
							dev->name, iwr->u.data.length, num_private_args);
					kfree(ioreq);
					return -E2BIG;
				}
#endif	/* WE_STRICT_WRITE */

				/* Set the number of available ioctls. */
				iwr->u.data.length = num_private_args;
				/* Copy structure to the user buffer. */
				if (copy_to_user(iwr->u.data.pointer, ieee80211_priv_args, sizeof(struct iw_priv_args) * iwr->u.data.length))
				{
					DBG_PRINT("%s:%d copy_to_user :%p:%p\n", __func__, __LINE__, iwr->u.data.pointer, priv_descr);
					kfree(ioreq);
					return -EFAULT;
				}
				kfree(ioreq);
				return 0;
				//return ioctl_export_private()
			}
		default:
			{
				int ret;
				if (ioreq->cmd < SIOCIWFIRSTPRIV)
				{
					ret = vnet_ioctl_standard_call(dev, ifr, cmd);
				}
				else
				{
					ret = vnet_ioctl_private_call(dev, ifr, cmd);
				}
				kfree(ioreq);
				return ret;
			}
	} //end of switch
	kfree(ioreq);
	//prochao-

	return ret;
}

static int vnet_change_mtu(struct net_device *dev, int new_mtu)
{   
	struct ioctl_req *ioreq = NULL;
	int ret  = 0;

	ioreq = (struct ioctl_req *)kmalloc(sizeof(struct ioctl_req), GFP_KERNEL);
	sprintf(ioreq->ifname, "%s", dev->name);
	ioreq->cmd = SIOCSIFMTU;
	ioreq->ifr.ifr_mtu = new_mtu;

	ret = ioctl_send(ioreq);
	kfree(ioreq);
	return ret;
}


static int vnet_init(void)
{
	struct net_device *dev;

	init_waitqueue_head(&my_waitqueue);
	dev = alloc_netdev(sizeof(struct net_device), "wifi%d", ether_setup);
	if (!dev)
	{
		DBG_PRINT("%s:%d no memory for device state.\n", __func__, __LINE__);
		return(-ENODEV);
	}
//prochao+, 14-03-2007, to support getting the statistics
#if	1
	dev->get_stats = vnet_get_stats;
//	dev->get_wireless_stats = vnet_get_wireless_stats;
#else
	dev->get_stats = NULL;
#endif	//prochao-
	dev->open = vnet_open;
	dev->stop = NULL;
	dev->hard_start_xmit = vnet_tx;
	dev->do_ioctl = vnet_ioctl;
	dev->poll = NULL;
	dev->weight = 64;
	dev->hard_header_len = ETH_HLEN;
	if (register_netdev(dev))
	{
		DBG_PRINT(KERN_ERR "%s: unable to register device\n", dev->name);
		return(-ENODEV);
	}
	g_mps_cpu0_netdev = dev;
	return 0;
}

static int 
mps_init(void)
{
    printk("number of MPS entries %d\n", MPS_ENTRY_NUM);
	if (ifx_mps_buff_pool_init(MPS_SRV_DS_DATA_ID, 1, MPS_ENTRY_NUM) != IFX_OK)
	{
		DBG_PRINT("%s:%d pool init failed\n", __func__, __LINE__);
		return -1;
	}
	if (ifx_mps_queue_init(MPS_SRV_DS_DATA_ID, MPS_QUEUE_SIZE, MPS_ENTRY_NUM, 0) != IFX_OK)
	{
		DBG_PRINT("%s:%d queue init failed\n", __func__, __LINE__);
		/* XXX destroy pool */
		return -1;
	}
/* XXX: Need to check which seq num really needs to be init and allocated ?? */
	if (ifx_mps_buff_pool_init(MPS_SRV_US_DATA_ID, 1, MPS_ENTRY_NUM) != IFX_OK)
	{
		DBG_PRINT("%s:%d pool init failed\n", __func__, __LINE__);
		return -1;
	}
	if (ifx_mps_queue_init(MPS_SRV_US_DATA_ID, MPS_QUEUE_SIZE, MPS_ENTRY_NUM, 0)
		!= IFX_OK)
	{
		DBG_PRINT("%s:%d queue init failed\n", __func__, __LINE__);
		/* XXX destroy pool */
		return -1;
	}
	return 0;
}

static int __init init_cpu0_wifi(void)
{
	int ret;
	if (mps_init())
	{
		DBG_PRINT("%s:%d mps init failed.\n", __func__, __LINE__);
		return -1;
	}
	if (ifx_mps_register_callback(MPS_SRV_US_DATA_ID, (FNPTR_T)us_data_cb) != IFX_OK)
	{
		DBG_PRINT("%s:%d ifx_mps_register_callback failed.\n", __func__, __LINE__);
		return -1;
	}

	if (ifx_mps_register_callback(MPS_SRV_US_MSG_ID, (FNPTR_T)us_message_cb) != IFX_OK)
	{
		DBG_PRINT("%s:%d ifx_mps_register_callback failed.\n", __func__, __LINE__);
		return -1;
	}

#ifdef WIRELESS_EVENT_SUPPORT
	if (ifx_mps_register_callback(MPS_WLAN_EVENT_ID, (FNPTR_T)wlan_event_cb) != IFX_OK)
	{
		DBG_PRINT("%s:%d ifx_mps_register_callback failed.\n", __func__, __LINE__);
		return -1;
	}
#endif

	ret = vnet_init(); /* XXX adapt ioctl from dure_core_wlanconfig */
	return ret;
}
module_init(init_cpu0_wifi);

static void __exit exit_cpu0_wifi(void)
{
	if (ifx_mps_unregister_callback(MPS_SRV_US_DATA_ID) != IFX_OK)
	{
		DBG_PRINT("%s:%d mps_unregister_callback() failed.\n", __func__, __LINE__);
		return;
	}

	if (ifx_mps_unregister_callback(MPS_SRV_US_MSG_ID) != IFX_OK)
	{
		DBG_PRINT("%s:%d mps_unregister_callback() failed.\n", __func__, __LINE__);
		return;
	}
	//Jeffrey ++ unregister wifi0
	unregister_netdev(g_mps_cpu0_netdev);

	return;
}

module_exit(exit_cpu0_wifi);

