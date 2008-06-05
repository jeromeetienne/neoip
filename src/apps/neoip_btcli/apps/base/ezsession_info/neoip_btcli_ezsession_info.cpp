/*! \file
    \brief Definition of the \ref btcli_ezsession_info_t

- TODO to rename btcli_ezsession_info_t into btcli_ezswarm_info_t
 
*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_btcli_ezsession_info.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_rate_sched.hpp"
#include "neoip_xmlrpc.hpp"

NEOIP_NAMESPACE_BEGIN;

#define COMMAND_FOR_EACH_FIELD( COMMAND )		\
	COMMAND( double		, recv_maxrate)		\
	COMMAND( double		, xmit_maxrate)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor from a bt_ezsession_t 
 */
btcli_ezsession_info_t::btcli_ezsession_info_t(bt_ezsession_t *bt_ezsession)	throw()
{
	// set the default value
	recv_maxrate	( -1	);
	xmit_maxrate	( -1	);
	// fill the struct when possible
	if(bt_ezsession->recv_rsched())	recv_maxrate( bt_ezsession->recv_rsched()->max_rate() );
	if(bt_ezsession->xmit_rsched())	xmit_maxrate( bt_ezsession->xmit_rsched()->max_rate() );
}

	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	btcli_ezsession_info_t::is_null()	const throw()
{
	// if this point is reached, the object is NOT null
	return false;
}
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	btcli_ezsession_info_t::to_string()				const throw()
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
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc btcli_ezsession_info_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for btcli_ezsession_info_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const btcli_ezsession_info_t &ezsession_info)	throw()
{
	// Start the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;	
	// define the macro
	#define CMD(var_type, var_name)						\
		xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG( #var_name );	\
		xmlrpc_build		<< ezsession_info.var_name();		\
		xmlrpc_build	<< xmlrpc_build_t::MEMBER_END;
	// serialize all the field
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	// Start the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief unserialize xmlrpc for btcli_ezsession_info_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, btcli_ezsession_info_t &ezsession_info)	throw(xml_except_t)
{
	btcli_ezsession_info_t & dest_obj	 = ezsession_info;
	// start the struct
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	// define the macro
	#define CMD(var_type, var_name)						\
		var_type	var_name;					\
		xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG( #var_name);	\
		xmlrpc_parse		>> var_name;				\
		xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_END;			\
		dest_obj.var_name(var_name);
	// unserialize all the field
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	// end the struct
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;

	// return the object itself
	return xmlrpc_parse;
}

NEOIP_NAMESPACE_END

