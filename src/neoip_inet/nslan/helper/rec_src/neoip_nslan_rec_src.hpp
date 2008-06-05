/*! \file
    \brief Declaration of the nslan_rec_src_t
    
*/


#ifndef __NEOIP_NSLAN_REC_SRC_HPP__ 
#define __NEOIP_NSLAN_REC_SRC_HPP__ 
/* system include */
#include <vector>
#include <list>
/* local include */
#include "neoip_nslan_rec_src_cb.hpp"
#include "neoip_nslan_rec_src_wikidbg.hpp"
#include "neoip_nslan_rec.hpp"
#include "neoip_nslan_query_cb.hpp"
#include "neoip_nslan_id.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class nslan_peer_t;

/** \brief Act as a source record from nslan_srv_t
 * 
 * - it provides a never-ending list
 */
class nslan_rec_src_t : NEOIP_COPY_CTOR_DENY, private nslan_query_cb_t, private zerotimer_cb_t
			, private wikidbg_obj_t<nslan_rec_src_t, nslan_rec_src_wikidbg_init>
			{
private:
	nslan_peer_t *	nslan_peer;		//!< backpointer on the attached nslan_peer_t
	nslan_keyid_t	keyid;			//!< the nslan_keyid_t to query
	bool		want_more;		//!< true if the caller want more
						//!< addresses. false otherwise.
						
	std::list<std::pair<nslan_rec_t, ipport_addr_t> >	nslan_rec_db;	//!< all the record to be delivered
	bool		do_delivery()	throw();

	/*************** nslan_query	***************************************/
	nslan_query_t *	nslan_query;	//!< the nslan_query_t on which to operate
	bool		neoip_nslan_query_cb(void *cb_userptr, nslan_query_t &nslan_query
						, const nslan_event_t &nslan_event)	throw();

	/*************** zerotimer to avoid notification during user call *****/
	zerotimer_t	zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw();


	/*************** data for result notification	***********************/
	nslan_rec_src_cb_t*	callback;	//!< the callback to notify ipport_addr_t
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const nslan_rec_t &nslan_rec
					, const ipport_addr_t &src_addr)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
	
public:
	/*************** ctor/dtor	***************************************/
	nslan_rec_src_t()		throw();
	~nslan_rec_src_t()		throw();
	
	/*************** Setup Function	***************************************/
	inet_err_t	start(nslan_peer_t *nslan_peer, const nslan_keyid_t &keyid
					, nslan_rec_src_cb_t *callback, void *userptr)	throw();

	/*************** Query Function	***************************************/
	void		get_more()						throw();
	void		push_back(const nslan_rec_t &nslan_rec
					, const ipport_addr_t &src_addr)	throw();

	/*************** List of friend function	***********************/
	friend	class	nslan_rec_src_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NSLAN_REC_SRC_HPP__  */



