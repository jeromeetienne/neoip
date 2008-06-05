/*! \file
    \brief Header of the bt_peersrc_nslan_t
    
*/


#ifndef __NEOIP_BT_PEERSRC_NSLAN_HPP__ 
#define __NEOIP_BT_PEERSRC_NSLAN_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_peersrc_nslan_wikidbg.hpp"
#include "neoip_bt_peersrc_vapi.hpp"
#include "neoip_bt_peersrc_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_nslan_rec_src_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	nslan_peer_t;
class	nslan_publish_t;

/** \brief class definition for bt_swarm_tracker
 * 
 * - it MUST be ctor after bt_swarm_t and dtor before
 */
class bt_peersrc_nslan_t : NEOIP_COPY_CTOR_DENY, public bt_peersrc_vapi_t, private nslan_rec_src_cb_t
			, private wikidbg_obj_t<bt_peersrc_nslan_t, bt_peersrc_nslan_wikidbg_init, bt_peersrc_vapi_t>
			{
private:
	bt_swarm_t *		bt_swarm;
	nslan_peer_t *		nslan_peer;
	size_t			m_nb_seeder;
	size_t			m_nb_leecher;
	
	/*************** peer record stuff	*******************************/
	bt_err_t		publish_peer_record()		throw();
	nslan_publish_t *	nslan_publish;	//!< the nslan_publish_t for the peer record

	/*************** nslan_rec_src_t	*******************************/
	nslan_rec_src_t *	nslan_rec_src;	//!< the nslan_rec_src_t to query the nslan_peer_t
	bool			neoip_nslan_rec_src_cb(void *cb_userptr, nslan_rec_src_t &cb_nslan_rec_src
						, const nslan_rec_t &nslan_rec
						, const ipport_addr_t &src_addr)		throw();
	void			handle_recved_nslan_rec(const nslan_rec_t &nslan_rec
						, const ipport_addr_t &publisher_ipport)	throw();

	/*************** callback stuff	***************************************/
	bt_peersrc_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_peersrc_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_peersrc_nslan_t() 		throw();
	~bt_peersrc_nslan_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_swarm_t *bt_swarm, nslan_peer_t *nslan_peer
					, bt_peersrc_cb_t *callback, void *userptr)	throw();

	/*************** bt_peersrc_vapi_t	*******************************/
	size_t		nb_seeder()		const throw()	{ return m_nb_seeder;	}
	size_t		nb_leecher()		const throw()	{ return m_nb_leecher;	}

	/*************** List of friend class	*******************************/
	friend class	bt_peersrc_nslan_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_NSLAN_HPP__  */



