/*! \file
    \brief Definition of the \ref file_aio_profile_t

*/


/* system include */
/* local include */
#include "neoip_file_aio_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref file_aio_profile_t constant
const file_size_t	file_aio_profile_t::AREAD_ITERLEN	= 512*1024;
const file_size_t	file_aio_profile_t::AWRITE_ITERLEN	= 512*1024;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
file_aio_profile_t::file_aio_profile_t()	throw()
{
	aread_iterlen	(AREAD_ITERLEN);
	awrite_iterlen	(AWRITE_ITERLEN);
}

/** \brief Destructor
 */
file_aio_profile_t::~file_aio_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
file_err_t	file_aio_profile_t::check()	const throw()
{
	// return no error
	return file_err_t::OK;
}

NEOIP_NAMESPACE_END

