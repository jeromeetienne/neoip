/*! \file
    \brief Header of the \ref btcli_resp_t class
    
*/


#ifndef __NEOIP_BTCLI_RESP_HPP__ 
#define __NEOIP_BTCLI_RESP_HPP__ 
/* system include */
/* local include */
#include "neoip_btcli_apps.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_err_t;
class	xmlrpc_resp0_t;

/** \brief A responder for xmlrpc of neoip-btcli
 */
class btcli_resp_t : NEOIP_COPY_CTOR_DENY {
private:
	btcli_apps_t *	m_btcli_apps;	//!< pointer on the btcli_apps_t on which the rpc operate
	xmlrpc_resp0_t*	xmlrpc_resp;	//!< the xmlrpc_resp_t which handle the rpc via xmlrpc
public:
	/*************** ctor/dtor	***************************************/
	btcli_resp_t()		throw();
	~btcli_resp_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(btcli_apps_t *m_btcli_apps)	throw();
	
	/*************** Query function	***************************************/
	btcli_apps_t *	btcli_apps()	throw()	{ return m_btcli_apps;			}
	bt_ezsession_t *bt_ezsession()	throw()	{ return btcli_apps()->bt_ezsession();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTCLI_RESP_HPP__  */



