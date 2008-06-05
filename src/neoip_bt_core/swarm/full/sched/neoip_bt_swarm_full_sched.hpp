/*! \file
    \brief Header of the bt_swarm_full_sched_t
    
*/


#ifndef __NEOIP_BT_SWARM_FULL_SCHED_HPP__ 
#define __NEOIP_BT_SWARM_FULL_SCHED_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_bt_swarm_full_sched_wikidbg.hpp"
#include "neoip_bt_reqauth_type.hpp"
#include "neoip_bt_swarm_sched_cnx_vapi.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_full_t;
class	bt_swarm_sched_request_t;
class	file_range_t;

/** \brief class definition for bt_swarm_full_sched_t
 * 
 * - it contains the scheduling context in relation with a given bt_swarm_full_t
 */
class bt_swarm_full_sched_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
		, public bt_swarm_sched_cnx_vapi_t
		, private wikidbg_obj_t<bt_swarm_full_sched_t, bt_swarm_full_sched_wikidbg_init, bt_swarm_sched_cnx_vapi_t>
		{
public:
	typedef	std::set<bt_swarm_sched_request_t *> request_db_t;
private:
	bt_swarm_full_t *	swarm_full;	//!< the bt_swarm_full_t on which this request occurs

	/*************** reqauth_timeout_t	*******************************/
	bt_reqauth_type_t	m_reqauth_type;
	timeout_t		reqauth_timeout;
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** request_t in progress	*******************************/
	request_db_t		request_db;
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_full_sched_t(bt_swarm_full_t *swarm_full)	throw();
	~bt_swarm_full_sched_t()				throw();
	
	/*************** query function	***************************************/
	request_db_t &		get_request_db()	throw()		{ return request_db;		}
	bt_swarm_full_t *	get_swarm_full()	const throw()	{ return swarm_full;		}
	
	size_t			nb_pending_request()	const throw()	{ return request_db.size();	}
	size_t			req_queue_maxlen()	const throw();
	bool			has_matching_request(const file_range_t &totfile_range)	const throw();
	bt_swarm_sched_request_t *request_by_pieceidx(size_t pieceidx)	const throw();

	/*************** reqauth function	*******************************/
	const bt_reqauth_type_t&reqauth_type()	const throw()	{ return m_reqauth_type;	}
	void		reqauth_type(const bt_reqauth_type_t &m_reqauth_type
					, const delay_t &expire_delay = delay_t::NEVER)	throw();
	void		sync_doauth_req_cmd()						throw();

	/*************** bt_swarm_sched_cnx_vapi_t	***********************/
	void		sched_req_dolink_start(bt_swarm_sched_request_t *request) 	throw();
	void		sched_req_unlink(bt_swarm_sched_request_t *request)		throw();
	void		sched_req_cancel(bt_swarm_sched_request_t *request)		throw();

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_full_sched_wikidbg_t;
	friend class	bt_swarm_sched_request_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_FULL_SCHED_HPP__  */



