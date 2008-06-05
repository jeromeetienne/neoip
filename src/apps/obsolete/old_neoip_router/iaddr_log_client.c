/*! \file
    \brief the internal address allocated on loggin client

*/

/* system include */

/* local include */
#include "iaddr_log_client.h"
#include "iaddr_log_server.h"
#include "util.h"
#include "otsp.h"
#include "nipid.h"
#include "payl.h"
#include "packet.h"
#include "rand.h"

/**
 * return a string describing this iaddr_log_client_t
 */
static char *iaddr_log_client_str( iaddr_log_client_t *iaddr_log_client )
{
	static	char	buf1[300];
	static	char	buf2[300];
	static 	int	var;
	char	*str;
	
	/* swap buffers */
	var	^= 1;
	str	= var ? buf1 : buf2;
	/* put string in buffer */
	snprintf( str, sizeof(buf1), "iaddr_log_client(caddr=%s)", otsp_caddr_str(&iaddr_log_client->client_caddr));
	return str;	
}

/**
 * return failure to the caller
 */
static void iaddr_log_client_report_failure( iaddr_log_client_t *iaddr_log_client, iaddr_log_client_state_t state )
{
	// report data and faillure on the callback
	if( iaddr_log_client->callback )
		iaddr_log_client->callback( iaddr_log_client->userptr, state, NULL, NULL );
}

/**
 * the otsp_cb_t to receive the packet on this connection
 */
static int iaddr_log_client_otsp_inerr_cb( void *userptr, otsp_caddr_t *otsp_caddr
			, char *pkt, int pkt_len, otsp_err_t *otsp_err )
{
	iaddr_log_client_t *iaddr_log_client	= userptr;
	proto_hd_t		*hd		= (proto_hd_t *)pkt;
	gen_hd_t		*gen_hd;
	nipid_t			nodeid;
	payl_nounce_t		payl_nounce;
	iaddr_log_server_result_t result;
	ip_addr_t		inner_addr;
	ip_netmask_t		inner_netmask;
	DBG("enter\n");
// handle incoming error if needed
	if( otsp_err ){
		LOGM_NOTICE("%s received an outter connection error %s\n"
			, iaddr_log_client_str(iaddr_log_client), otsp_err_str(otsp_err) );
		// if the error is fatal, declare the server as unreachable
		if( otsp_err_is_fatal(otsp_caddr, otsp_err) ){
			iaddr_log_client_report_failure( iaddr_log_client, IADDR_LOG_CLIENT_STATE_UNREACHABLE );
			// close otsp connection
			otsp_client_close( &iaddr_log_client->client_caddr );
			memset( &iaddr_log_client->client_caddr, 0, sizeof(iaddr_log_client->client_caddr) );
		}
		return 0;
	}
// handle the incoming packet
	// check payload grammar
	if( !pkt_gram_is_valid( YAV_PKT_IADDR_REP, pkt, pkt_len ) )
		return YAV_ERR_BOGUS_PKT;
	// read the remote_nodeid from the payload
	gen_hd		= payl_proto_hd_data( hd );
	nipid_build_from_bin( &nodeid, payl_gen_hd_data(gen_hd) );
	if( !nipid_is_equal( &nodeid, &iaddr_log_client->nodeid ) )
		return YAV_ERR_INV_NIPID;
	// get the nounce
	gen_hd		= payl_gen_hd_next( gen_hd );
	memcpy( &payl_nounce, payl_gen_hd_data(gen_hd), sizeof(payl_nounce) );
	if( memcmp(payl_gen_hd_data(gen_hd), &iaddr_log_client->payl_nounce, sizeof(payl_nounce) ) )
		return YAV_ERR_INV_NOUNCE;
	// get the result
	gen_hd		= payl_gen_hd_next( gen_hd );
	result		= payl_uint32_from_bin( payl_gen_hd_data(gen_hd) );
	
	// test the result
	if( result != IADDR_LOG_SERVER_OK ){
		iaddr_log_client_report_failure( iaddr_log_client, IADDR_LOG_CLIENT_STATE_NOT_AVAILABLE );
		return 0;
	}
	/* parse the IP_ADDR payload */
	gen_hd 		= payl_gen_hd_next( gen_hd );
	ip_addr_from_payload( &inner_addr, payl_gen_hd_data( gen_hd ) );
	/* parse the IP_NETMASK payload */
	gen_hd 		= payl_gen_hd_next( gen_hd );
	ip_netmask_from_payload( &inner_netmask, payl_gen_hd_data( gen_hd ) );
	DBG("ip_addr=%s ip_netmask=%s\n", ip_addr_str(&inner_addr), ip_netmask_str(&inner_netmask) );

	// notify the keep alive i received a proof
	pkt_keepalive_rcved_proof( &iaddr_log_client->pkt_keepalive );

	// report data and success on the callback
	if( iaddr_log_client->callback ){
		iaddr_log_client->callback( iaddr_log_client->userptr, IADDR_LOG_CLIENT_STATE_SUCCEED
						, &inner_addr, &inner_netmask );
	}
	return 0;
}

/**
 * build a request packet
 */
static int iaddr_log_client_build_request_pkt( iaddr_log_client_t *iaddr_log_client, char **pkt_return )
{
	int		pkt_len	= sizeof( proto_hd_t )
				+ sizeof( gen_hd_t ) + sizeof(payl_nipid_t)
				+ sizeof( gen_hd_t ) + sizeof(payl_nounce_t);
	char		*pkt 	= nipmem_zalloc( pkt_len );
	proto_hd_t 	*hd	= (void *) pkt;
	gen_hd_t	*gen_hd;

	// init the header
	payl_proto_hd_init( hd, YAV_PKT_IADDR_REQ );

	// init the payl_nipid for the nipid
	gen_hd		= payl_proto_hd_data( hd );
	payl_gen_hd_init( gen_hd, YAV_PAYL_NIPID, sizeof(payl_nipid_t) );
	nipid_build_to_bin(payl_gen_hd_data( gen_hd ), &iaddr_log_client->nodeid);
	// init the payl_nounce
	gen_hd		= payl_gen_hd_next( gen_hd );
	payl_gen_hd_init( gen_hd, YAV_PAYL_NOUNCE, sizeof(payl_nounce_t) );
	memcpy( payl_gen_hd_data( gen_hd ), &iaddr_log_client->payl_nounce, sizeof(payl_nounce_t) );

	// sanity check on the packet grammar before sending it
	DBG_ASSERT(pkt_gram_is_valid( YAV_PKT_IADDR_REQ, pkt, pkt_len ));

	// return the result
	*pkt_return = pkt;
	return pkt_len;
}

/**
 * callback called by pkt_keepalive when the reachability state change
 */
static void iaddr_log_client_newstate_cb( void *userptr, pkt_keepalive_state_t keepalive_state )
{
	iaddr_log_client_t	*iaddr_log_client 	= userptr;
	if( keepalive_state == PKT_KEEPALIVE_STATE_UNREACHABLE ){
		LOGM_INFO("%s server is declared unreachable\n", iaddr_log_client_str(iaddr_log_client));
		iaddr_log_client_report_failure( iaddr_log_client, IADDR_LOG_CLIENT_STATE_UNREACHABLE );
	}
}

/**
 * callback called by pkt_keepalive to send a packet
 */
static void iaddr_log_client_sendpkt_cb( void *userptr )
{
	iaddr_log_client_t	*iaddr_log_client	= userptr;
	// if otsp connection isnt yet init, do it now
	if( otsp_caddr_is_null(&iaddr_log_client->client_caddr) ){
		int err = otsp_client_connect( &iaddr_log_client->client_caddr
				, &iaddr_log_client->server_addr
				, iaddr_log_client_otsp_inerr_cb, iaddr_log_client );
		if( err ){
			LOGM_NOTICE("unable to connect %s\n", otsp_addr_str(&iaddr_log_client->server_addr) );
			return;
		}
	}
	// send the packet over the otsp_client
	otsp_send( &iaddr_log_client->client_caddr, iaddr_log_client->pkt, iaddr_log_client->pkt_len );
}

/**
 * open a iaddr_log client
 */
int iaddr_log_client_open( iaddr_log_client_t *iaddr_log_client, nipid_t *nodeid
				, otsp_addr_t *server_addr
				, iaddr_log_client_cb_t callback, void *userptr )
{
	DBG("start a iaddr_log client toward %s\n", otsp_addr_str(server_addr));
	// zero the struct
	memset( iaddr_log_client, 0, sizeof(*iaddr_log_client) );
	// fill the struct
	iaddr_log_client->nodeid	= *nodeid;
	iaddr_log_client->callback	= callback;
	iaddr_log_client->userptr	= userptr;
	iaddr_log_client->state 	= IADDR_LOG_CLIENT_STATE_QUERYING;
	iaddr_log_client->server_addr	= *server_addr;
	// init the nounce
	rand_get_deriv( &iaddr_log_client->payl_nounce, sizeof(payl_nounce_t) );
	// otsp client on the fly

	// build the packet
	iaddr_log_client->pkt_len = iaddr_log_client_build_request_pkt(iaddr_log_client, &iaddr_log_client->pkt );
	EXP_ASSERT( iaddr_log_client->pkt_len != -1 );

	// init packet keepalive
	pkt_keepalive_open( &iaddr_log_client->pkt_keepalive
				, IADDR_LOG_CLIENT_ALIVE_DELAY, IADDR_LOG_CLIENT_DEAD_DELAY
				, IADDR_LOG_CLIENT_PROBE_PERIOD, IADDR_LOG_CLIENT_PROBE_NB
				, iaddr_log_client_newstate_cb, iaddr_log_client_sendpkt_cb
				, iaddr_log_client );
	return 0;
}

/**
 * close a iaddr_log client
 */
void iaddr_log_client_close( iaddr_log_client_t *iaddr_log_client )
{
	DBG("close a iaddr_log client %s\n", iaddr_log_client_str(iaddr_log_client));
	// close the packet keep alive
	pkt_keepalive_close( &iaddr_log_client->pkt_keepalive );
	// free the packet
	nipmem_free( iaddr_log_client->pkt );
	// close the otsp client
	if( !otsp_caddr_is_null(&iaddr_log_client->client_caddr) )
		otsp_client_close( &iaddr_log_client->client_caddr );
	// zero the memory
	memset( iaddr_log_client, 0, sizeof(*iaddr_log_client) );	
}





