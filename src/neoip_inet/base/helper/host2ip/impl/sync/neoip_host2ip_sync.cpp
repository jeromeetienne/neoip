/*! \file
    \brief Definition of the \ref host2ip_sync_t class

\par Brief description
\ref host2ip_sync_t convert a hostname into a list of ip_addr_t. this particular
implementation performs it via a synchronous gethostbyname(), this break
the assumption "all iteration of the event loop MUST be short" so this is to 
be used with GREAT CARE. 
It serves as a 'known to work' way to do hostname resolution.

*/

/* system include */
#ifdef _WIN32
#	include <winsock2.h>
#	undef ERROR
#	undef INFINITE
#	undef OPTIONAL
#else
#	include <netdb.h>
#	include <sys/socket.h>
#endif
/* local include */
#include "neoip_host2ip_sync.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
host2ip_sync_t::host2ip_sync_t()	throw()
{
}

/** \brief Destructor
 */
host2ip_sync_t::~host2ip_sync_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
inet_err_t	host2ip_sync_t::start(const std::string &m_hostname, host2ip_cb_t *callback
							, void *userptr)	throw()
{
	// copy the parameter
	this->m_hostname	= m_hostname;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// launch the zerotimer_t to perform the gethostbyname in the next iteration of the event loop
	zerotimer.append(this, NULL);
	
	// return no error
	return inet_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	host2ip_sync_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter hostname="<< hostname());
#ifdef	__linux__
	char		buf[10*1024];
	struct hostent	hostbuf, *entry;
	int		herr, res;
	// call the gethostbyname() which is thread-ok
	res = gethostbyname_r(hostname().c_str(), &hostbuf, buf, sizeof(buf), &entry, &herr);
	// handle error case
	if( res || entry == NULL ){
#else
	struct hostent *	entry;
	// call the gethostbyname()
	// - it IS NOT thread-ok but as neoip doesnt use thread it is ok :)
	entry	= gethostbyname(hostname().c_str());
	if( entry == NULL ){
#endif
		inet_err_t inet_err	= inet_err_t(inet_err_t::ERROR, "Unable to resolve " + hostname());
		return notify_callback(inet_err, std::vector<ip_addr_t>() );
	}
	
	// copy the address list from entry to ip_addr_db
	// - NOTE: it is IPv4 only
	std::vector<ip_addr_t>	ip_addr_db;	
	for(size_t i = 0; entry->h_addr_list[i]; i++){
		struct 	in_addr addr;
#ifndef _WIN32
		bcopy(entry->h_addr_list[i], (char *) &addr, sizeof(addr));
#else
		memcpy((char *) &addr, entry->h_addr_list[i], sizeof(addr));
#endif
		// log to debug
		KLOG_DBG("ip_addr=" << ip_addr_t(ntohl(addr.s_addr)) );
		// put the ip_addr_t in the ip_addr_db
		ip_addr_db.push_back( ip_addr_t(ntohl(addr.s_addr)) );
	}	
	// notify the result
	return notify_callback(inet_err_t::OK, ip_addr_db);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool host2ip_sync_t::notify_callback(const inet_err_t &inet_err, const std::vector<ip_addr_t> &result)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_host2ip_cb(userptr, *this, inet_err, result);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END

