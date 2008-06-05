/*! \file
    \brief Declaration of the ntudp_aview_pool_t
    
*/


#ifndef __NEOIP_NTUDP_AVIEW_POOL_HPP__ 
#define __NEOIP_NTUDP_AVIEW_POOL_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <set>
/* local include */
#include "neoip_ntudp_aview_pool_wikidbg.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_npos_saddrecho_cb.hpp"
#include "neoip_event_hook.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_npos_saddrecho_t;

/** \brief class definition for ntudp_aview_pool_t
 */
class ntudp_aview_pool_t  : NEOIP_COPY_CTOR_DENY, private ntudp_npos_saddrecho_cb_t
			, private event_hook_cb_t
			, private wikidbg_obj_t<ntudp_aview_pool_t, ntudp_aview_pool_wikidbg_init>
			{
public:	/////////////////////// constant declaration ///////////////////////////
	//! the number of initial ipport_aview_t needed
	static const size_t	NB_NEEDED_AVIEW_INIT;
private:
	/*************** internal data	***************************************/
	ntudp_peer_t *	ntudp_peer;	//!< backpointer on the attached ntudp_peer_t
	size_t		nb_needed_aview;//!< the current needed number of simultaneous pview
	event_hook_t	new_aview_hook;	//!< the event_hook_t for all the callers to register on.

	/*************** callback for npos_saddrecho	***********************/
	std::set<ntudp_npos_saddrecho_t *>	saddrecho_db;	//!< the database of all the npos_saddrecho
	bool 		neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
						, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();
	/*************** callback for ntudp_pserver_pool_t	***************/
	bool neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level) throw();

	/*************** Internal function	*******************************/
	void		create_needed_saddrecho()	throw();
	bool		create_one_saddrecho()		throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_aview_pool_t()			throw();
	~ntudp_aview_pool_t()			throw();

	/*************** Setup function	***************************************/
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer, size_t nb_needed_aview)	throw();
	
	/*************** Query function	***************************************/
	size_t			size()	const throw();
	bool			empty() const throw()	{ return this->size() == 0;	}
	ntudp_npos_saddrecho_t *steal_one_saddrecho()	throw();

	/*************** for caller to BE notified when new aview occurs ******/
	void	new_aview_subscribe(event_hook_cb_t *callback, void *userptr)		throw()
					{ new_aview_hook.append(0, callback, userptr); }
	void	new_aview_unsubscribe(event_hook_cb_t *callback, void *userptr)	throw()
					{ new_aview_hook.remove(0, callback, userptr); }
					
	/*************** list of friend	***************************************/
	friend	class ntudp_aview_pool_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_AVIEW_POOL_HPP__  */



