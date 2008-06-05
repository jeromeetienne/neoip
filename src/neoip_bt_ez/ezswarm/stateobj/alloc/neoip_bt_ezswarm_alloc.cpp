/*! \file
    \brief Class to handle the bt_ezswarm_alloc_t
    
\par Brief Description
\ref bt_ezswarm_alloc_t handle the bt_ezswarm_state_t::ALLOC for bt_ezswarm_t

*/

/* system include */
/* local include */
#include "neoip_bt_ezswarm_alloc.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_alloc.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_ezswarm_alloc_t::bt_ezswarm_alloc_t()		throw()
{
	// zero some field
	bt_ezswarm	= NULL;
	bt_alloc	= NULL;
}

/** \brief Destructor
 */
bt_ezswarm_alloc_t::~bt_ezswarm_alloc_t()		throw()
{
	// delete the bt_alloc if needed
	nipmem_zdelete	bt_alloc;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_ezswarm_alloc_t::start(bt_ezswarm_t *bt_ezswarm)		throw()
{
	const bt_mfile_t &		bt_mfile	= bt_ezswarm->mfile();
	const bt_ezswarm_profile_t	profile		= bt_ezswarm->profile();
	bt_err_t			bt_err;
	// copy the parameter
	this->bt_ezswarm= bt_ezswarm;	
	// start the bt_alloc_t
	bt_alloc	= nipmem_new bt_alloc_t();
	bt_err		= bt_alloc->set_profile(profile.alloc_profile())
					.start(bt_mfile, profile.alloc_policy(), this, NULL);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_alloc_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_alloc_t to provide event
 */
bool	bt_ezswarm_alloc_t::neoip_bt_alloc_cb(void *cb_userptr, bt_alloc_t &cb_bt_alloc
						, const bt_err_t &bt_err)	throw() 
{
	// log to debug
	KLOG_ERR("enter bt_err=" << bt_err);

	// delete the bt_alloc_t
	nipmem_zdelete bt_alloc;
	
	// if a error is notified by bt_alloc_t,
	if( bt_err.failed() )	return bt_ezswarm->stateobj_notify_failure(bt_err);
	
	// notify the bt_ezswarm_t that this bt_alloc has been sucessfully completed
	return bt_ezswarm->stateobj_notify_success();
}

NEOIP_NAMESPACE_END





