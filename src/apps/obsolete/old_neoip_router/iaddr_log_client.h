/*! \file
    \brief .h of the inner address on loggin client
*/

#ifndef __IADDR_LOG_CLIENT_H__
#define __IADDR_LOG_CLIENT_H__

/* system include */
/* local include */
#include "otsp_addr.h"
#include "ip_addr.h"
#include "nodeid_iaddr.h"
#include "pkt_keepalive.h"

#define IADDR_LOG_CLIENT_PROBE_PERIOD	1000		//!< time between probe
#define IADDR_LOG_CLIENT_PROBE_NB	10		//!< nb probe to try before declaring it unreachable
#define IADDR_LOG_CLIENT_ALIVE_DELAY	(30*60*1000)	//!< delay between proof until it is declared unreachable
#define IADDR_LOG_CLIENT_DEAD_DELAY	(5*1000)	//!< time to wait before retrying after declared unreachable

//! the list of state used by rdv_client
typedef enum {
	IADDR_LOG_CLIENT_STATE_NONE,
	IADDR_LOG_CLIENT_STATE_QUERYING,	//!< when the client is trying to reach the server
	IADDR_LOG_CLIENT_STATE_SUCCEED,		//!< when the client has successfully registered to the server
	IADDR_LOG_CLIENT_STATE_NOT_AVAILABLE,	//!< when the server reports no more address are available
	IADDR_LOG_CLIENT_STATE_UNREACHABLE,	//!< when the server became unreachable
	IADDR_LOG_CLIENT_STATE_MAX
} iaddr_log_client_state_t;

//! the type definition of the callback
typedef void (*iaddr_log_client_cb_t)( void *userptr, iaddr_log_client_state_t state,
				 ip_addr_t *ip_addr, ip_netmask_t *ip_netmask );

typedef struct {
	iaddr_log_client_state_t	state;	//!< the current state of the client
	// network stuff
	otsp_caddr_t		client_caddr;	//!< the otsp client connection
	otsp_addr_t		server_addr;	//!< address of the server
	// callback stuff
	iaddr_log_client_cb_t	callback;	//!< callback to call when a result is known
	void			*userptr;	//!< userptr associated with the above callback

	nipid_t			nodeid;
	payl_nounce_t		payl_nounce;

	char			*pkt;			//!< request sent by pkt_keepalive
	int			pkt_len;		//!< length of the pkt
	pkt_keepalive_t		pkt_keepalive;		//!< keepalive which ensure pkt rxmit and timer (run only when not in UNREACHABLE)
} iaddr_log_client_t;

/* prototype definition */
int iaddr_log_client_open( iaddr_log_client_t *iaddr_log_client, nipid_t *nodeid
				, otsp_addr_t *server_addr
				, iaddr_log_client_cb_t callback, void *userptr );
void iaddr_log_client_close( iaddr_log_client_t *iaddr_log_client );


#endif /* __IADDR_LOG_CLIENT_H__ */




