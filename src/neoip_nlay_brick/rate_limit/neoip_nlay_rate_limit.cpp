/*! \file
    \brief Definition of the nlay_rate_limit_t

\par Implementation Notes
- this layer is too simple to be coded externally to nlay_full_t

*/

/* system include */
/* local include */
#include "neoip_nlay_rate_limit.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


// definition of \ref rate_limit_t constant
#if 1
	const uint64_t	nlay_rate_limit_t::XMIT_MAX_THROUGHPUT_DFL	= 0;
	const delay_t	nlay_rate_limit_t::XMIT_ESTIM_RANGE_DFL		= delay_t::from_sec(20);
	const delay_t	nlay_rate_limit_t::XMIT_ESTIM_GRANULARITY_DFL	= delay_t::from_msec(500);
	const uint64_t	nlay_rate_limit_t::RECV_MAX_THROUGHPUT_DFL	= 0;
	const delay_t	nlay_rate_limit_t::RECV_ESTIM_RANGE_DFL		= delay_t::from_sec(20);
	const delay_t	nlay_rate_limit_t::RECV_ESTIM_GRANULARITY_DFL	= delay_t::from_msec(500);
#else	// value for debug
	const uint64_t	nlay_rate_limit_t::XMIT_MAX_THROUGHPUT_DFL	= 5*1024;
	const delay_t	nlay_rate_limit_t::XMIT_ESTIM_RANGE_DFL		= delay_t::from_sec(20);
	const delay_t	nlay_rate_limit_t::XMIT_ESTIM_GRANULARITY_DFL	= delay_t::from_msec(500);
	const uint64_t	nlay_rate_limit_t::RECV_MAX_THROUGHPUT_DFL	= 5*1024;
	const delay_t	nlay_rate_limit_t::RECV_ESTIM_RANGE_DFL		= delay_t::from_sec(20);
	const delay_t	nlay_rate_limit_t::RECV_ESTIM_GRANULARITY_DFL	= delay_t::from_msec(500);
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_rate_limit_t::nlay_rate_limit_t()	throw()
{
	// set default value
	xmit_max_throughput	= XMIT_MAX_THROUGHPUT_DFL;
	recv_max_throughput	= RECV_MAX_THROUGHPUT_DFL;
		
	// set up the rat0_limit_t for xmit/recv
	xmit_limit	= rat0_limit_t<uint64_t>(XMIT_ESTIM_RANGE_DFL, XMIT_ESTIM_GRANULARITY_DFL);
	recv_limit	= rat0_limit_t<uint64_t>(RECV_ESTIM_RANGE_DFL, RECV_ESTIM_GRANULARITY_DFL);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 function inherited from nlay_full_api_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */        
void	nlay_rate_limit_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{	
	// register a it as a default packet handler
	regpkt_lower->set_dfl_handler(this);	
}

/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_rate_limit_t::set_parameter(const nlay_profile_t *nlay_profile
					, const nlay_nego_result_t *nego_result)	throw()
{
	const nlay_rate_limit_profile_t	&rate_limit_profile	= nlay_profile->rate_limit();
	// check the profile
	nlay_err_t	err = rate_limit_profile.check();
	if( err.failed() )	return err;
		
	// copy the profile data
	xmit_max_throughput	= rate_limit_profile.xmit_max_throughput();
	recv_max_throughput	= rate_limit_profile.recv_max_throughput();
	// set up the rate_limit_t for xmit/recv
	xmit_limit	= rat0_limit_t<uint64_t>(rate_limit_profile.xmit_estim_range()
						, rate_limit_profile.xmit_estim_granularity());
	recv_limit	= rat0_limit_t<uint64_t>(rate_limit_profile.recv_estim_range()
						, rate_limit_profile.recv_estim_granularity());
	
	return nlay_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  pkt filtering from lower/upper layer
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer
 */
nlay_err_t	nlay_rate_limit_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	// if no limit is set, do nothing
	if( xmit_max_throughput == 0 )	return nlay_err_t::OK;
	// if this packet would exceed the limit, swallow it
	if( xmit_limit.would_exceed(pkt.get_len(), xmit_max_throughput) ){
		pkt = pkt_t();
		return nlay_err_t::OK;
	}
	// count it in the limit
	xmit_limit.count(pkt.get_len());
	return nlay_err_t::OK;
}

/** \brief handle packet from the lower network layer
 */
nlay_err_t	nlay_rate_limit_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	// if no limit is set, do nothing
	if( recv_max_throughput == 0 )	return nlay_err_t::OK;
	// if this packet would exceed the limit, swallow it
	if( recv_limit.would_exceed(pkt.get_len(), recv_max_throughput) ){
		pkt = pkt_t();
		return nlay_err_t::OK;
	}
	// count it in the limit
	recv_limit.count(pkt.get_len());
	return nlay_err_t::OK;
}



NEOIP_NAMESPACE_END


