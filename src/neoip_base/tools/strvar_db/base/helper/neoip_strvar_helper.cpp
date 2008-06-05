/*! \file
    \brief Definition for strvar_helper_t


*/

/* system include */
#include <iomanip>
#include <iostream>
#include <fstream>
/* local include */
#include "neoip_strvar_helper.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         torrent file convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief read a strvar_db_t from a file
 */
strvar_db_t	strvar_helper_t::from_file(const file_path_t &file_path)	throw()
{
	strvar_db_t	strvar_db;
	std::ifstream 	file_in;
	// open the file
	try {
		file_in.open( file_path.to_string().c_str(), std::ios::in );
	}catch(...){
		KLOG_ERR("Cant open filename " << file_path);
		return strvar_db_t();
	}
	// read until the end of the file
	while( file_in.good() ){
		char	line[1024+1];
		// read one file
		file_in.getline( line, sizeof(line)-1 );
		// if the line start with a '#', it is a comment, skip it
		if( line[0] == '#' )	continue;
		// if the line is empty, skip it
		if( strlen(line) == 0 )	continue;
		char	*p_equal = strchr(line, '=');
		DBG_ASSERT( p_equal );
		// replace '=' by '\0'
		*p_equal = '\0';
		// add the variable itself
		strvar_db.append(line, p_equal+1);
	}
	// return the just built strvar_db_t
	return strvar_db;
}


NEOIP_NAMESPACE_END;






