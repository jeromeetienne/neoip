/*! \file
    \brief Implementation of raw ip function

*/

/* system include */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <net/ethernet.h>
#include <errno.h>


/* local include */
#include "rawip.h"
#include "util.h"

static uint16_t	global_ipid;	/* ip_id used only if smptu is disabled 
				** MAY collide with kernel one */
/**
 * send a raw ip packet
 */
static int raw_ip_send( void *pkt, int pkt_len )
{
	struct 	iphdr	*iph	= pkt;
	int		sock_fd;
	int		len;
	struct sockaddr_in to;
//DBG("enter pkt_len=%d\n", pkt_len );DBG_DUMP( pkt, pkt_len );
	DBG_ASSERT( pkt_len >= sizeof(struct iphdr) );

	/* open the socket */
	sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if( sock_fd < 0 )	return -1;

	/* fill the destination address - both already in network order */
	memset( &to, 0, sizeof(to) );
	to.sin_family		= AF_INET;
	to.sin_addr.s_addr	= iph->daddr;

	/* send the packet */
	len=sendto(sock_fd, pkt, pkt_len, 0, (struct sockaddr*)&to, sizeof(to));

	/* close the socket */
	close( sock_fd );
	return len;
}

/**
 * send a icmp reply to the sender of the received <pkt> thru a raw socket
 * - TODO see rfc1812.3.3.2 and see if it is ok 
 */
int raw_icmp_reply_send( int type, int code, int data
				, char *pkt, int pkti_len )
{
/* max size of a icmpv4 packet - rfc1812.4.3.2.3 */	
#define ICMP4_MAX_LEN	576
	char	buffer[ICMP4_MAX_LEN];
	struct 	iphdr	*trig_iph	= (struct iphdr *)pkt;
	struct 	icmphdr	*icmph		= (struct icmphdr *)buffer;
	int		hd_len		= sizeof(*icmph);
	int		pkt_len;
	int		fd 	= socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	struct sockaddr_in to;
	/* sanity check */
	DBG_ASSERT( fd >= 0 );
	if( fd < 0 )	return -1;
	
	/* sanity check on the triggering packet */
	if( pkti_len < sizeof(struct iphdr) )
		return -1;

	/* compute the icmp length. no more than ICMP4_MAX_LEN */
	pkt_len = hd_len + pkti_len;
	if( pkt_len > ICMP4_MAX_LEN )
		pkt_len = ICMP4_MAX_LEN;

	/* copy part or all the triggering packet */
	memcpy( buffer+hd_len, pkt, pkt_len - hd_len );

	/* fill the icmp header */
	icmph->type 		= type;
	icmph->code		= code;
	/* TODO here to write in un.gateway may assume stuff.. which one */
	icmph->un.gateway	= htonl(data);
	icmph->checksum		= 0;
	icmph->checksum		= in_csum( icmph, pkt_len, 0 );

	/* fill the destination address - both already in network order */
	memset( &to, 0, sizeof(to) );
	to.sin_family		= AF_INET;
	to.sin_addr.s_addr	= trig_iph->saddr;
	/* send the packet */
	sendto(fd, (char *)icmph,pkt_len, 0, (struct sockaddr*)&to, sizeof(to));
	/* close the socket */
	close(fd);

	return 0; 
}


/**
 * send a udp packet 
 * - see rfc0768
 */
int raw_udp_send( otsp_addr_t *local_addr, otsp_addr_t *remote_addr, char *pkt, int pkt_len)
{
	char		pkto[YAV_STACK_MTU];
	struct iphdr	*iph		= (struct iphdr *)pkto;
	int		ippayl_len	= pkt_len + sizeof(struct udphdr);
	struct udphdr	*udph;
	int		hd_len;
	int		pkto_len;

	// build ip header
	iph->version	= 4;				/* version */
	iph->ihl	= 5;				/* header lenght */
	iph->tos	= 0;				/* type of service */
	iph->tot_len	= htons(ippayl_len+iph->ihl*4);	/* total length */
	iph->frag_off	= 0;				/* fragment offset field */
	iph->id		= global_ipid++;		/* IP identificator */
	iph->ttl	= IPDEFTTL;
	iph->protocol	= IPPROTO_UDP;
	iph->saddr	= htonl( local_addr->addr );
	iph->daddr	= htonl( remote_addr->addr );
	iph->check	= 0;
		
	// update pointer
	udph		= (struct udphdr *)(pkto + iph->ihl*4);
	hd_len		= iph->ihl*4 + sizeof(struct udphdr);
	pkto_len	= pkt_len + hd_len;

	/* build the UDP header */
	udph->source	= htons( local_addr->port );
	udph->dest	= htons( remote_addr->port );
	udph->len	= htons( ntohs(iph->tot_len) - iph->ihl*4 );
	udph->check	= 0; // TODO bug here the udp checksum isnt computed

	// compute ip header
	iph->check	= in_csum( iph, iph->ihl*4, 0 );

	/* copy the payload */
	memcpy( pkto + hd_len, pkt, pkt_len );
	
	// actually send the packet thru RAW socket
	return raw_ip_send( pkto, pkto_len );
}

