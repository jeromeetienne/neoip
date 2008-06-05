/*! \file
    \brief Definition of the \ref bt_cast_spos_t

\par Brief Description
bt_cast_spos_t contains the start position in the casti stream associated with
a casti date at which the start position has been observed by casti

*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_bt_cast_spos.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

#define COMMAND_FOR_EACH_FIELD( COMMAND )			\
	COMMAND( file_size_t	, byte_offset)			\
	COMMAND( date_t		, casti_date)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	bt_cast_spos_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( byte_offset().is_null() )		return true;
	if( casti_date().is_null() )		return true;
	// if this point is reached, the object is NOT null
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_cast_spos_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[";
	// define the macro
	#define CMD(var_type, var_name)				\
		if( oss.str().size() > 1 )	oss << " ";	\
		oss << #var_name << "=" << var_name();
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	oss << "]";
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          serialization
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_cast_spos_t
 */
serial_t& operator << (serial_t& serial, const bt_cast_spos_t &cast_spos)	throw()
{
	const bt_cast_spos_t & orig_obj	 = cast_spos;
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

/** \brief unserialze a bt_cast_spos_t
 */
serial_t& operator >> (serial_t & serial, bt_cast_spos_t &cast_spos)	throw(serial_except_t)
{
	bt_cast_spos_t & dest_obj	 = cast_spos;
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xmlrpc bt_cast_spos_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for bt_cast_spos_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const bt_cast_spos_t &cast_spos)	throw()
{
	// Start the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;	
	// define the macro
	#define CMD(var_type, var_name)						\
		xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG( #var_name );	\
		xmlrpc_build		<< cast_spos.var_name();		\
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

/** \brief unserialize xmlrpc for bt_cast_spos_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, bt_cast_spos_t &cast_spos)	throw(xml_except_t)
{
	bt_cast_spos_t & dest_obj	 = cast_spos;

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

