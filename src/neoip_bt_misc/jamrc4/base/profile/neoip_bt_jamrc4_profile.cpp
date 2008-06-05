/*! \file
    \brief Definition of the \ref bt_jamrc4_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_jamrc4_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_jamrc4_profile_t constant
const bt_jamrc4_type_t	bt_jamrc4_profile_t::ITOR_TYPE_DEFAULT	= strtype_bt_jamrc4_type_t::NOJAM;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_jamrc4_profile_t::bt_jamrc4_profile_t()	throw()
{
	// init each plain field with its default value
	itor_type_default	(ITOR_TYPE_DEFAULT);
	// special set for resp_type_accept_arr
	resp_type_accept_arr().append(bt_jamrc4_type_t::NOJAM);
}

/** \brief Destructor
 */
bt_jamrc4_profile_t::~bt_jamrc4_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_jamrc4_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

