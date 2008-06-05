/*! \file
    \brief Header of the nlay_idletimeout_t

*/


#ifndef __NEOIP_NLAY_IDLETIMEOUT_HPP__
#define __NEOIP_NLAY_IDLETIMEOUT_HPP__

/* system include */
/* local include */
#include "neoip_nlay_full_api.hpp"
#include "neoip_timeout.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief to a idle timeout on \ref nlay_full_t
 * 
 * - idle timeout means the caller doesnt transmit or receive data on this socket
 *   - aka all the signalisation packets internal to nlay_full_t are not taken into account
 */
class nlay_idletimeout_t : public timeout_cb_t, public nlay_full_api_t {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default amount of time before expiring the idle timeout
	static const delay_t	IDLE_DELAY_DFL;
private:
	delay_t		idle_delay;
	timeout_t	idle_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();

	void		recv_proof()					throw();
public:
	// ctor/dtor
	nlay_idletimeout_t()	throw();
	
	// function inherited from nlay_full_api_t
	void		register_handler(nlay_regpkt_t *regpkt_lower)	throw();
	nlay_err_t	set_parameter(const nlay_profile_t *nlay_profile
				, const nlay_nego_result_t *nego_result)	throw();	
	nlay_err_t	pkt_from_upper(pkt_t &pkt)				throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)				throw();	
	nlay_err_t	start()							throw();
	void		start_closure()						throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_idletimeout_t);	
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_IDLETIMEOUT_HPP__ 



