/*! \file
    \brief Definition of the \ref upnp_portcleaner_item_t

*/

/* system include */
/* local include */
#include "neoip_upnp_portcleaner_item.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


#define COMMAND_FOR_EACH_FIELD( COMMAND )		\
	COMMAND( upnp_portdesc_t, portdesc)		\
	COMMAND( date_t		, creation_date)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Contructor by value
 */
upnp_portcleaner_item_t::upnp_portcleaner_item_t(const upnp_portdesc_t &a_portdesc
					, const date_t &a_creation_date)	throw()
{
	// copy the arguments
	portdesc	( a_portdesc		);
	creation_date	( a_creation_date	);
	// sanity check - upnp_portdesc_t MUST have a portcleaner tag
	DBG_ASSERT( portdesc().has_portcleaner_tag() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if this item is declared obsolete
 */
bool	upnp_portcleaner_item_t::is_obsolete()	const throw()
{
	// sanity check - upnp_portdesc_t MUST have a portcleaner tag
	DBG_ASSERT( portdesc().has_portcleaner_tag() );
	// if it is not yet expired, return false
	if( expire_date() >= date_t::present() )	return false;
	// if all previous tests passed, return true 
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 upnp_portcleaner_item_t and return value ala strcmp/memcmp
 */
int	upnp_portcleaner_item_t::compare(const upnp_portcleaner_item_t &other)	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;

	// define the macro
	#define CMD(var_type, var_name)					\
		if( var_name() < other.var_name() )	return -1;	\
		if( var_name() > other.var_name() )	return +1;
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD

	// at this point, both are considered equal, so return 0
	return 0;
}
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	upnp_portcleaner_item_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[";
	// define the macro
	#define CMD(var_type, var_name)				\
		if( !oss.str().empty() )	oss << " ";	\
		oss << #var_name << "=" << var_name();
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	oss << "]";
	// return the just built string
	return oss.str();
}


NEOIP_NAMESPACE_END

