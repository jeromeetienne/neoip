/*! \file
    \brief the internal address allocated on loggin server

*/

/* system include */
#include <string.h>

/* local include */
#include "iaddr_log_server.h"
#include "unbound_inpkt.h"
#include "packet.h"
#include "util.h"
#include "payl.h"
#include "otsp.h"
#include "usercfg.h"
#include "nipid.h"

static iaddr_log_server_t *iaddr_log_server = NULL;

/**
 * allocate a iaddr to this nodeid
 */
static int iaddr_log_allocate_iaddr( nipid_t *nodeid, ip_addr_t *iaddr )
{
	nodeid_iaddr_t *nodeid_iaddr = nodeid_iaddr_from_nodeid(&iaddr_log_server->nodeid_iaddr_db, nodeid );
	if( nodeid_iaddr == NULL ){
		ip_addr_t	first_try = iaddr_log_server->next_inner_addr;
		do{
			nodeid_iaddr = nodeid_iaddr_from_ip_addr(&iaddr_log_server->nodeid_iaddr_db, &iaddr_log_server->next_inner_addr );
			if( nodeid_iaddr == NULL ){
				nodeid_iaddr = nodeid_iaddr_db_add(&iaddr_log_server->nodeid_iaddr_db, nodeid, &iaddr_log_server->next_inner_addr);
				ip_addr_next( &iaddr_log_server->next_inner_addr, &iaddr_log_server->inner_netmask );
				break;
			}
			ip_addr_next( &iaddr_log_server->next_inner_addr, &iaddr_log_server->inner_netmask );
		} while( ip_addr_is_equal( &first_try, &iaddr_log_server->next_inner_addr ) == 0 );
		if( ip_addr_is_equal( &first_try, &iaddr_log_server->next_inner_addr ) )
			return -1;
	}else{
		nodeid_iaddr_renew( &iaddr_log_server->nodeid_iaddr_db, nodeid_iaddr );
	}
	
	*iaddr = nodeid_iaddr->ip_addr;
	return 0;
}

/**
 * send a iaddr reply
 */
static int iaddr_log_server_send_reply( otsp_caddr_t *otsp_caddr, nipid_t *nodeid
			, payl_nounce_t *payl_nounce
			, iaddr_log_server_result_t result
			, ip_addr_t *ip_addr )
{
	int		pkt_len	= sizeof( proto_hd_t ) 
				+ sizeof(gen_hd_t) + sizeof(payl_nipid_t)
				+ sizeof(gen_hd_t) + sizeof(payl_nounce_t)
				+ sizeof(gen_hd_t) + sizeof(payl_uint32_t)
				+ sizeof(gen_hd_t) + sizeof(payl_ip_addr_t)
				+ sizeof(gen_hd_t) + sizeof(payl_ip_netmask_t);
	char		*pkt 	= nipmem_zalloc( pkt_len );	
	proto_hd_t 		*hd;
	gen_hd_t		*gen_hd;

	DBG_ASSERT( pkt != NULL );
	// build the packet header
	hd 			= (proto_hd_t *)pkt;
	payl_proto_hd_init( hd, YAV_PKT_IADDR_REP );
	// init the payl_nipid for the nipid
	gen_hd			= payl_proto_hd_data( hd );
	payl_gen_hd_init( gen_hd, YAV_PAYL_NIPID, sizeof(payl_nipid_t) );
	nipid_build_to_bin(payl_gen_hd_data( gen_hd ), nodeid);		
	// init the payl_nounce
	gen_hd			= payl_gen_hd_next( gen_hd );
	payl_gen_hd_init( gen_hd, YAV_PAYL_NOUNCE, sizeof(payl_nounce_t) );
	memcpy( payl_gen_hd_data( gen_hd ), payl_nounce, sizeof(payl_nounce_t) );
	// build the status payload
	gen_hd			= payl_gen_hd_next( gen_hd );
	payl_gen_hd_init( gen_hd, YAV_PAYL_UINT32, sizeof(payl_uint32_t) );
	payl_uint32_to_bin( payl_gen_hd_data(gen_hd), result );
	// build the inner address payload
	gen_hd			= payl_gen_hd_next( gen_hd );
	payl_gen_hd_init( gen_hd, YAV_PAYL_IP_ADDR, sizeof(payl_ip_addr_t) );
	if( result == IADDR_LOG_SERVER_OK )
		ip_addr_to_payload( payl_gen_hd_data(gen_hd), ip_addr );
	// build the inner address payload
	gen_hd			= payl_gen_hd_next( gen_hd );
	payl_gen_hd_init( gen_hd, YAV_PAYL_IP_NETMASK, sizeof(payl_ip_netmask_t) );
	if( result == IADDR_LOG_SERVER_OK )
		ip_netmask_to_payload( payl_gen_hd_data(gen_hd), &iaddr_log_server->inner_netmask );

	// sanity check on the packet grammar before sending it
	DBG_ASSERT(pkt_gram_is_valid( YAV_PKT_IADDR_REP, pkt, pkt_len ));
	// send the packet
	otsp_send( otsp_caddr, pkt, pkt_len );
	// free the packet
	nipmem_free( pkt );
	return 0;
}

/**
 * callback called from the unbound_inpkt
 */
static int iaddr_log_server_unbound_cb( void *userptr, otsp_caddr_t *otsp_caddr, char *pkt, int pkt_len )
{
	proto_hd_t	*hd 	= (proto_hd_t *)pkt;
	gen_hd_t	*gen_hd;
	nipid_t		remote_nodeid;
	payl_nounce_t	remote_nounce;	
	ip_addr_t	ip_addr;
	int		err;
	iaddr_log_server_result_t	result = IADDR_LOG_SERVER_OK;
	DBG("enter\n");
	// check payload grammar
	if( !pkt_gram_is_valid( YAV_PKT_IADDR_REQ, pkt, pkt_len ) )
		return YAV_ERR_BOGUS_PKT;	

	// read the remote_nodeid from the payload
	gen_hd	= payl_proto_hd_data( hd );
	nipid_build_from_bin( &remote_nodeid, payl_gen_hd_data(gen_hd) );
	// read the remote_nounce from the payload
	gen_hd	= payl_gen_hd_next( gen_hd );
	memcpy( &remote_nounce, payl_gen_hd_data(gen_hd), sizeof(payl_nounce_t) );		

	// get the allocated iaddr
	err = iaddr_log_allocate_iaddr( &remote_nodeid, &ip_addr );
	if( err )	result = IADDR_LOG_SERVER_NOT_AVAILABLE;
	DBG("err=%d ipaddr=%s\n", err, ip_addr_str( &ip_addr ) );

	// send the reply
	return iaddr_log_server_send_reply( otsp_caddr, &remote_nodeid, &remote_nounce, result, &ip_addr );
}

/**
 * return 1 is the iaddr_log_server is init, 0 otherwise
 */
int iaddr_log_server_is_init( void )
{
	if( !iaddr_log_server )			return 0;
	if( !iaddr_log_server->init_counter )	return 0;
	return 1;
}

/**
 * open the public address server
 */
int iaddr_log_server_open( void )
{
	char	fname[PATH_MAX];
	// to support multiple initialization
	if( iaddr_log_server_is_init() ){
		iaddr_log_server->init_counter++;
		return 0;
	}
	// allocate memory
	iaddr_log_server = nipmem_zalloc( sizeof( *iaddr_log_server ));
	// init the inner netmask and next_inner_addr
	iaddr_log_server->inner_netmask = *usercfg_get_inner_netmask();
	ip_netmask_first_addr( &iaddr_log_server->inner_netmask, &iaddr_log_server->next_inner_addr );
	// open the nodeid_iaddr_db
	nodeid_iaddr_db_open( &iaddr_log_server->nodeid_iaddr_db );
	// load the nodeid_iaddr_db
	snprintf(fname, sizeof(fname), "%snodeid_iaddr.db", usercfg_get_config_dir() );
	nodeid_iaddr_db_load( &iaddr_log_server->nodeid_iaddr_db, fname );
	// register the unbound_inpkt callback	
	unbound_inpkt_reg_local_cb( YAV_PKT_IADDR_REQ, iaddr_log_server_unbound_cb, NULL );
	// to support multiple initialisation
	iaddr_log_server->init_counter++;
	return 0;
}

/**
 * close the public address server
 */
void iaddr_log_server_close( void )
{
	char	fname[PATH_MAX];
	// to support multiple initialization
	iaddr_log_server->init_counter--;
	if( iaddr_log_server_is_init() )	return;
	// save the database
	snprintf(fname, sizeof(fname), "%snodeid_iaddr.db", usercfg_get_config_dir() );
	nodeid_iaddr_db_save( &iaddr_log_server->nodeid_iaddr_db, fname );
	// close the database
	nodeid_iaddr_db_close( &iaddr_log_server->nodeid_iaddr_db );
	// unregister the unbound_inpkt callback
	unbound_inpkt_unreg_local_cb( YAV_PKT_IADDR_REQ );
	// free memory
	nipmem_free( iaddr_log_server );
	iaddr_log_server = NULL;
}




