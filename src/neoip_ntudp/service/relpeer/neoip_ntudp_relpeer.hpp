/*! \file
    \brief Declaration of the ntudp_relpeer_t
    
*/


#ifndef __NEOIP_NTUDP_RELPEER_HPP__ 
#define __NEOIP_NTUDP_RELPEER_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_ntudp_relpeer_cb.hpp"
#include "neoip_ntudp_relpeer_tunnel_cb.hpp"
#include "neoip_ntudp_relpeer_profile.hpp"
#include "neoip_ntudp_relpeer_wikidbg.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_rdvpt_arr.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

//lis tof forward declaration
class ntudp_relpeer_tunnel_t;
class ntudp_dircnx_server_t;
class ntudp_peer_t;
class ipport_addr_t;

/** \brief class definition for ntudp_relpeer_t
 */
class ntudp_relpeer_t : NEOIP_COPY_CTOR_DENY, private event_hook_cb_t, private ntudp_relpeer_tunnel_cb_t
				, private wikidbg_obj_t<ntudp_relpeer_t, ntudp_relpeer_wikidbg_init>
				{
private:
	/*************** internal data	***************************************/
	ntudp_peer_t *		ntudp_peer;	//!< backpoint on the ntudp_peer
	size_t			nb_needed_tunnel;//!< current number of needed simultaneous tunnel
	ntudp_dircnx_server_t *	dircnx_server;	//!< the server to accept direct cnx connection
	ntudp_relpeer_profile_t	profile;	//!< the profile attached to this object

	/*************** Internal function	*******************************/
	bool		tunnel_use_pserver_addr(const ipport_addr_t &pserver_addr)	const throw();
	void		create_needed_tunnel()	throw();
	bool		create_one_tunnel()	throw();

	/*************** List of tunnel	***************************************/
	std::set<ntudp_relpeer_tunnel_t *>	tunnel_db;
	bool 		neoip_ntudp_relpeer_tunnel_cb(void *cb_userptr, ntudp_relpeer_tunnel_t &cb_tunnel
							, const bool established)	throw();

	/*************** callback for ntudp_pserver_pool_t	***************/
	bool neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level) throw();

	/*************** Callback	***************************************/
	ntudp_relpeer_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback()	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

public:
	/*************** ctor/dtor	***************************************/
	ntudp_relpeer_t()			throw();
	~ntudp_relpeer_t()			throw();

	/*************** Setup function	***************************************/
	ntudp_relpeer_t &	set_profile(const ntudp_relpeer_profile_t &profile)	throw();
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer, ntudp_relpeer_cb_t *callback, void *userptr)throw();

	/*************** Query function	***************************************/
	ntudp_rdvpt_arr_t	get_established_tunnel_rdvpt()	const throw();
					
	/*************** Friend functions	*******************************/
	friend class ntudp_relpeer_tunnel_t;
	friend class ntudp_relpeer_wikidbg_t;
	friend class ntudp_relpeer_tunnel_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RELPEER_HPP__  */



