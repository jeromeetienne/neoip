/*===========================[ (c) JME SOFT ]===================================
FILE        : [vdev.c]
CREATED     : 01/03/16 13:08:03		LAST SAVE    : 02/02/05 18:33:43
WHO         : jerome@mycpu Linux 2.2.14
REMARK      :
================================================================================

==============================================================================*/

#include <linux/version.h>
#if LINUX_VERSION_CODE > 0x20400
#	define USE_GENERIC_TUN
#endif

/* system include */
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
//#include <linux/netlink.h>
#include <linux/sockios.h>
#include <linux/if_packet.h>
#ifdef USE_GENERIC_TUN
#	include <linux/if_tun.h>
#endif


/* local include */
#include "vdev.h"
#include "util.h"
#include "rawip.h"
#include "stun_pkt.h"
#include "netif.h"

#if IFNAMSIZ_LAME != IFNAMSIZ
#	error IFNAMSIZ_LAME isnt the proper value
#endif

struct ethertap_hd {
	uint16_t	info;
	uint8_t		ether_dhost[ETH_ALEN];	/* destination eth addr */
    	uint8_t		ether_shost[ETH_ALEN];	/* source ether addr    */
     	uint16_t	proto;			/* packet type ID field */
};

/****************************************************************
 NAME	: vdev_prepare				01/03/16 13:21:29
 AIM	:
 REMARK	:
****************************************************************/
static gboolean vdev_prepare(GSource *source,gint *timeout)
{
//DBG("enter\n");
	*timeout = -1;
	return FALSE;
}

/****************************************************************
 NAME	: vdev_check				01/03/16 13:22:18
 AIM	:
 REMARK	:
****************************************************************/
static gboolean vdev_check( GSource *source )
{
	GSList *item;
	for (item = source->poll_fds; item != NULL; item = item->next) {
		GPollFD *gfd = (GPollFD *)item->data;
		if(gfd->revents != 0)
			return TRUE;
	}
	return FALSE;
}

/****************************************************************
 NAME	: vdev_strip_header			01/12/11 21:22:07
 AIM	: 
 REMARK	:
****************************************************************/
static int vdev_strip_header( char *pkt, int pkt_len, int *proto )
{
#ifdef USE_GENERIC_TUN
	struct tun_pi	*hd = (void *)(pkt);
#else
	struct ethertap_hd	*hd = (void *)(pkt);
#endif
	if( pkt_len < sizeof(*hd) )	return -1;
	*proto = ntohs(hd->proto);
	return sizeof( *hd );
}

/****************************************************************
 NAME	: vdev_check				01/03/16 13:22:18
 AIM	:
 REMARK	:
****************************************************************/
static gboolean vdev_dispatch(GSource *source, GSourceFunc callback,
			gpointer user_data)
{
	char		buf[YAV_STACK_MTU], *pkt = buf;
	vdev_t 		*vdev 	= user_data;
	int		pkt_len, hd_len, ethertype;
//	stun_t		*stun;
DBG("enter--------------------------------------------\n");
	pkt_len = read( vdev->gfd.fd, buf, sizeof(buf) );
	/* strip the header */
	hd_len = vdev_strip_header( pkt, pkt_len, &ethertype );
	if( hd_len < 0 )	return TRUE;
DBG("ethertype=0x%x\n",ethertype);
	/* update pointer/length to strip vdev_header */
	pkt	+= hd_len;
	pkt_len -= hd_len;

DBG_DUMP( pkt, pkt_len );
	if( vdev->pkt_in_callback )
		vdev->pkt_in_callback( vdev->user_ptr, ethertype, pkt, pkt_len );
	if( vdev->pkt_in_callback == NULL )
		DBG("received packet in vdev with no callback to call\n");

	return TRUE;
}

static GSourceFuncs vdev_srcfuncs = {
	vdev_prepare,
	vdev_check,
	vdev_dispatch,
	NULL
};

/****************************************************************
 NAME	: vdev_updown				01/11/08 20:15:12
 AIM	:
 REMARK	:
****************************************************************/
static int vdev_updown( vdev_t *vdev, int upF )
{
	int		fd = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq	ifr;
	/* get the IFFFLAGS */
	memset(&ifr, 0, sizeof(ifr));
	memcpy(ifr.ifr_name, vdev->dev_name, IFNAMSIZ);
	if(ioctl( fd, SIOCGIFFLAGS, &ifr) < 0)	goto err;

	/* modify the IFFFLAGS */
	if( upF )	ifr.ifr_flags |= IFF_UP|IFF_RUNNING;
	else		ifr.ifr_flags &= ~(IFF_UP|IFF_RUNNING);
	/* set the flags */
	if(ioctl( fd, SIOCSIFFLAGS, &ifr) < 0)	goto err;
	/* close the socket */
	close(fd);	return(0);
err:;	close(fd);	return -1;
}

#ifdef USE_GENERIC_TUN
/****************************************************************
 NAME	: vdev_alloc_tun			01/12/11 17:41:10
 AIM	:
 REMARK	:
****************************************************************/
static int vdev_alloc( char *dev_name )
{
	struct	ifreq	ifr;
	int 		fd, err;
	char		*netif_name;
	/* open the device */
	fd	= open("/dev/net/tun", O_RDWR);
	if( fd < 0 )	return -1;
	/* allocate it */	
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN;
//	ifr.ifr_flags = IFF_TAP;
	if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
		perror("ioctl(SIOCSIFNAME)");
		close(fd);
		return err;
	}
#if 0	// TODO i dont know why it doesnt work... 
	//      - maybe i dont use it properly
	//      - maybe not supported by the driver
	// to rename the interface
	netif_name = "yav";
	strncpy(ifr.ifr_newname,netif_name,sizeof(ifr.ifr_newname));
	if (ioctl(fd, SIOCSIFNAME, &ifr)==-1) {
		perror("ioctl(SIOCSIFNAME)");
		return 1;
	}
#else
	netif_name = ifr.ifr_name;
#endif
	
	/* if all succeed, copy the name */
	strcpy( dev_name, netif_name );
	return fd;
}
#else
/****************************************************************
 NAME	: vdev_alloc_ethertap			01/11/08 19:39:27
 AIM	:
 REMARK	:
****************************************************************/
static int vdev_alloc( char *dev_name )
{
	char tapname[PATH_MAX];
	int i, fd;

	for (i=0; i < 255; i++) {
		snprintf(tapname, sizeof(tapname), "/dev/tap%d", i);
		/* Open device */
		if ( (fd=open(tapname, O_RDWR)) >= 0 ) {
			sprintf( dev_name, "tap%d", i );
DBG("i=%d fd=%d\n", i, fd );
			return fd;
		}
	}
	return -1;
}
#endif /* USE_GENERIC_TUN */

/****************************************************************
 NAME	: vdev_open				01/03/16 13:08:14
 AIM	:
 REMARK	:
****************************************************************/
int vdev_open( vdev_t *vdev )
{
	int	fd;
DBG("enter\n");

	/* allocate the device */
	fd = vdev_alloc( vdev->dev_name );
	if( fd < 0 ){
		LOGM_ERR( "can't allocate a virtual device\n" );
		DBG_ASSERT( fd != -1 );
		return -1;
	}
DBG("%s allocated MTU=%d.\n", vdev->dev_name, netif_get_mtu( vdev->dev_name ));

	if( vdev_updown( vdev, 1 ) ){
		LOGM_EMERG("can't UP a virtual device (%s)\n", vdev->dev_name);
		/* WORK: close the init stuff */
		close( fd );
		return -1;
	}

	/* init the struct */
	vdev->gfd.fd	= fd;
	vdev->gfd.events= G_IO_IN | G_IO_HUP | G_IO_ERR;

	/* add it to the source poll */
   	if( (vdev->gsource = g_source_new(&vdev_srcfuncs, sizeof(GSource))) == NULL ){
		LOGM_ERR("couldn't allocate source\n");
        	goto error;
	}
	g_source_add_poll(vdev->gsource, &vdev->gfd);
	g_source_set_priority(vdev->gsource, G_PRIORITY_DEFAULT);
	g_source_set_callback(vdev->gsource, NULL, vdev, NULL);
	g_source_attach(vdev->gsource, NULL);
    
DBG("exit\n");
	return(0);

error:;	close(fd);
	return -1;
}

/****************************************************************
 NAME	: vdev_close				01/04/01 22:35:33
 AIM	:
 REMARK	:
****************************************************************/
void vdev_close( vdev_t *vdev )
{
	vdev_updown( vdev, 0 );
	// close glib stuff
        g_source_remove_poll(vdev->gsource, &vdev->gfd);
        g_source_destroy(vdev->gsource);
        // close fd
        close(vdev->gfd.fd);
}

/****************************************************************
 NAME	: vdev_register_callback
 AIM	:
 REMARK	:
****************************************************************/
void vdev_register_callback( vdev_t *vdev, void (*pkt_in_callback)( void *userptr, int ethertype, char *buf, int buf_len ) )
{
	DBG_ASSERT( vdev->pkt_in_callback == NULL );
	vdev->pkt_in_callback = pkt_in_callback;
}

#ifdef USE_GENERIC_TUN
/****************************************************************
 NAME	: vdev_xmit				01/03/17 16:40:52
 AIM	: send a raw packet directly to the device (tun version)
 REMARK	:
****************************************************************/
int vdev_xmit( vdev_t *vdev, char *pkt, int pkt_len, int ethertype )
{
	char	buf[YAV_STACK_MTU];
	struct	tun_pi *tun_hd	= (struct tun_pi *)buf;
	int	buf_len		= pkt_len + sizeof(*tun_hd);

DBG_ASSERT(ethertype != ETHERTYPE_YAV_SIG );

	/* build the tunnel header */
	tun_hd->flags = 0;
	tun_hd->proto = htons(ethertype);

	/* copy the packet */
	memcpy( buf+sizeof(*tun_hd), pkt, pkt_len );
DBG("%s:bufLen=%d\n", vdev->dev_name, buf_len);
DBG_DUMP( buf, buf_len );

	/* send it */
	return write(vdev->gfd.fd, buf, buf_len );
}

#else /* USE_GENERIC_TUN */

/****************************************************************
 NAME	: vdev_xmit				01/03/17 16:40:52
 AIM	: send a raw packet directly to the device (ethertap version)
 REMARK	:
****************************************************************/
int vdev_xmit( vdev_t *vdev, char *pkt, int pkt_len, int ethertype )
{
	char	buf[YAV_STACK_MTU];
	struct 	ethertap_hd	*eth	= (void *)(buf);
	int	buf_len			= pkt_len + sizeof(*eth);
	/* build the fake ethernet header */
	memcpy( eth->ether_dhost, "\xfe\xfd\x00\x00\x00\x00",
			sizeof(eth->ether_dhost) );
	memset(eth->ether_shost, 0, sizeof(eth->ether_shost));
	eth->proto = htons(ethertype);

	/* copy the packet */
	memcpy( buf+sizeof(*eth), pkt, pkt_len );

	/* send it */
	return write(vdev->gfd.fd, buf, buf_len );
}
#endif /* USE_GENERIC_TUN */

