/*! \file
    \brief Header of the \ref kad_kbucket_t
    
*/


#ifndef __NEOIP_KAD_KBUCKET_HPP__ 
#define __NEOIP_KAD_KBUCKET_HPP__ 
/* system include */
#include <vector>
#include <map>
/* local include */
#include "neoip_kad_kbucket_wikidbg.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_delay.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class kad_peer_t;
class kad_addr_arr_t;
class kad_event_t;

/** \brief class definition for id
 */
class kad_kbucket_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t
				, private wikidbg_obj_t<kad_kbucket_t, kad_kbucket_wikidbg_init>
				{
private:
	kad_peer_t *			kad_peer;	//!< backpointer to the kad_peer_t
	size_t				kbucket_width;
	size_t				kbucket_height;

	/*************** bucklist_t	***************************************/
	class				bucklist_t;
	std::vector<bucklist_t *>	bucklist_db;	//!< store all the bucklist_t

	/*************** bucknode_t	***************************************/
	class					bucknode_t;
	std::map<kad_targetid_t, bucknode_t *>	bucknode_db;	//!< store all bucknode_t in kad_kbucket_t
	std::map<kad_targetid_t, kad_addr_t *>	pending_node_db;//!< store all the nodes which are pending
								//!< while testing current nodes
	
	size_t		get_bucket_idx(const kad_peerid_t &peerid)		const throw();
	size_t		get_bucket_idx(const bucklist_t *bucklist)		const throw();
	bucknode_t *	get_bucknode(const kad_peerid_t &peerid)		throw();
	bool		is_pending_node(const kad_peerid_t &peerid)		throw();
	
	/*************** peerid_todelete stuff	*******************************/
	// - this mechanism deletes bucknodes by their peerid asynchronously via zerotimer
	//   it is used by the notify_failed_rpc() to avoid any race or weird nested issue
	std::list<kad_peerid_t>		peerid_todelete_db;	//!< db of peerid to delete
	zerotimer_t			peerid_todelete_zerotimer;
	bool	neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw();	
public:
	/*************** ctor/dtor	***************************************/
	kad_kbucket_t(kad_peer_t *kad_peer)	throw();
	~kad_kbucket_t()			throw();
	
	/*************** Query function	***************************************/
	//! return the current number of nodes in the kbucket (not including the pending ones)
	size_t		size()			const throw() { return bucknode_db.size();	}
	kad_addr_arr_t	get_noldest_addr(size_t max_nb_addr)				const throw();
	kad_addr_arr_t	get_nclosest_addr(const kad_targetid_t &peerid, size_t max_nb_addr
					, kad_addr_t::filter_fct_t kad_addr_filter)	const throw();
	
	/*************** Notification	***************************************/
	void	notify_srckaddr(const kad_addr_t &kad_addr)					throw();
	void	notify_nsearch(const kad_targetid_t &target_id)					throw();
	void	notify_failed_rpc(const kad_peerid_t &peerid, const kad_event_t &kad_event
						, const delay_t &expire_delay = delay_t())	throw();
	void	refresh_post_bstrap()								throw();
	void	refresh_all_bucklist()								throw();
	

	/*************** List of Friend function	***********************/
	friend	class	kad_kbucket_wikidbg_t;	
	friend	class	kad_kbucklist_wikidbg_t;
	friend	class	kad_kbucknode_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_KBUCKET_HPP__  */










