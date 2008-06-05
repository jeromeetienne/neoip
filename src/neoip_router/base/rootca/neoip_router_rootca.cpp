/*! \file
    \brief Definition of the \ref router_rootca_t

\par Brief Description
\ref router_rootca_t hold a certificate for a root ca which is allow to generate
certificate for all the domains in its domain_db.
- it is its roles to ensure that all the generated certificate are unique inside
  those domains.

*/


/* system include */
/* local include */
#include "neoip_router_rootca.hpp"
#include "neoip_dns_helper.hpp"
#include "neoip_string.hpp"
#include "neoip_x509_request.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_base64.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_rootca_t::router_rootca_t(const x509_cert_t &m_cert, const item_arr_t<router_name_t> &m_domain_db)	throw()
{
	// sanity check - the domain_db MUST NOT be empty - it should at least contain one name
	DBG_ASSERT( m_domain_db.size() >= 1 );	
	// copy the parameter
	this->m_cert		= m_cert;
	this->m_domain_db	= m_domain_db;
	// sanity check - all item of the domain_db MUST be is_domain_only
	for(size_t i = 0; i < domain_db().size(); i++ )
		DBG_ASSERT( domain_db()[i].is_domain_only() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if this router_rootca_t matches this dnsname
 */ 
bool	router_rootca_t::match_dnsname(const router_name_t &dnsname)	const throw()
{
	// sanity check - the dnsname MUST be fully_qualified
	DBG_ASSERT( dnsname.is_fully_qualified() );
	// sanity check - all item of the domain_db MUST be is_domain_only
	for(size_t i = 0; i < domain_db().size(); i++ ){
		const router_name_t &	domain_name	= domain_db()[i];
		// if this domain_name matches the dnsname, return true
		if( domain_name.match(dnsname) )	return true;
	}
	// if this point is reached, no domain matches this router_name_t, return false
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//		 comparison operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 router_rootca_t and return value ala strcmp/memcmp
 */
int	router_rootca_t::compare(const router_rootca_t &other)	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// handle the cert
	if( cert() < other.cert() )			return -1;
	if( cert() > other.cert() )			return +1;
	// handle the domain_db
	if( domain_db() < other.domain_db() )		return -1;
	if( domain_db() > other.domain_db() )		return +1;
	// here both are considered equal
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            string convertion
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a canonical string
 */
std::string	router_rootca_t::to_canonical_string()	const throw()
{
	std::ostringstream	oss;
	bytearray_t		buffer;
	// handle the null case
	if( is_null() )		return "null";
	// put the certificate
	oss << base64_t::encode(cert().to_der_datum());	
	// put all the name of domain_db
	for(size_t i = 0; i < domain_db().size(); i++ )	oss << " " << domain_db()[i];
	// return the just built string	
	return oss.str();
}

/** \brief build the object from a canonical string
 * 
 * @return the built router_rootca_t or a null one if an error occured
 */
router_rootca_t	router_rootca_t::from_canonical_string(const std::string &str)	throw()
{
	std::vector<std::string>	words;
	x509_cert_t			cert;		
	item_arr_t<router_name_t>		domain_db;
	// parse the input string
	words	= string_t::split(str, " ");
	// if the number of parameter is less than 2, return an error
	// - 1 for x509_cert_t, +1 for at least one domain	
	if( words.size() < 2 )		return router_rootca_t();
	
	// parse the certificate
	cert	= x509_cert_t::from_der_datum( base64_t::decode(words[0]) );
	
	// copy the rest of the parameters into the domain_db
	for(size_t i = 1; i < words.size(); i++){
		router_name_t	domain_name	= router_name_t(words[i]);
		// if this domain_name IS NOT a router_name_t::is_domain_only() return a null object
		if( !domain_name.is_domain_only() )	return router_rootca_t();
		// else appends it
		domain_db	+= domain_name;
	}
	
	// build a router_rootca_t with the data from the parsed string
	return router_rootca_t(cert, domain_db);
}


NEOIP_NAMESPACE_END

