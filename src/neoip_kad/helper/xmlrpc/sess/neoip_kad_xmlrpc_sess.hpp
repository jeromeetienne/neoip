/*! \file
    \brief Header of the \ref kad_xmlrpc_t class
    
*/


#ifndef __NEOIP_KAD_XMLRPC_SESS_HPP__ 
#define __NEOIP_KAD_XMLRPC_SESS_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_xmlrpc.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Define the context for the session opened via the xmlrpc
 */
class kad_xmlrpc_t::xmlrpc_sess_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t {
private:
	kad_xmlrpc_t *	kad_xmlrpc;	//!< back pointer on the kad_xmlrpc_t
	slot_id_t	peer_slotid;	//!< the peer_slotid of this session
	slot_id_t	sess_slotid;	//!< the session slot_id of this session
	
	timeout_t	idle_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();	 

	/*************** utility function	*******************************/
	kad_listener_t &	get_kad_listener()	const throw()
			{ return kad_xmlrpc->get_kad_listener();	}
public:
	/*************** ctor/dtor	***************************************/
	xmlrpc_sess_t(kad_xmlrpc_t *kad_xmlrpc)	throw();
	~xmlrpc_sess_t()					throw();

	kad_realmid_t	realmid;
	kad_peerid_t	peerid;
	kad__profile_t	profile;	
	/*************** setup function	***************************************/
	kad_err_t	start(const kad_realmid_t &realmid, const kad_peerid_t &peerid
						, const kad_profile_t &profile)	throw();
	/*************** utility function	*******************************/
	void		notify_new_command()	throw();
	slot_id_t	get_sess_slotid()	const throw()	{ return sess_slotid;	}
	slot_id_t	get_peer_slotid()	const throw()	{ return peer_slotid;	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_XMLRPC_SESS_HPP__  */



