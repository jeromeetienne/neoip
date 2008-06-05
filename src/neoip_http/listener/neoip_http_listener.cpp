/*! \file
    \brief Definition of the \ref http_listener_t

\par Possible Improvement
- to allow the http_resp_t to put back the socket_full_t once done
  - thus it would allows multiple requests per connection
  - http_listener_t::putback_socket_full(socket_full_t *socket_full, const bytearray_t &recved_data)
  - the recved_data being the data read on the socket_full_t which didnt belong to the previous request

*/

/* system include */
#include <algorithm>
/* local include */
#include "neoip_http_listener.hpp"
#include "neoip_http_listener_cnx.hpp"
#include "neoip_http_resp.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
http_listener_t::http_listener_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	socket_resp	= NULL;	
}

/** \brief Destructor
 */
http_listener_t::~http_listener_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// close all pending http_listener_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
	// close all pending http_resp_t
	DBGNET_ASSERT( resp_db.empty() );	
	while( !resp_db.empty() )	nipmem_delete resp_db.front();
	// delete the socket_resp if needed
	nipmem_zdelete	socket_resp;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
http_listener_t &	http_listener_t::set_profile(const http_listener_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check().succeed() );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
http_err_t	http_listener_t::start(const socket_resp_arg_t &resp_arg)	throw()
{
	socket_err_t	socket_err;
	// start the socket_resp_t
	socket_resp	= nipmem_new socket_resp_t();
	socket_err	= socket_resp->start(resp_arg, this, NULL);
	if( socket_err.failed() )	return http_err_from_socket(socket_err);	
	
	// return no error
	return http_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Functor to sort http_resp_t. it put the longest listen_uri path first
 */
struct cmp_resp_path_t : public std::binary_function<http_resp_t *, http_resp_t *, bool> {
	bool	operator()(http_resp_t *resp1, http_resp_t *resp2)	throw() 
	{
		const file_path_t &	path1	= resp1->get_listen_uri().path(); 
		const file_path_t &	path2	= resp2->get_listen_uri().path();
		return path1.size() > path2.size();
	}
};

/** \brief dolink a http_resp_t to this listener
 */
void	http_listener_t::resp_dolink(http_resp_t *resp) 	throw()
{
	// insert the responder at the end of the resp_db
	resp_db.push_back(resp);
	// sort it to keep the longest listen_uri path first
	resp_db.sort(cmp_resp_path_t());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a pointer on a http_resp_t which matches this http_reqhd_t, or NULL if none does
 */
http_resp_t *	http_listener_t::find_resp(const http_reqhd_t &http_reqhd)	throw()
{
	std::list<http_resp_t *>::iterator	iter;
	// go thru the whole resp_db
	for(iter = resp_db.begin(); iter != resp_db.end(); iter++){
		http_resp_t *	http_resp	= *iter;
		// if this http_resp_t match the http_reqhd_t, return it
		if( http_resp->may_handle(http_reqhd) )	return http_resp;
	}
	// if this point is reached, no http_resp_t matches this http_reqhd_t
	return NULL;
}

/** \brief Return the listen_addr of the http_listener_t
 */
const socket_addr_t  &	http_listener_t::listen_addr()		const throw()
{
	return socket_resp->listen_addr();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_resp_t when a connection is established
 */
bool	http_listener_t::neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( socket_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:{
			http_listener_cnx_t *	listener_cnx;
			http_err_t		http_err;
			listener_cnx	= nipmem_new http_listener_cnx_t();
			http_err	= listener_cnx->start(this, socket_event.get_cnx_established());
			if( http_err.failed() )	nipmem_delete listener_cnx;
			return true;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END;




