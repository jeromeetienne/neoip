/*! \file
    \brief Definition of the \ref casti_swarm_udata_profile_t

*/


/* system include */
/* local include */
#include "neoip_casti_swarm_udata_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref casti_swarm_udata_profile_t constant
const delay_t	casti_swarm_udata_profile_t::XMIT_MAXDELAY		= delay_t::from_sec(10);	
const size_t	casti_swarm_udata_profile_t::XMIT_MAXPIECE		= 4;
const delay_t	casti_swarm_udata_profile_t::RECV_UDATA_MAXDELAY	= delay_t::from_sec(60);	
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
casti_swarm_udata_profile_t::casti_swarm_udata_profile_t()	throw()
{
	xmit_maxdelay		(XMIT_MAXDELAY);
	xmit_maxpiece		(XMIT_MAXPIECE);
	recv_udata_maxdelay	(RECV_UDATA_MAXDELAY);
}

/** \brief Destructor
 */
casti_swarm_udata_profile_t::~casti_swarm_udata_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	casti_swarm_udata_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

