/*! \file
    \brief Definition of the \ref file_mode_t

- TODO should be ported on top of bitflag_t

*/

/* system include */
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* local include */
#include "neoip_file_mode.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref file_mode_t constant
const uint32_t	file_mode_t::READ	= 1 << 0;
const uint32_t	file_mode_t::WRITE	= 1 << 1;
const uint32_t	file_mode_t::APPEND	= 1 << 2;
const uint32_t	file_mode_t::CREATE	= 1 << 3;
const uint32_t	file_mode_t::TRUNCATE	= 1 << 4;
const uint32_t	file_mode_t::RW		= (1 << 0) | (1 << 1);	
const uint32_t	file_mode_t::WPLUS	= (1 << 1) | (1 << 3) | (1 << 4);	
// end of constants definition


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the file_mode_t is to be considered null
 */
bool	file_mode_t::is_null()	const throw()
{
	// if this file_mode_t contains at least READ or WRITE, it is not null
	if( this->include(file_mode_t::READ) )	return false;
	if( this->include(file_mode_t::WRITE) )	return false;
	// else it is considered null
	return true;
}

/** \brief return true if this file_mode_t include the other
 */
bool	file_mode_t::include(const file_mode_t &other)	const throw()
{
	// if the other.mode is NOT included in this one, return false
	if( (mode & other.mode) != other.mode )	return false;
	// else return true
	return true;
}

/** \brief Convert the file_mode_t into flag for the open(2) syscall
 */
int	file_mode_t::to_open_flags()		const throw()
{
	int	flags = 0;
	// convert the file_mode_t into a open(2) flag
	if( include( file_mode_t::READ | file_mode_t::WRITE ) )	flags	|= O_RDWR;
	else if( include( file_mode_t::READ ) )			flags	|= O_RDONLY;
	else if( include( file_mode_t::WRITE ) )		flags	|= O_WRONLY;
	if( include( file_mode_t::APPEND ) )			flags	|= O_APPEND;
	if( include( file_mode_t::CREATE ) )			flags	|= O_CREAT;
	if( include( file_mode_t::TRUNCATE ) )			flags	|= O_TRUNC;
	// return the result
	return flags;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief insert a new file_mode_t into this one
 */
file_mode_t &	file_mode_t::insert(const file_mode_t &other)	throw()
{
	// insert the other mode
	mode	|= other.mode;
	// return the object itself
	return *this;
}

/** \brief remove a file_mode_t from this one
 */
file_mode_t &	file_mode_t::remove(const file_mode_t &other)	throw()
{
	// remove the other mode
	mode	&= ~other.mode;
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main compare function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int file_mode_t::compare(const file_mode_t &other)	const throw()
{
	// compare the mode as if it was a integer value
	if( mode < other.mode )		return -1;	
	if( mode > other.mode )		return +1;
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
std::string file_mode_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// build the string
	if( this->include( file_mode_t::READ) )		oss << "r";
	else						oss << "-";
	if( this->include( file_mode_t::WRITE) )	oss << "w";
	else						oss << "-";
	if( this->include( file_mode_t::APPEND) )	oss << "a";
	else						oss << "-";
	if( this->include( file_mode_t::CREATE) )	oss << "c";
	else						oss << "-";
	if( this->include( file_mode_t::TRUNCATE) )	oss << "t";
	else						oss << "-";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






