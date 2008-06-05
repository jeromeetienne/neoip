/*! \file
    \brief Class to handle the bt_ezswarm_check_t
    
\par Brief Description
\ref bt_ezswarm_check_t handle the bt_ezswarm_state_t::CHECK for bt_ezswarm_t

*/

/* system include */
/* local include */
#include "neoip_bt_ezswarm_check.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_check.hpp"
#include "neoip_bt_io_vapi.hpp"
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
bt_ezswarm_check_t::bt_ezswarm_check_t()		throw()
{
	// zero some field
	bt_ezswarm	= NULL;
	bt_check	= NULL;
}

/** \brief Destructor
 */
bt_ezswarm_check_t::~bt_ezswarm_check_t()		throw()
{
	// delete bt_check if needed
	nipmem_zdelete	bt_check;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_ezswarm_check_t::start(bt_ezswarm_t *bt_ezswarm)		throw()
{
	const bt_swarm_resumedata_t &	swarm_resumedata= bt_ezswarm->swarm_resumedata();
	const bt_ezswarm_profile_t	profile		= bt_ezswarm->profile();
	bt_err_t			bt_err;	
	// copy the parameter
	this->bt_ezswarm	= bt_ezswarm;

	// start the bt_check_t
	bt_check	= nipmem_new bt_check_t();
	bt_err		= bt_check->start(profile.check_policy(), swarm_resumedata
						, bt_ezswarm->io_vapi(), this, NULL);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_check_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_check_t to provide event
 */
bool	bt_ezswarm_check_t::neoip_bt_check_cb(void *cb_userptr, bt_check_t &cb_bt_check
				, const bt_err_t &bt_err
				, const bt_swarm_resumedata_t &swarm_resumedata)	throw() 
{
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err << " swarm_resumedata=" << swarm_resumedata);

	// update the bt_ezswarm_t with the notified bt_swarm_resumedata_t
	bt_ezswarm->m_swarm_resumedata	= swarm_resumedata;

	// delete the bt_check_t
	nipmem_zdelete bt_check;

	// if a error is notified by bt_check_t, forward it to the bt_ezswarm_t caller
	if( bt_err.failed() )	return bt_ezswarm->stateobj_notify_failure(bt_err);
	
	// notify the bt_ezswarm_t that this bt_check_t has been sucessfully completed
	return bt_ezswarm->stateobj_notify_success();
}

NEOIP_NAMESPACE_END





