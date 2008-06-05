

#ifndef __NEOIP_NTUDP_SOCKET_NUNIT_SERVER_CLOSE_HPP__ 
#define __NEOIP_NTUDP_SOCKET_NUNIT_SERVER_CLOSE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_resp_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;

/** \brief Class which implement a nunit for the ntudp_resp_t which close immediatly
 */
class ntudp_socket_nunit_server_close_t : private ntudp_resp_cb_t {
private:
	/*************** ntudp_resp_t stuff	*******************************/
	ntudp_resp_t *		ntudp_resp;
	bool neoip_ntudp_resp_event_cb(void *cb_userptr, ntudp_resp_t &cb_ntudp_resp
							, const ntudp_event_t &ntudp_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_socket_nunit_server_close_t()	throw();
	~ntudp_socket_nunit_server_close_t()	throw();

	/*************** Setup function	***************************************/
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_SOCKET_NUNIT_SERVER_CLOSE_HPP__  */



