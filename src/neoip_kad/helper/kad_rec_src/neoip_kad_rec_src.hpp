/*! \file
    \brief Declaration of the kad_rec_src_t
    
*/


#ifndef __NEOIP_KAD_REC_SRC_HPP__ 
#define __NEOIP_KAD_REC_SRC_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_rec_src_cb.hpp"
#include "neoip_kad_query_cb.hpp"
#include "neoip_kad_rec.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class kad_peer_t;

/** \brief Act as a source kad_rec_t from kad_listener_t
 * 
 * - this provide a source with a end.
 */
class kad_rec_src_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t, private kad_query_cb_t {
private:
	std::list<kad_rec_t> record_db;		//!< all the record to be delivered
	bool		notify_inprogress;	//!< True if the object is currently
						//!< IN a notifying callback. false otherwise
	bool		want_more;		//!< true if the caller want more
						//!< addresses. false otherwise.

	kad_peer_t *	kad_peer;	//!< the kad_peer_t to query
	kad_keyid_t	keyid;		//!< the keyid to query

	/*************** kad_query_t	***************************************/
	kad_query_t *	kad_query;	//!< the kad_query_t to get kad_rec_t from the kad_peer_t
	bool		neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw();


	/*************** zerotimer to avoid notification during user call *****/
	zerotimer_t	zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw();

	/*************** data for result notification	***********************/
	kad_rec_src_cb_t*	callback;	//!< the callback to notify ipport_addr_t
	void *			userptr;	//!< the userptr associated with the callback
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
	
	/*************** Internal function	*******************************/
	bool			do_delivery()	throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_rec_src_t()		throw();
	~kad_rec_src_t()	throw();
	
	/*************** Setup Function	***************************************/
	kad_err_t	start(kad_peer_t *kad_peer, const kad_keyid_t &keyid
					, kad_rec_src_cb_t *callback, void *userptr)	throw();

	/*************** Query Function	***************************************/
	void		get_more()				throw();
	void		push_back(const kad_rec_t &record)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_REC_SRC_HPP__  */



