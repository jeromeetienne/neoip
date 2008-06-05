/*! \file
    \brief Definition of the \ref bt_swarm_stats_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_swarm_stats.hpp"
#include "neoip_xmlrpc.hpp"

NEOIP_NAMESPACE_BEGIN;

#define COMMAND_FOR_EACH_FIELD( COMMAND )			\
	COMMAND( file_size_t	, uloaded_datalen)		\
	COMMAND( file_size_t	, dloaded_datalen)		\
	COMMAND( file_size_t	, uloaded_fulllen)		\
	COMMAND( file_size_t	, dloaded_fulllen)		\
	COMMAND( file_size_t	, deleted_dload_datalen)	\
	COMMAND( size_t		, nb_hash_failed)		\
	COMMAND( size_t		, timedout_req_nb)		\
	COMMAND( size_t		, total_req_nb)			\
	COMMAND( file_size_t	, total_req_len)		\
	COMMAND( size_t		, dup_req_nb)			\
	COMMAND( file_size_t	, dup_req_len)			\
	COMMAND( size_t		, dup_rep_nb)			\
	COMMAND( file_size_t	, dup_rep_len)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_stats_t::bt_swarm_stats_t()	throw()
{
	// set the initial value
	uloaded_datalen		( 0 );
	dloaded_datalen		( 0 );
	uloaded_fulllen		( 0 );
	dloaded_fulllen		( 0 );
	deleted_dload_datalen	( 0 );
	nb_hash_failed		( 0 );
	timedout_req_nb		( 0 );
	total_req_nb		( 0 );
	total_req_nb		( 0 );
	dup_req_nb		( 0 );
	dup_req_len		( 0 );
	dup_rep_nb		( 0 );
	dup_rep_len		( 0 );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the 'share ratio' aka the uloaded_datalen / dloaded_datalen
 */
double		bt_swarm_stats_t::share_ratio()		const throw()
{
	return uloaded_datalen().to_double() / dloaded_datalen().to_double();
}

/** \brief Return the overhead for the uloaded
 */	
file_size_t	bt_swarm_stats_t::uloaded_overhd()	const throw()
{
	// sanity check - uloaded_fulllen MUST be >= than uloaded_datalen
	DBG_ASSERT( uloaded_fulllen() >= uloaded_datalen() );
	// compute the value to return
	return uloaded_fulllen() - uloaded_datalen();
}

/** \brief Return the overhead for the dloaded
 */	
file_size_t	bt_swarm_stats_t::dloaded_overhd()	const throw()
{
	// sanity check - dloaded_fulllen MUST be >= than dloaded_datalen
	DBG_ASSERT( dloaded_fulllen() >= dloaded_datalen() );
	// compute the value to return
	return dloaded_fulllen() - dloaded_datalen();
}

/** \brief return the overhead ratio for the uloaded
 */
double		bt_swarm_stats_t::uloaded_overhd_ratio()	const throw()
{
	return uloaded_overhd().to_double() / uloaded_fulllen().to_double();
}

/** \brief return the overhead ratio for the dloaded
 */
double		bt_swarm_stats_t::dloaded_overhd_ratio()	const throw()
{
	return dloaded_overhd().to_double() / dloaded_fulllen().to_double();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_swarm_stats_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// build the string to return
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          serialization
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_swarm_stats_t
 */
serial_t& operator << (serial_t& serial, const bt_swarm_stats_t &swarm_stats)	throw()
{
	const bt_swarm_stats_t & orig_obj	 = swarm_stats;
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

/** \brief unserialze a bt_swarm_stats_t
 */
serial_t& operator >> (serial_t & serial, bt_swarm_stats_t &swarm_stats)	throw(serial_except_t)
{

	bt_swarm_stats_t & dest_obj	 = swarm_stats;
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc bt_swarm_stats_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for bt_swarm_stats_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const bt_swarm_stats_t &swarm_stats)	throw()
{
	const bt_swarm_stats_t & orig_obj	 = swarm_stats;
	// Start the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	// serialize all the field
	// define the macro
	#define CMD(var_type, var_name)						\
		xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG( #var_name );	\
		xmlrpc_build		<< orig_obj.var_name();			\
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

/** \brief unserialize xmlrpc for bt_swarm_stats_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, bt_swarm_stats_t &swarm_stats)	throw(xml_except_t)
{
	bt_swarm_stats_t & dest_obj	 = swarm_stats;
	// start the struct
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	// unserialize all the field
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

