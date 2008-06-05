/*! \file
    \brief Declaration of the ntudp_npos_eval_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_EVAL_HPP__ 
#define __NEOIP_NTUDP_NPOS_EVAL_HPP__ 
/* system include */
#include <set>
#include <map>
/* local include */
#include "neoip_ntudp_npos_eval_cb.hpp"
#include "neoip_ntudp_npos_eval_profile.hpp"
#include "neoip_ntudp_npos_eval_wikidbg.hpp"
#include "neoip_ntudp_npos_res.hpp"
#include "neoip_ntudp_npos_saddrecho_cb.hpp"
#include "neoip_ntudp_npos_inetreach_cb.hpp"
#include "neoip_ntudp_npos_natlback_cb.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declarations
class	ntudp_pserver_pool_t;
class	ntudp_npos_server_t;

/** \brief Perform a full evaluation of the network position and notify the result
 */
class ntudp_npos_eval_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t, private event_hook_cb_t
				, private ntudp_npos_inetreach_cb_t
				, private ntudp_npos_saddrecho_cb_t
				, private ntudp_npos_natlback_cb_t
				, private wikidbg_obj_t<ntudp_npos_eval_t, ntudp_npos_eval_wikidbg_init>
				{
private:
	ipport_addr_t		listen_addr_pview_cfg;	//!< the public view of listen address of the
							//!< local peer FROM THE CONFIGURATION. aka
							//!< not the current one
	ntudp_pserver_pool_t *	pserver_pool;
	ntudp_npos_server_t *	npos_server;
	ntudp_npos_res_t	current_res;

	ntudp_npos_eval_profile_t	profile;	//!< the profile attached to this object

	/*************** internal function	*******************************/
	bool		create_needed_client()					throw();
	
	/*************** scheduling function	*******************************/
	// TODO this is a nested class... i dont like nested class.... what is this class anyway
	class		count_db_t;
	size_t		count_nb_client_on_unknown()				const throw();
	ipport_addr_t	get_least_used_reach(const count_db_t &excluded_db)	const throw();
	ipport_addr_t	get_least_used_unknown(const count_db_t &excluded_db)	const throw();


	/*************** callback for ntudp_pserver_pool_t	***************/
	bool neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level) throw();
								
	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;		//!< to detect timeout for the whole ntudp_npos_eval_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();


	/*************** ntudp_npos_inetreach_t	*******************************/
	std::set<ntudp_npos_inetreach_t *>	inetreach_db;	//!< the database of the currently
								//!< running ntudp_npos_inetreach_t
	bool	launch_inetreach()		throw();
	bool 	neoip_ntudp_npos_inetreach_event_cb(void *cb_userptr
						, ntudp_npos_inetreach_t &cb_ntudp_npos_inetreach
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();

	/*************** saddrecho callback to dispatch to natted or natsym ***/
	bool 	neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
						, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();

	/*************** ntudp_npos_saddrecho_t for natted	***************/
	std::set<ntudp_npos_saddrecho_t *>	natted_db;	//!< the database of the currently running
								//!< ntudp_npos_saddrecho_t for natted
	bool	launch_natted()			throw();
	bool 	neoip_ntudp_npos_natted_event_cb(void *cb_userptr
						, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();
	ipport_aview_t	natted_aview;		//!< the natted_aview_t reported by the completed 
						//!< natted client. is_null()==true before this
	ipport_addr_t	natted_pserver_addr;	//!< the public server address used by the completed
						//!< natted client. is_null()==true before this
							
	/*************** ntudp_npos_saddrecho_t for natsym	***************/
	std::set<ntudp_npos_saddrecho_t *>	natsym_db;	//!< the database of the currently running
								//!< ntudp_npos_saddrecho_t for natsym
	bool	launch_natsym()			throw();
	bool 	neoip_ntudp_npos_natsym_event_cb(void *cb_userptr
						, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();

	/*************** ntudp_npos_natlback_t	*******************************/
	std::set<ntudp_npos_natlback_t *>	natlback_db;	//!< the database of the currently running
								//!< ntudp_npos_natlback_t
	bool	launch_natlback()		throw();
	bool 	neoip_ntudp_npos_natlback_event_cb(void *cb_userptr
						, ntudp_npos_natlback_t &cb_ntudp_npos_natlback
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();

	/*************** Callback	***************************************/
	ntudp_npos_eval_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const ntudp_npos_res_t &ntudp_npos_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_eval_t()		throw();
	~ntudp_npos_eval_t()		throw();
	
	/*************** Setup function	***************************************/
	ntudp_npos_eval_t &	set_profile(const ntudp_npos_eval_profile_t &profile)	throw();	
	ntudp_err_t		start(ntudp_pserver_pool_t *pserver_pool, ntudp_npos_server_t *npos_server
					, const ipport_addr_t &listen_addr_pview_cfg
					, ntudp_npos_eval_cb_t *callback, void *userptr)	throw();

	/*************** List of friend classes	*******************************/
	friend class	ntudp_npos_eval_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_EVAL_HPP__  */



