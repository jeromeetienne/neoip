/*! \file
    \brief Definition of the \ref btcli_ezswarm_info_t

*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_btcli_ezswarm_info.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_helper.hpp"
#include "neoip_xmlrpc.hpp"

NEOIP_NAMESPACE_BEGIN;

#define COMMAND_FOR_EACH_FIELD( COMMAND )			\
	COMMAND( bt_id_t		, infohash)		\
	COMMAND( std::string		, mfile_name)		\
	COMMAND( std::string		, cur_state)		\
	COMMAND( file_size_t		, totfile_size)		\
	COMMAND( file_size_t		, totfile_anyavail)	\
	COMMAND( double			, recv_rate)		\
	COMMAND( double			, xmit_rate)		\
	COMMAND( size_t			, peersrc_nb_leech)	\
	COMMAND( size_t			, peersrc_nb_seed)	\
	COMMAND( size_t			, swarm_nb_leech)	\
	COMMAND( size_t			, swarm_nb_seed)	\
	COMMAND( bt_swarm_stats_t	, swarm_stats)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor from a bt_ezsession_t 
 */
btcli_ezswarm_info_t::btcli_ezswarm_info_t(bt_ezswarm_t *bt_ezswarm)	throw()
{
	const bt_mfile_t &	bt_mfile = bt_ezswarm->mfile();
	// fill the struct
	infohash	( bt_ezswarm->mfile().infohash()	);
	cur_state	( bt_ezswarm->cur_state().to_string()	);
	mfile_name	( bt_mfile.name().to_string() 		);
	totfile_size	( bt_mfile.totfile_size()		);

	if( bt_ezswarm->in_share() ){
		bt_swarm_t * bt_swarm	= bt_ezswarm->share()->bt_swarm(); 
		totfile_anyavail( bt_swarm->totfile_anyavail()			);
		recv_rate	( bt_swarm->recv_rate()				);
		xmit_rate	( bt_swarm->xmit_rate()				);
		peersrc_nb_leech( bt_swarm_helper_t::peersrc_nb_leech(bt_swarm)	);
		peersrc_nb_seed	( bt_swarm_helper_t::peersrc_nb_seed(bt_swarm)	);
		swarm_nb_leech	( bt_swarm_helper_t::swarm_nb_leech(bt_swarm)	);
		swarm_nb_seed	( bt_swarm_helper_t::swarm_nb_seed(bt_swarm)	);
		swarm_stats	( bt_swarm->swarm_stats()			);
	}else{
		totfile_anyavail( 0 );
		recv_rate	( 0 );
		xmit_rate	( 0 );
		peersrc_nb_leech( 0 );
		peersrc_nb_seed	( 0 );
		swarm_nb_leech	( 0 );
		swarm_nb_seed	( 0 );
	}
}

	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	btcli_ezswarm_info_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( cur_state().empty() )	return true;
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
std::string	btcli_ezswarm_info_t::to_string()				const throw()
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
//                          xmlrpc btcli_ezswarm_info_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for btcli_ezswarm_info_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const btcli_ezswarm_info_t &ezswarm_info)	throw()
{
	// Start the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;	
	// define the macro
	#define CMD(var_type, var_name)						\
		xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG( #var_name );	\
		xmlrpc_build		<< ezswarm_info.var_name();		\
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

/** \brief unserialize xmlrpc for btcli_ezswarm_info_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, btcli_ezswarm_info_t &ezswarm_info)	throw(xml_except_t)
{
	btcli_ezswarm_info_t & dest_obj	 = ezswarm_info;
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

