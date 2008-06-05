/*! \file
    \brief Header of the \ref kad_nsearch_t
    
*/


#ifndef __NEOIP_KAD_NSEARCH_LNODE_HPP__ 
#define __NEOIP_KAD_NSEARCH_LNODE_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_kad_nsearch_lnode_wikidbg.hpp"
#include "neoip_kad_nsearch_lnode_state.hpp"
#include "neoip_kad_nsearch_llist.hpp"
#include "neoip_kad_findnode_rpc_cb.hpp"
#include "neoip_kad_findsomeval_rpc_cb.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_cookie.hpp"
#include "neoip_date.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration

/** \brief store a 'learnednode' for \ref kad_nsearch_t
 */
class kad_nsearch_lnode_t : NEOIP_COPY_CTOR_DENY, private kad_findnode_rpc_cb_t
			, private kad_findsomeval_rpc_cb_t
			, private wikidbg_obj_t<kad_nsearch_lnode_t, kad_nsearch_lnode_wikidbg_init> {
public:
	typedef	kad_nsearch_lnode_state_t	state_t;
private:
	kad_nsearch_t *		kad_nsearch;	//!< backpointer to the kad_nsearch
	kad_nsearch_llist_t *	llist;		//!< backpointer to the llist_t
	
	kad_addr_t	m_kad_addr;	//!< the kad_addr_t of this node
	cookie_id_t	m_cookie_id;	//!< the cookie_id replied by this kad_addr
					//!< (may be used later with rpc which needs it e.g STORE/DELETE)
	state_t		m_current_state;//!< the current state of this lnode_t
	
	
	date_t		rpc_birthdate;	//!< date of when the rpc started (is null otherwise)
	date_t 		rpc_age()	const throw()	{ DBG_ASSERT( !rpc_birthdate.is_null() );
							  return date_t::present() - rpc_birthdate;	}

	/*************** kad_findnode_rpc_t	*******************************/
	kad_findnode_rpc_t *	findnode_rpc;
	bool 			neoip_kad_findnode_rpc_cb(void *cb_userptr, kad_findnode_rpc_t &cb_findnode_rpc
						, const kad_event_t &kad_event)	throw();
	/*************** kad_findsomeval_rpc_t	*******************************/
	kad_findsomeval_rpc_t *	findsomeval_rpc;
	bool 			neoip_kad_findsomeval_rpc_cb(void *cb_userptr, kad_findsomeval_rpc_t &cb_findsomeval_rpc
						, const kad_event_t &kad_event)	throw();

	bool		rpc_event_cb(const kad_event_t &kad_event)	throw();
						
	/*************** reply handling	***************************************/
	bool		handle_reply(const kad_event_t &kad_event)		throw();
	bool		handle_findnode_reply(const kad_event_t &kad_event)	throw();
	bool		handle_findsomeval_reply(const kad_event_t &kad_event)	throw();

	void		process_replied_addr_arr(const kad_addr_arr_t &replied_addr_arr)	throw();	
public:
	/*************** ctor/dtor	***************************************/
	kad_nsearch_lnode_t(kad_nsearch_llist_t *llist, const kad_addr_t &kad_addr)	throw();
	~kad_nsearch_lnode_t()								throw();
	
	/*************** Start function	***************************************/
	kad_err_t		start()		throw();
	
	/*************** Query function	***************************************/
	bool			is_online()	const throw();
	bool			is_offline()	const throw();
	const kad_addr_t &	kad_addr()	const throw()	{ return m_kad_addr;		}
	const cookie_id_t &	cookie_id()	const throw()	{ return m_cookie_id;		}	
	const state_t &		current_state()	const throw()	{ return m_current_state;	}

	/*************** Compatibility layer	*******************************/
	const kad_addr_t &	get_kad_addr()	const throw()	{ return kad_addr();		}
	const cookie_id_t &	get_cookie_id()	const throw()	{ return cookie_id();		}	
	const state_t &		get_state()	const throw()	{ return current_state();	}

	/*************** List of friend classes	*******************************/
	friend class	kad_nsearch_lnode_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_NSEARCH_LNODE_HPP__  */










