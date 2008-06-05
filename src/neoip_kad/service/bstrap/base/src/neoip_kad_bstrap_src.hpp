/*! \file
    \brief Declaration of the kad_bstrap_src_t
    
*/


#ifndef __NEOIP_KAD_BSTRAP_SRC_HPP__ 
#define __NEOIP_KAD_BSTRAP_SRC_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_bstrap_src_cb.hpp"
#include "neoip_kad_bstrap_src_profile.hpp"
#include "neoip_ipport_strlist_src_cb.hpp"
#include "neoip_nslan_rec_src_cb.hpp"
#include "neoip_nslan_id.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_expireset.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	nslan_peer_t;
class	file_path_t;

/** \brief Act as a source for kad_bstrap_t
 * 
 * - it provides a never-ending source
 * - NOTE: it is a addition of multiple sources
 *   - one ipport_strlist_src_t from a file containing static ipport_addr_t
 *     - It allows to bootstrap brand new kad_peer_t to the realm_id
 *     - it is used only after a given delay after start()
 *   - one ipport_strlist_src_t from a file containing dynamic ipport_addr_t saved
 *     from the last run of this kad_peer_t
 *     - It allows to reduce the load on the static server from the fixed list
 *     - it is used immediatly after start()
 *   - one from a nslan_rec_src_t to get bootstrapping from the lan as well
 *     - it starts immediatly after start()
 *     - it allow to bootstrap nodes even on a LAN even without any server
 *       (aka the dentist office situation)
 * - it provide a negative cache aka if a ipport_addr_t is pushed in the negative
 *   cache, it won't be notified for a given delay
 */
class kad_bstrap_src_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t, private timeout_cb_t
					, private nslan_rec_src_cb_t
					, private ipport_strlist_src_cb_t {
private:
	/*************** Internal Data		*******************************/
	bool				want_more;	//!< true if the caller want more. false otherwise.
	std::list<kad_addr_t>		tobedelivered_db;//!< the address to be delivered
	size_t				delivery_rrobin;//!< current round robin index for delivery [0,2]
	expireset_t<kad_addr_t> *	negcache;	//!< the negative cache
	kad_bstrap_src_profile_t	profile;	//!< the profile attached to this object

	/*************** nslan_src	***************************************/
	nslan_rec_src_t *	nslan_src;	//!< the source from nslan
	kad_addr_t		nslan_last_item;//!< the last item provided by nslan_src
	bool			nslan_got_item;	//!< true if the source already provided item
	bool			nslan_is_empty;	//!< true if the source has been determined as empty	
	bool 			neoip_nslan_rec_src_cb(void *cb_userptr, nslan_rec_src_t &cb_nslan_rec_src
						, const nslan_rec_t &nslan_rec
						, const ipport_addr_t &src_addr)	throw();
	timeout_t		nslan_noquery_timeout;	//!< if it is is running, no query on this source
	bool			nslan_noquery_timeout_cb(void *userptr, timeout_t &cb_timeout) throw();

	/*************** global ipport_strlist_src_t callback	***************/
	bool 			neoip_ipport_strlist_src_cb(void *cb_userptr
						, ipport_strlist_src_t &cb_ipport_strlist_src
						, const ipport_addr_t &ipport_addr)	throw();
	/*************** global timeout_t callback	***********************/
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout) throw();
	
	/*************** dfile_src	***************************************/
	ipport_strlist_src_t *	dfile_src;	//!< the ipport_strlist_src_t from the dynamic file.
	kad_addr_t		dfile_last_item;
	bool			dfile_got_item;	//!< true if the source already provided item
	bool			dfile_is_empty;	//!< true if the source has been determined as empty
	bool			dfile_src_cb(const kad_addr_t &kad_addr)	throw();
	timeout_t		dfile_noquery_timeout;	//!< if it is is running, no query on this source
	bool			dfile_noquery_timeout_cb(void *userptr, timeout_t &cb_timeout) throw();

	/*************** sfile_src	***************************************/
	ipport_strlist_src_t *	sfile_src;	//!< the ipport_strlist_src_t from the static file
						//!< If == NULL, the dfile_src is not yet started
	kad_addr_t		sfile_last_item;//!< the last item provided by sfile_src
	bool			sfile_got_item;	//!< true if the source already provided item
	bool			sfile_is_empty;	//!< true if the source has been determined as empty
	bool			sfile_src_cb(const kad_addr_t &kad_addr)	throw();
	timeout_t		sfile_noquery_timeout;	//!< if it is is running, no query on this source
	bool			sfile_noquery_timeout_cb(void *userptr, timeout_t &cb_timeout) throw();
	
	/*************** sfile_src creation	*******************************/
	std::string		sfile_name;	//!< the filename of the sfile filename
	kad_err_t		sfile_create()	throw();
	timeout_t		sfile_create_timeout; //!< timeout for the creation of sfile_src
	bool			sfile_create_timeout_cb(void *userptr, timeout_t &cb_timeout) throw();

	/*************** delivery_zerotimer	********************************/
	zerotimer_t		delivery_zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw();

	/*************** internal function	*******************************/
	bool			do_delivery()					throw();
	void			want_full_item()				throw();
	bool			item_are_available()				throw();
	/*************** data for result notification	***********************/
	kad_bstrap_src_cb_t *	callback;	//!< the callback to notify ipport_addr_t
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const kad_addr_t &kad_addr)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_bstrap_src_t()		throw();
	~kad_bstrap_src_t()		throw();
	
	/*************** Setup Function	***************************************/
	kad_bstrap_src_t &set_profile(const kad_bstrap_src_profile_t &profile)		throw();
	kad_err_t	start(const file_path_t &dynfile_path, const file_path_t &fixfile_path
					, nslan_peer_t *nslan_peer, const nslan_keyid_t &nslan_keyid
					, kad_bstrap_src_cb_t *callback, void *userptr)	throw();
	
	/*************** Query Function	***************************************/
	void		get_more()							throw();
	void		push_back(const kad_addr_t &kad_addr)				throw();
	void		push_negcache(const kad_addr_t &kad_addr, const delay_t &delay)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAP_SRC_HPP__  */



