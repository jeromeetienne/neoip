/*! \file
    \brief Definition of the \ref ipport_aview_t

\par about is_cfg_ok() and is_run_ok()
- a ipport_aview_t MAY be in 2 'states'
  - cfg: in which is it done for configuration - aka pre-binding
    - aka when the ipport_aview_t::lview() IS NOT YET bound by a socket
  - run: in which it is done for running - aka post-binding
    - aka when the ipport_aview_t::lview() IS ALREDY bound by a socket

*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_ipport_aview.hpp"
#include "neoip_xmlrpc.hpp"

NEOIP_NAMESPACE_BEGIN;


// definition of \ref ipport_addr_t constant
const ipport_aview_t	ipport_aview_t::ANY_IP4	= ipport_aview_t("0.0.0.0:0", ipport_addr_t());;
// end of constants definition

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	ipport_aview_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( lview().is_null() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			is_sane_* function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief check if the ipport_aview_t is sane - while in the state 'run'
 * 
 * - the state 'run' means that the ipport_addr_t IS ALREADY fully initialized
 */
bool	ipport_aview_t::is_run_ok()		const throw()
{
	// sanity check - do all the common tests first
	DBG_ASSERT( is_sane_common() );
	// sanity check - the lview() MUST have a port
	DBG_ASSERT( lview().port() );
	// if this point is reached, return true
	return true;
}

/** \brief check if the ipport_aview_t is sane - while in the state 'cfg'
 * 
 * - the state 'cfg' means that the ipport_addr_t WILL BE used to initialize
 */
bool	ipport_aview_t::is_cfg_ok()		const throw()
{
	// sanity check - do all the common tests first
	DBG_ASSERT( is_sane_common() );
	// if this point is reached, return true
	return true;
}


/** \brief check if the ipport_aview_t is sane (independantly of the state run/cfg)
 */
bool	ipport_aview_t::is_sane_common()	const throw()
{
	// sanity check - the lview() MUST be non-null
	DBG_ASSERT( !lview().is_null() );
	// sanity check - the lview().ipaddr() MUST be is_any() or is_fully_qualified
	DBG_ASSERT( lview().ipaddr().is_any() || lview().ipaddr().is_fully_qualified() );

	// if the pview is not null, the ipport_aview_t is inetreachable
	// - so pview().port() MUST be non-null zero
	// - so pview().ipaddr() MUST BE either fully_qualified or any
	//   - if it is any, it is up to the receiver of the xmit_listen_ipport to determine
	//     the ip_addr_t from its own view of the source ip_addr_t which sent the xmit_listen_ipport 
	if(!pview().is_null())	DBG_ASSERT( pview().port() );
	if(!pview().is_null())	DBG_ASSERT( pview().ipaddr().is_any() || pview().ipaddr().is_fully_qualified() );

	// if this point is reached, return true
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Update a is_cfg_ok() ipport_aview_t to a is_run_ok() ipport_aview_t
 * 
 * - this is usefull to pass from a ipport_aview_t in cfg mode to one in a run mode
 * - this update the local view in case it has been dynamically bound 
 *   - i.e. with the lview().port() was 0 before and it has been set by a bind()
 * 
 * @param bound_lview	is a local ipport_addr_t once it has been bound
 */
ipport_aview_t &ipport_aview_t::update_once_bound(const ipport_addr_t &bound_lview)	throw()
{
	// sanity check - the object MUST BE is_cfg_ok() before the update
	DBG_ASSERT( is_cfg_ok() );
	// copy the bound_lview to the lview
	lview(bound_lview);
	// sanity check - the object MUST BE is_run_ok() after the update
	DBG_ASSERT( is_run_ok() );
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 ipport_aview_t and return value ala strcmp/memcmp
 */
int	ipport_aview_t::compare(const ipport_aview_t &other)	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// handle the address
	if( lview() < other.lview() )			return -1;
	if( lview() > other.lview() )			return +1;
	// handle the port
	if( pview() < other.pview() )			return -1;
	if( pview() > other.pview() )			return +1;
	return 0;
}
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	ipport_aview_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[lview="		<< lview();
	oss << " pview="		<< pview();
	oss << "]";
	// return the just built string
	return oss.str();
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc ipport_aview_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for ipport_aview_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const ipport_aview_t &ipport_aview)	throw()
{
	// serialize the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build 	<< xmlrpc_build_t::MEMBER_BEG("lview") << ipport_aview.lview()	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build 	<< xmlrpc_build_t::MEMBER_BEG("pview") << ipport_aview.pview()	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief unserialize xmlrpc for ipport_aview_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, ipport_aview_t &ipport_aview)	throw(xml_except_t)
{
	ipport_addr_t	lview;
	ipport_addr_t	pview;
	// unserialize the struct
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG("lview") >> lview	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG("pview") >> pview	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;
	// set the unserialized value in the ipport_aview_t
	ipport_aview.lview(lview);
	ipport_aview.pview(pview);
	// return the object itself
	return xmlrpc_parse;

}

NEOIP_NAMESPACE_END

