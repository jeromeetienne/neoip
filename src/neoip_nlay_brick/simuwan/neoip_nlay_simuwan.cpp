/*! \file
    \brief Definition of the nlay_simuwan_t
*/

/* system include */
/* local include */
#include "neoip_nlay_simuwan.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_simuwan_t::nlay_simuwan_t()	throw()
{
}

/** \brief Destructor
 */
nlay_simuwan_t::~nlay_simuwan_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         nlay_full_api_t functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_simuwan_t::set_parameter(const nlay_profile_t *nlay_profile
					, const nlay_nego_result_t *nego_result)	throw()
{
	simuwan_err_t	err = simuwan.set_from_profile(nlay_profile->simuwan());
	if( err.failed() )	return nlay_err_from_simuwan(err);
	return nlay_err_t::OK;
}

/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_simuwan_t::start()			throw()
{
	// Start the action
	simuwan_err_t	simuwan_err = simuwan.start(this, NULL);
	if( !simuwan_err.succeed() )	return nlay_err_from_simuwan(simuwan_err);
	return nlay_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  pkt filtering from lower/upper layer
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer
 * 
 */
nlay_err_t	nlay_simuwan_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	simuwan_err_t	simuwan_err	= simuwan.pkt_from_upper(pkt);
	// handle the error
	if( !simuwan_err.succeed() )	return nlay_err_from_simuwan(simuwan_err);
	// else report that no error occured
	return nlay_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    simuwan callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief the callback class to notify event from \ref simuwan_t
 */
bool	nlay_simuwan_t::neoip_simuwan_event_cb(void *cb_userptr, simuwan_t &cb_simuwan
					, const simuwan_event_t &simuwan_event ) throw()
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	nlay_event_t	nlay_event;
	pkt_t *		pkt;
	
	// handle the event depending of its type
	switch(simuwan_event.get_value()){
	case simuwan_event_t::PKT_TO_LOWER:
			// process the packet thru the lower layers of nlay_full_t
			pkt = simuwan_event.get_pkt_to_lower();
			return nlay_full->notify_pkt_to_lower(*pkt, get_full_api_lower());
	default:	DBG_ASSERT(0);
	}
	return true;
}

NEOIP_NAMESPACE_END


