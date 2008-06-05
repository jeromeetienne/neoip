/*! \file
    \brief Definition of the \ref file_range_t

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_file_range.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     query function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true if the object is considered sane
 * 
 * - aimed at being used inside DBG_ASSERT()
 */
bool	file_range_t::is_sane()				const throw()
{
	// if the object is null, return true
	if( is_null() )				return true;
#if 1	// TODO to remove - special code to display data as the following assert fails
	// - the version in the #else is the same wihtout the KLOG_ERR...
	// if m_beg or m_end is null, return false
	if( m_beg.is_null() || m_end.is_null())	{ KLOG_ERR("file_range=" << *this);	return false;}
	// if m_beg is file_size_t::MAX, return false
	if( m_beg == file_size_t::MAX )		{ KLOG_ERR("file_range=" << *this);	return false;}
	// if the begining is > than the end, return false
	if( m_beg > m_end )			{ KLOG_ERR("file_range=" << *this);	return false;}
#else
	// if m_beg or m_end is null, return false
	if( m_beg.is_null() || m_end.is_null())	return false;
	// if m_beg is file_size_t::MAX, return false
	if( m_beg == file_size_t::MAX )		return false;
	// if the begining is > than the end, return false
	if( m_beg > m_end )			return false;
#endif
	// if this point is reached, return true
	return true;
}

/** \brief Return true if this file_range_t is distinct from the other
 */
bool	file_range_t::is_distinct(const file_range_t &other)	const throw()
{
	// sanity check - both file_range_t MUST NOT be null
	DBG_ASSERT( !is_null() && !other.is_null() );
	// if this begin is greater than the end of the other one, return true
	if( beg() > other.end() )	return true;
	// if the end of this file_range_t is less than other begin, return true
	if( end() < other.beg() )	return true;
	// if all previous tests passed, return false
	return false;
}

/** \brief Return true if this file_range_t fully include the other
 */
bool	file_range_t::fully_include(const file_range_t &other)	const throw()
{
	// sanity check - both file_range_t MUST NOT be null
	DBG_ASSERT( !is_null() && !other.is_null() );
	// if this begin is greater than the other begin, return false
	if( beg() > other.beg() )	return false;
	// if this end is less than other end, return true
	if( end() < other.end() )	return false;
	// if all previous tests passed, return true
	return true;
}

/** \brief Return true if this file_range_t is contiguous to the other one
 * 
 * - it test if there are STRICLY contiguous, aka it reject overlapping file_range_t
 */
bool	file_range_t::is_contiguous(const file_range_t &other)	const throw()
{
	// sanity check - both file_range_t MUST NOT be null
	DBG_ASSERT( !is_null() && !other.is_null() );
	// if this begin is equal to the other end, return true
	if( beg() == other.end()+1 )	return true;
	// if this end is equal to the other begin, return true
	if( end()+1 == other.beg() )	return true;
	// if this point is reached, return false
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    action function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief clamp this file_range_t with the beg/end of the other
 */
file_range_t &	file_range_t::clamped_by(const file_range_t &other)	throw()
{
	// sanity check - both file_range_t MUST NOT be null
	DBG_ASSERT( !is_null() && !other.is_null() );
	// if this file_range_t is from the other, nullify this one and return now
	if( is_distinct(other) ){
		*this	= file_range_t();
		return *this;
	}
	// if this begin is less than the other one, clamp this beg() with the other
	if( beg() < other.beg() )	beg( other.beg() );
	// if this end is more than the other one, clamp this end() with the other
	if( end() > other.end() )	end( other.end() );
	// sanity check - the result MUST be sane
	DBG_ASSERT(is_sane());
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int file_range_t::compare(const file_range_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// compare the beg
	if( beg() < other.beg() )		return -1;	
	if( beg() > other.beg() )		return +1;	
	// NOTE: here both have the same beg

	// compare the end()
	if( end() < other.end() )		return -1;	
	if( end() > other.end() )		return +1;	
	// NOTE: here both have the same end()

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
std::string file_range_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss        << "beg="	<< beg();
	oss << " " << "end="	<< end();
	oss << "(" << len()	<< "-byte)";
	// return the just built string
	return oss.str();
}

/** \brief convert the object into a string
 */
std::string file_range_t::to_human_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss        << "beg="	<< beg().to_human_string();
	oss << " " << "end="	<< end().to_human_string();
	oss << "(" << len().to_human_string()	<< ")";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






