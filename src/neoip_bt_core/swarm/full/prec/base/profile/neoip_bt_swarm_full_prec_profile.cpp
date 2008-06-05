/*! \file
    \brief Definition of the \ref bt_swarm_full_prec_profile_t


*/


/* system include */
/* local include */
#include "neoip_bt_swarm_full_prec_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_swarm_full_prec_profile_t constant
// TODO those numbers have been taken at random with no brain
// - maybe a good idea to put non random number for a crucial network performance stuff :)
const double	bt_swarm_full_prec_profile_t::XMIT_PREC_BASE		= 50;
const double	bt_swarm_full_prec_profile_t::XMIT_REQAUTH_RANGE	= 500;
const double	bt_swarm_full_prec_profile_t::RECV_PREC_BASE		= 50;
const double	bt_swarm_full_prec_profile_t::RECV_REQAUTH_RANGE	= 500;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_full_prec_profile_t::bt_swarm_full_prec_profile_t()	throw()
{
	xmit_prec_base		( XMIT_PREC_BASE	);
	xmit_reqauth_range	( XMIT_REQAUTH_RANGE	);
	recv_prec_base		( RECV_PREC_BASE	);
	recv_reqauth_range	( RECV_REQAUTH_RANGE	);
}

/** \brief Destructor
 */
bt_swarm_full_prec_profile_t::~bt_swarm_full_prec_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_swarm_full_prec_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

