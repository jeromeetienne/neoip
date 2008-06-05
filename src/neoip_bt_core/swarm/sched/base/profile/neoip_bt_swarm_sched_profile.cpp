/*! \file
    \brief Definition of the \ref bt_swarm_sched_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_swarm_sched_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_swarm_sched_profile_t constant
#if 0
	const delay_t	bt_swarm_sched_profile_t::REQUEST_TIMEOUT_NORMAL	= delay_t::from_sec(60);
	const delay_t	bt_swarm_sched_profile_t::REQUEST_TIMEOUT_ENDGAME	= delay_t::from_sec(2);
	const delay_t	bt_swarm_sched_profile_t::REQ_QUEUE_DELAY		= delay_t::from_msec(200);
	const size_t	bt_swarm_sched_profile_t::REQ_QUEUE_MINLEN		= 1*(16*1024);
	const size_t	bt_swarm_sched_profile_t::REQ_QUEUE_MAXLEN		= 30*(16*1024);
	const bool	bt_swarm_sched_profile_t::SEQ_PSELECT_IFEQU		= true;
#else	// debug only value
	const delay_t	bt_swarm_sched_profile_t::REQUEST_TIMEOUT_NORMAL	= delay_t::from_sec(20);
	const delay_t	bt_swarm_sched_profile_t::REQUEST_TIMEOUT_ENDGAME	= delay_t::from_sec(2);
	const delay_t	bt_swarm_sched_profile_t::REQ_QUEUE_DELAY		= delay_t::from_msec(1000);
	const size_t	bt_swarm_sched_profile_t::REQ_QUEUE_MINLEN		= 1*(16*1024);
	const size_t	bt_swarm_sched_profile_t::REQ_QUEUE_MAXLEN		= 30*(16*1024);
	const bool	bt_swarm_sched_profile_t::SEQ_PSELECT_IFEQU		= true;
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_sched_profile_t::bt_swarm_sched_profile_t()	throw()
{
	request_timeout_normal	(REQUEST_TIMEOUT_NORMAL);
	request_timeout_endgame	(REQUEST_TIMEOUT_ENDGAME);
	req_queue_delay		(REQ_QUEUE_DELAY);
	req_queue_minlen	(REQ_QUEUE_MINLEN);
	req_queue_maxlen	(REQ_QUEUE_MAXLEN);
	seq_pselect_ifequ	(SEQ_PSELECT_IFEQU);
}

/** \brief Destructor
 */
bt_swarm_sched_profile_t::~bt_swarm_sched_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_swarm_sched_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

