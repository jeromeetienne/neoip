/*! \file
    \brief Definition of the \ref rate_prec_t

\par Brief Description
\ref rate_prec_t counts the number of occurences of each piece.

*/

/* system include */
/* local include */
#include "neoip_rate_prec.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref rate_prec_arr_t constant
const size_t	rate_prec_t::NONE		= 0;
const size_t	rate_prec_t::LOWEST		= 1;
const size_t	rate_prec_t::HIGHEST		= std::numeric_limits<size_t>::max();
const size_t	rate_prec_t::DEFAULT		= std::numeric_limits<size_t>::max()/2;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare function - retuning result ala memcmp
 */
int	rate_prec_t::compare(const rate_prec_t &other)		const throw()
{
	// compare the piece_arr
	if( to_size_t() < other.to_size_t() )	return -1;
	if( to_size_t() > other.to_size_t() )	return +1;
	// NOTE: both are considered equal
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string rate_prec_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << to_size_t();
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






