/*! \file
    \brief Header of the bt_swarm_sched_request_t
    
*/


#ifndef __NEOIP_BT_SWARM_SCHED_REQUEST_HPP__ 
#define __NEOIP_BT_SWARM_SCHED_REQUEST_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_swarm_sched_request_wikidbg.hpp"
#include "neoip_file_range.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_sched_cnx_vapi_t;
class	bt_swarm_sched_block_t;

/** \brief class definition for bt_swarm_sched
 */
class bt_swarm_sched_request_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
			, private wikidbg_obj_t<bt_swarm_sched_request_t, bt_swarm_sched_request_wikidbg_init, timeout_cb_t>
			{
private:
	bt_swarm_sched_cnx_vapi_t *	sched_cnx;	//!< the bt_swarm_sched_cnx_vapi_t on which this request occurs
	bt_swarm_sched_block_t *	sched_block;	//!< the requested bt_swarm_sched_block_t

	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;	//!< triggered when the remote peer has been idle too long
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_sched_request_t(bt_swarm_sched_cnx_vapi_t *sched_cnx, bt_swarm_sched_block_t* sched_block
					, const delay_t	&expire_delay)		throw();
	~bt_swarm_sched_request_t()						throw();
	
	/*************** query function	***************************************/
	bt_swarm_sched_cnx_vapi_t *get_sched_cnx()	const throw()	{ return sched_cnx;	}
	bt_swarm_sched_block_t*	get_sched_block()	const throw()	{ return sched_block;	}

	bool			is_expired()		const throw()	{ return !is_pending();	}
	bool			is_pending()		const throw()	{ return expire_timeout.is_running();	}

	size_t			piece_idx()		const throw();
	const file_range_t &	totfile_range()		const throw();
	const file_size_t &	totfile_beg()		const throw()	{ return totfile_range().beg();	}
	const file_size_t &	totfile_end()		const throw()	{ return totfile_range().end();	}

	/*************** action function	*******************************/
	void		notify_complete_from(bt_swarm_sched_cnx_vapi_t *sched_cnx)	throw();
	void		restart_timeout(const delay_t &max_expire_delay)		throw();

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_sched_request_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_SCHED_REQUEST_HPP__  */



