/*! \file
    \brief Header of the \ref bt_oload0_swarm_t
    
*/


#ifndef __NEOIP_BT_OLOAD0_SWARM_HPP__ 
#define __NEOIP_BT_OLOAD0_SWARM_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_oload0_swarm_profile.hpp"
#include "neoip_bt_oload0_swarm_wikidbg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_httpo_full_cb.hpp"
#include "neoip_bt_swarm_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_err.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_oload0_t;
class	bt_io_pfile_t;
class	bt_peersrc_kad_t;
class	bt_peersrc_http_t;
class	bt_http_ecnx_pool_t;
class	bt_swarm_pdeletor_t;
class	bt_mfile_t;

/** \brief Handle the communication between a bt_swarm_t and all the bt_httpo_full_t attached
 */
class bt_oload0_swarm_t : NEOIP_COPY_CTOR_DENY, private bt_httpo_full_cb_t, private bt_swarm_cb_t
				, private wikidbg_obj_t<bt_oload0_swarm_t, bt_oload0_swarm_wikidbg_init>
				{
private:
	bt_oload0_t *		bt_oload0;	//!< backpointer to the attached bt_oload0_t
	bt_oload0_swarm_profile_t profile;	//!< the profile to use for the bt_oload0_swarm_t
	http_uri_t		nested_uri;

	/*************** Internal function	*******************************/
	bt_err_t		build_bt_swarm(const bt_mfile_t &bt_mfile)	throw();
	bool			autodelete()					throw();
	
	/*************** bt_httpo_full_t	*******************************/
	std::list<bt_httpo_full_t *>	httpo_full_db;	
	bool			neoip_bt_httpo_full_cb(void *cb_userptr, bt_httpo_full_t &cb_bt_httpo_full
						, const bt_httpo_event_t &httpo_event)	throw();

	/*************** bt_swarm_t	***************************************/
	bt_http_ecnx_pool_t *	http_ecnx_pool;
	bt_peersrc_kad_t *	peersrc_kad;
	bt_peersrc_http_t *	peersrc_http;	
	bt_io_pfile_t *		bt_io_pfile;
	bt_swarm_t *		bt_swarm;
	bt_swarm_pdeletor_t *	swarm_pdeletor;
	bool 			neoip_bt_swarm_cb(void *cb_userptr, bt_swarm_t &cb_bt_swarm
						, const bt_swarm_event_t &swarm_event)	throw();	
public:
	/*************** ctor/dtor	***************************************/
	bt_oload0_swarm_t()		throw();
	~bt_oload0_swarm_t()		throw();

	/*************** Setup function	***************************************/
	bt_oload0_swarm_t &	set_profile(const bt_oload0_swarm_profile_t &profile)	throw();	
	bt_err_t		start(bt_oload0_t *bt_oload0, const http_uri_t &nexted_uri
					, const bt_mfile_t &bt_mfile
					, const std::list<bt_httpo_full_t *> httpo_full_db)	throw();
	
	/*************** Query function	***************************************/
	const http_uri_t &	get_nested_uri()	const throw()	{ return nested_uri;	}
	bt_http_ecnx_pool_t *	get_http_ecnx_pool()	const throw()	{ return http_ecnx_pool;}

	/*************** Action function	*******************************/
	void		add_httpo_full(bt_httpo_full_t *httpo_full)	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_oload0_swarm_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_OLOAD0_SWARM_HPP__ */










