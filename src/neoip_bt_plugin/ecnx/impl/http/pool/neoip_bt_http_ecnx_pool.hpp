/*! \file
    \brief Header of the bt_http_ecnx_pool_t
    
*/


#ifndef __NEOIP_BT_HTTP_ECNX_POOL_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_POOL_HPP__ 
/* system include */
#include <list>
#include <set>
/* local include */
#include "neoip_bt_http_ecnx_pool_wikidbg.hpp"
#include "neoip_bt_http_ecnx_pool_profile.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_http_ecnx_cnx_t;
class	bt_http_ecnx_herr_t;
class	http_client_pool_t;


/** \brief class definition for bt_ecnx
 * 
 * - it MUST be ctor after bt_swarm_t and dtor before
 */
class bt_http_ecnx_pool_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<bt_http_ecnx_pool_t, bt_http_ecnx_pool_wikidbg_init>
			{
private:
	bt_http_ecnx_pool_profile_t profile;		//!< the profile to use for the bt_http_ecnx_pool_t
	bt_swarm_t *		bt_swarm;		//!< backpointer to the attached bt_swarm_t 
	http_client_pool_t *	http_client_pool;	//!< http_client_pool_t for all bt_http_ecnx_cnx_t
	std::set<std::string>	m_hostport_db;		//!< all the hostport for this bt_swarm_T

	/*************** bt_http_ecnx_cnx_t	*******************************/
	std::list<bt_http_ecnx_cnx_t *>	ecnx_cnx_db;	//!< bt_http_ecnx_cnx_t database
	void	ecnx_cnx_dolink(bt_http_ecnx_cnx_t *cnx)	throw()	{ ecnx_cnx_db.push_back(cnx);	}
	void	ecnx_cnx_unlink(bt_http_ecnx_cnx_t *cnx)	throw()	{ ecnx_cnx_db.remove(cnx);	}

	/*************** bt_http_ecnx_herr_t	*******************************/
	std::map<std::string, bt_http_ecnx_herr_t *>	ecnx_herr_db;	//!< bt_http_ecnx_herr_t index by hostport_str
	void	ecnx_herr_dolink(bt_http_ecnx_herr_t *herr)	throw();
	void 	ecnx_herr_unlink(bt_http_ecnx_herr_t *herr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_http_ecnx_pool_t() 		throw();
	~bt_http_ecnx_pool_t()		throw();
	
	/*************** setup function	***************************************/
	bt_http_ecnx_pool_t&	set_profile(const bt_http_ecnx_pool_profile_t &profile)	throw();	
	bt_err_t		start(bt_swarm_t *bt_swarm)				throw();
	
	/*************** query function	***************************************/
	bt_swarm_t *			get_swarm()		throw()		{ return bt_swarm;	}
	const bt_http_ecnx_pool_profile_t &get_profile()	const throw()	{ return profile;	}
	http_client_pool_t *		get_http_client_pool()	const throw()	{ return http_client_pool;}
	bt_err_t			launch_ecnx_cnx_if_needed(const std::string &hostport_str)	throw();
	bt_http_ecnx_herr_t *		ecnx_herr_by_hostport_str(const std::string &hostport_str)	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_http_ecnx_pool_wikidbg_t;
	friend class	bt_http_ecnx_cnx_t;
	friend class	bt_http_ecnx_herr_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_POOL_HPP__  */



