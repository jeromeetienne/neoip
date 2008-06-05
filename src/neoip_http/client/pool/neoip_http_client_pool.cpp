/*! \file
    \brief Definition of the \ref http_client_pool_t

- TODO relation with the GET and POST
  - does this work with POST ? unlikely. if so document the reason

\par Brief Description
\ref http_client_pool_t is a pool which store the connection when they are not
used by http_client_t. When http_client_t starts, it queries the pool to see
if a connection http_client_pool_cnx_t is already established for the http_uri_t
to handle. If one is available, the http_client_t steals its socket_full_t and 
delete the http_client_pool_t to acquire it. Once the connection has been used
for a request, it may be put back into the pool by creating a new
http_client_pool_cnx_t.
If the maximum number of connection is reached, the oldest is discard using 
a classical LRU.

\par Possible improvement
- currently this doesnt handle the case when socket_itor_t is set in the http_client_t

*/

/* system include */
/* local include */
#include "neoip_http_client_pool.hpp"
#include "neoip_http_client_pool_cnx.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
http_client_pool_t::http_client_pool_t()	throw()
{
}

/** \brief Destructor
 */
http_client_pool_t::~http_client_pool_t()	throw()
{
	// close all pending http_client_pool_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
http_client_pool_t &	http_client_pool_t::set_profile(const http_client_pool_profile_t &profile)	throw()
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
http_err_t	http_client_pool_t::start()	throw()
{
	// return no error
	return http_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a http_client_pool_cnx_t which may handle this http_uri_t, or NULL if none may
 */
http_client_pool_cnx_t * http_client_pool_t::get_cnx_by_uri(const http_uri_t &http_uri)	const throw()
{
	std::list<http_client_pool_cnx_t *>::const_iterator	iter;
	// go thru the whole cnx_db
	for(iter = cnx_db.begin(); iter != cnx_db.end(); iter++){
		http_client_pool_cnx_t *	pool_cnx = *iter;
		// if this pool_cnx contains this http_uri, return this pointer
		if( pool_cnx->contain(http_uri) )	return pool_cnx;
	}
	// if this point is reached, no http_client_pool_cnx_t matches, return NULL
	return NULL;
}

/** \brief remote one http_client_pool_cnx_t from the pool (the oldest one for a LRU)
 */
void	http_client_pool_t::cnx_remove_older()	throw()
{
	// sanity check - the cnx_db MUST NOT be empty
	DBG_ASSERT( !cnx_db.empty() );
	// delete the oldest http_client_cnx_t
	nipmem_delete	cnx_db.front();
}


NEOIP_NAMESPACE_END;




