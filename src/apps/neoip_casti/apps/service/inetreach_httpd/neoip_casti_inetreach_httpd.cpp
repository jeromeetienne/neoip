/*! \file
    \brief Definition of the \ref casti_inetreach_httpd_t

\par Brief Description
this object is have a http_listener_t which attempts to be inetreach

- TODO if ndiag_aview change... all casti_swarm_t MUST republish
  - currently they dont
  - do casti_swarm->publish_start
    and casti_swarm->publish_restart
  - if ndiag_aview change, go thru all casti_swarm
    - if published, do restart

*/

/* system include */
/* local include */
#include "neoip_casti_inetreach_httpd.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_ndiag_aview.hpp"
#include "neoip_ndiag_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
casti_inetreach_httpd_t::casti_inetreach_httpd_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_http_listener	= NULL;
	m_ndiag_aview	= NULL;
}

/** \brief Destructor
 */
casti_inetreach_httpd_t::~casti_inetreach_httpd_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the ndiag_aview
	nipmem_zdelete	m_ndiag_aview;
	// delete http_listener_t if needed
	nipmem_zdelete	m_http_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	casti_inetreach_httpd_t::start(casti_inetreach_httpd_cb_t *callback
							, void *userptr)	throw()
{
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;

	/*************** init the http_listener_t	***********************/
	// build the resp_arg for the http_listener_t 
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
					.listen_addr("tcp://0.0.0.0:0");
	// start the http_listener_t
	http_err_t	http_err;
	m_http_listener	= nipmem_new http_listener_t();
	http_err	= m_http_listener->start(resp_arg);
	if( http_err.failed() )		return bt_err_from_http(http_err);
	
	/*************** init the ndiag_aview_t	*******************************/
	// get the listen_addrfrom the m_http_listener
	// - NOTE: going thru string to convert socket_addr_t into ipport_addr_t is ugly
	ipport_addr_t	listen_addr	= m_http_listener->listen_addr().get_peerid_vapi()->to_string()
					+ std::string(":")
					+ m_http_listener->listen_addr().get_portid_vapi()->to_string();
	KLOG_ERR("http listen addr=" << listen_addr );
	// create a ndiag_aview_t for the http listener
	ipport_aview_t	listen_aview	= ipport_aview_t().lview(listen_addr);
	ndiag_err_t	ndiag_err;
	m_ndiag_aview	= nipmem_new ndiag_aview_t();	
	ndiag_err	= m_ndiag_aview->start(listen_aview, upnp_sockfam_t::TCP
						, "neoip casti mdata srv", this, NULL);
	if( ndiag_err.failed() )	return bt_err_from_ndiag(ndiag_err);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the local view of the listen ipport_addr_t 
 */
const ipport_addr_t &	casti_inetreach_httpd_t::listen_ipport_lview()	const throw()
{
	return m_ndiag_aview->ipport_aview_init().lview();
}

/** \brief return the public view of the listen ipport_addr_t 
 */
const ipport_addr_t &	casti_inetreach_httpd_t::listen_ipport_pview()	const throw()
{
	return m_ndiag_aview->ipport_pview_curr();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ndiag_aview_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ndiag_aview_t to notify an event
 */
bool 	casti_inetreach_httpd_t::neoip_ndiag_aview_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
						, const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter new_listen_pview=" << new_listen_pview);
	
	// notify the caller that ndiag_aview_t may have changed
	bool	tokeep	= notify_callback(new_listen_pview);
	if( !tokeep )	return false;
	
	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the bt_err_t
 */
bool casti_inetreach_httpd_t::notify_callback(const ipport_addr_t &new_listen_pview)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_casti_inetreach_httpd_cb(userptr, *this, new_listen_pview);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;




