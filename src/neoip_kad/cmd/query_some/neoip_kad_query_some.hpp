/*! \file
    \brief Declaration of the kad_query_some_t
    
*/


#ifndef __NEOIP_KAD_QUERY_SOME_HPP__ 
#define __NEOIP_KAD_QUERY_SOME_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_query_some_cb.hpp"
#include "neoip_kad_query_some_wikidbg.hpp"
#include "neoip_kad_nsearch_cb.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;

/** \brief try to find kad_rec_t matching a kad_keyid_t among the max_nb_record kad_peer_t
 *         the closest to it. and stop as soon as at least one record is found
 */
class kad_query_some_t : NEOIP_COPY_CTOR_DENY, private kad_nsearch_cb_t
				, private wikidbg_obj_t<kad_query_some_t, kad_query_some_wikidbg_init> {
private:
	kad_addr_arr_t	base_addr_arr;	//!< the kad_addr_arr_t to start with (may be null)
	
	/*************** kad_nsearch_t	***************************************/
	kad_nsearch_t *	kad_nsearch;
	bool 		neoip_kad_nsearch_cb(void *cb_userptr, kad_nsearch_t &cb_kad_nsearch
							, const kad_event_t &kad_event)		throw();

	/*************** Callback	***************************************/
	kad_query_some_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_query_some_t()	throw();
	~kad_query_some_t()	throw();
	
	/*************** Setup Functoin	***************************************/
	kad_query_some_t &set_base_addr_arr(const kad_addr_arr_t &base_addr_arr)	throw();
	kad_err_t	start(kad_peer_t *kad_peer, const kad_keyid_t &kad_keyid
					, size_t max_nb_record, size_t max_nb_node
					, const delay_t &expire_delay
					, kad_query_some_cb_t *callback, void *userptr) throw();

	/*************** List of friend classes	*******************************/
	friend class	kad_query_some_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_QUERY_SOME_HPP__ 



