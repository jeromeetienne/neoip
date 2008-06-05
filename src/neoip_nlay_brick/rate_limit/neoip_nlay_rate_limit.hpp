/*! \file
    \brief Header of the nlay_rate_limit_t

*/


#ifndef __NEOIP_NLAY_RATE_LIMIT_HPP__
#define __NEOIP_NLAY_RATE_LIMIT_HPP__

/* system include */
/* local include */
#include "neoip_nlay_full_api.hpp"
#include "neoip_rat0_limit.hpp"
#include "neoip_timeout.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief to a idle timeout on \ref nlay_full_t
 * 
 * - idle timeout means the caller doesnt transmit or receive data on this socket
 *   - aka all the signalisation packets internal to nlay_full_t are not taken into account
 */
class nlay_rate_limit_t : public nlay_full_api_t {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default max throughput for xmit (expressed in byte per sec)
	static const uint64_t	XMIT_MAX_THROUGHPUT_DFL;
	//!< the default estimation range for xmit
	static const delay_t	XMIT_ESTIM_RANGE_DFL;
	//!< the default estimation granularity for xmit
	static const delay_t	XMIT_ESTIM_GRANULARITY_DFL;
	//! the default max throughput for recv (expressed in byte per sec)
	static const uint64_t	RECV_MAX_THROUGHPUT_DFL;
	//!< the default estimation range for recv
	static const delay_t	RECV_ESTIM_RANGE_DFL;
	//!< the default estimation granularity for recv
	static const delay_t	RECV_ESTIM_GRANULARITY_DFL;
private:
	uint64_t		xmit_max_throughput;
	rat0_limit_t<uint64_t>	xmit_limit;

	uint64_t		recv_max_throughput;
	rat0_limit_t<uint64_t>	recv_limit;
public:
	// ctor/dtor
	nlay_rate_limit_t()	throw();
	
	// function inherited from nlay_full_api_t
	void		register_handler(nlay_regpkt_t *regpkt_lower)	throw();
	nlay_err_t	set_parameter(const nlay_profile_t *nlay_profile
				, const nlay_nego_result_t *nego_result)	throw();	
	nlay_err_t	pkt_from_upper(pkt_t &pkt)				throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)				throw();	
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_rate_limit_t);	
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_RATE_LIMIT_HPP__ 



