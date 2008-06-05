/*! \file
    \brief Declaration of the ntudp_dircnx_server_t
    
*/


#ifndef __NEOIP_NTUDP_DIRCNX_SERVER_HPP__ 
#define __NEOIP_NTUDP_DIRCNX_SERVER_HPP__ 
/* system include */
#include <list>
#include <map>
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_udp_vresp_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class ntudp_peer_t;

/** \brief Server to accept direct connection
 * 
 * - this is used by the ntudp_pserver_t *and* by the ntudp_relpeer_t as they are both
 *   able to accept direction connection
 */
class ntudp_dircnx_server_t : private udp_vresp_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	ntudp_peer_t *	ntudp_peer;	//!< the ntudp_peer_t to which it is attached

	/*************** Handle the udp_vresp_t	*******************************/
	bool neoip_inet_udp_vresp_event_cb(void *cb_userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event)	throw();	

	/*************** List of incoming connection	***********************/
	class				cnx_t;
	std::list<cnx_t *>		cnx_list;
	void cnx_link(cnx_t *cnx)	throw()	{ cnx_list.push_back(cnx);	}
	void cnx_unlink(cnx_t *cnx)	throw()	{ cnx_list.remove(cnx);		}
public:
	/*************** ctor/dtor	***************************************/
	ntudp_dircnx_server_t()		throw();
	~ntudp_dircnx_server_t()	throw();

	/*************** Setup function	***************************************/
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_DIRCNX_SERVER_HPP__  */



