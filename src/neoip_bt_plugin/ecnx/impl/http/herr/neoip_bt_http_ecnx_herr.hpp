/*! \file
    \brief Header of the bt_http_ecnx_herr_t
    
*/


#ifndef __NEOIP_BT_HTTP_ECNX_HERR_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_HERR_HPP__ 
/* system include */
#include <string>
#include <set>
/* local include */
#include "neoip_bt_http_ecnx_herr_wikidbg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_http_ecnx_pool_t;

/** \brief class definition for bt_ecnx_herr_t
 */
class bt_http_ecnx_herr_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
			, private wikidbg_obj_t<bt_http_ecnx_herr_t, bt_http_ecnx_herr_wikidbg_init, timeout_cb_t>
			{
private:
	bt_http_ecnx_pool_t *	ecnx_pool;	//!< backpointer to the bt_http_ecnx_herr_t
	std::string		hostport_str;	//!< the hostport_str of this bt_http_ecnx_herr_t	
	std::set<http_uri_t>	uri_blacklist;
	bt_err_t		m_last_error;	//!< the last error received

	/*************** Internal function	*******************************/
	void			notify_common_failed(const bt_err_t &bt_err)	throw();

	/*************** retry_timeout	***************************************/
	delaygen_t		retry_delaygen;	//!< the delay_t generator for retrying 	
	timeout_t		retry_timeout;
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_http_ecnx_herr_t(bt_http_ecnx_pool_t *ecnx_pool, const std::string &hostport_str) 	throw();
	~bt_http_ecnx_herr_t()									throw();
	
	/*************** Query function	***************************************/
	const std::string &	get_hostport_str()	const throw()	{ return hostport_str;		}
	bt_http_ecnx_pool_t *	get_ecnx_pool()		const throw()	{ return ecnx_pool;		}	
	const bt_err_t &	last_error()		const throw()	{ return m_last_error;		}
	std::set<http_uri_t>	get_uri_blacklist()	const throw()	{ return uri_blacklist;		}
	
	/*************** Action function	*******************************/
	void	notify_cnx_failed(const bt_err_t &bt_err)				throw();
	void	notify_cnx_succeed()							throw();
	void	notify_http_failed(const http_uri_t &http_uri, const bt_err_t &bt_err)	throw();
	/*************** List of friend class	*******************************/
	friend class	bt_http_ecnx_herr_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_HERR_HPP__  */



