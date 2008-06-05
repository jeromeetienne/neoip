/*! \file
    \brief Header of the \ref kad_db_t

*/


#ifndef __NEOIP_KAD_DB_REPLICATE_HPP__ 
#define __NEOIP_KAD_DB_REPLICATE_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_db_replicate_wikidbg.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_kad_store_rpc_cb.hpp"
#include "neoip_kad_findnode_rpc_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;
class	kad_db_t;
class	kad_rec_t;
class	kad_recdups_t;
class	cookie_id_t;
class	kad_addr_t;

/** \brief handle the replication of a kad_rec_t from the kad_db_t
 * 
 * - replication is storing a kad_rec_t to a single remote kad_peer_t
 */
class kad_db_replicate_t : NEOIP_COPY_CTOR_DENY, private kad_findnode_rpc_cb_t
			, private kad_store_rpc_cb_t
			, private wikidbg_obj_t<kad_db_replicate_t, kad_db_replicate_wikidbg_init>
			{
private:
	kad_db_t *	kad_db;		//!< backpointer to the kad_db_t

	/*************** kad_store_rpc_t	*******************************/
	std::list<kad_store_rpc_t *>	store_rpc_db;
	bool 				neoip_kad_store_rpc_cb(void *cb_userptr, kad_store_rpc_t &cb_store_rpc
							, const kad_event_t &kad_event)	throw();
	kad_err_t			start_store_rpc(const kad_recdups_t &kad_recdups
							, const cookie_id_t &cookie_id
							, const kad_addr_t &remote_addr)	throw();

	/*************** kad_findnode_rpc_t	*******************************/	
	std::list<kad_findnode_rpc_t *>	findnode_rpc_db;
	bool 				neoip_kad_findnode_rpc_cb(void *cb_userptr, kad_findnode_rpc_t &cb_findnode_rpc
							, const kad_event_t &kad_event)	throw();
	std::list<kad_recdups_t *>	findnode_recdups_db;	//!< to store the kad_recdups to replicate
								//!< in case of store_cookie_ok() during
								//!< the dummy FINDNODE to get a cookie
	kad_err_t			start_findnode_rpc(const kad_recdups_t &kad_recdups
							, const kad_addr_t &remote_addr)	throw();

public:
	/*************** ctor/dtor	***************************************/
	kad_db_replicate_t(kad_db_t *kad_db)	throw();
	~kad_db_replicate_t()			throw();

	kad_err_t	replicate_in(const kad_rec_t &kad_rec, const kad_addr_t &dest_node)	throw();
	kad_err_t	replicate_in(const kad_recdups_t &kad_recdups, const kad_addr_t &dest_node)
												throw();
	/*************** List of friend function	***********************/
	friend	class	kad_db_replicate_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_DB_REPLICATE_HPP__  */










