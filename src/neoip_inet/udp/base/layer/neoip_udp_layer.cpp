/*! \file
    \brief Definition of the \ref udp_layer_t

*/

/* system include */
/* local include */
#include "neoip_udp_layer.hpp"
#include "neoip_udp_layer_http.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_namespace.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
udp_layer_t::udp_layer_t()	throw()
{
	// hook the httpd init to init the httpd_handler_t
	// - needed as the httpd will be init only after this layer
	lib_session_get()->event_hook_append( lib_session_t::HOOK_POST_HTTPD_INIT, this, NULL );
	lib_session_get()->event_hook_append( lib_session_t::HOOK_PRE_HTTPD_DEINIT, this, NULL );	
}

/** \brief Destructor
 */
udp_layer_t::~udp_layer_t()	throw()
{
	// delete the http handler
	if( udp_layer_http )	delete udp_layer_http;

	// unhook the httpd init to init the httpd_handler_t
	lib_session_get()->event_hook_remove( lib_session_t::HOOK_POST_HTTPD_INIT, this, NULL );
	lib_session_get()->event_hook_remove( lib_session_t::HOOK_PRE_HTTPD_DEINIT, this, NULL );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          event_hook_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief called when a event_hook_t level is notified
 */
bool	udp_layer_t::neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
							, int hook_level)	throw()
{
	// if the event if POST_HTTPD_INIT, init the httpd handler
	if( hook_level == lib_session_t::HOOK_POST_HTTPD_INIT ){
		udp_layer_http	= nipmem_new udp_layer_http_t(this);
	}else{	// if the event if PRE_HTTPD_DEINIT, init the httpd handler
		DBG_ASSERT( hook_level == lib_session_t::HOOK_PRE_HTTPD_DEINIT );
		nipmem_delete udp_layer_http;
		udp_layer_http = NULL;
	}
	return true;
}

NEOIP_NAMESPACE_END

