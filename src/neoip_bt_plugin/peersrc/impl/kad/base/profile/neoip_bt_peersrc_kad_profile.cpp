/*! \file
    \brief Definition of the \ref bt_peersrc_kad_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_peersrc_kad_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_peersrc_kad_profile_t constant
#if 0
	const delay_t	bt_peersrc_kad_profile_t::RETRY_DELAY_LOCAL_ERR	= delay_t::from_sec(2*60);
	const delay_t	bt_peersrc_kad_profile_t::RETRY_DELAY_CNX_ERR	= delay_t::from_sec(5*60);
	const delay_t	bt_peersrc_kad_profile_t::RETRY_DELAY_SUCCEED	= delay_t::from_sec(10*60);
	const delay_t	bt_peersrc_kad_profile_t::RECORD_TTL		= delay_t::from_sec(10*60);
	const delay_t	bt_peersrc_kad_profile_t::QUERY_TIMEOUT		= delay_t::from_sec(60);
#else	// debug value
	const delay_t	bt_peersrc_kad_profile_t::RETRY_DELAY_LOCAL_ERR	= delay_t::from_sec(2*60);
	const delay_t	bt_peersrc_kad_profile_t::RETRY_DELAY_CNX_ERR	= delay_t::from_sec(10);
	const delay_t	bt_peersrc_kad_profile_t::RETRY_DELAY_SUCCEED	= delay_t::from_sec(10);
	const delay_t	bt_peersrc_kad_profile_t::RECORD_TTL		= delay_t::from_sec(10*60);
	const delay_t	bt_peersrc_kad_profile_t::QUERY_TIMEOUT		= delay_t::from_sec(60);
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_peersrc_kad_profile_t::bt_peersrc_kad_profile_t()	throw()
{
	retry_delay_local_err	(RETRY_DELAY_LOCAL_ERR);
	retry_delay_cnx_err	(RETRY_DELAY_CNX_ERR);
	retry_delay_succeed	(RETRY_DELAY_SUCCEED);
	record_ttl		(RECORD_TTL);
	query_timeout		(QUERY_TIMEOUT);
}

/** \brief Destructor
 */
bt_peersrc_kad_profile_t::~bt_peersrc_kad_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_peersrc_kad_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

