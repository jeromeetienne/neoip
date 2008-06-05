/*! \file
    \brief Definition for handling the convertion of bt_swarm_resumedata_t from/to file

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_resumedata_helper.hpp"
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         torrent file convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a bt_swarm_resumedata_t from a file_path_t
 * 
 * - just a wrapper on top of from_bencode and file_sio_t
 * - it has no error report except that any error return a null bt_swarm_resumedata_t
 */
bt_swarm_resumedata_t	bt_swarm_resumedata_helper_t::from_file(const file_path_t &file_path)	throw()
{
	datum_t			resumedata_datum;
	bt_swarm_resumedata_t	swarm_resumedata;
	// read the whole file
	file_err_t	file_err;
	file_err	= file_sio_t::readall(file_path, resumedata_datum);
	if( file_err.failed() )	return bt_swarm_resumedata_t();
	// try to unserialized the swarm_resumedata
	bytearray_t	bytearray( resumedata_datum );
	try{
		bytearray	>> swarm_resumedata;
	}catch(serial_except_t &e){
		return bt_swarm_resumedata_t();
	}
	// sanity check - the bt_swarm_resumedata_t MUST NOT be null (or an except should have been thrown)
	DBG_ASSERT( !swarm_resumedata.is_null() );
	// return the bt_swarm_resumedata_t
	return swarm_resumedata; 
}

/** \brief store a fully init bt_swarm_resumedata_t  to a file_path_t
 * 
 * - just a wrapper on top of from_bencode and file_sio_t
 */
bt_err_t bt_swarm_resumedata_helper_t::to_file(const bt_swarm_resumedata_t &swarm_resumedata
					, const file_path_t &file_path)		throw()
{
	bytearray_t	bytearray	= bytearray_t().serialize( swarm_resumedata );
	// write the whole file
	file_err_t	file_err;
	file_err	= file_sio_t::writeall(file_path, bytearray.to_datum(datum_t::NOCOPY));
	if( file_err.failed() )	return bt_err_from_file(file_err);
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END;






