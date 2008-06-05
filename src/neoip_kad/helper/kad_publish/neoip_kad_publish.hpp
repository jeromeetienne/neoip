/*! \file
    \brief Declaration of the kad_publish_t
    
*/


#ifndef __NEOIP_KAD_PUBLISH_HPP__ 
#define __NEOIP_KAD_PUBLISH_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_publish_cb.hpp"
#include "neoip_kad_publish_wikidbg.hpp"
#include "neoip_kad_store_cb.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;

/** \brief \ref kad_publish_t keeps records published but only during the lifetime of this
 *         object.
 * 
 * - aka it does a kad_store_t on creation and does a kad_delete_t on destruction
 */
class kad_publish_t : NEOIP_COPY_CTOR_DENY, private kad_store_cb_t
				, private wikidbg_obj_t<kad_publish_t, kad_publish_wikidbg_init>
				{
private:
	kad_peer_t *		kad_peer;	//!< backpointer on the kad_peer_t
	
	/*************** record identification	*******************************/
	kad_keyid_t		kad_keyid;	//!< the kad_keyid_t of the published records
	std::list<kad_recid_t>	recid_db;	//!< the kad_recid_t of all the published records

	/*************** kad_store_t	***************************************/
	kad_store_t *		kad_store;
	bool 			neoip_kad_store_cb(void *cb_userptr, kad_store_t &cb_kad_store
							, const kad_event_t &kad_event)		throw();
						
	/*************** Callback	***************************************/
	kad_publish_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_publish_t()		throw();
	~kad_publish_t()	throw();
	
	/*************** Setup Functoin	***************************************/
	kad_err_t	start(kad_peer_t *kad_peer, const kad_recdups_t &kad_recdups
						, kad_publish_cb_t *callback, void *userptr) 	throw();

	//! Start helper to ease the publication of a single kad_rec_t
	kad_err_t	start(kad_peer_t *kad_peer, const kad_rec_t &kad_rec, kad_publish_cb_t *callback
						, void *userptr)		throw()
			{ return start(kad_peer, kad_recdups_t().update(kad_rec), callback, userptr); }

	/*************** List of friend classes	*******************************/
	friend class	kad_publish_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_PUBLISH_HPP__ 



