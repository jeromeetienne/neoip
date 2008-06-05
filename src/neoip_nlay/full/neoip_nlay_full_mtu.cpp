/*! \file
    \brief Definition of the \ref nlay_full_t specific to the mtu

// - TODO currently the MTU from udp is reported here.. updated with the stack
//   overhead and then from here, the mtu is reported back via nlay event
// - so from socket_full_udp_t, the event comes from udp_full_t, it is forwarded to 
//   nlay_full_t and then forwarded to socket_full_udp_t which forward it to 
//   the socket caller
// - nlay_full never change the mtu by itself. aka it is always due to a reaction 
//   of the nlay's caller.
// - the nlay_full event seems unnecessary.
// - moreover it prevent management of too short MTU. because the nlay_full_t
//   full function MUST return a bool tokeep and thus an error would be unclean
//   (as it should be put in parameter)
// - POSSIBLE solutions
//   - to have a function which set the mtu in nlay_full_t and return the most inner mtu
//     - pro: this avoid the unnecessary nlay_event_t
//     - pro: this allow to return an error (e.g. 0 as most inner mtu)
//       - thus the caller can take special actions
//       - a possible one would be to remove the pathmtu discovery from the outter transport
//       - so it would be more robust
///////////////////////
// - change the full_api mtu callback to return the upper mtu thus the decision is more
//   local ?
//   - unsure...

*/


/* system include */
/* local include */
#include "neoip_nlay_full.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief propagate the mtu thru all the brick
 */
size_t	nlay_full_t::mtu_propagate(size_t most_outter_mtu, nlay_full_api_t *full_api)	throw()
{
	size_t	outter_mtu;
	// go up to the lowest level
	// - reccursive functions to go down the stack (required as the most_outter_mtu is the lowest)
	//   and then update and set the outter mtu of each nlay_full_api_t from the lowest to the uppest
	if( full_api->get_full_api_lower() ){
		// go down one more level to compute the outter mtu of this level
		outter_mtu = mtu_propagate(most_outter_mtu, full_api->get_full_api_lower() );
	} else {
		// if the lowest is reached, the outter mtu of this level is the most_outter_mtu
		outter_mtu = most_outter_mtu;
	}

	// set the outter mtu for this brick
	nlay_err_t	nlay_err = full_api->set_outter_mtu(outter_mtu);
	// TODO what to do in case of error ? return 0 ? and up to the nlay caller to react ?
	// - in case of socket_full_udp_t, it could remove the pmtudisc and set a mtu to something accepted
	DBG_ASSERT( nlay_err.succeed() );
	
	// return the outter mtu for the upper brick
	DBG_ASSERT( outter_mtu >= full_api->get_mtu_overhead() );	
	return outter_mtu - full_api->get_mtu_overhead();
}


/** \brief Change the outter mtu and return the inner mtu
 */
void	nlay_full_t::mtu_outter(size_t new_outter_mtu)		throw()
{
	// propagate the mtu thru all the bricks
	size_t	inner_mtu = mtu_propagate(new_outter_mtu, uppest_full_api);

	// logging for debug
	KLOG_DBG("the new most outter mtu is " << new_outter_mtu);
	KLOG_DBG("the new most inner mtu is " << inner_mtu);
	
	// sanity check - the inner_mtu MUST be equal to the outter_mtu - mtu_overhead()
	DBG_ASSERT( inner_mtu == new_outter_mtu - mtu_overhead() );
}


/** \brief Return the mtu overhead of this layer
 */
size_t	nlay_full_t::mtu_overhead()		const throw()
{
	size_t	sum_overhead	= 0;
	// go thru all the nlay_full_api_t and sum their respective mtu_overhead()
	for(nlay_full_api_t *full_api = uppest_full_api; full_api; full_api = full_api->get_full_api_lower() ){
		// add the mtu_overhead() of this nlay_full_api_t to the sum_overhead
		sum_overhead	+= full_api->get_mtu_overhead();	
	}
	// return the just computed value
	return sum_overhead;
}

NEOIP_NAMESPACE_END


