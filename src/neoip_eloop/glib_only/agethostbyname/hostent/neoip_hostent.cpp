/*! \file
    \brief Definition of the \ref hostent_t class
    
*/

/* system include */
#include <netdb.h>
#include <sys/socket.h>
/* local include */
#include "neoip_hostent.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor from the C struct hostent
 */
hostent_t::hostent_t(struct hostent *entry)	throw()
{
	// copy hostname
	hostname	= entry->h_name;
	// log to debug
	KLOG_DBG("hostname=" << hostname);
	// copy the alias list
	for( size_t i = 0; entry->h_aliases[i]; i++ ){
		// log to debug
		KLOG_DBG("alias=" << entry->h_aliases[i]);
		// put the alias in the alias_db
		alias_db.push_back( entry->h_aliases[i] );
	}
	// copy the address list
	// - NOTE: it is IPv4 only
	for( size_t i = 0; entry->h_addr_list[i]; i++ ){
		struct 	in_addr addr;
		bcopy(entry->h_addr_list[i], (char *) &addr, sizeof(addr));
		// log to debug
		KLOG_DBG("ip_addr=" << ip_addr_t(ntohl(addr.s_addr)) );
		// put the ip_addr_t in the ip_addr_db
		ip_addr_db.push_back( ip_addr_t(ntohl(addr.s_addr)) );
	}	
}

/** \brief Destructor
 */
hostent_t::~hostent_t()	throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      to_string() function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Convert the object into a string
 */
std::string	hostent_t::to_string()	const throw()
{
	std::ostringstream	oss;
	
	// handle the null case
	if( is_null() )	return "null hostent_t";
	
	// put the hostname
	oss << "hostname=" << hostname;
	oss << " ";
	// put the alias list
	oss << "alias_db=[";
	for( size_t i = 0; i < alias_db.size(); i++ ){
		if( i != 0 )	oss << " ";
		oss << alias_db[i];
	}
	oss << "] ";
	// put the ip_addr_t list
	oss << "ip_addr_db=[";
	for( size_t i = 0; i < ip_addr_db.size(); i++ ){
		if( i != 0 )	oss << " ";
		oss << ip_addr_db[i];
	}
	oss << "]";
	
	// return the built string
	return oss.str();
}



NEOIP_NAMESPACE_END

