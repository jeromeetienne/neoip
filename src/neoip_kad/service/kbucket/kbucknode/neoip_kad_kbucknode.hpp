/*! \file
    \brief Header of the \ref kad_kbucket_t
    
*/


#ifndef __NEOIP_KAD_KBUCKNODE_HPP__ 
#define __NEOIP_KAD_KBUCKNODE_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_kbucknode_wikidbg.hpp"
#include "neoip_kad_kbucket.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_kad_ping_rpc_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief store a single node in \ref kad_kbucket_t
 */
class kad_kbucket_t::bucknode_t : NEOIP_COPY_CTOR_DENY, private kad_ping_rpc_cb_t
			, private wikidbg_obj_t<kad_kbucket_t::bucknode_t, kad_kbucknode_wikidbg_init>
			{
private:
	kad_kbucket_t *	kad_kbucket;	//!< backpointer to the kad_kbucket
	kad_addr_t	kad_addr;
	
	/*************** ping stuff	***************************************/
	kad_ping_rpc_t *ping_rpc;	//!< kad_ping_rpc_t for pinging the current node
	bool 		neoip_kad_ping_rpc_cb(void *cb_userptr, kad_ping_rpc_t &cb_ping_rpc
						, const kad_event_t &kad_event)	throw();
	kad_addr_t *	pending_node;	
	void		ping_stop()	throw();
public:
	/*************** ctor/dtor	***************************************/
	bucknode_t(const kad_addr_t &kad_addr_t, kad_kbucket_t *kad_kbucket)	throw();
	~bucknode_t()								throw();

	/*************** start pinging	***************************************/
	kad_err_t	ping_start(const kad_addr_t &pending_node)	throw();
	
	/*************** Query function	***************************************/
	const kad_addr_t &	get_kad_addr()		const throw() {	return kad_addr;		}
	bool			ping_in_progress()	const throw() { return ping_rpc != NULL;	}

	/*************** Friend class	***************************************/
	friend	class	kad_kbucknode_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_KBUCKNODE_HPP__  */










