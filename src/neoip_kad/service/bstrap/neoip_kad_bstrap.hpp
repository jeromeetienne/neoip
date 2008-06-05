/*! \file
    \brief Header of the \ref kad_bstrap_t
    
*/


#ifndef __NEOIP_KAD_BSTRAPHPP__ 
#define __NEOIP_KAD_BSTRAPHPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_bstrap_cb.hpp"
#include "neoip_kad_bstrap_profile.hpp"
#include "neoip_kad_bstrap_src_cb.hpp"
#include "neoip_kad_bstrap_cand_cb.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_date.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;

/** \brief class definition for bootstraping
 */
class kad_bstrap_t : NEOIP_COPY_CTOR_DENY, private kad_bstrap_src_cb_t, private kad_bstrap_cand_cb_t {
private:
	/*************** internal data	***************************************/
	kad_peer_t *		kad_peer;	//!< backpointer to the kad_peer_t
	kad_bstrap_profile_t	profile;	//!< the profile attached to this object
	date_t			start_date;	//!< the date_t at which the kad_bstrap_t started

	/*************** internal function	*******************************/
	bool			may_stop()					const throw();
	kad_bstrap_cand_t *	cand_by_kad_addr(const kad_addr_t &kad_addr)	const throw();

	/*************** kad_bstrap_src_t callback	***********************/
	kad_bstrap_src_t *	bstrap_src;	//!< the kad_bstrap_src_t which is tested
	bool 			neoip_kad_bstrap_src_cb(void *cb_userptr
						, kad_bstrap_src_t &cb_kad_bstrap_src
						, const kad_addr_t &kad_addr_t)	throw();
					
	/*************** kad_bstrap_cand_t database	***********************/
	std::list<kad_bstrap_cand_t *>	cand_db;
	void cand_dolink(kad_bstrap_cand_t *cand)	throw()	{ cand_db.push_back(cand);	}
	void cand_unlink(kad_bstrap_cand_t *cand)	throw()	{ cand_db.remove(cand);		}
	bool		neoip_kad_bstrap_cand_cb(void *cb_userptr, kad_bstrap_cand_t &cb_kad_bstrap
						, const kad_err_t &kad_err)	throw();

	/*************** Callback	***************************************/
	kad_bstrap_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback()	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_bstrap_t()		throw();
	~kad_bstrap_t()	throw();

	/*************** setup function	***************************************/
	kad_bstrap_t &	set_profile(const kad_bstrap_profile_t &profile)	throw();
	kad_err_t	start(kad_peer_t *kad_peer, kad_bstrap_cb_t *callback, void *userptr)	throw();

	/*************** Friend Class	***************************************/
	friend class 	kad_bstrap_cand_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAPHPP__  */










