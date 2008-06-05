/*! \file
    \brief Definition of the \ref nlay_full_t

*/


/* system include */
/* local include */
#include "neoip_nlay_full.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// include the nlay_closure_t due to its particular handling when nlay_full_t is closing
#include "neoip_nlay_closure.hpp"

// include for the nlay_full_upapi_t
#include "neoip_nlay_scnx_full.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief default constructor
 */
nlay_full_t::nlay_full_t()	throw()
{
	uppest_full_api	= NULL;
	lowest_regpkt	= NULL;

	// zero some nlay_full_api_t which need direct access
	nlay_closure	= NULL;
	nlay_scnx_full	= NULL;
	nlay_reliability= NULL;
}

/** \brief Destructor
 */
nlay_full_t::~nlay_full_t()	throw()
{
	// if there is a closure brick, notify it of the imminent destruction
	// - NOTE: this function sends packet thru the stack - to use with care
	// - especially because we are in a destructor ... real dirty ...
	// - TODO maybe another function would be better...
	if( nlay_closure )	nlay_closure->notify_imminent_destroy();
	// delete the whole stack
	stack_destroy();
}

/** \brief Constructor with value
 */
nlay_full_t::nlay_full_t(const nlay_profile_t *nlay_profile, const nlay_nego_result_t *nego_result
				, const nlay_type_t &inner_type, const nlay_type_t &outter_type
				, scnx_full_t *scnx_full)			throw()
{
	// zero some nlay_full_api_t which need direct access
	nlay_closure	= NULL;
	nlay_scnx_full	= NULL;
	nlay_reliability= NULL;

	// set the inner and outter nlay_type_t	
	this->inner_type	= inner_type;
	this->outter_type	= outter_type;

	// build the stack
	switch( outter_type.get_value() ){
	case nlay_type_t::DGRAM:	stack_build_outter_dgram(scnx_full);	break;
	case nlay_type_t::STREAM:	//build_stack_stream(scnx_full);	break;
					// TODO not yet coded. 
					// - waiting to be stabilize on DGRAM
					// - moreover the outter transport in STREAM isnt yet done
					// - moreover there are still issue to fix
					//   - how to report the maysend
					//   - how to handle sendbuf (all in tcp ? or additionnal 
					//     internal buffer)
	default:	DBG_ASSERT(0);
	}

	// set the parameter for the whole stack	
	nlay_err_t	nlay_err = stack_set_parameter(nlay_profile, nego_result);
	if( nlay_err.failed() )	stack_destroy();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start() functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
nlay_err_t	nlay_full_t::start(nlay_full_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// start the whole stack
	return stack_start();
}

/** \brief Start the closure process
 */
void	nlay_full_t::start_closure()					throw()
{
	// start the closure
	stack_start_closure();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   nlay_scnx_full_api_t function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to access nlay_scnx_full_api_t of this nlay_full_t
 */
const nlay_scnx_full_api_t &	nlay_full_t::scnx()	const throw()
{
	// sanity check nlay_scnx_full_t MUST be non null
	DBG_ASSERT( nlay_scnx_full );
	// return the nlay_scnx_full
	return *nlay_scnx_full;
}
	
/** \brief to access nlay_scnx_full_api_t of this nlay_full_t
 */
nlay_scnx_full_api_t &		nlay_full_t::scnx()		throw()
{
	// sanity check nlay_scnx_full_t MUST be non null
	DBG_ASSERT( nlay_scnx_full );
	// return the nlay_scnx_full
	return *nlay_scnx_full;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  notification from the nlay_full_api_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify a nlay_event_t directly to the nlay_full_t caller.
 * 
 * - this event MUST NOT be PKT_TO_LOWER or PKT_TO_UPPER.
 *   - for those events, use the specific function, notify_pkt_to_lower() and notify_pkt_to_upper()
 */
bool	nlay_full_t::notify_event(const nlay_event_t &nlay_event)	throw()
{
	// sanity check - here the event MUST NOT be pkt_to_lower() or pkt_to_upper()
	DBG_ASSERT( !nlay_event.is_pkt_to_lower() );
	DBG_ASSERT( !nlay_event.is_pkt_to_upper() );
	// notify the caller
	return notify_callback(nlay_event);
}

/** \brief handle packet from the upper layer
 * 
 * - it goes throught the lower bricks and then notify the event
 */
bool	nlay_full_t::notify_pkt_to_lower(pkt_t &pkt, nlay_full_api_t *full_api_lower)	throw()
{
	nlay_err_t	nlay_err;
	// process the packet thru the stack
	nlay_err = pkt_from_upper_api(pkt, full_api_lower);
	if( !nlay_err.succeed() ){
		KLOG_ERR("Cant process packet due to " + nlay_err.to_string());
		return true;
	}
	// if the packet has been swallowed, return now
	if( pkt.is_null() )	return true;
	// send a nlay_event
	return notify_callback( nlay_event_t::build_pkt_to_lower(&pkt) );
}

/** \brief handle packet from the upper layer
 *  
 * - it goes throught the upper bricks and then notify the event
 */
bool	nlay_full_t::notify_pkt_to_upper(pkt_t &pkt, nlay_regpkt_t *first_regpkt)	throw()
{
	nlay_err_t	nlay_err;
	// process the packet thru the stack
	nlay_err = pkt_from_lower_api(pkt, first_regpkt);
	if( !nlay_err.succeed() ){
		KLOG_ERR("Cant process packet due to " + nlay_err.to_string());
		return true;
	}
	// if the packet has been swallowed, return now
	if( pkt.is_null() )		return true;
	// send a nlay_event
	return notify_callback( nlay_event_t::build_pkt_to_lower(&pkt) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                internal function for pkt_from_lower/upper
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief handle packet from the upper layer for full connection
 */
nlay_err_t	nlay_full_t::pkt_from_lower_api(pkt_t &pkt, nlay_regpkt_t *first_regpkt)	throw()
{	
	nlay_full_api_t *	full_api;
	nlay_regpkt_t *		regpkt;	
	nlay_err_t		nlay_err;

	// go thru all the nlay_regpkt_t until the upper regpkt is null
	for(regpkt = first_regpkt; regpkt; regpkt = full_api->get_regpkt_upper()){
		nlay_pkttype_t	pkttype;
		// read the pkttype (without consuming)	
		try {
			pkt.unserial_peek( pkttype );
		}catch(serial_except_t &e){
			return nlay_err_t(nlay_err_t::BOGUS_PKT, "Cant read the packet type due to " + e.what());
		}
		// find the nlay_full_api_t matching this pkttype
		full_api	= regpkt->find(pkttype);
		if( !full_api )
			return nlay_err_t(nlay_err_t::BOGUS_PKT, "Unknown packet type :" + pkttype);
		// process this packet
		nlay_err	= full_api->pkt_from_lower(pkt);
		// if the processing fails, forward the error
		if( nlay_err.failed() )	return nlay_err;
		// if the packet is swallowed, stop
		if( pkt.is_null() )	return nlay_err_t::OK;	
	}
	// NOTE: here no error occured and there is a packet to deliver


	// return no error
	return nlay_err_t::OK;	
}

/** \brief handle packet from the upper layer for full connection
 */
nlay_err_t	nlay_full_t::pkt_from_upper_api(pkt_t &pkt, nlay_full_api_t *full_api_lower)	throw()
{
	nlay_full_api_t *	full_api;
	nlay_err_t		nlay_err;

	// go lower thru the stack from full_api_lower
	// - aka go thru all the full_api_t until it is null
	for( full_api = full_api_lower; full_api; full_api = full_api->get_full_api_lower()){
		// process this packet
		nlay_err	= full_api->pkt_from_upper(pkt);
		// if the processing fails, forward the error
		if( nlay_err.failed() )	return nlay_err;
		// if the packet has been swallowed, return now
		if( pkt.is_null() )	return nlay_err;
	}

	// return no error
	return nlay_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           pkt from upper/lower
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from upper - upper the whole nlay_full_t
 * 
 * - it goes thru the whole nlay_full_t stack
 */
nlay_err_t	nlay_full_t::pkt_from_upper(pkt_t &pkt)	throw()
{
	return pkt_from_upper_api(pkt, uppest_full_api);
}

/** \brief handle packet from lower - lower the whole nlay_full_t stack
 * 
 * - it goes thru the whole nlay_full_t stack
 */
nlay_err_t	nlay_full_t::pkt_from_lower(pkt_t &pkt)	throw()
{
	nlay_err_t	nlay_err;
	// go thru the rest of the bricks
	nlay_err	= pkt_from_lower_api(pkt, lowest_regpkt);
	if( nlay_err.failed() )	return nlay_err;
	// if the packet is swallowed, return OK now
	if( pkt.is_null() )	return nlay_err_t::OK;
	// return noerror
	return nlay_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	nlay_full_t::notify_callback(const nlay_event_t &nlay_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_nlay_full_event_cb( userptr, *this, nlay_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


