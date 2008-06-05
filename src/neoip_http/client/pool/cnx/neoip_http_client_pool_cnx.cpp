/*! \file
    \brief Class to handle the http_client_pool_cnx_t

*/

/* system include */
/* local include */
#include "neoip_http_client_pool_cnx.hpp"
#include "neoip_http_client_pool.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
http_client_pool_cnx_t::http_client_pool_cnx_t()		throw()
{
	// zero some fields
	client_pool	= NULL;
	socket_full	= NULL;
}

/** \brief Destructor
 */
http_client_pool_cnx_t::~http_client_pool_cnx_t()		throw()
{
	// unlink this object from the http_client_pool_t
	if( client_pool )	client_pool->cnx_unlink(this);
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;		
}

/** \brief Autodelete the object and return false to ease readability
 */
bool	http_client_pool_cnx_t::autodelete()		throw()
{
	nipmem_delete	this;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
http_err_t	http_client_pool_cnx_t::start(http_client_pool_t *client_pool, socket_full_t *socket_full
				, const http_uri_t &current_uri
				, const std::set<http_uri_t> &handled_uri_db)	throw()
{
	const http_client_pool_profile_t & profile	= client_pool->get_profile();
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the current_uri MUST be present in the handled_uri_db
	DBG_ASSERT( handled_uri_db.find(current_uri) != handled_uri_db.end() );
	DBG_ASSERT( !current_uri.is_null() );
	DBG_ASSERT( socket_full );
	// copy the parameter
	this->client_pool	= client_pool;
	this->socket_full	= socket_full;
	this->current_uri	= current_uri;
	this->handled_uri_db	= handled_uri_db;

	// make room for this connection in the pool, if needed
	if( client_pool->size() >= profile.cnx_nb_max() )
		client_pool->cnx_remove_older();

	// link this object to the http_client_pool_t
	client_pool->cnx_dolink(this);
	DBG_ASSERT(client_pool->size() <= profile.cnx_nb_max());

	// redirect the socket_full callback to here
	socket_full->set_callback(this, NULL);	
	// start the expire_timeout
	expire_timeout.start(profile.cnx_timeout_delay(), this, NULL);
	
	// update the http_client_pool_stat_t
	http_client_pool_stat_t & pool_stat	= client_pool->pool_stat;
	pool_stat.nb_cnx_put	( pool_stat.nb_cnx_put() + 1 );
	pool_stat.nb_cnx_peak	( std::max(pool_stat.nb_cnx_peak(), client_pool->size()) );
	
	// return no error
	return http_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true this http_uri_t may be handled by this htt_client_pool_cnx_t
 */
bool	http_client_pool_cnx_t::contain(const http_uri_t &http_uri)	const throw()
{
	// sanity check - the socket_full MUST NOT be NULL
	DBG_ASSERT( socket_full );
	// if the http_uri is not contained in handled_uri_db, return false
	if( handled_uri_db.find(http_uri) == handled_uri_db.end() )	return false;
	// return true
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Steal the socket_full_t and delete the http_client_pool_cnx_t
 */
socket_full_t *	http_client_pool_cnx_t::steal_full_and_dtor()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// update the http_client_pool_stat_t
	http_client_pool_stat_t & pool_stat	= client_pool->pool_stat;
	pool_stat.nb_cnx_get	( pool_stat.nb_cnx_get() + 1 );
	// sanity check - socket_full_t MUST be non-null
	DBG_ASSERT( socket_full );
	// mark a copy the socket_full
	socket_full_t *	socket_full_stolen = socket_full;
	// set the callback to NULL 
	// - protective measure to ensure it wont call this socket_full_t without noticing
	socket_full_stolen->set_callback(NULL, NULL);
	// mark socket_full as unused
	socket_full	= NULL;
	// autodelete
	nipmem_delete	this;
	// return the copy
	return socket_full_stolen;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       expire_timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	http_client_pool_cnx_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// update the http_client_pool_stat_t
	http_client_pool_stat_t & pool_stat	= client_pool->pool_stat;
	pool_stat.nb_cnx_died_internal	( pool_stat.nb_cnx_died_internal() + 1 );
	// autodelete
	return autodelete();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when a connection is established
 */
bool	http_client_pool_cnx_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// update the http_client_pool_stat_t
	http_client_pool_stat_t & pool_stat	= client_pool->pool_stat;
	pool_stat.nb_cnx_died_external	( pool_stat.nb_cnx_died_external() + 1 );

	// delete the connection on ALL socket_event_t
	return autodelete();
}

NEOIP_NAMESPACE_END





