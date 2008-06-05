/*! \file
    \brief Implementation of the fdwatch_cond0_t

*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_fdwatch_cond0.hpp"
NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     OSTREAM redirection
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::ostream & operator << (std::ostream & os, const fdwatch_cond0_t &fdwatch_cond ) throw()
{
	if( fdwatch_cond.is_input() )	os << "(input)";
	if( fdwatch_cond.is_output() )	os << "(output)";
	if( fdwatch_cond.is_error() )	os << "(error)";
	return os;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     general flags handling
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true if the flag is set, otherwise return false 
 * 
 * - Generic function to get the flags
 */
bool fdwatch_cond0_t::get_flag( fdwatch_cond0_t::type flag ) const throw()
{
	return	value & flag;
}

/** \brief Set the flag in the \ref fdwatch_cond0_t
 * 
 * - Generic function to set the flags
 */
fdwatch_cond0_t &fdwatch_cond0_t::set_flag( fdwatch_cond0_t::type flag, bool on )	throw()
{
	if( on )	value	|= flag;
	else		value	&= ~flag;
	return *this;
}

/** \brief reset the flag
 */
fdwatch_cond0_t &fdwatch_cond0_t::reset()		throw()
{
	value = 0;
	return *this;
}

/** \brief return true if none condition are set
 */
bool fdwatch_cond0_t::is_null(void)	const throw()
{
	if( is_input() )	return false;
	if( is_output() )	return false;
	if( is_error() )	return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       convertion from/to glib_cond
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** convert a fdwatch_cond0_t to a glib GIOCondition
 */
GIOCondition fdwatch_cond0_t::to_glib_cond()					const throw()
{
	int	val = 0;
	if( is_input() )	val |= G_IO_IN;
	if( is_output() )	val |= G_IO_OUT;
	if( is_error() )	val |= G_IO_ERR;
	return static_cast<GIOCondition>(val);
}

/** convert a glib GIOCondition to a fdwatch_cond0_t
 */
fdwatch_cond0_t fdwatch_cond0_t::from_glib_cond(GIOCondition glib_cond)		throw()	
{
	fdwatch_cond0_t	cond;
	if( glib_cond & G_IO_IN )	cond.set_input(true);
	if( glib_cond & G_IO_OUT )	cond.set_output(true);
	if( glib_cond & G_IO_ERR )	cond.set_error(true);
	return cond;
}

NEOIP_NAMESPACE_END

