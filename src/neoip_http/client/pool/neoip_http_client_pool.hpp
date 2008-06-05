/*! \file
    \brief Header of the \ref http_client_pool_t
    
*/


#ifndef __NEOIP_HTTP_CLIENT_POOL_HPP__ 
#define __NEOIP_HTTP_CLIENT_POOL_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_http_client_pool_wikidbg.hpp"
#include "neoip_http_client_pool_stat.hpp"
#include "neoip_http_client_pool_profile.hpp"
#include "neoip_http_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_client_pool_cnx_t;
class	http_uri_t;

/** \brief Handle a pool of already established connection for http_client_t
 */
class http_client_pool_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<http_client_pool_t, http_client_pool_wikidbg_init>
			{
private:
	http_client_pool_profile_t	profile;	//!< the profile used by this http_client_pool_t
	http_client_pool_stat_t		pool_stat;	//!< to maintain statistic about the pool

	/*************** store the http_client_pool_cnx_t	***********************/
	std::list<http_client_pool_cnx_t *>	cnx_db;
	void cnx_dolink(http_client_pool_cnx_t *cnx) 	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(http_client_pool_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}
	void cnx_remove_older()				throw();
public:
	/*************** ctor/dtor	***************************************/
	http_client_pool_t()		throw();
	~http_client_pool_t()		throw();
	
	/*************** Setup function	***************************************/
	http_client_pool_t &	set_profile(const http_client_pool_profile_t &profile)	throw();	
	http_err_t		start()							throw();
	
	/*************** Query function	***************************************/
	http_client_pool_cnx_t *	get_cnx_by_uri(const http_uri_t &http_uri) const throw();
	size_t				size()			const throw()	{ return cnx_db.size();	}
	const http_client_pool_profile_t &get_profile()		const throw()	{ return profile;	}

	/*************** list of friend class	*******************************/
	friend class	http_client_pool_wikidbg_t;
	friend class	http_client_pool_cnx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_CLIENT_POOL_HPP__  */










