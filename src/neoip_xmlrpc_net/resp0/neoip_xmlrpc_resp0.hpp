/*! \file
    \brief Header of the \ref xmlrpc_resp0_t class
    
*/


#ifndef __NEOIP_XMLRPC_RESP0_HPP__ 
#define __NEOIP_XMLRPC_RESP0_HPP__ 
/* system include */
/* local include */
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	xmlrpc_err_t;
class	xmlrpc_resp0_sfct_db_t;

/** \brief A resp0onder for xmlrpc
 */
class xmlrpc_resp0_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t {
private:
	std::string 				url_path;
	item_arr_t<xmlrpc_resp0_sfct_db_t *>	m_sfct_db_arr;
	/*************** old httpd callback	*******************************/
	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();	
public:
	/*************** ctor/dtor	***************************************/
	xmlrpc_resp0_t()		throw();
	~xmlrpc_resp0_t()	throw();
	
	/*************** Setup function	***************************************/
	xmlrpc_err_t	start(const std::string &url_path)	throw();
	
	/*************** Query function	***************************************/
	item_arr_t<xmlrpc_resp0_sfct_db_t *> &	sfct_db_arr()	throw()		{ return m_sfct_db_arr;	}
	const item_arr_t<xmlrpc_resp0_sfct_db_t*>& sfct_db_arr()	const throw()	{ return m_sfct_db_arr;	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_RESP0_HPP__  */



