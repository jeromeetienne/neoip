/*! \file
    \brief Definition of the \ref oload_flash_xdom_t

\par Brief Description
oload_flash_xdom_t 'publish' a crossdomain.xml at the root of neoip-oload 'domain'

- NOTE: this is needed to allow flash to download external rescource thru neoip-oload
  - flash plugin got a 'security policy' which requires to get this file
  - http://www.crossdomainxml.org/
  - http://kb.adobe.com/selfservice/viewContent.do?externalId=tn_14213&sliceId=2
  
*/

/* system include */
/* local include */
#include "neoip_oload_flash_xdom.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_http_sresp.hpp"
#include "neoip_http_sresp_ctx.hpp"
#include "neoip_http_resp_mode.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
oload_flash_xdom_t::oload_flash_xdom_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some variables
	m_http_sresp	= NULL;
}

/** \brief Destructor
 */
oload_flash_xdom_t::~oload_flash_xdom_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete m_http_sresp if needed
	nipmem_zdelete	m_http_sresp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	oload_flash_xdom_t::start(http_listener_t *http_listener)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// launch the xdomain_sresp - used to have 'security policy' of flash
	http_err_t	http_err;
	m_http_sresp	= nipmem_new http_sresp_t();
	http_err	= m_http_sresp->start(http_listener, "http://0.0.0.0/crossdomain.xml"
				, http_method_t::GET, http_resp_mode_t::REJECT_SUBPATH
				, this, NULL);
	if( http_err.failed() )	return bt_err_from_http(http_err);
	
	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			http_sresp_t main callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_resp_t when to notify an event
 */
bool	oload_flash_xdom_t::neoip_http_sresp_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
				, http_sresp_ctx_t &sresp_ctx)		throw()
{
	// log to debug
	KLOG_ERR("enter");
	// Set the mime-type for xml
	sresp_ctx.rephd().header_db().update("Content-Type", "text/xml");
	// put the xmlrpc response into the sresp_ctx.response_body
	std::ostringstream &	oss	= sresp_ctx.response_body();
	oss	<< "<?xml version=\"1.0\"?>\n";
	oss	<< "<!DOCTYPE cross-domain-policy SYSTEM \"http://www.macromedia.com/xml/dtds/cross-domain-policy.dtd\">\n";
	oss	<< "<cross-domain-policy>\n";
	oss	<< "\t<allow-access-from domain=\"*\" />\n";
	oss	<< "</cross-domain-policy>\n";

	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END;




