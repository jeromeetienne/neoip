/*! \file
    \brief Definition of the \ref bt_cast_udata_t


\par Brief Description
bt_cast_udata_t stores the data to update the initial bt_cast_mdata_t

*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_bt_cast_udata.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

#define COMMAND_FOR_EACH_FIELD( COMMAND )		\
	COMMAND( size_t		, boot_nonce)		\
	COMMAND( size_t		, pieceq_beg)		\
	COMMAND( size_t		, pieceq_end)		\
	COMMAND( date_t		, casti_date)		\
	COMMAND( bt_cast_spos_arr_t, cast_spos_arr)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default Constructor
 */
bt_cast_udata_t::bt_cast_udata_t()		throw()
{
	// init all the fields to an impossible default without
	boot_nonce	( 0 					);
	pieceq_beg	( std::numeric_limits<size_t>::max() 	);
	pieceq_end	( std::numeric_limits<size_t>::max() 	);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	bt_cast_udata_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( boot_nonce() == 0 )						return true;
	if( pieceq_beg() == std::numeric_limits<size_t>::max() )	return true;
	if( pieceq_end() == std::numeric_limits<size_t>::max() )	return true;
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
std::string	bt_cast_udata_t::to_string()				const throw()
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


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          serialization
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_cast_udata_t
 */
serial_t& operator << (serial_t& serial, const bt_cast_udata_t &cast_udata)		throw()
{
	const bt_cast_udata_t & orig_obj	 = cast_udata;

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

/** \brief unserialze a bt_cast_udata_t
 */
serial_t& operator >> (serial_t & serial, bt_cast_udata_t &cast_udata)		throw(serial_except_t)
{

	bt_cast_udata_t & dest_obj	 = cast_udata;
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
//                          xmlrpc bt_cast_udata_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for bt_cast_udata_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const bt_cast_udata_t &cast_udata)	throw()
{
	const bt_cast_udata_t & orig_obj	 = cast_udata;
	// Start the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;	
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

/** \brief unserialize xmlrpc for bt_cast_udata_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, bt_cast_udata_t &cast_udata)	throw(xml_except_t)
{
	bt_cast_udata_t & dest_obj	 = cast_udata;
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

