/*! \file
    \brief Declaration of the udp_itor_t
    
*/


#ifndef __NEOIP_NC2_CLIENT_HPP__ 
#define __NEOIP_NC2_CLIENT_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_udp.hpp"
#include "neoip_nlay_itor.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief do a unit test on \ref udp_itor_t
 * 
 * - it initiates a udp connection and periodically send packets over it
 */
class nc2_client_t : public udp_itor_cb_t, public udp_full_cb_t
				, public nlay_itor_cb_t, public nlay_full_cb_t
				, public timeout_cb_t {
private:
	udp_itor_t	udp_itor;	//!< to initiate the udp connection
	bool		neoip_inet_udp_itor_event_cb(void *userptr, udp_itor_t &cb_udp_itor
							, const udp_event_t &udp_event)	throw();
	udp_full_t *	udp_full;	//!< the udp_full_t struct
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw();
	nlay_itor_t *	nlay_itor;	//!< to initiate the nlay connection
	bool		neoip_nlay_itor_event_cb(void *cb_userptr, nlay_itor_t &cb_nlay_itor
						, const nlay_event_t &nlay_event)	throw();
	nlay_full_t *	nlay_full;
	bool		neoip_nlay_full_event_cb(void *cb_userptr, nlay_full_t &cb_nlay_full
						, const nlay_event_t &nlay_event)	throw();

	nlay_profile_t	nlay_profile;
	timeout_t	ping_timeout;	//!< to periodically send packet over the udp_full
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();	
public:
	nc2_client_t(const std::string &addr_str)		throw();
	~nc2_client_t()		throw();
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_NC2_CLIENT_HPP__ 



