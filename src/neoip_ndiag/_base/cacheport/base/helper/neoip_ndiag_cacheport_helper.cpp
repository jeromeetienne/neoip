/*! \file
    \brief Definition for handling the convertion of ndiag_cacheport_t from/to file

*/

/* system include */
/* local include */
#include "neoip_ndiag_cacheport_helper.hpp"
#include "neoip_ndiag_cacheport.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_file.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			from/to_file function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a ndiag_cacheport_t from a file_path_t to a .torrent file
 * 
 * - just a wrapper on top of from_bencode and file_sio_t
 * - it has no error report except that any error return a null ndiag_cacheport_t
 */
ndiag_cacheport_t	ndiag_cacheport_helper_t::from_file(const file_path_t &file_path)	throw()
{
	datum_t			resumedata_datum;
	ndiag_cacheport_t	ndiag_cacheport;
	// read the whole file
	file_err_t	file_err;
	file_err	= file_sio_t::readall(file_path, resumedata_datum);
	if( file_err.failed() )	return ndiag_cacheport_t();
	// try to unserialized the ndiag_cacheport
	bytearray_t	bytearray( resumedata_datum );
	try{
		bytearray	>> ndiag_cacheport;
	}catch(serial_except_t &e){
		return ndiag_cacheport_t();
	}
	// return the ndiag_cacheport_t
	return ndiag_cacheport; 
}

/** \brief store a fully init ndiag_cacheport_t  to a file_path_t
 * 
 * - just a wrapper on top of from_bencode and file_sio_t
 */
ndiag_err_t ndiag_cacheport_helper_t::to_file(const ndiag_cacheport_t &ndiag_cacheport
					, const file_path_t &file_path)		throw()
{
	bytearray_t	bytearray	= bytearray_t().serialize(ndiag_cacheport);
	// write the whole file
	file_err_t	file_err;
	file_err	= file_sio_t::writeall(file_path, bytearray.to_datum(datum_t::NOCOPY));
	if( file_err.failed() )	return ndiag_err_from_file(file_err);
	// return no error
	return ndiag_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			misc function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the default path where the ndiag_cacheport_t is stored on disk
 */
file_path_t	ndiag_cacheport_helper_t::default_path()				throw()
{
	file_path_t	file_path;
	// build the file_path
	file_path	= lib_session_get()->cache_rootdir() / "ndiag_cacheport";
	// return the just built file_path_t
	return file_path;
}


NEOIP_NAMESPACE_END;






