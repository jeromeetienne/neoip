/*! \file
    \brief Header of the bt_peersrc_http_t
    
*/


#ifndef __NEOIP_BT_PEERSRC_HTTP_HPP__ 
#define __NEOIP_BT_PEERSRC_HTTP_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_peersrc_http_profile.hpp"
#include "neoip_bt_peersrc_http_wikidbg.hpp"
#include "neoip_bt_peersrc_vapi.hpp"
#include "neoip_bt_peersrc_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_tracker_client_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_tracker_request_t;
class	ipport_addr_t;

/** \brief class definition for bt_peersrc for http
 * 
 * - it MUST be ctor after bt_swarm_t and dtor before
 */
class bt_peersrc_http_t : NEOIP_COPY_CTOR_DENY, public bt_peersrc_vapi_t
			, private bt_tracker_client_cb_t, private timeout_cb_t
			, private wikidbg_obj_t<bt_peersrc_http_t, bt_peersrc_http_wikidbg_init, bt_peersrc_vapi_t>
			{
private:
	bt_swarm_t *		bt_swarm;	//!< backpointer to the next
	std::string		m_failure_reason;//!< the last failure_reason from the bt_tracker_client_t
	size_t			m_nb_seeder;	//!< nb_seeder from the last reply
	size_t			m_nb_leecher;	//!< nb_leecher from the last reply
	delay_t			request_period;	//!< request_period from the last reply
	std::string		m_tracker_key;	//!< the bt_tracker_request_t::key() to use
	bt_peersrc_http_profile_t profile;

	bool			prev_seed_state;//!< the previous seed state of the bt_swarm_t
	size_t			nb_request_sent;//!< the number of request already sent
	
	/*************** Internal function	*******************************/
	bt_tracker_request_t	build_tracker_request(const std::string &event_str)	const throw();

	/*************** next_request_timeout_t	*******************************/
	timeout_t		next_request_timeout;	//!< to periodically send a bt_tracker_request_t
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** bt_tracker_client_t	*******************************/
	bt_tracker_client_t *	bt_tracker_client;
	bool 			neoip_bt_tracker_client_cb(void *cb_userptr
						, bt_tracker_client_t &cb_bt_tracker_client
						, const bt_err_t &bt_err
						, const bt_tracker_reply_t &reply)	throw();

	/*************** callback stuff	***************************************/
	bt_peersrc_cb_t *callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_peersrc_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_peersrc_http_t() 		throw();
	~bt_peersrc_http_t()		throw();
	
	/*************** setup function	***************************************/
	bt_peersrc_http_t&	set_profile(const bt_peersrc_http_profile_t &profile)	throw();
	bt_err_t		start(bt_swarm_t *bt_swarm, bt_peersrc_cb_t *callback
								, void *userptr)	throw();

	/*************** query function	***************************************/
	const delay_t &		get_request_period()	const throw()	{ return request_period;	}
	const std::string &	failure_reason()	const throw()	{ return m_failure_reason;	}

	/*************** bt_peersrc_vapi_t	*******************************/
	size_t			nb_seeder()		const throw()	{ return m_nb_seeder;	}
	size_t			nb_leecher()		const throw()	{ return m_nb_leecher;	}

	/*************** update function	*******************************/	
	void		update_listen_pview(const ipport_addr_t &new_listen_pview)	throw();
	
	
	/*************** List of friend class	*******************************/
	friend class	bt_peersrc_http_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_HTTP_HPP__  */



