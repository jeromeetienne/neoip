/*! \file
    \brief Definition of the \ref bt_mfile_subfile_t

*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_bt_mfile_subfile.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the object is to be considered null, false otherwise
 * 
 * - check if all the required field are set
 *   - aka the field needed for is_sfile_ok AND is_pfile_ok
 *   - dont check the optionnal field
 */
bool	bt_mfile_subfile_t::is_null()	const throw()
{
	// check that piece_len is set
	if( len().is_null() )				return true;
	// check that the mfile_path is set
	if( mfile_path().is_null() )			return true;
	// if this point is reached, the bt_mfile_subfile_t is to be considered non null
	return false;
}

/** \brief return true if the bt_mfile_subfile_t is_full_init
 */
bool	bt_mfile_subfile_t::is_fully_init()			const throw()
{
	// if it is null, return false
	if( is_null() )				return false;
	// check totfile_range if the len() is greater than 0
	if( len() > 0 ){
		// if the totfile_range is not computed, return false
		if( totfile_range().is_null() )	return false;
	}
	// return true - if all the previous tests passed, it is considered fully initialized
	return true;
}

/** \brief Return true if the bt_mfile_subfile_t is ok for a bt_io_sfile_t usage
 * 
 * - TODO this function is likely not to contained all the required tests
 */
bool	bt_mfile_subfile_t::is_sfile_ok()			const throw()
{
	// if the bt_mfile_subfile_t is not fully init, return false
	if( is_fully_init() )		return false;
	// if there are no local_path, return false
	if( local_path().is_null() )	return false;
	// if all previous tests passes, return true
	return true;
}

/** \brief Return true if the bt_mfile_subfile_t is ok for a bt_io_sfile_t usage
 * 
 * - TODO this function is likely not to contained all the required tests
 */
bool	bt_mfile_subfile_t::is_pfile_ok()			const throw()
{
	// if the bt_mfile_subfile_t is not fully init, return false
	if( is_fully_init() )	return false;
	// if all previous tests passes, return true
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_mfile_subfile_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss        << "local_path="	<< local_path();
	oss << " " << "totfile_beg="	<< totfile_beg();
	oss << " " << "mfile_path="	<< mfile_path();
	oss << " " << "len="		<< len();
	oss << " " << "uri_arr="	<< uri_arr();
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_mfile_subfile_t
 * 
 * - support null bt_mfile_subfile_t
 * - a serialized bt_mfile_subfile_t stores only the non precomputed value
 */
serial_t& operator << (serial_t& serial, const bt_mfile_subfile_t &mfile_subfile)		throw()
{
	// serialize each field of the object
	serial << mfile_subfile.mfile_path();
	serial << mfile_subfile.local_path();
	serial << mfile_subfile.len();
	serial << mfile_subfile.uri_arr();
	// return serial
	return serial;
}

/** \brief unserialze a bt_mfile_subfile_t
 * 
 * - support null bt_mfile_subfile_t
 * - a serialized bt_mfile_subfile_t stores only the non precomputed value
 *   - thus it should be passed thru bt_mfile_t::complete_init() after 
 *     unserialization
 */
serial_t& operator >> (serial_t & serial, bt_mfile_subfile_t &mfile_subfile)		throw(serial_except_t)
{	
	file_path_t	mfile_path;
	file_path_t	local_path;
	file_size_t	len;
	http_uri_arr_t	uri_arr;
	// reset the destination variable
	mfile_subfile	= bt_mfile_subfile_t();

	// unserialize the data
	serial >> mfile_path;
	serial >> local_path;
	serial >> len;
	serial >> uri_arr;

	// set the returned variable
	mfile_subfile.mfile_path(mfile_path).local_path(local_path).len(len).uri_arr(uri_arr);
	// return serial
	return serial;
}
NEOIP_NAMESPACE_END

