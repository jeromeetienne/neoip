/*! \file
    \brief Declaration of the ntudp_itor_estarelay_2pserver_cb_t
    
*/


#ifndef __NEOIP_NTUDP_ITOR_ESTARELAY_2PSERVER_CB_HPP__ 
#define __NEOIP_NTUDP_ITOR_ESTARELAY_2PSERVER_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_itor_estarelay_2pserver_t;
class ipport_addr_t;


/** \brief the callback class for ntudp_itor_estarelay_2pserver_t
 */
class ntudp_itor_estarelay_2pserver_cb_t {
public:
	/** \brief callback notified by \ref ntudp_itor_estarelay_2pserver_t when the remote_peer_ipport
	 *         is known
	 * 
	 * @param userptr  		the userptr associated with this callback
	 * @param cb_estarelay_2pserver the ntudp_itor_t which notified this callback
	 * @param remote_peer_ipport	the ipport_addr_t of the remote peer - if .is_null(), it failed
	 * @return a tokeep aka false if the notifier has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_ntudp_itor_estarelay_2pserver_cb(void *cb_userptr
				, ntudp_itor_estarelay_2pserver_t &cb_estarelay_2pserver
				, const ipport_addr_t &remote_peer_ipport)	throw() = 0;
	virtual ~ntudp_itor_estarelay_2pserver_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_ITOR_ESTARELAY_2PSERVER_CB_HPP__  */



