/*! \file
    \brief Definition of the \ref bt_ezsession_profile_t

- TODO to complete, it doesnt contains all the subprofile stuff
  - all the subprofile are required to maintain the configurability
*/


/* system include */
/* local include */
#include "neoip_bt_ezsession_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_ezsession_profile_t constant
const std::string	bt_ezsession_profile_t::NSLAN_ADDR	= "255.255.255.255:7777";
const std::string	bt_ezsession_profile_t::NSLAN_REALMID	= "bt_ezsession_t nslan_realmid";
const std::string	bt_ezsession_profile_t::KAD_REALMID	= "bt_ezsession_t kad_realmid";
const size_t		bt_ezsession_profile_t::XMIT_MAXRATE	= std::numeric_limits<size_t>::max();
const size_t		bt_ezsession_profile_t::RECV_MAXRATE	= std::numeric_limits<size_t>::max();
const rate_prec_t	bt_ezsession_profile_t::DFL_RATE_PREC	= rate_prec_t(50);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_ezsession_profile_t::bt_ezsession_profile_t()	throw()
{
	nslan_addr	(NSLAN_ADDR);
	nslan_realmid	(NSLAN_REALMID);
	kad_realmid	(KAD_REALMID);
	xmit_maxrate	(XMIT_MAXRATE);
	recv_maxrate	(RECV_MAXRATE);
	dfl_rate_prec	(DFL_RATE_PREC);
}

/** \brief Destructor
 */
bt_ezsession_profile_t::~bt_ezsession_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_ezsession_profile_t::check()	const throw()
{
	bt_err_t	bt_err;
	// check the bt_session_profile_t
	bt_err	= session().check();
	if( bt_err.failed() )	return bt_err;
#if 0	// NOTE: disable as sometime the b_io_cache_pool_t is not used so the profile is not set
	// - dirty...
	
	// check the bt_io_cache_pool_profile_t
	bt_err	= io_cache_pool().check();
	if( bt_err.failed() )	return bt_err;
#endif
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

