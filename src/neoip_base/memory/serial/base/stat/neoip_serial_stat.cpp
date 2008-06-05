/*! \file
    \brief Definition of the \ref serial_stat_t

*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_serial_stat.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
serial_stat_t::serial_stat_t()	throw()
{
	// zero some field
	m_len_copied	= 0;
	m_obj_copied	= 0;
	m_head_realloc	= 0;
	m_head_memshift	= 0;
	m_tail_realloc	= 0;
	m_tail_memshift	= 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        arithmetic operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief the += operator
 */
serial_stat_t &	serial_stat_t::operator +=(const serial_stat_t &other)	throw()
{
	// sum every fields
	m_len_copied	+= other.m_len_copied;
	m_obj_copied	+= other.m_obj_copied;
	m_head_realloc	+= other.m_head_realloc;
	m_head_memshift	+= other.m_head_memshift;
	m_tail_realloc	+= other.m_tail_realloc;
	m_tail_memshift	+= other.m_tail_memshift;
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	serial_stat_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// build the string to return
	oss << "(" << "len_copied="	<< len_copied();
	oss << " " << "obj_copied="	<< obj_copied();	
	oss << " " << "head_realloc="	<< head_realloc();	
	oss << " " << "head_memshift="	<< head_memshift();	
	oss << " " << "tail_realloc="	<< tail_realloc();	
	oss << " " << "tail_memshift="	<< tail_memshift();	
	oss << ")";
	// return the just built string
	return oss.str();
}


NEOIP_NAMESPACE_END

