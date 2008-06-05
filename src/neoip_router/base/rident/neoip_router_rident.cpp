/*! \file
    \brief Definition of the \ref router_rident_t

- TODO to rename
  - the selfsigned matter MUST appears in the class name
  - what about :
    - router_rident_t => router_ssign_ident_t ?
    - router_lident_t => router_local_ident_t ?
    - quite long name but more explicit

\par Brief Description
\ref router_rident_t holds a selfsigned certificates with its router_peerid_t and 
its names.
- the peerid is assumed to be statically globally unique
- the names are only for the local view, aka it is used only for the local peer
  to do a hostname resolution on the remote peer
- the selfsigned certificates allows to avoid any dependancy to a third party
  certificate generator.
  - it provides ssh type of pki aka (i) independant on any third party ca
    but (ii) scale badly as it requires user intervention to set it up in
    EACH allowed peer, 
- look at neoip_router_lident.cpp file header for more information

*/


/* system include */
/* local include */
#include "neoip_router_rident.hpp"
#include "neoip_router_profile.hpp"
#include "neoip_dns_helper.hpp"
#include "neoip_string.hpp"
#include "neoip_x509_request.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_base64.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_rident_t::router_rident_t(const router_peerid_t &m_peerid, const x509_cert_t &m_cert
				, const router_name_t &m_hostonly_lview)	throw()
{
	// copy the parameter
	this->m_peerid		= m_peerid;
	this->m_cert		= m_cert;
	this->m_hostonly_lview	= m_hostonly_lview;

	// sanity check - the object MUST be sane
	DBG_ASSERT( is_sane() );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the router_lident_t is_sane() false otherwise
 */
bool	router_rident_t::is_sane()	const throw()
{
	// sanity check - the router_peerid_t MUST NOT be null
	DBG_ASSERT( !peerid().is_null() );
	// sanity check - the x509_cert_t MUST NOT be null
	DBG_ASSERT( !cert().is_null() );
	// sanity check - the cert MUST be selfsigned
	DBG_ASSERT( cert().subject_name() == cert().issuer_name() );
	// sanity check - m_dnsname MUST be is_host_only
	DBG_ASSERT( hostonly_lview().is_host_only() );
	// if all tests passed, it is considered sane, so return true
	return true;
}

/** \brief return a fully qualified router_name_t for this router_lident_t
 * 
 * - NOTE: the router_profile_t is required to get the selfsigned_domain()
 */
router_name_t	router_rident_t::dnsfqname(const router_profile_t &profile)	const throw()
{
	return hostonly_lview() / profile.selfsigned_domain();
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		 comparison operator
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 router_rident_t and return value ala strcmp/memcmp
 */
int	router_rident_t::compare(const router_rident_t &other)	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// handle the peerid
	if( peerid() < other.peerid() )			return -1;
	if( peerid() > other.peerid() )			return +1;
	// handle the cert
	if( cert() < other.cert() )			return -1;
	if( cert() > other.cert() )			return +1;
	// handle the hostonly_lview
	if( hostonly_lview() < other.hostonly_lview() )	return -1;
	if( hostonly_lview() > other.hostonly_lview() )	return +1;
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
std::string	router_rident_t::to_canonical_string()	const throw()
{
	std::ostringstream	oss;
	bytearray_t		buffer;
	// handle the null case
	if( is_null() )			return "null";
	// put the router_peerid_t
	oss << peerid();
	// put the certificate
	oss << " " << base64_t::encode(cert().to_der_datum());	
	// put the dnsname
	oss << " " << hostonly_lview();
	// return the just built string	
	return oss.str();
}

/** \brief build the object from a canonical string
 * 
 * @return the built router_rident_t or a null one if an error occured
 */
router_rident_t	router_rident_t::from_canonical_string(const std::string &str)	throw()
{
	std::vector<std::string>	words;
	router_peerid_t			peerid;
	x509_cert_t			cert;		
	router_name_t			hostonly_lview;
	// parse the input string
	words	= string_t::split(str, " ");
	// if the number of parameter is less than 3, return an error
	// - 3 for peerid + cert + hostonly_lview	
	if( words.size() != 3 )		return router_rident_t();
	
	// parse the string
	peerid		= words[0];
	cert		= x509_cert_t::from_der_datum( base64_t::decode(words[1]) );
	hostonly_lview	= router_name_t(words[2]);
	
	// build a router_rident_t with the data from the parsed string
	return router_rident_t(peerid, cert, hostonly_lview);
}


NEOIP_NAMESPACE_END

