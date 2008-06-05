/*! \file
    \brief Header of the bt_swarm_peersrc_t
    
*/


#ifndef __NEOIP_BT_SWARM_PEERSRC_HPP__ 
#define __NEOIP_BT_SWARM_PEERSRC_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_swarm_peersrc_wikidbg.hpp"
#include "neoip_bt_swarm_itor_cb.hpp"
#include "neoip_bt_peersrc_peer.hpp"
#include "neoip_bt_peersrc_cb.hpp"
#include "neoip_expireset.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_swarm_itor_t;
class	bt_peersrc_peer_arr_t;

/** \brief class definition for bt_swarm_peersrc_t
 */
class bt_swarm_peersrc_t : NEOIP_COPY_CTOR_DENY
				, private bt_swarm_itor_cb_t, public bt_peersrc_cb_t
				, private zerotimer_cb_t
				, private wikidbg_obj_t<bt_swarm_peersrc_t, bt_swarm_peersrc_wikidbg_init>
				{
public:
	typedef expireset_t<bt_peersrc_peer_t>	itor_blacklist_t;
private:
	bt_swarm_t *			bt_swarm;	//!< backpointer to the linked bt_swarm_t
	std::list<bt_peersrc_peer_t>	peersrc_peer_db;//!< list of all the peer to connect
	itor_blacklist_t *		itor_blacklist;	//!< the bt_peersrc_peer_t blacklist

	/*************** Internal function	*******************************/
	void		feed_new_peer(const bt_peersrc_peer_t &peersrc_peer)	throw();
	void		try_launch_new_itor()					throw();
	/*************** zerotimer_t	***************************************/
	zerotimer_t	newitor_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw();

	/*************** bt_peersrc_vapi_t	*******************************/
	std::list<bt_peersrc_vapi_t *>	peersrc_db;
	bool neoip_bt_peersrc_cb(void *cb_userptr, bt_peersrc_vapi_t &cb_peersrc_vapi
					, const bt_peersrc_event_t &peersrc_event)	throw();
					
	/*************** store the bt_swarm_itor_t	***********************/
	std::list<bt_swarm_itor_t *>	itor_db;
	void itor_dolink(bt_swarm_itor_t *itor) throw()	{ itor_db.push_back(itor);	}
	void itor_unlink(bt_swarm_itor_t *itor)	throw()	{ itor_db.remove(itor);		}
	bool		neoip_bt_swarm_itor_cb(void *cb_userptr, bt_swarm_itor_t &cb_swarm_itor
					, const bt_err_t &itor_err, socket_full_t *socket_full
					, const bytearray_t &recved_data
					, const bt_handshake_t &remote_handshake)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_peersrc_t() 		throw();
	~bt_swarm_peersrc_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_swarm_t *bt_swarm, const bt_peersrc_peer_arr_t &peersrc_peer_arr)	throw();

	/*************** query function	***************************************/
	bt_swarm_t *	get_swarm()		const throw()	{ return bt_swarm;	}
	size_t		nb_seeder()		const throw();
	size_t		nb_leecher()		const throw();	
	bt_swarm_itor_t*itor_by_remote_addr(const socket_addr_t &remote_addr)	throw();
	bool		is_new_itor_allowed()					const throw();

	/*************** Action function	*******************************/
	void		may_launch_new_itor()	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_peersrc_wikidbg_t;
	friend class	bt_swarm_itor_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_PEERSRC_HPP__  */



