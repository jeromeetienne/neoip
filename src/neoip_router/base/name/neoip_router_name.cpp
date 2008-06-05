/*! \file
    \brief Implementation of \ref router_name_t
    
\ref Brief description
router_name_t handles the dns hostname stuff. They may be idn but the idn 
to ascii translation is handled externally aka it MUST be done before constructing
a router_name_t.

\par Examples
- *.exampledomain.com is a router_name_t is_domain_only
- myhost.* is a router_name_t is_host_only
- myhost.exampledomain.com is a router_name_t::is_fully_qualified

\ref host/domain functions
- is_host_only() is true if the first label is '*', false otherwise
- is_domain_only() is true if the last label is '*', false otherwise
- host() will return the hostname part of the router_name_t
- domain() will return the domainname part of the router_name_t

\par host/domain name and labels
- the hostname may contains multiple labels
- the domainname may contains multiple labels
- NOTE: when parsing a router_name_t from a raw string, the host name is
  assumed to be the first item. all the remaining of the string being the
  domain name

*/

/* system include */
/* local include */
#include "neoip_router_name.hpp"
#include "neoip_dns_helper.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_router_profile.hpp"	// only to get the is_XXXXsigned_ok
#include "neoip_router_peerid.hpp"	// only to get the is_XXXXsigned_ok


NEOIP_NAMESPACE_BEGIN;

// definition of \ref nunit_path_t constant
const char *	router_name_t::SEPARATOR	= ".";
const char *	router_name_t::GLOB_STR		= "*";
// end of constants definition

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor from std::string
 */
router_name_t::router_name_t(const std::string &str)		throw()
{
	// log to debug
	KLOG_DBG("enter str=" << str);
	// split the str into 2 words - to 'artificially' get a hostname/domain
	std::vector<std::string>	words = string_t::split(str, SEPARATOR, 2);
	// if words doesnt have 2 items, return a null object now
	if( words.size() != 2 )	return;
	// forward to ctor_internal - with hostname = first item, domainname = the remaining
	ctor_internal( words[0], words[1] );
}

/** \brief Constructor from explicit m_host / m_domain
 */
router_name_t::router_name_t(const std::string &m_host, const std::string &m_domain)	throw()
{
	// just forward to ctor_internal
	ctor_internal(m_host, m_domain);
}

/** \brief Constructor from explicit m_host / m_domain
 */
void router_name_t::ctor_internal(const std::string &m_host
					, const std::string &m_domain)	throw()
{
	// log to debug
	KLOG_DBG("enter host=" << m_host << " domain=" << m_domain);

	// sanity check - m_host either contains no GLOB_STR or is only GLOB_STR
	if( m_host != GLOB_STR && m_host.find(GLOB_STR) != std::string::npos )		return;
	// sanity check - m_domain either contains no GLOB_STR or is only GLOB_STR
	if( m_domain != GLOB_STR && m_domain.find(GLOB_STR) != std::string::npos )	return;
	// sanity check - m_host and m_domain MUST NOT be both GLOB_STR
	if( m_host == GLOB_STR && m_domain == GLOB_STR )				return;
	// sanity check - all label of the result MUST be legal
	std::vector<std::string>	words = string_t::split(m_host+SEPARATOR+m_domain, SEPARATOR);
	for(size_t i = 0;i < words.size(); i++ ){
		// if this label is a GLOB_STR, goto the next
		if( words[i] == GLOB_STR )			continue;
		// if this label is valid, goto the next
		if( dns_helper_t::label_is_valid(words[i]) )	continue;
		// else return a null object now
		return;
	}
	// copy the parameter
	this->m_host	= m_host;
	this->m_domain	= m_domain;
	// sanity check - the result MUST NOT be null
	DBG_ASSERT( !is_null() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if this router_name_t contains the other router_name_t
 * 
 * - WARNING: this router_name_t MUST be is_domain_only_t and the other MUST be
 *   is_fully_qualified()
 */
bool	router_name_t::match(const router_name_t &other)		const throw()
{
	// log to debug
	KLOG_DBG("enter this=" << *this << " other=" << other);
	// sanity check - this router_name_t MUST be a is_domain_only() or is_host_only()
	DBG_ASSERT( is_domain_only() || is_host_only() );
	// sanity check - the other router_name_t MUST be a is_fully_qualified()
	DBG_ASSERT( other.is_fully_qualified() );
	// return a string_t::glob_match
	// - it is ok because GLOB_STR == "*" which is the same for string_t::glob_match	
	return string_t::glob_match(to_string(), other.to_string());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if this router_name_t is valid for a selfsigned identity
 * 
 * - a selfsigned name is host_only
 */
bool	router_name_t::is_selfsigned_ok()	const throw()
{
	return is_host_only();
}

/** \brief Return true if this router_name_t is valid for a aithsigned identity
 * 
 * - a authsigned name is fully-qualified AND NOT have a NONESIGNED_DOMAIN
 */
bool	router_name_t::is_authsigned_ok()	const throw()
{
	if( !is_fully_qualified() )					return false;
	if(  domain() == router_profile_t::NONESIGNED_DOMAIN_STR )	return false;
	// if all previous tests passed, return true
	return true;
}

/** \brief Return true if this router_name_t if valid for a authsigned identity
 * 
 * - a nonesigned name is fully-qualified AND have a NONESIGNED_DOMAIN_STR
 *   AND have a canonical router_peerid_t as host
 */
bool	router_name_t::is_nonesigned_ok()	const throw()
{
	// log to debug
	KLOG_DBG("enter this=" << *this);
	// perform all the tests
	if( !is_fully_qualified() )					return false;
	if(  domain() != router_profile_t::NONESIGNED_DOMAIN_STR )	return false;
	if(  router_peerid_t::from_canonical_string(host()).is_null() )	return false;
	// if all previous tests passed, return true
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief add a domain to a host_only router_name_t
 * 
 * - WARNING: this router_name_t MUST be host_only and the other MUST be domain_only
 */
router_name_t &	router_name_t::add_domain(const router_name_t &other)	throw()
{
	// sanity check - this router_name_t MUST be a is_host_only()
	DBG_ASSERT( is_host_only() );
	// sanity check - the other router_name_t MUST be a is_domain_only()
	DBG_ASSERT( other.is_domain_only() );

	// copy the other.domain() into the local one
	m_domain	= other.domain();
		
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int router_name_t::compare(const router_name_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// compare the host()
	if( host() < other.host() )		return -1;
	if( host() > other.host() )		return +1;
	// NOTE: here both host() are equal

	// compare the domain()
	if( domain() < other.domain() )		return -1;
	if( domain() > other.domain() )		return +1;
	// NOTE: here both domain() are equal

	// here both are considered equal
	return 0;
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     to_string() function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Convert the object into a string
 */
std::string	router_name_t::to_string()	const throw()
{
	std::ostringstream 	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << host();
	oss << SEPARATOR;
	oss << domain();
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END



