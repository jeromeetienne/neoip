/*! \file
    \brief Declaration of the tcp_itor_t
    
*/


#ifndef __NEOIP_TCP_UTEST_ITOR_HPP__ 
#define __NEOIP_TCP_UTEST_ITOR_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_tcp.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief do a unit test on \ref tcp_itor_t
 * 
 * - it initiates a tcp connection and periodically send packets over it
 */
class utest_tcp_itor_t : public tcp_itor_cb_t, public tcp_full_cb_t, private timeout_cb_t {
private:
	tcp_itor_t	tcp_itor;	//!< to initiate the connection
	bool		neoip_tcp_itor_event_cb(void *userptr, tcp_itor_t &cb_tcp_itor
							, const tcp_event_t &tcp_event)
							throw();
	tcp_full_t *	tcp_full;	//!< the tcp_full_t struct
	bool		neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event)
							throw();
	timeout_t	ping_timeout;	//!< to periodically send packet over the tcp_full
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();	
public:
	utest_tcp_itor_t()		throw();
	~utest_tcp_itor_t()		throw();
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_TCP_UTEST_ITOR_HPP__ 



