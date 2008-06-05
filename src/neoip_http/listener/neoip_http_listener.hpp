/*! \file
    \brief Header of the \ref http_listener_t
    
*/


#ifndef __NEOIP_HTTP_LISTENER_HPP__ 
#define __NEOIP_HTTP_LISTENER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_http_listener_wikidbg.hpp"
#include "neoip_http_listener_profile.hpp"
#include "neoip_http_err.hpp"
#include "neoip_socket_resp_cb.hpp"
#include "neoip_socket_resp_arg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_listener_cnx_t;
class	http_resp_t;
class	http_resp_notfound_t;
class	http_reqhd_t;

/** \brief Accept the http connections and then spawn http_listener_cnx_t to handle them
 */
class http_listener_t : NEOIP_COPY_CTOR_DENY, private socket_resp_cb_t
			, private wikidbg_obj_t<http_listener_t, http_listener_wikidbg_init> {
private:
	http_listener_profile_t	profile;	//!< the profile used by this http_listener_t

	/*************** store the http_listener_cnx_t	***********************/
	std::list<http_listener_cnx_t *>	cnx_db;
	void cnx_dolink(http_listener_cnx_t *cnx) 	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(http_listener_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}

	/*************** store the http_resp_t	*******************************/
	std::list<http_resp_t *>		resp_db;
	void resp_dolink(http_resp_t *resp) 		throw();
	void resp_unlink(http_resp_t *resp)		throw()	{ resp_db.remove(resp);		}

	/*************** socket_resp_t	***************************************/
	socket_resp_t *	socket_resp;
	bool		neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
							, const socket_event_t &socket_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	http_listener_t()		throw();
	~http_listener_t()		throw();
	
	/*************** Setup function	***************************************/
	http_listener_t &	set_profile(const http_listener_profile_t &profile)	throw();	
	http_err_t		start(const socket_resp_arg_t &resp_arg)		throw();
	
	/*************** Query function	***************************************/
	const http_listener_profile_t &	get_profile()	const throw()	{ return profile;	}
	http_resp_t *		find_resp(const http_reqhd_t &http_reqhd)	throw();
	const socket_addr_t  &	listen_addr()					const throw();


	/*************** list of friend class	*******************************/
	friend class	http_listener_wikidbg_t;
	friend class	http_listener_cnx_t;
	friend class	http_resp_t;
	friend class	http_resp_notfound_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_LISTENER_HPP__  */










