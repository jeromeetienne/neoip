/*! \file
    \brief Implementation of outter net (aka the layer which xmit/recv packet for the neoip-router)

*/


/* system include */
#include <stdlib.h>
#include <net/ethernet.h>
#include <string.h>
#include <netinet/ip_icmp.h>

/* local include */
#include "onet.h"
#include "vdev.h"
#include "util.h"
#include "initiator.h"
#include "stun_out.h"
#include "stun_httpd.h"
#include "usercfg.h"
#include "rawip.h"
#include "netif.h"
#include "ns_publish.h"
#include "ns_db.h"
#include "ns_util.h"
#include "ns_query.h"
#include "ip_addr.h"
#include "iaddr_log_client.h"
#include "nipid.h"
#include "payl.h"
#include "prop_util.h"
#include "dnsgrab.h"
#include "watchdog.h"
#include "httpd.h"
#include "dst_iaddr_negcache.h"
#include "rate_estim.h"
#include "rate_limit.h"


typedef struct {
	ip_addr_t		dst_iaddr;
	ns_query_req_cb_t	*ns_req;
} onet_ns_req_dst_iaddr_t;

typedef struct {
	nipid_t			remote_nipid;
	ns_query_req_cb_t	*req_cb;	
} onet_resp_iaddr_req_t;

typedef struct {
	ip_addr_t	remote_iaddr;
	stun_t		*stun;
	rate_estim_t	*pkt_rate;
	rate_estim_t	*throughput;
	rate_limit_t	*thput_limit;
// PART itor only
	itor_t		*itor;
	// to store the trigger packet --rfc1122.2.3.2.2
	uint16_t	trigger_pkt_type;
	void		*trigger_pkt;
	int		trigger_pkt_len;
// PART resp only
	onet_resp_iaddr_req_t *resp_iaddr_req;
} onet_tunnel_t;

typedef struct {
	ip_addr_t	ip_iaddr;
	ip_netmask_t	ip_netmask;
	
	iaddr_log_client_t	iaddr_log_client;
	
	dst_iaddr_negcache_t	*dst_iaddr_negcache;	//!< the tunnel negative cache
	nipid_t		local_cnxid;	//!< the responder local cnxid
	resp_t		*resp;
	vdev_t		vdev;		//!< the virtual network device
	GList		*tunnel_list;
	GList		*ns_req_dst_iaddr_list;
} onet_t;

static onet_t *onet_main = NULL;
static void onet_itor_cnx_result_cb( void *userptr, itor_cnx_result_t cnx_result, stun_t *stun );
static void onet_tunnel_close( onet_tunnel_t *tunnel );
static int onet_set_ip_addr( ip_addr_t *ip_addr );
static int onet_add_route_netmask( ip_netmask_t *netmask );
static void onet_tunnel_resp_iaddr_req_close( onet_tunnel_t *tunnel );
static int onet_httpd_handler_cb(char *path, httpd_var_t *httpd_var, GByteArray *buf );

/**
 * callback called by nipns_client
 * - this seems used for the iaddr in the stun_t which will soon be obsolete
 *   by the abstraction of tunnel and connection
 */
static void onet_tunnel_resp_iaddr_req_cb( void *userptr, ns_result_t result )
{
	onet_tunnel_t		*tunnel		= userptr;
	onet_resp_iaddr_req_t 	*iaddr_req	= tunnel->resp_iaddr_req;
	ns_db_record_t		*record;
	gen_hd_t		*gen_hd;
	// check if the record has been found. if not, close the tunnel
	if( result != NS_RESULT_FOUND ){
		LOGM_ERR("cant get the inner address of the remote nipid for this connection. ns_result=%s\n", ns_result_str(result));
		onet_tunnel_close( tunnel );
		return;
	}
	// get the found record
	record = ns_util_db_get( NULL, &iaddr_req->remote_nipid, NS_REC_DNS_HOSTNAME );
	DBG_ASSERT( record );
	// copy the remote_iaddr
	gen_hd	= record->data;
	ip_addr_from_payload( &tunnel->remote_iaddr, payl_gen_hd_data(gen_hd) );
	DBG("set the tunnel remote_iaddr to %s\n", ip_addr_str(&tunnel->remote_iaddr) );
	// close the resp_iaddr_req
	onet_tunnel_resp_iaddr_req_close( tunnel );
	// logging
	LOGM_WARNING("tunnel established with %s (%s) as responder\n"
			, stun_get_remote_identity(tunnel->stun)
			, ip_addr_str(&tunnel->remote_iaddr) );
}

/**
 * open a onet_resp_iaddr_req
 */
static void onet_tunnel_resp_iaddr_req_open( onet_tunnel_t *tunnel )
{
	onet_resp_iaddr_req_t	*iaddr_req;
	tunnel->resp_iaddr_req	=  nipmem_zalloc(sizeof(*iaddr_req));
	iaddr_req		=  tunnel->resp_iaddr_req;
	// fill the struct
	iaddr_req->remote_nipid = *stun_get_remote_cnxid( tunnel->stun );
	// launch the query
	iaddr_req->req_cb = ns_query_req_open( NULL, &iaddr_req->remote_nipid, NS_REC_DNS_HOSTNAME
					, NS_QUERY_TIMEOUT_DEFAULT
					, onet_tunnel_resp_iaddr_req_cb, tunnel );
}

/**
 * close a onet_resp_iaddr_req
 */
static void onet_tunnel_resp_iaddr_req_close( onet_tunnel_t *tunnel )
{
	onet_resp_iaddr_req_t *iaddr_req = tunnel->resp_iaddr_req;
	// close the name request
	ns_query_req_close( NULL, iaddr_req->req_cb );
	// free memory
	nipmem_free( iaddr_req );
	tunnel->resp_iaddr_req = NULL;
}

static void onet_tunnel_init_common( onet_tunnel_t *tunnel )
{
	// sanity check
	DBG_ASSERT( tunnel->stun );
	// set the idle timeout
	stun_idle_timeout_set( tunnel->stun, ONET_TUNNEL_IDLE_TIMEOUT );
	// init pkt_rate and throughtput
	tunnel->pkt_rate	= rate_estim_open_delay( 30*1000, 200 );
	tunnel->throughput	= rate_estim_open_delay( 30*1000, 200 );
	// open the limit (TMP: just to test the limiter)
	tunnel->thput_limit	= rate_limit_open_delay( tunnel->throughput, 1*1000, 200*1024, 1000 );
}

/**
 * open a tunnel as responder
 */
static onet_tunnel_t *onet_tunnel_open_resp( stun_t *stun )
{
	onet_t		*onet		= onet_main;
	onet_tunnel_t	*tunnel 	= nipmem_zalloc( sizeof(*tunnel) );

	// fill the struct
	tunnel->stun	= stun;
	// complete the init with the common part
	onet_tunnel_init_common( tunnel );
	// update the tunnel_list
	onet->tunnel_list = g_list_append(onet->tunnel_list, tunnel );

	// launch the iaddr request for the responder
	onet_tunnel_resp_iaddr_req_open( tunnel );
	return tunnel;
}

/**
 * open a tunnel as initiator
 */
static onet_tunnel_t *onet_tunnel_open_itor( ip_addr_t *remote_iaddr )
{
	onet_t		*onet	= onet_main;
	onet_tunnel_t	*tunnel = nipmem_zalloc( sizeof(*tunnel) );
	ns_db_record_t	*peer_rec, *ip_addr_rec;
	char		*remote_hostname;
	nipid_t		remote_nipid;
	// get the hostname from the remote_iaddr
	ip_addr_rec	= ns_util_db_get_dns_ip_addr( NULL, remote_iaddr, &remote_hostname );
	DBG_ASSERT( ip_addr_rec );
	// get the peer record
	peer_rec	= ns_util_get_or_fetch( NULL, &ip_addr_rec->peer_id, NS_REC_PEER );
	if( !peer_rec ){
		LOGM_ERR("unable to get the peer record from the ip address record\n");
		return NULL;
	}
	// build the remote_nipid
	nipid_build_dns_hostname( &remote_nipid, remote_hostname );	
	// free the record hostname
	nipmem_free( remote_hostname );
	// if no stun/itor is present for this packet, create an itor
	tunnel->itor = itor_start( &onet->local_cnxid, &remote_nipid
				, &peer_rec->record_id, &peer_rec->peer_rdvpt
				, onet_itor_cnx_result_cb, tunnel );
	if( !tunnel->itor ){
		LOGM_ERR("unable to start a nsock itor\n");
		goto free_stt;
	}
	// set the remote_iaddr
	tunnel->remote_iaddr	= *remote_iaddr;
	// update the tunnel_list
	onet->tunnel_list = g_list_append(onet->tunnel_list, tunnel );
	return tunnel;
free_stt:;	nipmem_free( tunnel );
		return NULL;
}

/**
 * close a tunnel
 */
static void onet_tunnel_close( onet_tunnel_t *tunnel )
{
	onet_t		*onet		= onet_main;
	// update the tunnel_list
	onet->tunnel_list = g_list_remove(onet->tunnel_list, tunnel );
	// close pkt_rate and throughtput
	if( tunnel->pkt_rate )		rate_estim_close( tunnel->pkt_rate );
	if( tunnel->throughput )	rate_estim_close( tunnel->throughput );
	// close the limit (TMP: just to test the limiter)
	if( tunnel->thput_limit )	rate_limit_close( tunnel->thput_limit );

	// close resp_iaddr_req if present
	if( tunnel->resp_iaddr_req )	onet_tunnel_resp_iaddr_req_close( tunnel );
	// close itor if present
	if( tunnel->itor )		itor_stop( tunnel->itor );
	// close stun if present
	if( tunnel->stun ) 		stun_close( tunnel->stun );
	// free the trigger packet
	if( tunnel->trigger_pkt ){
		nipmem_free( tunnel->trigger_pkt );
		tunnel->trigger_pkt = NULL;
	}
	// free memory
	nipmem_free(tunnel);
}

/**
 * set the trigger packet when the tunnel is as itor
 */
static void onet_tunnel_itor_trigger_pkt_set( onet_tunnel_t *tunnel, uint16_t pkt_type, void *pkt, int pkt_len )
{
	// sanity check
	DBG_ASSERT( tunnel->itor );
	// if the old trigger_pkt if it exists
	if( tunnel->trigger_pkt )	nipmem_free( tunnel->trigger_pkt );
	// update the tigger_pkt
	tunnel->trigger_pkt_type 	= pkt_type;
	tunnel->trigger_pkt_len		= pkt_len;
	tunnel->trigger_pkt 		= nipmem_malloc( pkt_len );
	EXP_ASSERT( tunnel->trigger_pkt );
	memcpy( tunnel->trigger_pkt, pkt, pkt_len );	
}

/**
 * send the trigger packet if present
 */
static void onet_tunnel_itor_trigger_pkt_flush( onet_tunnel_t *tunnel )
{
	// if no trigger packet, return
	if( !tunnel->trigger_pkt )	return;
	// send the trigger packet
	stun_out_data( tunnel->stun, tunnel->trigger_pkt_type, tunnel->trigger_pkt
						, tunnel->trigger_pkt_len );
	// free the memory
	nipmem_free( tunnel->trigger_pkt );
	tunnel->trigger_pkt = NULL;
}

/**
 * callback called when the stun received input or error
 */
static int onet_stun_inerr_cb( void *userptr, uint16_t pkt_type, void *pkt, int pkt_len, stun_cnx_err_t stun_err )
{
	onet_tunnel_t	*tunnel = userptr;
	DBG("enter pkt_type=%d pkt=%p pkt_len=%d stun_err=%d\n", pkt_type, pkt, pkt_len, stun_err );
	// if the stun received an error, close the tunnel
	if( stun_cnx_err_is_fatal(stun_err) ){
		LOGM_ERR("tunnel with %s closed. reason is %s\n", stun_get_remote_identity(tunnel->stun), stun_cnx_err_str(stun_err) );
		onet_tunnel_close( tunnel );
		return 0;
	}
	DBG("received a data packet from stunnel to forward. pkt_len=%d\n", pkt_len );DBG_DUMP(pkt,pkt_len);
	// if this is a tun_stub, dont send the packet
	if( prop_get_bool_dfl( "neoip_router", "debug:tun_stub", 0 ) )	return 0;
	vdev_xmit( &onet_main->vdev, pkt, pkt_len, pkt_type);
	return 0;
}

/**
 * callback called by itor when the connection succeed or fail
 */
static void onet_itor_cnx_result_cb( void *userptr, itor_cnx_result_t result, stun_t *stun )
{
	onet_t		*onet	= onet_main;
	onet_tunnel_t	*tunnel = userptr;
	DBG("enter result=%d stun=%p\n", result, stun);
	// if the result isnt connected, close this tunnel
	if( result != ITOR_RESULT_CONNECTED ){
		// put the destination ip address in the negcache
		dst_iaddr_negcache_add( onet->dst_iaddr_negcache, &tunnel->remote_iaddr, 0 );
		// loging for diagnostict
		LOGM_ERR("tunnel initialization failed because %s\n", itor_cnx_result_str(result) );
		// close the tunnel
		onet_tunnel_close( tunnel );
		return;
	}
	// sanity check
	DBG_ASSERT( tunnel->itor );
	DBG_ASSERT( stun );
	// close the itor
	itor_stop( tunnel->itor );
	tunnel->itor = NULL;
	// update the struct
	tunnel->stun	= stun;
	// set the stun callback
	stun_set_inerr_cb( tunnel->stun, onet_stun_inerr_cb, tunnel );
	// complete the init with the common part
	onet_tunnel_init_common( tunnel );
	// flush triggering packet
	onet_tunnel_itor_trigger_pkt_flush( tunnel );
	// logging
	LOGM_WARNING("tunnel established with %s (%s) as itor\n"
			, stun_get_remote_identity(tunnel->stun)
			, ip_addr_str(&tunnel->remote_iaddr) );
}

/**
 * callback called when a connection is accepted (so fully established)
 */
static void onet_resp_accepted_cb( void *userptr, nipid_t *resp_nipid, stun_t *stun )
{
	onet_t		*onet	= onet_main;
	onet_tunnel_t	*tunnel;
	DBG("enter stun=%p\n", stun);
	// sanity check
	DBG_ASSERT( stun );
	DBG_ASSERT( nipid_is_equal( &onet->local_cnxid, resp_nipid ) );
	// open the tunnel as responder
	tunnel = onet_tunnel_open_resp( stun );
	if( !tunnel ){
		LOGM_ERR("cant open tunnel as responder\n");
		return;
	}
	// set the stun callback
	stun_set_inerr_cb( tunnel->stun, onet_stun_inerr_cb, tunnel );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//             handle NS_REQ_DST_IADDR
// - this is used to query the NS_REC_DNS_IP_ADDR of the dst_iaddr
//   and to feed the dst_iaddr negcache in case of faillure
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/**
 * return the onet_ns_req_dst_iaddr_t matching this dst_iaddr
 */
static onet_ns_req_dst_iaddr_t *onet_ns_req_dst_iaddr_get( ip_addr_t *dst_iaddr )
{
	onet_t	*onet	= onet_main;	
	GList	*elem;
	for( elem = onet->ns_req_dst_iaddr_list; elem; elem = g_list_next( elem ) ){
		onet_ns_req_dst_iaddr_t	*ns_req_dst_iaddr = elem->data;
		if( ip_addr_is_equal(dst_iaddr, &ns_req_dst_iaddr->dst_iaddr))
			return ns_req_dst_iaddr;
	}
	return NULL;
}

/**
 * delete a onet_ns_req_dst_iaddr_t
 */
static void onet_ns_req_dst_iaddr_del( onet_ns_req_dst_iaddr_t *ns_req_dst_iaddr )
{
	onet_t	*onet	= onet_main;	
	// update to the list
	onet->ns_req_dst_iaddr_list = g_list_remove(onet->ns_req_dst_iaddr_list, ns_req_dst_iaddr );	
	// close the ns_query
	ns_query_req_close( NULL, ns_req_dst_iaddr->ns_req );
	// free the memory
	nipmem_free( ns_req_dst_iaddr );
}
/**
 * callback called by ns_query to provide a result
 */
static void onet_ns_req_dns_ip_addr_ns_query_cb( void *userptr, ns_result_t result )
{
	onet_ns_req_dst_iaddr_t	*ns_req_dst_iaddr = userptr;
	onet_t			*onet	= onet_main;
	// log to debug
	DBG("result = %s\n", ns_result_str( result ));
	// if the record isnt found, put it in dst_iaddr_negcache
	if( result != NS_RESULT_FOUND ){
		dst_iaddr_negcache_add( onet->dst_iaddr_negcache, &ns_req_dst_iaddr->dst_iaddr, 0 );
	}else{	// sanity check
		nipid_t			recordid;
		nipid_build_dns_ip_addr( &recordid, &ns_req_dst_iaddr->dst_iaddr );
		DBG_ASSERT( ns_util_db_get( NULL, &recordid, NS_REC_DNS_IP_ADDR ) );
	}
	// delete the query
	onet_ns_req_dst_iaddr_del( ns_req_dst_iaddr );
}

/**
 * return 0 if the local database contain a NS_REC_DNS_IP_ADDR for this dst_iaddr, non null otherwise
 * - if there is no record, launch a query for it
 */
static int onet_ns_req_dst_iaddr_test( ip_addr_t *dst_iaddr )
{
	onet_t			*onet	= onet_main;	
	nipid_t			recordid;
	onet_ns_req_dst_iaddr_t	*ns_req_dst_iaddr;
	nipid_build_dns_ip_addr( &recordid, dst_iaddr );
	// if the record is already in the database, do nothing
	if( ns_util_db_get( NULL, &recordid, NS_REC_DNS_IP_ADDR ) )	return 0;
	// if there is a pending ns_query for it, do nothing but return faillure
	if( onet_ns_req_dst_iaddr_get( dst_iaddr ) )			return 1;
	// if there is no query, launch one
	ns_req_dst_iaddr = nipmem_zalloc(sizeof(*ns_req_dst_iaddr));
	// fill the struct 
	ns_req_dst_iaddr->dst_iaddr = *dst_iaddr;
	ns_req_dst_iaddr->ns_req = ns_query_req_open( NULL, &recordid, NS_REC_DNS_IP_ADDR, ONET_DELAY_B4_ICMP
					, onet_ns_req_dns_ip_addr_ns_query_cb, ns_req_dst_iaddr );
	// update to the list
	onet->ns_req_dst_iaddr_list = g_list_append(onet->ns_req_dst_iaddr_list, ns_req_dst_iaddr );	
	DBG("launch a ns_req_dst_iaddr for %s\n", ip_addr_str(dst_iaddr) );
	// return faillure
	return 1;
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/*
 * return a tunnel matching the remote iaddr
 */
static onet_tunnel_t *onet_tunnel_from_remote_iaddr( ip_addr_t *dst_iaddr )
{
	onet_t	*onet	= onet_main;	
	GList	*elem;
	for( elem = onet->tunnel_list; elem; elem = g_list_next( elem ) ){
		onet_tunnel_t	*tunnel	= elem->data;
		if( ip_addr_is_equal( dst_iaddr, &tunnel->remote_iaddr ))
			return tunnel;
	}
	return NULL;
}

/**
 * callback to received data from the vdev
 * - if a established tunnel exist, send the packet to it
 * - if a tunnel is currently in establishment, set the triggering packet
 * - if no tunnel exist, start the establishement on one
 */
static void onet_vdev_in( void *userptr, int ethertype, char *pkt, int pkt_len )
{
	struct 	iphdr	*iph	= (struct iphdr *)pkt;
	onet_t		*onet	= onet_main;	
	onet_tunnel_t	*tunnel;
	ip_addr_t	dst_iaddr;
	
	DBG("enter ethertype=0x%x\n", ethertype );
	// handle only ipv4 for now
	EXP_ASSERT( ethertype == ETHERTYPE_IP );
	// sanity check
// TODO put the basic check of the ipv4 packet in a function
	if( pkt_len < sizeof(*iph) ){
		LOG(0,"received bogus packet of %d-byte. not even big enought for an ipv4 header\n", pkt_len );
		return;
	}
	if( pkt_len < iph->ihl*4 ){
		LOG(0,"received bogus packet of %d-byte with ipv4_hd->ihl=%d\n", pkt_len, iph->ihl*4 );
		return;
	}
	// get the destination ip address from the packet
	ip_addr_v4_set( &dst_iaddr, ntohl(iph->daddr) );
	// find a existing tunnel if there is any
	tunnel = onet_tunnel_from_remote_iaddr( &dst_iaddr );
	// if there is a tunnel and the connection is already established, send it thru it
	if( tunnel && tunnel->stun ){
		DBG("there is already a establish link for this packet \n");
		// TMP: just to test the limitor
		if( rate_limit_exceeded( tunnel->thput_limit) ){
//			LOGM_ERR("packet discarded due to rate limiter\n");
			return;
		}
		// update the pkt_rate and throughput
		rate_estim_add( tunnel->pkt_rate, 1 );
		rate_estim_add( tunnel->throughput, pkt_len );
		// send the packet
		stun_out_data( tunnel->stun, ethertype, pkt, pkt_len );
		return;
	}

	// if the ipaddr is in the dst_iaddr_negcache, return
	if( dst_iaddr_negcache_is_present( onet->dst_iaddr_negcache, &dst_iaddr) ){
		// return an ICMP if the ip record is in the dst_iaddr_negcache
		// - apply the concept of not replying a icmp immediatly to let
		//   the time to resolve the address
		// - similar to the time to solve the hw address with ARP
		// - as in rfc2461.7.2.2, ICMP must be replied after 3sec
		//   - it is ONET_DELAY_B4_ICMP
		// TODO the timer aspect isnt well respected now
		//      - itor has its own timer see bug 359
		//      - onet_ns_req_dst_iaddr_* honor it tho
		raw_icmp_reply_send( ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, 0, pkt, pkt_len );
		return;		
	}
	// if there no record for this dst_iaddr in the local database, discard the packet
	if( onet_ns_req_dst_iaddr_test( &dst_iaddr ) ){
		DBG("received packet for which im unable to find a rdvpoint\n" );
		return;
	}

	// if a establishing tunnel exists, update the trigerring packet
	if( tunnel ){
		DBG_ASSERT( tunnel->itor );
		// update the trigger packet
		onet_tunnel_itor_trigger_pkt_set( tunnel, ethertype, pkt, pkt_len );
		DBG("tunnel is currently in establishement for this packet\n");
		return;
	}
	DBG_ASSERT( !tunnel );

	// create a tunnel as itor
	tunnel = onet_tunnel_open_itor( &dst_iaddr );
	if( !tunnel ){
		LOGM_ERR("can't initiate a tunnel toward the iaddr %s\n", ip_addr_str( &dst_iaddr ) );
		return;
	}
	// set the trigger packet
	onet_tunnel_itor_trigger_pkt_set( tunnel, ethertype, pkt, pkt_len );
}

/**
 * init the onet once the iaddr has been obtained
 */
static int onet_init_post_iaddr( void )
{
	onet_t	*onet	= onet_main;
	int	err;

	// start the responder
	onet->resp = resp_start(&onet->local_cnxid, usercfg_get_auth_type(), onet_resp_accepted_cb, NULL);
	if( !onet->resp ){
		LOGM_CRIT("cant start onet's responder\n");
		return -1;
	}
	resp_set_label(onet->resp, "NeoIP_router tunnel");
	// publish the records for this router
	ns_util_publish_dns_hostname( NULL, usercfg_get_peername(), &onet->ip_iaddr, ONET_DNS_RECORD_TTL, NULL, NULL );
	ns_util_publish_dns_ip_addr( NULL, &onet->ip_iaddr, usercfg_get_peername(), ONET_DNS_RECORD_TTL, NULL, NULL );

	// open the virtual netif
	err = vdev_open( &onet->vdev );
	if( err ){
		LOGM_ERR("cant init vdev err=%d\n", err );
		return -1;
	}
	// set the mtu
	netif_set_mtu( onet->vdev.dev_name, 10000 );
	// register callback for the virtual netif
	vdev_register_callback( &onet->vdev, onet_vdev_in );
	// set the ip addr and netmask
	onet_set_ip_addr( &onet->ip_iaddr );
	onet_add_route_netmask( &onet->ip_netmask );

	// start the dnsgrab layer	
	if( prop_get_bool_dfl( "neoip_router", "debug:dnsgrab", 1 ) ){
		if( dnsgrab_start() ){
			LOGM_CRIT("cant start the dns grabber\n");
		}
		if( watchdog_open() ){
			LOGM_CRIT("cant start the watchdog\n");
		}
	}
	// add the handler in httpd
	httpd_handler_add( "/neoip_router", onet_httpd_handler_cb );
	return 0;
}
/**
 * called when the iaddr allocation provides a result
 */
static void onet_iaddr_log_client_cb( void *userptr, iaddr_log_client_state_t result,
				 ip_addr_t *ip_addr, ip_netmask_t *ip_netmask )
{
	onet_t	*onet	= onet_main;
	DBG("enter result=%d\n", result);
	if( result != IADDR_LOG_CLIENT_STATE_SUCCEED ){
		// TODO what do i do here ?!?!?!?! i close the tunnel ?
		// - i retry in 10sec ? YES!!! anyway you cant do anything without it
		return;
	}
	// sanity check
	DBG_ASSERT( result == IADDR_LOG_CLIENT_STATE_SUCCEED );
	// some logging
	DBG("SUCCEED ip_addr=%s ip_netmask=%s\n", ip_addr_str(ip_addr), ip_netmask_str(ip_netmask) );
	// update the struct
	onet->ip_iaddr	 	= *ip_addr;
	onet->ip_netmask	= *ip_netmask;
	// close the iaddr_log_client
	iaddr_log_client_close(	&onet->iaddr_log_client );
	// complete the init of the onet
	onet_init_post_iaddr();
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//        HTTPD DISPLAY
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

static int onet_httpd_handler_cb(char *path, httpd_var_t *httpd_var, GByteArray *buf )
{
	onet_t	*onet	= onet_main;	
	GList	*elem;
	httpd_printf_page_title(buf, "NeoIp Router" );
	httpd_printf(buf,"<b>Local ip address:</b> %s<br>", ip_addr_str(&onet->ip_iaddr) );
	httpd_printf(buf,"<b>Local ip netmask:</b> %s<br>", ip_netmask_str(&onet->ip_netmask) );
	httpd_printf(buf,"<b>Local cnxid:</b> %s<br>", nipid_str(&onet->local_cnxid) );
	httpd_printf(buf,"<b>virtual network device:</b> %s<br>", onet->vdev.dev_name );

	httpd_printf(buf,"<hr><h3><div align=\"center\">Tunnel List:</div></h3>" );
	httpd_printf_table_start(buf);
	httpd_printf_tr_start(buf);
	httpd_printf_th(buf, "remote iaddr");
	httpd_printf_th(buf, "state");
	httpd_printf_th(buf, "remote identity");
	httpd_printf_th(buf, "info on connection");
	httpd_printf_th_title(buf, "number of packets per sec estimated of the last 5-sec", "packet rate");
	httpd_printf_th_title(buf, "number of kbyte per sec estimated of the last 5-sec", "throughput");
	httpd_printf_tr_end(buf);
	for( elem = onet->tunnel_list; elem; elem = g_list_next( elem ) ){
		onet_tunnel_t	*tunnel	= elem->data;
		httpd_printf_tr_start(buf);
		if( tunnel->itor ){
			httpd_printf_td(buf, "%s", ip_addr_str(&tunnel->remote_iaddr));
			httpd_printf_td(buf, "Initiating");
			httpd_printf_td(buf, "none yet");
			httpd_printf_td(buf, "<a href=\"%s\" title=\"Provide details on this connection's initiator\">X</a>"
								, itor_httpd_get_link(tunnel->itor, "disp_single"));
		}else if( tunnel->resp_iaddr_req ){
			httpd_printf_td(buf, "none yet");
			httpd_printf_td(buf, "Responding");
			httpd_printf_td(buf, "none yet");
			httpd_printf_td(buf, "none yet");
		}else{
			httpd_printf_td(buf, "%s", ip_addr_str(&tunnel->remote_iaddr));
			httpd_printf_td(buf, "Established");
			DBG_ASSERT( tunnel->stun );
			httpd_printf_td(buf, "%s", stun_get_remote_identity(tunnel->stun) );
			httpd_printf_td(buf, "<a href=\"%s\" title=\"Provide details on this established connection\">X</a>"
								, stun_httpd_get_link(tunnel->stun, "disp_single"));
		}
		httpd_printf_td(buf, "%.2lf pkt/sec", rate_estim_get_avg_delay(tunnel->pkt_rate, 5*1000, 1000) );
		httpd_printf_td(buf, "%.2lf kbyte/sec", rate_estim_get_avg_delay(tunnel->throughput, 5*1000, 1000)/1024 );
		httpd_printf_tr_end(buf);
	}
	httpd_printf_table_end(buf);		
	return 0;
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//        LAYER FUNCTIONS
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/**
 * open onet layer
 */
int onet_openlay( void )
{
	onet_t	*onet	= nipmem_zalloc( sizeof(*onet) );
	int	err	= 0;
	// build the local_cnxid
	nipid_build_dns_hostname( &onet->local_cnxid, usercfg_get_peername() );
	// open the dst_iaddr_negcache
	onet->dst_iaddr_negcache = dst_iaddr_negcache_open();
	// get the local iaddr
	err = iaddr_log_client_open( &onet->iaddr_log_client, &onet->local_cnxid
			, usercfg_get_iaddr_log_srv(), onet_iaddr_log_client_cb, onet );
	if( err )	return -1;
	// set the onet_main
	onet_main = onet;
	return err;
}

/**
 * close onet layer
 */
void onet_closelay( void )
{
	onet_t	*onet	= onet_main;
	GList	*elem;
	// close all tunnel
	while( (elem = onet->tunnel_list) ){
		onet_tunnel_t	*tunnel = elem->data;
		onet_tunnel_close( tunnel );
	}
	// close all pending ns_req_dst_iaddr
	while( (elem = onet->ns_req_dst_iaddr_list) ){
		onet_ns_req_dst_iaddr_t	*ns_req_dst_iaddr = elem->data;
		onet_ns_req_dst_iaddr_del( ns_req_dst_iaddr );
	}
	// close the iaddr_log_client if it is still running
	if( ip_addr_is_null(&onet->ip_iaddr) ){
		iaddr_log_client_close( &onet->iaddr_log_client );
	}else{
		// delete the handler in httpd
		httpd_handler_del_by_path( "/neoip_router" );		
		// vdev_unregister_callback( vdev, onet_vdev_in );
		vdev_close( &onet->vdev );
		// stop responder
		resp_stop(onet->resp);
		// stop the dnsgrab layer	
		if( prop_get_bool_dfl( "neoip_router", "debug:dnsgrab", 1) ){
			dnsgrab_stop();
			watchdog_close();
		}
	}
	// open the dst_iaddr_negcache
	dst_iaddr_negcache_close( onet->dst_iaddr_negcache );
	// free the memory
	nipmem_free( onet );
	onet_main = NULL;
}

/**
 * set the ip address of the virtual device
 */
static int onet_set_ip_addr( ip_addr_t *ip_addr )
{
	// honor the tun_stub user option
	if( prop_get_bool_dfl( "neoip_router", "debug:tun_stub", 0 ) )	return 0;
	return netif_set_ip_addr( onet_main->vdev.dev_name, ip_addr );
}

/**
 * set a netmask route for this vdev
 */
static int onet_add_route_netmask( ip_netmask_t *netmask )
{
	ip_addr_t	ip_addr;
	char	cmd[300];
	onet_main->ip_netmask	= *netmask;
	// honor the tun_stub user option
	if( prop_get_bool_dfl( "neoip_router", "debug:tun_stub", 0 ) )	return 0;
	// TODO this function is really lame
	// - the system()
	// - what if the ip addr already exists
	ip_netmask_any_addr(netmask, &ip_addr);
	snprintf(cmd,sizeof(cmd), "ip route add %s/%d dev %s"
					, ip_addr_str(&ip_addr)
					, ip_netmask_get_prefix_len(netmask)
					, onet_main->vdev.dev_name );	
	DBG("exec a cmd <%s> LAME change it to syscall\n", cmd );
	system( cmd );
	return 0;
}

/**
 * return non-null if the address is a local one
 */
int onet_is_local_addr( ip_addr_t *ip_addr )
{
	// TODO if ip_netmask is null, return 0;
	DBG("is %s belong to %s\n", ip_addr_str( ip_addr ), ip_netmask_str( &onet_main->ip_netmask) );
	if( ip_addr_belong_to_netmask( ip_addr, &onet_main->ip_netmask ) )
		return 1;
	return 0;
}
