/*! \file
    \brief Definition of the \ref bt_peersrc_http_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_peersrc_http_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_peersrc_http_profile_t constant
const delay_t	bt_peersrc_http_profile_t::RETRY_DELAY_LOCAL_ERR= delay_t::from_sec(2*60);
const delay_t	bt_peersrc_http_profile_t::RETRY_DELAY_CNX_ERR	= delay_t::from_sec(5*60);
const delay_t	bt_peersrc_http_profile_t::RETRY_DELAY_BT_ERR	= delay_t::from_sec(10*60);
const bool	bt_peersrc_http_profile_t::USE_COMPACT_REQUEST	= true;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_peersrc_http_profile_t::bt_peersrc_http_profile_t()	throw()
{
	retry_delay_local_err	(RETRY_DELAY_LOCAL_ERR);
	retry_delay_cnx_err	(RETRY_DELAY_CNX_ERR);
	retry_delay_bt_err	(RETRY_DELAY_BT_ERR);
	use_compact_request	(USE_COMPACT_REQUEST);
	
	// set some special value in bt_tracker_client_profile_t
	tracker_client().http_sclient().http_client().timeout_delay(delay_t::from_sec(30));
}

/** \brief Destructor
 */
bt_peersrc_http_profile_t::~bt_peersrc_http_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_peersrc_http_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

