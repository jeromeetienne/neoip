

#ifndef __NEOIP_NTUDP_SOCKET_NUNIT_CLIENT_CLOSE_HPP__ 
#define __NEOIP_NTUDP_SOCKET_NUNIT_CLIENT_CLOSE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_socket_nunit_client_close_cb.hpp"
#include "neoip_ntudp_client_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_peerid_t;

/** \brief Class which implement a nunit for the ntudp_client_t which close immediatly
 */
class ntudp_socket_nunit_client_close_t : private ntudp_client_cb_t	{
private:
	/*************** ntudp_itor_t stuff	*******************************/
	ntudp_client_t*		ntudp_client;
	bool neoip_ntudp_client_event_cb(void *cb_userptr, ntudp_client_t &cb_ntudp_client
							, const ntudp_event_t &ntudp_event)	throw();
	/*************** callback	***************************************/
	ntudp_socket_nunit_client_close_cb_t *	callback;	//!< the callback to notify in case of event
	void *			userptr;	//!< the userptr associated with the event callback
	bool			notify_callback(bool succeed)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_socket_nunit_client_close_t()	throw();
	~ntudp_socket_nunit_client_close_t()	throw();

	/*************** Setup function	***************************************/
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer, const ntudp_peerid_t &remote_peerid
					, ntudp_socket_nunit_client_close_cb_t *callback
					, void *userptr)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_SOCKET_NUNIT_CLIENT_CLOSE_HPP__  */



