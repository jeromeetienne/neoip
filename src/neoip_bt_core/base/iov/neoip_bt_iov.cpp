/*! \file
    \brief Definition of the \ref bt_iov_t

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_bt_iov.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Obsolete ctor to remove
 */
bt_iov_t::bt_iov_t(size_t subfile_idx, const file_size_t &file_off, const file_size_t &file_len)	throw()
{
	// sanity check - the file_len MUST be > 0
	DBG_ASSERT( file_len > 0 );
	// set the parameter
	this->m_subfile_idx	= subfile_idx;
	this->m_subfile_range	= file_range_t(file_off, file_off + file_len - 1);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     query function for interval
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Return true if this bt_iov_t is distinct from the other
 */
bool	bt_iov_t::is_distinct(const bt_iov_t &other)	const throw()
{
	// if the file_idx doesnt matches, both are distinct
	if( subfile_idx() != other.subfile_idx() )		return true;
	// test the subfile_range
	return subfile_range().is_distinct( other.subfile_range() );
}

/** \brief Return true if this bt_iov_t fully include the other
 */
bool	bt_iov_t::fully_include(const bt_iov_t &other)	const throw()
{
	// if the file_idx doesnt matches, return false
	if( subfile_idx() != other.subfile_idx() )		return false;
	// test the subfile_range
	return subfile_range().fully_include( other.subfile_range() );
}

/** \brief Return true if this bt_iov_t is contiguous to the other one
 */
bool	bt_iov_t::is_contiguous(const bt_iov_t &other)	const throw()
{
	// sanity check - this bt_iov_t MUST be distinct to the other
	DBG_ASSERT( is_distinct(other) );
	// if the file_idx doesnt matches, return false
	if( subfile_idx() != other.subfile_idx() )		return false;
	// test the subfile_range
	return subfile_range().is_contiguous( other.subfile_range() );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     query function for totfile
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the totfile_beg for this bt_iov_t
 * 
 * @param bt_mfile	the bt_mfile_t required to get the totfile
 */
file_size_t	bt_iov_t::totfile_beg(const bt_mfile_t &bt_mfile)	const throw()
{
	const bt_mfile_subfile_t &	mfile_subfile	= bt_mfile.subfile_arr()[subfile_idx()];
	// return the mfile_subfile.totfile_beg() + subfile_beg()
	return mfile_subfile.totfile_beg() + subfile_beg();
}

/** \brief return the totfile_end for this bt_iov_t
 * 
 * @param bt_mfile	the bt_mfile_t required to get the totfile
 */
file_size_t	bt_iov_t::totfile_end(const bt_mfile_t &bt_mfile)	const throw()
{
	const bt_mfile_subfile_t &	mfile_subfile	= bt_mfile.subfile_arr()[subfile_idx()];
	// return the mfile_subfile.totfile_beg() + subfile_end()
	return mfile_subfile.totfile_beg() + subfile_end();
}

/** \brief return the totfile_range for this bt_iov_t
 */
file_range_t	bt_iov_t::totfile_range(const bt_mfile_t &bt_mfile)	const throw()
{
	return file_range_t(totfile_beg(bt_mfile), totfile_end(bt_mfile));
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int bt_iov_t::compare(const bt_iov_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// NOTE: here both are NOT null
	
	// compare the file_idx
	if( subfile_idx() < other.subfile_idx() )	return -1;	
	if( subfile_idx() > other.subfile_idx() )	return +1;	
	// NOTE: here both have the same file_idx

	// compare the subfile_range
	if( subfile_range() < other.subfile_range() )	return -1;	
	if( subfile_range() > other.subfile_range() )	return +1;	
	// NOTE: here both have the same subfile_range

	// here both are equal
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bt_iov_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss        << "subfile_idx="	<< subfile_idx();
	oss << " " << "subfile_range="	<< subfile_range();
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






