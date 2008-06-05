/*! \file
    \brief Definition of the \ref bt_oload0_stat_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_oload0_stat_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_oload0_stat_profile_t constant
const size_t		bt_oload0_stat_profile_t::MFILE_PIECELEN		= 32*1024;
const std::string	bt_oload0_stat_profile_t::INFOHASH_PREFIX_STR	= "swarm static uri:";

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_oload0_stat_profile_t::bt_oload0_stat_profile_t()	throw()
{
	// init each plain field with its default value
	mfile_piecelen		(MFILE_PIECELEN);
	infohash_prefix_str	(INFOHASH_PREFIX_STR);
}

/** \brief Destructor
 */
bt_oload0_stat_profile_t::~bt_oload0_stat_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_oload0_stat_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

