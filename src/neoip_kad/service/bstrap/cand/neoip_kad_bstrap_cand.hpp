/*! \file
    \brief Header of the \ref kad_bstrap_t
    
*/


#ifndef __NEOIP_KAD_BSTRAP_CAND_HPP__ 
#define __NEOIP_KAD_BSTRAP_CAND_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_bstrap_cand_cb.hpp"
#include "neoip_kad_ping_rpc_cb.hpp"
#include "neoip_kad_closestnode_cb.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_tokeep_check.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_bstrap_t;

/** \brief define a candidate in kad_bstrap_t
 */
class kad_bstrap_cand_t : private kad_closestnode_cb_t, private kad_ping_rpc_cb_t {
private:
	kad_bstrap_t *		kad_bstrap;	//!< backpointer on the kad_bstrap_t
	kad_addr_t		remote_addr;	//!< the remote kad_addr_t of the candidate

	/*************** kad_ping_rpc_t to test reachability	***************/
	kad_ping_rpc_t *	ping_rpc;
	bool 			neoip_kad_ping_rpc_cb(void *cb_userptr, kad_ping_rpc_t &cb_ping_rpc
						, const kad_event_t &kad_event)	throw();

	/*************** kad_closestnode_t on the local peerid	***************/
	kad_closestnode_t *	kad_closestnode;
	bool 			neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
						, const kad_event_t &kad_event)		throw();
	/*************** Callback	***************************************/
	kad_bstrap_cand_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const kad_err_t &kad_err)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_bstrap_cand_t(kad_bstrap_t *kad_bstrap)	throw();
	~kad_bstrap_cand_t()				throw();

	/*************** start function	***************************************/
	kad_err_t	start(const kad_addr_t &remote_addr, kad_bstrap_cand_cb_t *callback
						, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	const kad_addr_t &	get_remote_addr() const throw()	{ return remote_addr;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAP_CAND_HPP__  */










