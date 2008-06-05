/*! \file
    \brief Header of the bt_peersrc_kad_t
    
*/


#ifndef __NEOIP_BT_PEERSRC_KAD_HPP__ 
#define __NEOIP_BT_PEERSRC_KAD_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_peersrc_kad_profile.hpp"
#include "neoip_bt_peersrc_kad_wikidbg.hpp"
#include "neoip_bt_peersrc_vapi.hpp"
#include "neoip_bt_peersrc_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_kad_publish_cb.hpp"
#include "neoip_kad_query_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	kad_peer_t;
class	kad_recdups_t;
class	ipport_addr_t;

/** \brief class definition for bt_swarm_tracker
 * 
 * - it MUST be ctor after bt_swarm_t and dtor before
 */
class bt_peersrc_kad_t : NEOIP_COPY_CTOR_DENY, public bt_peersrc_vapi_t
			, private kad_publish_cb_t, private kad_query_cb_t, private timeout_cb_t
			, private wikidbg_obj_t<bt_peersrc_kad_t, bt_peersrc_kad_wikidbg_init, bt_peersrc_vapi_t>
			{
private:
	bt_swarm_t *		bt_swarm;
	kad_peer_t *		kad_peer;
	size_t			m_nb_seeder;
	size_t			m_nb_leecher;
	bt_peersrc_kad_profile_t	profile;
	
	/*************** peer record stuff	*******************************/
	bt_err_t		publish_peer_record()		throw();
	kad_publish_t *		kad_publish;	//!< the kad_publish_t for the peer record
	bool			neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
						, const kad_event_t &cb_kad_event) throw();

	/*************** next_request_timeout_t	*******************************/
	timeout_t		next_request_timeout;	//!< to periodically send a bt_tracker_request_t
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** kad_query_t	***************************************/
	kad_query_t *		kad_query;
	bool			neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
								, const kad_event_t &kad_event)	throw();
	void			handle_recved_kad_recdups(const kad_recdups_t &kad_recdups)	throw();

	/*************** callback stuff	***************************************/
	bt_peersrc_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_peersrc_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_peersrc_kad_t() 		throw();
	~bt_peersrc_kad_t()		throw();
	
	/*************** setup function	***************************************/
	bt_peersrc_kad_t&set_profile(const bt_peersrc_kad_profile_t &profile)		throw();
	bt_err_t	start(bt_swarm_t *bt_swarm, kad_peer_t *kad_peer
					, bt_peersrc_cb_t *callback, void *userptr)	throw();

	/*************** bt_peersrc_vapi_t	*******************************/
	size_t		nb_seeder()		const throw()	{ return m_nb_seeder;	}
	size_t		nb_leecher()		const throw()	{ return m_nb_leecher;	}

	/*************** update function	*******************************/	
	void		update_listen_pview(const ipport_addr_t &new_listen_pview)	throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_peersrc_kad_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_KAD_HPP__  */



