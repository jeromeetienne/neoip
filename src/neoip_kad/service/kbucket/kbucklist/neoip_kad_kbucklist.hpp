/*! \file
    \brief Header of the \ref kad_kbucket_t
    
*/


#ifndef __NEOIP_KAD_KBUCKLIST_HPP__ 
#define __NEOIP_KAD_KBUCKLIST_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_kbucklist_wikidbg.hpp"
#include "neoip_kad_kbucknode.hpp"
#include "neoip_kad_closestnode_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief store a single bucket in \ref kad_kbucket_t
 */
class kad_kbucket_t::bucklist_t : NEOIP_COPY_CTOR_DENY, private kad_closestnode_cb_t
			, private timeout_cb_t
			, private wikidbg_obj_t<kad_kbucket_t::bucklist_t, kad_kbucklist_wikidbg_init>
			{
private:
	kad_kbucket_t *				kad_kbucket;	//!< backpointer on the kad_kbucket_t
	std::list<kad_kbucket_t::bucknode_t *>	bucknode_lru;	//!< the lru containing all the bucknode_t

	/*************** Internal function	*******************************/
	bool				peerid_is_present(const kad_peerid_t &peerid)	const throw();


	/*************** Refreshing	***************************************/
	timeout_t	refresh_timeout;//!< the timeout which trigger automatic refresh
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	kad_closestnode_t *kad_closestnode;	//!< the closestnode needed for refreshing
	bool 		neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
						, const kad_event_t &kad_event)	throw();

public:
	/*************** ctor/dtor	***************************************/
	bucklist_t(kad_kbucket_t *kad_kbucket)	throw();
	~bucklist_t()	throw();

	/*************** query function	***************************************/
	bool				is_refreshing()	const throw()	{ return kad_closestnode;	}	
	size_t				size()		const throw()	{ return bucknode_lru.size();	}
	kad_kbucket_t::bucknode_t *	get_first_notpinging()				throw();
	kad_kbucket_t::bucknode_t *	get_node_idx(size_t idx)			throw();
	kad_kbucket_t::bucknode_t*	get_bucknode(const kad_peerid_t &peerid)	throw();

	/*************** update function	*******************************/
	void	notify_nsearch()	throw();
	void	trigger_refresh_now()	throw();
	void	insert(kad_kbucket_t::bucknode_t *bucknode)	throw();
	void	update(kad_kbucket_t::bucknode_t *bucknode)	throw();
	void	remove(kad_kbucket_t::bucknode_t *bucknode)	throw();
	void	copy_to(kad_addr_arr_t &kad_addr_arr, kad_addr_t::filter_fct_t kad_addr_filter)	const throw();

	/*************** Friend class	***************************************/
	friend	class	kad_kbucklist_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_KBUCKLIST_HPP__  */










