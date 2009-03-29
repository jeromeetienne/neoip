/*! \file
    \brief Definition of the \ref bt_cast_mdata_t

- TODO add a delay between the initial bt_httpo_full_t initial position and
  the end of the pieceq
  - it will be tunable via casti
    - thus all casto will have the same initialposition

\par Brief Description
bt_cast_mdata_t contains the meta data about the cast.

\par About boot_nonce
boot_nonce is a statically unique number in a uint32_t. It is chosen by
the bt_cast_mdata_t sender to identity an instance of a stream. the
bt_cast_mdata_t and the following bt_cast_udata_t MUST ALL have the same
boot_nonce.
- if a received bt_cast_udata_t::boot_nonce is different from the first
  bt_cast_mdata_t::boot_nonce, this means casti_swarm_t rebooted in between
  so the receiver should shut down his own swarm
- Additionnally a boot_nonce of 0 has a special meaning, similar to SIGTERM.
  aka on reception of a boot_nonce 0, the receiver should shut down his own swarm.

*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_bt_cast_udata.hpp"
#include "neoip_bt_cast_pidx.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

#define COMMAND_FOR_EACH_FIELD( COMMAND )			\
	COMMAND( bt_cast_id_t	, cast_id)			\
	COMMAND( std::string	, cast_name)			\
	COMMAND( http_uri_t	, http_peersrc_uri)		\
	COMMAND( size_t		, boot_nonce)			\
	COMMAND( size_t		, slide_curs_nbpiece_hint)	\
	COMMAND( size_t		, pieceq_maxlen)		\
	COMMAND( size_t		, pieceq_beg)			\
	COMMAND( size_t		, pieceq_end)			\
	COMMAND( date_t		, casti_date)			\
	COMMAND( delay_t	, recv_udata_maxdelay)		\
	COMMAND( datum_t	, prefix_header)		\
	COMMAND( bt_cast_spos_arr_t, cast_spos_arr)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default Constructor
 */
bt_cast_mdata_t::bt_cast_mdata_t()		throw()
{
	boot_nonce		( 0 					);
	slide_curs_nbpiece_hint	( std::numeric_limits<size_t>::max()	);
	pieceq_beg		( std::numeric_limits<size_t>::max() 	);
	pieceq_end		( std::numeric_limits<size_t>::max() 	);
	pieceq_maxlen		( std::numeric_limits<size_t>::max() 	);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	bt_cast_mdata_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
KLOG_ERR("test passed");	
	if( cast_id().is_null() )						return true;
KLOG_ERR("test passed");	
	if( cast_name().empty() )						return true;
KLOG_ERR("test passed");	
	if( boot_nonce() == 0 )							return true;
KLOG_ERR("test passed");	
	if( slide_curs_nbpiece_hint() == std::numeric_limits<size_t>::max() )	return true;
KLOG_ERR("test passed");	
	if( pieceq_beg() == std::numeric_limits<size_t>::max() )		return true;
KLOG_ERR("test passed");	
	if( pieceq_end() == std::numeric_limits<size_t>::max() )		return true;
KLOG_ERR("test passed");	
	if( pieceq_maxlen() == std::numeric_limits<size_t>::max() )		return true;
KLOG_ERR("test passed");	
	if( casti_date().is_null() )						return true;
KLOG_ERR("test passed");	
	if( recv_udata_maxdelay().is_null() )					return true;
KLOG_ERR("test passed");	
	if( cast_spos_arr().empty() )						return true;
KLOG_ERR("test passed");	
	// if this point is reached, the object is NOT null
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Update the bt_cast_mdata_t with a bt_cast_udata_t
 */
void	bt_cast_mdata_t::updated_with(const bt_cast_udata_t &cast_udata
						, const bt_mfile_t &bt_mfile)	throw()
{
	// sanity check - the bt_cast_udata_t MUST be compatible with this bt_cast_mdata_t
	DBG_ASSERT( compatible_with(cast_udata)	);
	// update the fields which can be
	casti_date	( cast_udata.casti_date()	);
	pieceq_beg	( cast_udata.pieceq_beg()	);
	pieceq_end	( cast_udata.pieceq_end()	);

	/**************** update the bt_cast_spos_arr	***********************/
	// 1. merge the cast_udata.cast_spos_arr() to the bt_cast_mdata_t one into spos_arr_tmp
	//    - they should be merged because cast_udata.cast_spos_arr() may overlap
	//      the bt_cast_mdata_t one
	bt_cast_spos_arr_t	spos_arr_tmp	= cast_spos_arr();
	for(size_t i = 0; i < cast_udata.cast_spos_arr().size(); i++){
		const bt_cast_spos_t &	cast_spos	= cast_udata.cast_spos_arr()[i];
		// if this item is already contained, goto the next
		if( spos_arr_tmp.contain(cast_spos.byte_offset()) )	continue;
		// append this item to the temporary array
		spos_arr_tmp	+= cast_spos;
	}
	// 2. remove all obsoletes bt_cast_spos_t (aka the ones *before* AND *nomore* in pieceq)
	// - NOTE: as the pieceq is only increasing, obsolete position are only at the begining
	while( !spos_arr_tmp.empty() ){
		bt_cast_pidx_t		cast_pidx	= bt_cast_pidx_t().modulo(bt_mfile.nb_piece());
		const bt_cast_spos_t &	cast_spos	= spos_arr_tmp[0];
		// determine the pieceidx of this bt_cast_spos_t
		size_t pieceidx	= bt_unit_t::totfile_to_pieceidx(cast_spos.byte_offset(), bt_mfile);
		// if this pieceidx IS in pieceq, leave the loop
		if( cast_pidx.index(pieceidx).is_in(pieceq_beg(), pieceq_end()) )	break;
		// remove this bt_cast_spos_t from the spos_arr_tmp
		spos_arr_tmp.remove(0);
	}
	// 3. set the cast_spos_arr to the just recomputed tmp one
	cast_spos_arr	( spos_arr_tmp );

#if 1
	// sanity check - ALL cast_spos_arr() element MUST be in the pieceq
	for(size_t i = 0; i < cast_spos_arr().size(); i++){
		bt_cast_pidx_t		cast_pidx	= bt_cast_pidx_t().modulo(bt_mfile.nb_piece());
		const file_size_t &	byte_offset	= cast_spos_arr()[i].byte_offset();
		// determine the pieceidx of this byte_offset
		size_t pieceidx	= bt_unit_t::totfile_to_pieceidx(byte_offset, bt_mfile);
		// if this byte_offset pieceidx IS in pieceq, leave the loop
		DBGNET_ASSERT( cast_pidx.index(pieceidx).is_in(pieceq_beg(), pieceq_end()) );
	}
#endif
	// sanity check - the result MUST NOT be null
	DBG_ASSERT( !is_null() );
}

/** \brief Return true if the bt_cast_udata_t is compatible with the bt_cast_mdata_t
 */
bool	bt_cast_mdata_t::compatible_with(const bt_cast_udata_t &cast_udata)	const throw()
{
// TODO should i test if udata casti_date is >= mdata_casti date ?
	// if bt_bcast_mdata_t boot_nonce is not equal to bt_cast_udata_t, it is not compatible
	if( cast_udata.boot_nonce() != boot_nonce() )	return false;
	// if all previous tests passed, it is compatible
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_cast_mdata_t::to_string()				const throw()
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

/** \brief serialize a bt_cast_mdata_t
 */
serial_t& operator << (serial_t& serial, const bt_cast_mdata_t &cast_mdata)	throw()
{
	const bt_cast_mdata_t & orig_obj	 = cast_mdata;
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

/** \brief unserialze a bt_cast_mdata_t
 */
serial_t& operator >> (serial_t & serial, bt_cast_mdata_t &cast_mdata)		throw(serial_except_t)
{
	bt_cast_mdata_t & dest_obj	 = cast_mdata;
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
//			xmlrpc bt_cast_mdata_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for bt_cast_mdata_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const bt_cast_mdata_t &cast_mdata)	throw()
{
	// Start the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	// define the macro
	#define CMD(var_type, var_name)						\
		xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG( #var_name );	\
		xmlrpc_build		<< cast_mdata.var_name();		\
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

/** \brief unserialize xmlrpc for bt_cast_mdata_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, bt_cast_mdata_t &cast_mdata)	throw(xml_except_t)
{
	bt_cast_mdata_t & dest_obj	 = cast_mdata;

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

