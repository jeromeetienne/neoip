/*! \file
    \brief Definition of the \ref upnp_disc_res_t

*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_upnp_disc_res.hpp"

NEOIP_NAMESPACE_BEGIN;

#define COMMAND_FOR_EACH_FIELD( COMMAND )			\
	COMMAND( std::string	, service_name)			\
	COMMAND( std::string	, server_name)			\
	COMMAND( ip_addr_t	, local_ipaddr)			\
	COMMAND( ip_addr_t	, server_ipaddr)		\
	COMMAND( http_uri_t	, location_uri)			\
	COMMAND( http_uri_t	, control_uri)			\
	COMMAND( bool		, getport_endianbug)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief default contructor
 */
upnp_disc_res_t::upnp_disc_res_t()	throw()
{
	// put some default values
	getport_endianbug	(false);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	upnp_disc_res_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( service_name() != "WANIPConnection" && service_name() != "WANPPPConnection" )return true;
	if( location_uri().is_null() )			return true;
	if( control_uri().is_null() )			return true;
	if( !local_ipaddr().is_fully_qualified() )	return true;
	if( !server_ipaddr().is_fully_qualified() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int upnp_disc_res_t::compare(const upnp_disc_res_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// define the macro
	#define CMD(var_type, var_name)					\
		if( var_name() < other.var_name() )	return -1;	\
		if( var_name() > other.var_name() )	return +1;
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD

	// NOTE: here both are considered equal
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	upnp_disc_res_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// define the macro
	#define CMD(var_type, var_name)				\
		if( !oss.str().empty() )	oss << " ";	\
		oss << #var_name << "=" << var_name();
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	// return the just built string
	return oss.str();
}

/** \brief convert the object to a string more 'human readable'
 * 
 * - NOTE: it may contains \n or other 'beautifuller'
 */
std::string	upnp_disc_res_t::to_human_str()	const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// define the macro
	#define CMD(var_type, var_name)					\
		if( !oss.str().empty() )	oss << "\n";		\
		oss << "\t" << #var_name << "\t: " << var_name();
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial upnp_disc_res_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a upnp_disc_res_t
 */
serial_t& operator << (serial_t& serial, const upnp_disc_res_t &upnp_disc_res)		throw()
{
	const upnp_disc_res_t &	orig_obj	 = upnp_disc_res;
	// define the macro
	#define CMD(var_type, var_name)		\
		serial	<< orig_obj.var_name();
	// serialize all the field
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	// return serial
	return serial;
}

/** \brief unserialze a upnp_disc_res_t
 */
serial_t& operator >> (serial_t& serial, upnp_disc_res_t &upnp_disc_res)  		throw(serial_except_t)
{
	upnp_disc_res_t & dest_obj	 = upnp_disc_res;
	// define the macro
	#define CMD(var_type, var_name)			\
		var_type	var_name;		\
		serial	>> var_name;			\
		dest_obj.var_name(var_name);
	// unserialize all the field
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	// return serial
	return serial;
}
NEOIP_NAMESPACE_END

