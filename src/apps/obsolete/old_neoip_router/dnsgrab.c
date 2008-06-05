/*===========================[ (c) JME SOFT ]===================================
FILE        : [onet.c]
CREATED     : 01/03/27 00:48:02		LAST SAVE    : 01/11/27 22:43:39
WHO         : jerome@mycpu Linux 2.2.14
REMARK      :
================================================================================
- TODO
- write a dnsgrabd
  o a standalone deamon
  o launched by this process at init time
  o close by this process at close time
  o this deamon insert/remove the iptables for dns redirect
    + v1: launch a system("iptables")
    + v2: add/remove the rules by lib call
    + v3: insert the rules in the proper place in the list
  o this deamon gets queued packet
    + it forward it to a localhost udp
    + store it until a reply is received ACCEPT/DROP
- some watchdog to ensure the process is alive and there
- if other people does the queue stuff, this wont scale at all
  o later i could limit this to dns traffic
  o this traffic issue is due to netfilter design and cant really be avoided..
================================================================================
- reimplement the libipq and libiptc in LGPL
  o is the API protected too ?
================================================================================
- talk directly to the kernel from this process without taking any library...
  o this solution seems by far the best...
  o libipq is trivial to code
  o libiptc is a bit harder but i dont really need everything. the key would 
    be to know what i need or not.
================================================================================
possible plans:
---------------
- in order:
  1. implement it in this process with libipq and system("iptables");
  2. reimplement what you need of libipq
  3. reimplement what you need of /sbin/iptables
- thus the feature is available as soon as possible
  o the libipq code under glib is already there
  o just add a system(iptables) to create/remove the rules
- the reimplementation of iptables is important as we need to push the rules
  in the proper place of the list or some firewall may conflict
  o likely a watchdog to test periodically that the rules is still in place.
- apparently only one process can do ipqueue at the same time...
  o HUE??
================================================================================
- do a mod_probe ip_queue
- do a proper intergration with current iptables rules
- do a watchdog 
  o to be sure i dont leave a rules in case of failure
  o to be sure nobody else remove my rules
==============================================================================*/


/* system include */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <glib.h>
#include <linux/netfilter.h>
#include <libipq.h>
#include <arpa/nameser.h>
#include <arpa/nameser_compat.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

/* local include */
#include "dnsgrab.h"
#include "util.h"
#include "otsp_addr.h"
#include "ns_query.h"
#include "ns_util.h"
#include "nipmem.h"
#include "onet.h"
#include "usercfg.h"
#include "rawip.h"
#include "nipid.h"

#if 0
// iptables includes
#include "libiptc/libiptc.h"
#include "iptables.h"
#endif

#define BUFSIZE 2048

typedef struct {
	char 			*name;
	int			inverse_f;
	char 			*pkt;
	int			pkt_len;
	
	nipid_t			record_id;
	ns_rec_type_t		rec_type;
	
	ns_query_req_cb_t	*req_cb;	//!< the request id of this ns_query
} dnsgrab_ns_req_t;


typedef struct {
	GList			*ns_req_list;
	
	struct	ipq_handle	*h_ipq;	//!< ipqueue stuff to intercept dns packet
	// glib stuff to monitor ipqueue handler
	GPollFD			gfd;
	GSource			*gsource;
} dnsgrab_t;

// from rfc1035.4.1.1
#define DNS_QR_QUESTION		0
#define DNS_QR_RESPONSE		1
#define DNS_OPCODE_QUERY	0

// from rfc1035.3.2.2
#define DNS_TYPE_A		1
#define DNS_TYPE_PTR		12
#define DNS_TYPE_AAAA		28	// from rfc3696.2.1
// from rfc1035.3.2.4
#define DNS_CLASS_IN		1

static dnsgrab_t *dnsgrab = NULL;

static void dnsgrab_ns_query_close( dnsgrab_ns_req_t *req );

/**
 * parse a question section
 * - rfc1035.4.1.2
 * - return the question section length, and < 0 if an error occurs
 */
static int dnsgrab_parse_question( char *pPkt, int off, char *qname, int qnameLen
					, uint16_t *pType, uint16_t *pClass)
{
	char 	*p = qname;
	char	*pName = pPkt + off;
	long	i;
	// parse the name itself
	while( (i = *pName) ){
		if( i & 0xc0 ){
			i = ((i & 0x3f) << 8) + *(pName+1);
			pName = pPkt + i;
		}else{
			for( pName++ ; i ; *p++ = *pName++ , i-- );
			if( *pName )	*p++ = '.';
		}
	}
	*p = '\0';
	pName++;
	EXP_ASSERT( strlen(qname) < qnameLen );
	// get the qtype
	*pType = ntohs(*((uint16_t *)pName));
	pName += sizeof(uint16_t);
	// get the qclass
	*pClass = ntohs(*((uint16_t *)pName));
	DBG("type=0x%x class=0x%x\n", *pType, *pClass );
	pName += sizeof(uint16_t);
	return pName - (pPkt + off);
}

/****************************************************************
	FUNCTION: dispQuestSect			96/06/02 15:43:26
	AIM	: display the question section of a domain packet
		: return the lenght of the question section.
	REMARK	:
****************************************************************/
static int dnsgrab_disp_dnsQuest( char *pkt, int off )
{
	uint16_t	type, class;
	char		name[MAXDNAME];
	int		questionLen;
	questionLen = dnsgrab_parse_question( pkt, off, name, sizeof(name), &type, &class );
	if( questionLen < 0 ){
		LOGM_ERR("can't parse the question section");
		return -1;
	}

	DBG( "%s\t%hd\t%hd\tQUERY\n", name, class, type );
	
	return questionLen;
}

/****************************************************************
 NAME	: dnsgrab_disp_dnsmsg
 AIM	:
 REMARK	:
****************************************************************/
static void dnsgrab_disp_dnsmsg( char *dns_msg, int dns_msg_len )
{
	HEADER	*pHd = (HEADER *)dns_msg;
	long	off;
	int	i;
	DBG("domain: id: 0x%4.4x %s opcode:%d aa=%d tc=%d rd=%d ra=%d rcode=%d\n"
			,ntohs(pHd->id)
			,pHd->qr?"response":"question"
			,pHd->opcode
			,pHd->aa
			,pHd->tc,pHd->rd,pHd->ra,pHd->rcode); 
	DBG("        nQuestion:%d nAnswer:%d nAuthority:%d nResource:%d\n"
			,ntohs(pHd->qdcount)
			,ntohs(pHd->ancount)
			,ntohs(pHd->nscount)
			,ntohs(pHd->arcount)
			);
	off = sizeof(*pHd);
	for( i = 0 ; i < ntohs(pHd->qdcount) ; i++ ){
		int	len = dnsgrab_disp_dnsQuest( dns_msg , off  );
		if( len < 0 )	return;
		off += len;
	}
}



/**
 * return 0 if the name *IS* for me, non-null otherwise
 */
static int dnsgrab_is_for_extern( char *dns_msg, int dns_msg_len, char *qname, int qnameLen
					, uint16_t *pType, uint16_t *pClass )
{
	HEADER		*pHd	= (HEADER *)dns_msg;
	char		*inner_domain = usercfg_get_inner_domain();
	int		questionLen;
	// test it is a question
	if( pHd->qr != DNS_QR_QUESTION )	return 1;
	// test if the opcode is a query
	if( pHd->opcode != DNS_OPCODE_QUERY )	return 1;
	// test if the question is 1 
	if( ntohs(pHd->qdcount) != 1 )		return 1;
	// get the queried name	
	questionLen = dnsgrab_parse_question( dns_msg, sizeof(*pHd), qname, qnameLen, pType, pClass );
	if( questionLen < 0 ){
		LOGM_ERR("can't parse the question section");
		return 1;
	}
	// if it is a name -> addr, only name belonging to inner_domain isnt for extern
	if( *pClass == DNS_CLASS_IN && (*pType == DNS_TYPE_A || *pType == DNS_TYPE_AAAA) ){
		DBG("qname=<%s> inner_domain=<%s>\n", qname, inner_domain );
		// check the name length
		if( strlen( qname ) < strlen(inner_domain) )	return 1;
		// test if the domain is the proper's one
		if( strcmp( inner_domain, qname + strlen(qname) - strlen(inner_domain) ) )
			return 1;
		return 0;
	}
	// if it is a addr -> name, test if it is a local subnet
	if( *pClass == DNS_CLASS_IN && *pType == DNS_TYPE_PTR ){
		ip_addr_t	iaddr;
		ip_addr_from_inaddr_arpa( &iaddr, qname );		
		if( !onet_is_local_addr( &iaddr ) )		return 1;
		return 0;
	}

	return 1;
}

/****************************************************************
 NAME	: dnsgrab_put_name
 AIM	:
 REMARK	: rfc1035.4.1.2.QNAME
****************************************************************/
static char *dnsgrab_put_name( char *p, char *name )
{
	// display to debug
	// DBG("put_name %s\n", name );
	while( *name != 0 ){
		int	len;
		// compute the length of the label
		for( len = 0; *name != '.' && *name != '\0'; name++, len++ );
		// put the length of the label
		*p++ = len;
		// put the label itself
		memcpy( p, name - len, len );
		// go beyond this label
		p += len;
		// goes beyond the '.' in the name
		if( *name == '.' )	name++;
	}
	*p++ = '\0';
	return p;
}

/****************************************************************
 NAME	: dnsgrab_reply_inaddr
 AIM	:
 REMARK	: rfc1035.4.1.1
****************************************************************/
static int dnsgrab_reply_inaddr( char *name, int qtype, int qclass
					, void *resp, int resp_len, long ttl
					, char *pkt, int pkt_len )
{
	struct iphdr	*ip_hd		= (struct iphdr *)pkt;
	struct udphdr	*udp_hd 	= (void *)(pkt + ip_hd->ihl*4);
	char		pkto[NS_PACKETSZ];
	HEADER		*dnsmsgi	= (void *)(pkt + ip_hd->ihl*4 + sizeof(struct udphdr));
	HEADER		*dnsmsgo	= (void *)pkto;
	char		*p		= pkto + sizeof(*dnsmsgo);
	otsp_addr_t	src_peer, dst_peer;
	// build the src/dst_peer addr
	otsp_addr_udpv4_set( &src_peer, ntohl(ip_hd->saddr), ntohs(udp_hd->source) );
	otsp_addr_udpv4_set( &dst_peer, ntohl(ip_hd->daddr), ntohs(udp_hd->dest) );

	memset( pkto, 0, sizeof(pkto) );
	dnsmsgo->id	= dnsmsgi->id;
	dnsmsgo->qr	= DNS_QR_RESPONSE;
	dnsmsgo->aa	= 1;		// authoritativ
	dnsmsgo->ra	= 1;		// recursion available
	dnsmsgo->qdcount= htons(1);
	dnsmsgo->ancount= htons(1);


	// build the QUESTION part
	p = dnsgrab_put_name( p, name );
	*(u_short *)p = ntohs( qtype );				p += 2;
	*(u_short *)p = ntohs( qclass );				p += 2;
	// build the ANSWER part
	p = dnsgrab_put_name( p, name );
	*(u_short *)p 	= ntohs( qtype );			p += 2;
	*(u_short *)p	= ntohs( qclass );			p += 2;
	*(u_long *)p 	= ntohl( ttl );				p += 4;
	*(u_short *)p 	= ntohs( resp_len );			p += 2;
	memcpy( p, resp, resp_len );				p += resp_len;	
	
	// actually send the reply
	raw_udp_send( &dst_peer, &src_peer, pkto, p - pkto);
	
	return 0;
}

/****************************************************************
 NAME	: dnsgrab_reply_inaddr
 AIM	:
 REMARK	: rfc1035.4.1.1
****************************************************************/
static int dnsgrab_reply_error( char *name, int type, int class, int error, char *pkt, int pkt_len )
{
	struct iphdr	*ip_hd		= (struct iphdr *)pkt;
	struct udphdr	*udp_hd 	= (void *)(pkt + ip_hd->ihl*4);
	char		pkto[NS_PACKETSZ];
	HEADER		*dnsmsgi	= (void *)(pkt + ip_hd->ihl*4 + sizeof(struct udphdr));
	HEADER		*dnsmsgo	= (void *)pkto;
	char		*p		= pkto + sizeof(*dnsmsgo);
	otsp_addr_t	src_peer, dst_peer;
	// build the src/dst_peer addr
	otsp_addr_udpv4_set( &src_peer, ntohl(ip_hd->saddr), ntohs(udp_hd->source) );
	otsp_addr_udpv4_set( &dst_peer, ntohl(ip_hd->daddr), ntohs(udp_hd->dest) );

	DBG("dns request for %s src=%s dst=%s\n", name, otsp_addr_str(&src_peer), otsp_addr_str(&dst_peer) );

	memset( pkto, 0, sizeof(pkto) );
	dnsmsgo->id	= dnsmsgi->id;
	dnsmsgo->qr	= DNS_QR_RESPONSE;
	dnsmsgo->aa	= 1;		// authoritativ
	dnsmsgo->ra	= 1;		// recursion available
	dnsmsgo->qdcount= htons(1);
	dnsmsgo->rcode	= error;


	// build the QUESTION part
	p = dnsgrab_put_name( p, name );
	*(u_short *)p = ntohs( type );		p += 2;
	*(u_short *)p = ntohs( class );		p += 2;

	// actually send the reply
	raw_udp_send( &dst_peer, &src_peer, pkto, p - pkto);
	
	return 0;
}



/**
 * called when ns_query obtained a result
 */
static void dnsgrab_ns_query_cb( void *userptr, ns_result_t result )
{
	dnsgrab_ns_req_t 	*req = userptr;
	uint32_t		addr;
	void			*answer;
	int			ans_len;
	int			dns_type = req->inverse_f ? DNS_TYPE_PTR : DNS_TYPE_A;
	DBG("enter ns_result=%s\n", ns_result_str(result) );
							
	if( result == NS_RESULT_FOUND ){
		ns_db_record_t	*record;
		if( !req->inverse_f ){
			ip_addr_t	ip_addr;
			record = ns_util_db_get_dns_hostname( NULL, req->name, &ip_addr );
			EXP_ASSERT( record );
			addr	= ip_addr_v4_get_addr( &ip_addr );
			addr	= htonl(addr);
			answer	= &addr;
			ans_len	= sizeof(addr);
			DBG("answer for %s is %s\n", req->name, ip_addr_str(&ip_addr) );
			// add a dns ip address record simetrical to the dns hostname record
			// - it avoid a useless ns_query during the tunnel establihsment 
			//   when the ip packet for this hostname/ip_addr 
			ns_util_db_addupd_dns_ip_addr( NULL, &ip_addr, req->name, ns_db_record_sec_before_expire(record), &record->peer_id );
		}else{
			char		ans_name[NS_PACKETSZ];
			char		*hostname;
			ip_addr_t	ip_addr;
			char		*p;
			DBG_ASSERT( req->rec_type == NS_REC_DNS_IP_ADDR );
			ip_addr_from_inaddr_arpa( &ip_addr, req->name );
			record = ns_util_db_get_dns_ip_addr( NULL, &ip_addr, &hostname );
			DBG_ASSERT( record );
			p = dnsgrab_put_name( ans_name, hostname );
			answer	= ans_name;
			ans_len	= p-ans_name;
			DBG("answer for %s is %s\n", req->name, hostname );
			nipmem_free( hostname );
		}
		dnsgrab_reply_inaddr( req->name, dns_type, DNS_CLASS_IN
						, answer, ans_len
						, ns_db_record_sec_before_expire(record)
						, req->pkt, req->pkt_len );
	} else {	// else reply an error
		dnsgrab_reply_error( req->name, dns_type, DNS_CLASS_IN
					, ns_r_nxdomain, req->pkt, req->pkt_len );
	}
	dnsgrab_ns_query_close( req );
}

/**
 * start a ns_query
 */
static int dnsgrab_ns_query_open( char *name, int inverse_f, char *pkt, int pkt_len )
{
	// allocate the memory for a nipns_req	
	dnsgrab_ns_req_t	*req = nipmem_zalloc(sizeof(*req));
	EXP_ASSERT( req );
	// build a req
	req->name	= nipmem_strdup( name );
	req->inverse_f	= inverse_f;
	req->pkt_len	= pkt_len;
	req->pkt	= nipmem_malloc( pkt_len );
	memcpy( req->pkt, pkt, pkt_len );
	// if the request is for a name
	if( !req->inverse_f ){
		nipid_build_dns_hostname( &req->record_id, req->name );
		req->rec_type = NS_REC_DNS_HOSTNAME;
	}else{	// if the request is for a ip_addr
		ip_addr_t	iaddr;
		ip_addr_from_inaddr_arpa( &iaddr, req->name );
		nipid_build_dns_ip_addr( &req->record_id, &iaddr );
		req->rec_type = NS_REC_DNS_IP_ADDR;
	}
	// update the ns_req_list
	dnsgrab->ns_req_list = g_list_append( dnsgrab->ns_req_list, req );
	// start the query
	req->req_cb = ns_query_req_open( NULL, &req->record_id, req->rec_type
			, NS_QUERY_TIMEOUT_DEFAULT, dnsgrab_ns_query_cb, req );
	return 0;
}

/**
 * close a ns_query
 */
static void dnsgrab_ns_query_close( dnsgrab_ns_req_t *req )
{
	// update the ns_req_list
	dnsgrab->ns_req_list	= g_list_remove( dnsgrab->ns_req_list, req );
	// close nipns_client
	if( req->req_cb )	ns_query_req_close( NULL, req->req_cb );
	// free memory
	nipmem_free( req->name );
	nipmem_free( req->pkt );
	nipmem_free( req );
}

/****************************************************************
 NAME	: dnsgrab_prepare				01/03/16 13:21:29
 AIM	:
 REMARK	:
****************************************************************/
static gboolean dnsgrab_prepare(GSource *source,gint *timeout)
{
	*timeout = -1;
	return FALSE;
}

/****************************************************************
 NAME	: dnsgrab_check				01/03/16 13:22:18
 AIM	:
 REMARK	:
****************************************************************/
static gboolean dnsgrab_check( GSource *source )
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
 NAME	: dnsgrab_check				01/03/16 13:22:18
 AIM	:
 REMARK	:
****************************************************************/
static gboolean dnsgrab_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
	unsigned char	buf[BUFSIZE];
	ipq_packet_msg_t *ipq_msg;
	char	*pkt, *dns_msg;
	int	pkt_len, dns_msg_len;
	uint16_t qType, qClass;
	char	name[MAXDNAME];

	DBG("received packet\n");
	// read the packet
	if( ipq_read(dnsgrab->h_ipq, buf, sizeof(buf), 0) < 0 )
		return TRUE;
	// handle error
	if( ipq_message_type(buf) == NLMSG_ERROR ){
		LOGM_ERR("dnsgrab receive an error from ipq: %d\n", ipq_get_msgerr(buf) );
		return TRUE;
	}
	EXP_ASSERT( ipq_message_type(buf) == IPQM_PACKET );

	// get the packet
	ipq_msg	= ipq_get_packet(buf);
	// update ip packet pointer
	pkt	= (char *)ipq_msg->payload;
	pkt_len = ipq_msg->data_len;
	
{	struct iphdr	*iph	= (struct iphdr *)pkt;
	// TODO do a sanity check on the incoming packet 
	// - basic header length
	// - check it is a ipv4 packet (at least ipv4 basic header)
	// - take a global function for that
	// - check it is a udp 53
	// - on the dns message ? is it possible
	int		hd_len	= iph->ihl*4 + sizeof(struct udphdr);
	if( pkt_len < hd_len ){
		LOGM_ERR( "received bogus pkt in dnsgrab. not even long enougth for ip/udp header\n");
		return TRUE;
	}
	// setup dns_msg variables (aka skip the ip/udp header in the ip packet)
	dns_msg		= pkt + iph->ihl*4 + sizeof(struct udphdr);
	dns_msg_len	= pkt_len - iph->ihl*4 - sizeof(struct udphdr);
}

	// display to debug
	DBG_DUMP( dns_msg, dns_msg_len );
	dnsgrab_disp_dnsmsg( dns_msg, dns_msg_len );
	// filter the packet
	if( dnsgrab_is_for_extern( dns_msg, dns_msg_len, name, sizeof(name), &qType, &qClass ) ){
		DBG("this isnt a dns request for me\n");
		// accept to forward this packet
		ipq_set_verdict(dnsgrab->h_ipq, ipq_msg->packet_id, NF_ACCEPT, 0, NULL);
		return TRUE;
	}
	DBG("this IS a name for me <%s>\n", name);

	if( qClass == DNS_CLASS_IN && qType == DNS_TYPE_AAAA ){
		// if the request wish a ipv6 address from a name, return nxdomain
		dnsgrab_reply_error( name, qType, DNS_CLASS_IN, ns_r_nxdomain, pkt, pkt_len );
	}else if( qClass == DNS_CLASS_IN && qType == DNS_TYPE_A ){
		dnsgrab_ns_query_open( name, 0, pkt, pkt_len );
	}else{
		DBG_ASSERT( qClass == DNS_CLASS_IN && qType == DNS_TYPE_PTR );
		dnsgrab_ns_query_open( name, 1, pkt, pkt_len );
	}


	// refuse to forward this packet
	ipq_set_verdict(dnsgrab->h_ipq, ipq_msg->packet_id, NF_DROP, 0, NULL);
	
	// TODO who free this packet ?
	return TRUE;
}

static GSourceFuncs dnsgrab_srcfuncs = {
	dnsgrab_prepare,
	dnsgrab_check,
	dnsgrab_dispatch
};


/****************************************************************
 NAME	: dnsgrab_open				01/11/15 15:17:36
 AIM	:
 REMARK	:
****************************************************************/
static int dnsgrab_open_intercept(void)
{
	dnsgrab->h_ipq	= ipq_create_handle(0,PF_INET );
	if( !dnsgrab->h_ipq ){
		LOGM_ERR("cant create ipq_handle (%s)\n", ipq_errstr());
		return -1;
	}
	if( ipq_set_mode(dnsgrab->h_ipq, IPQ_COPY_PACKET, BUFSIZE) < 0 ){
		LOGM_ERR("cant ipq_set_mode (%s)\n", ipq_errstr());
		goto error;
	}

	/* feed glib fd */
	dnsgrab->gfd.fd    = dnsgrab->h_ipq->fd;
	dnsgrab->gfd.events= G_IO_IN | G_IO_HUP | G_IO_ERR;
	/* add it to the source poll */
   	if( (dnsgrab->gsource = g_source_new(&dnsgrab_srcfuncs, sizeof(GSource))) == NULL ){
		LOGM_ERR("couldn't allocate source\n");
        	goto error;
	}
	g_source_add_poll(dnsgrab->gsource, &dnsgrab->gfd);
	g_source_set_priority(dnsgrab->gsource, G_PRIORITY_DEFAULT);
	g_source_set_callback(dnsgrab->gsource, NULL, dnsgrab, NULL);
	g_source_attach(dnsgrab->gsource, NULL);
	return(0);
error:;
	ipq_destroy_handle( dnsgrab->h_ipq );
	return -1;
}

/****************************************************************
 NAME	: dnsgrab_open				01/11/15 15:17:36
 AIM	:
 REMARK	:
****************************************************************/
static void dnsgrab_close_intercept(void)
{
	// close glib stuff
        g_source_remove_poll(dnsgrab->gsource, &dnsgrab->gfd);
        g_source_destroy(dnsgrab->gsource);
        // destroy ipq handler
	ipq_destroy_handle( dnsgrab->h_ipq );
}

/****************************************************************
 NAME	: dnsgrab_insert_rule
 AIM	: 
 REMARK	:
****************************************************************/
static int dnsgrab_add_rule(void)
{
	// TODO SECU use directly the netlink socket!
	system("modprobe ip_queue");
	system("/sbin/iptables -I OUTPUT 1 -p udp --dport 53 -j QUEUE");
	return 0;
}

/****************************************************************
 NAME	: dnsgrab_insert_rule
 AIM	: 
 REMARK	:
****************************************************************/
static void dnsgrab_remove_rule(void)
{
	// TODO SECU use directly the netlink socket!
	system("/sbin/iptables -D OUTPUT -p udp --dport 53 -j QUEUE");
}

#if 0
/****************************************************************
 NAME	: dnsgrab_insert_rule
 AIM	: 
 REMARK	:
****************************************************************/
static void dnsgrab_disp_rule(void)
{
	iptc_handle_t	iptc_h;
	char		*chain = NULL;
	
	iptc_h = iptc_init("filter");
	if( iptc_h == NULL ){
		printf("Error initializing: %s\n", iptc_strerror(errno));
		return;
	}
	for (chain = iptc_first_chain(&iptc_h); chain; chain = iptc_next_chain(&iptc_h)){
		printf("%s\n", chain);
	}
}
#endif

/****************************************************************
 NAME	: dnsgrab_start
 AIM	: 
 REMARK	:
****************************************************************/
int dnsgrab_start(void)
{
	DBG("enter\n");
	// dnsgrab_disp_rule();
	// add iptables rules
	if( dnsgrab_add_rule() ){
		LOGM_ERR( "failed to install the dnsgrab iptables's rules\n");
		return -1;
	}
	// allocate memory
	DBG_ASSERT( dnsgrab == NULL );
	dnsgrab = nipmem_zalloc( sizeof(*dnsgrab) );
	EXP_ASSERT( dnsgrab );
	// install packet interceptor
	if( dnsgrab_open_intercept() ){
		LOGM_ERR( "failed to install the dnsgrab packet interceptor\n");
		goto remove_rule;
	}
	
	return 0;
remove_rule:;	dnsgrab_remove_rule();
		nipmem_free( dnsgrab );
		dnsgrab = NULL;
		return -1;
}

/****************************************************************
 NAME	: dnsgrab_stop
 AIM	: 
 REMARK	:
****************************************************************/
void dnsgrab_stop(void)
{
	GList	*elem;	
	DBG("enter\n");
	// free all pending request
	while( (elem = dnsgrab->ns_req_list) ){
		dnsgrab_ns_req_t *req = elem->data;
		dnsgrab_ns_query_close( req );
	}
	// close intercept
	dnsgrab_close_intercept();	
       	// free memory
	nipmem_free( dnsgrab );
	dnsgrab = NULL;	
	// remove iptables rule
	dnsgrab_remove_rule();
}
