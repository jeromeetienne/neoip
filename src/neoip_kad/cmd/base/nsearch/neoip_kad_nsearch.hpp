/*! \file
    \brief Header of the \ref kad_nsearch_t
    
*/


#ifndef __NEOIP_KAD_NSEARCH_HPP__ 
#define __NEOIP_KAD_NSEARCH_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_nsearch_cb.hpp"
#include "neoip_kad_nsearch_wikidbg.hpp"
#include "neoip_kad_pkttype.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;
class	kad_nsearch_llist_t;

/** \brief class to get the n closest node to a given kad_targetid_t
 */
class kad_nsearch_t : private timeout_cb_t, NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<kad_nsearch_t, kad_nsearch_wikidbg_init> {
private:
	kad_peer_t *		kad_peer;	//!< backpointer for this peer
	kad_targetid_t		target_id;	//!< the kad_id which is looked for
	size_t			n_closest;	//!< the number of closest nodes requested by the caller
	kad_pkttype_t		query_type;	//!< the type of query to use in this node search
	/*************** List of learner node	*******************************/
	kad_nsearch_llist_t *	llist;

	/*************** kad_nsearch_t expiration	***********************/
	timeout_t		expire_timeout;
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();	
	
	/*************** post processing of all start() functions	*******/
	kad_err_t		post_start(size_t n_closest, const delay_t &timeout_delay
						, const kad_addr_arr_t &base_addr_arr) 	throw();

	/*************** Callback	***************************************/
	kad_nsearch_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

public:
	/*************** ctor/dtor	***************************************/
	kad_nsearch_t(kad_nsearch_cb_t *callback, void *userptr, kad_peer_t *kad_peer)	throw();
	~kad_nsearch_t()	throw();
	
	/*************** Start function	***************************************/
	kad_err_t start_findnode(const kad_peerid_t &peerid, size_t n_closest
				, const delay_t &timeout_delay
				, const kad_addr_arr_t &base_addr_arr = kad_addr_arr_t())	throw();
	kad_err_t start_findsomeval(const kad_keyid_t &keyid, size_t max_nb_record
				, size_t n_closest, const delay_t &timeout_delay
				, const kad_addr_arr_t &base_addr_arr = kad_addr_arr_t())	throw();

	/*************** Query function	***************************************/
	const kad_pkttype_t &	get_query_type()const throw()	{ return query_type;	}


	/*************** List of friend classes	*******************************/
	friend class	kad_nsearch_wikidbg_t;
	friend class	kad_nsearch_llist_t;
	friend class	kad_nsearch_lnode_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_NSEARCH_HPP__  */










