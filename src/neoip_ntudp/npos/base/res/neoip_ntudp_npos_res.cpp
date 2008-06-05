/*! \file
    \brief Definition of the \ref ntudp_npos_res_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_npos_res.hpp"
#include "neoip_ipport_aview.hpp"	

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_npos_res_t::ntudp_npos_res_t()	throw()
{
#define SLOTI(item_name, initial_value)			\
	item_name##_val		= initial_value;	\
	item_name##_been_set	= false;

	SLOTI(natted			, false);
	SLOTI(natsym			, false);
	SLOTI(natlback			, false);
	SLOTI(inetreach			, false);
	SLOTI(local_ipaddr_lview	, ip_addr_t());
	SLOTI(local_ipaddr_pview	, ip_addr_t());
#undef SLOTI
}

/** \brief Destructor
 */
ntudp_npos_res_t::~ntudp_npos_res_t()	throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Mist function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a ntudp_npos_res_t from a ipport_aview_t only
 */
ntudp_npos_res_t	ntudp_npos_res_t::from_ipport_aview(const ipport_aview_t &ipport_aview)	throw()
{
	ntudp_npos_res_t	npos_res;
	// build the ntudp_npos_res_t
	npos_res.local_ipaddr_lview( ipport_aview.lview().ipaddr() );
	// if the ipport_aview.pview() is fully qualified, considere it inetreach ok
	if( ipport_aview.pview().is_fully_qualified() ){
		npos_res.inetreach(true);
		npos_res.local_ipaddr_pview( ipport_aview.pview().ipaddr() );
	}else{
		npos_res.inetreach(false);
	}
	// if local_ipaddr_lview is != local_ipaddr_pview, set natted to true, else to false
	if( npos_res.local_ipaddr_pview_present() && npos_res.local_ipaddr_lview() == npos_res.local_ipaddr_pview() ){
		npos_res.natted(false);
	}else{
		npos_res.natted(true);
	}
	// if npos_res.natted(), assume the most conservative nat possible just-in-case
	// - as no info is available about the type of nat
	if( npos_res.natted() ){
		npos_res.natsym		(true);
		npos_res.natlback	(false);
	}
	// sanity check - the just built ntudp_npos_res_t MUST be completed()
	DBG_ASSERT( npos_res.completed() );
	// return the just built ntudp_npos_res_t
	return npos_res;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//              compute the ntudp_cnxtype for a remote responder
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Compute the ntudp_cnxtype_t with this ntudp_npos_res_t as itor
 *         and the remote ntudp_npos_res_t as resp
 * 
 * - natlback() has no influence here. natlback stuff is handled by the responder 
 *   providing several listen address and the iterator trying all of them.
 */
ntudp_cnxtype_t	ntudp_npos_res_t::cpu_cnxtype(const ntudp_npos_res_t &resp_res)	const throw()
{
	// if the responder is inetreach(), connect the responder directly
	if( resp_res.inetreach() )	return ntudp_cnxtype_t::DIRECT;
	// if the local peer is inetreach(), connect the responder via a reverse connection
	if( this->inetreach() )		return ntudp_cnxtype_t::REVERSE;
	// if both are natted() but not natsym(), connect the responder via a relayed establishement
	if( this->natted() && !this->natsym() && resp_res.natted() && !resp_res.natsym() )
		return ntudp_cnxtype_t::ESTARELAY;

	// NOTE: here a full relay would be needed, but currently full relay is not implemented

	// if this point is reached, the connection is impossible
	return ntudp_cnxtype_t::IMPOSSIBLE;
}

/** \brief Return true if the peer having this ntudp_npos_res_t needs relay to be responder
 *         false otherwise
 */
bool	ntudp_npos_res_t::need_relay_for_resp()		const throw()
{
	// if the peer is inetreach(), no relay is needed
	if( inetreach() )	return false;
	// if this point is reached, relays are needed
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int ntudp_npos_res_t::compare(const ntudp_npos_res_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null


#define SLOTI(item_name)							\
	if( item_name##_present() ){						\
		if( item_name##_val < other.item_name##_val )	return -1;	\
		if( item_name##_val > other.item_name##_val )	return +1;	\
	}

	SLOTI(natted);
	SLOTI(natsym);
	SLOTI(natlback);
	SLOTI(inetreach);
	SLOTI(local_ipaddr_lview);
	SLOTI(local_ipaddr_pview);
#undef SLOTI

	// NOTE: here both are considered equal
	return 0;
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     to_string() function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Convert the object into a string
 */
std::string	ntudp_npos_res_t::to_string()	const throw()
{
	std::ostringstream oss;
	// handle the null case
	if( is_null() )	return "null";
	
	oss << std::boolalpha;	// to display the bool as true/false and not 1/0
	oss << "natted="		<< natted_val;
	oss << " natsym="		<< natsym_val;
	oss << " natlback="		<< natlback_val;
	oss << " inetreach="		<< inetreach_val;
	oss << " local_ipaddr_lview="	<< local_ipaddr_lview_val;
	oss << " local_ipaddr_pview="	<< local_ipaddr_pview_val;
	return oss.str();
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                    completed() function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the ntudp_npos_res_t is fully set/completed
 */
bool	ntudp_npos_res_t::completed()	const throw()
{
	if( !natted_present() )			return false;
	if( natted() && !natlback_present() )	return false;
	if( natted() && !natsym_present() )	return false;
	if( !inetreach_present() )		return false;

	// NOTE: the local_ipaddr_lview/pview are not included in the completed() test
	// - as they may be unknown during the initialisation see get_default()
	// - but they are mandatorily present if the ntudp_npos_res_t comes from a ntudp_npos_eval_t	

	// if inetreach() == true, it is completed IIF listen_ipaddr_lview/pview are not null
	// - according to the principle "if listen_addr_pview_cfg has ANY as ip addr, get
	//   it from the npos_res_t"
	if( inetreach() && local_ipaddr_lview().is_null() )	return false;
	if( inetreach() && local_ipaddr_pview().is_null() )	return false;

	// if this point is reached, the ntudp_npos_res_t is considered completed
	return true;
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//           static function to get the safest default
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////


/** \brief Return the safest default value for ntudp_npos_res_t
 * 
 * - here the safest = the most conservative
 *   - it may be seen as the worst case for connectivity
 */
ntudp_npos_res_t	ntudp_npos_res_t::get_default()	throw()
{
	ntudp_npos_res_t	result;
	// set the most conservative values
	result.inetreach(false);
	result.natted(true);
	result.natsym(true);
	result.natlback(false);

	// NOTE: no value is set for the listen_addr_lview/pview as no default value are possible 

	// sanity check - the default ntudp_npos_res_t MUST be completed
	DBG_ASSERT( result.completed() );
	// return the result
	return result;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ntudp_npos_res_t
 */
serial_t& operator << (serial_t& serial, const ntudp_npos_res_t &npos_res)		throw()
{
	// sanity check - a ntudp_npos_res_t MUST be completed to be serialized
	DBG_ASSERT( npos_res.completed() );
	// serialize the object
	serial << npos_res.inetreach_val;
	serial << npos_res.natted_val;
	serial << npos_res.natsym_val;
	serial << npos_res.natlback_val;
	serial << npos_res.local_ipaddr_lview_val;
	serial << npos_res.local_ipaddr_pview_val;
	// return serial
	return serial;
}

/** \brief unserialze a ntudp_npos_res_t
 */
serial_t& operator >> (serial_t & serial, ntudp_npos_res_t &npos_res)		throw(serial_except_t)
{
	bool		inetreach_val, natted_val, natsym_val, natlback_val;
	ip_addr_t	local_ipaddr_lview_val, local_ipaddr_pview_val;
	// unserialize the object
	serial >> inetreach_val;
	serial >> natted_val;
	serial >> natsym_val;
	serial >> natlback_val;
	serial >> local_ipaddr_lview_val;
	serial >> local_ipaddr_pview_val;
	// reset the result
	npos_res	= ntudp_npos_res_t();
	// set the returned variable
	npos_res.inetreach(inetreach_val).natted(natted_val);
	if( natted_val )			npos_res.natsym(natsym_val).natlback(natlback_val);
	if( !local_ipaddr_lview_val.is_null() )	npos_res.local_ipaddr_lview(local_ipaddr_lview_val);
	if( !local_ipaddr_pview_val.is_null() )	npos_res.local_ipaddr_pview(local_ipaddr_pview_val);
	// if the unserialized ntudp_npos_res_t is not completed, report an error
	if( npos_res.completed() == false ){
		// reset the ntudp_npos_res_t
		npos_res	= ntudp_npos_res_t();
		// throw an exception
		nthrow_serial_plain("Unserialized an incomplete ntudp_npos_res_t!");
	}
	// return serial
	return serial;
}
NEOIP_NAMESPACE_END

