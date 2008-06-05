/*! \file
    \brief Definition of the \ref bt_io_pfile_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_io_pfile_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_io_pfile_profile_t constant
const bool	bt_io_pfile_profile_t::HAS_CIRCULARIDX		= false;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_io_pfile_profile_t::bt_io_pfile_profile_t()	throw()
{
	// TODO this is unclear this is usefull
	// - it may be replaced by a bt_mfile_t::has_circularidx
	has_circularidx	(HAS_CIRCULARIDX);
}

/** \brief Destructor
 */
bt_io_pfile_profile_t::~bt_io_pfile_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_io_pfile_profile_t::check()	const throw()
{
	// dirpath MUST be set
	if( dest_dirpath().is_null() )	return bt_err_t(bt_err_t::ERROR, "dirpath is not set");
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

