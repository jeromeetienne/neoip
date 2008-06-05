/*! \file
    \brief Definition of the \ref router_lident_t

\par Brief Description
\ref router_lident_t handle the local identity for a router_peer_t. Its include a 
router_peerid_t, the cert/privkey for the authentication and the router_name_t. 
It has 2 'modes' : 
- selfsigned: when the certificate is selfsigned. this is intended to suppport a ssh-like pki.
- authsigned: when the certificate is signed by another certificate. This is intended to support 
  a https-like pki.

\par About selfsigned mode
- the certificate is selfsigned. this allows a ssh-like PKI.
  - PRO: multiple router_peer_t can authenticate each other without using any 
    third party authenticator.
  - CON: this scales badly as any router_peer_t willing to accept a router_lident_t
    needs to 'manually' include the router_rident_t derived from it 
- the certificate subject name is the router_peerid_t canonical string
- the router_peerid_t is generated at random and thus is assumed statically globally unique
- the router_name_t is_host_only and represent 'only' a local view of the dnsname
  - it doesnt have to be the same on each peer

\par About authsigned mode
- the certificate is signed by a third party authority.
  - this third party authoriy is described in a router_rootca_t for its public information
  - the registration process is not described here, and is likely external to any neoip tool
  - this allows a https-like PKI
    - PRO: it scales very well, as a router_lident_t registered to router_rootca_t will be 
      accepted by any router_peer_t accepting this router_rootca_t
    - CON: this create a dependance on the third party authority. especially during the 
      registration process.
- the certificate subject name is the global router_name_t
- the router_peerid_t is directly derived from the router_name_t
  - it is assumed statically globally unique because the router_name_t is ensured to be unique
    by the router_rootca_t which generated the certificate
- the router_name_t is_fully_qualified and represent a global view of the dnsname for 
  for the scope of all router_peer_t accepting this router_rootca_t.

*/


/* system include */
/* local include */
#include "neoip_router_lident.hpp"
#include "neoip_router_rident.hpp"
#include "neoip_router_profile.hpp"
#include "neoip_dns_helper.hpp"
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
router_lident_t::router_lident_t(const router_peerid_t &m_peerid, const x509_privkey_t &m_privkey
			, const x509_cert_t &m_cert, const router_name_t &m_dnsname)	throw()
{
	// copy the parameter
	this->m_peerid	= m_peerid;
	this->m_privkey	= m_privkey;
	this->m_cert	= m_cert;
	this->m_dnsname	= m_dnsname;
	
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
bool	router_lident_t::is_sane()	const throw()
{
	// sanity check - the router_lident_t MUST be either selfsigned/authsigned/nonesigned
	DBG_ASSERT( is_selfsigned() || is_authsigned() || is_nonesigned() );
	// sanity check - the router_peerid_t MUST NOT be null
	DBG_ASSERT( !peerid().is_null() );
	// sanity check - the x509_privkey_t MUST NOT be null
	DBG_ASSERT( !privkey().is_null() );
	// sanity check - the x509_cert_t MUST NOT be null
	DBG_ASSERT( !cert().is_null() );
	// sanity check - m_dnsname MUST be either is_host_only() or is_fully_qualified()
	DBG_ASSERT( dnsname().is_host_only() || dnsname().is_fully_qualified() );
	// sanity check - if the dnsname() is_selfsigned_ok
	if( dnsname().is_selfsigned_ok() ){
		// sanity check - the subject_name MUST be the peerid canonical string
		DBG_ASSERT( peerid() == router_peerid_t::from_canonical_string(cert().subject_name()) );
		// sanity check - the cert MUST be selfsigned
		DBG_ASSERT( cert().subject_name() == cert().issuer_name() );
	}
	// sanity check - if dnsname() is_authsigned_ok
	if( dnsname().is_authsigned_ok() ){
		// sanity check - peerid MUST be directly derived from the dnsname
		DBG_ASSERT( peerid() == dnsname().to_string() );
		// sanity check - the cert MUST NOT be selfsigned
		DBG_ASSERT( cert().subject_name() != cert().issuer_name() );
	}
	// sanity check - if dnsname() is_nonesigned_ok
	if( dnsname().is_nonesigned_ok() ){
		// sanity check - peerid MUST be directly derived from router_name_t::host()
		DBG_ASSERT( peerid() == router_peerid_t::from_canonical_string(dnsname().host()) );
		// sanity check - the cert MUST be selfsigned
		DBG_ASSERT( cert().subject_name() == cert().issuer_name() );
	}
	// if all tests passed, it is considered sane, so return true
	return true;
}


/** \brief return a fully qualified router_name_t for this router_lident_t
 * 
 * - NOTE: the router_profile_t is required to get the selfsigned_domain()
 */
router_name_t	router_lident_t::dnsfqname(const router_profile_t &profile)	const throw()
{
	// if is_authsigned, the dnsname is already fully qualified
	if( is_authsigned() )	return dnsname();
	// if is_nonesigned, the dnsname is already fully qualified
	if( is_nonesigned() )	return dnsname();
	// if is_selfsigned, the dnsname is_host_only, so append selsigned_domain, and return it
	DBG_ASSERT( is_selfsigned() );	
	return dnsname() / profile.selfsigned_domain();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Extract the router_rident_t matching this router_lident_t
 */
router_rident_t	router_lident_t::to_rident()	const throw()
{
	// sanity check - the router_lident_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// sanity check - only selfsigned are allowed to produce a router_rident_t
	DBG_ASSERT( is_selfsigned() );
	// build the router_rident_t without any alias
	return router_rident_t(peerid(), cert(), dnsname());
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    build function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a new router_lident_t - return a null one if it failed
 * 
 * - NOTE: if ca_cert.is_null(), generate a selfsigned router_lident_t
 *   else generate an authsigned router_lident_t signed by this ca_cert
 */
router_lident_t	router_lident_t::generate(const router_peerid_t &peerid
			, const router_name_t &dnsname, int key_len
			, const x509_cert_t &ca_cert, const x509_privkey_t &ca_privkey)	throw()
{
	x509_privkey_t		privkey;
	x509_request_t		request;
	x509_cert_t		cert;
	item_arr_t<std::string>	name_db;
	std::string		peername;
	crypto_err_t		crypto_err;
	// log to debug
	KLOG_ERR("enter peerid=" << peerid << " router_name=" << dnsname);
	// sanity check - ca_cert and ca_privkey are either both is_null() or none are 
	DBG_ASSERT( ca_cert.is_null() == ca_privkey.is_null() );
	// sanity check - if dnsname is_authsigned_ok, ca_privkey MUST be null
	DBG_ASSERT( dnsname.is_authsigned_ok() != ca_cert.is_null() );
	// sanity check - if the ca_cert is null, dnsname MUST be is_fully_qualified
	if( !ca_cert.is_null() )	DBG_ASSERT( dnsname.is_fully_qualified() );

	// generate the private key
	crypto_err	= privkey.generate(key_len);
	if( crypto_err.failed() )	return router_lident_t();
	// generate the certificate request needed to build the certificate 
	// - if it is selfsigned, the subject_name is peerid.to_canonical string()
	// - if it is authsigned, the subject_name is dnsname.to_string()
	// - if it is nonesigned, the subject_name is dnsname.to_string()
	std::string	subject_name;
	if( dnsname.is_selfsigned_ok() )	subject_name	= peerid.to_canonical_string();
	else if( dnsname.is_authsigned_ok() )	subject_name	= dnsname.to_string();
	else if( dnsname.is_nonesigned_ok() )	subject_name	= dnsname.to_string();
	else	DBG_ASSERT( 0 );

	// generate the certificate request needed to build the certificate 
	crypto_err	= request.generate(subject_name, privkey);
	if( crypto_err.failed() )	return router_lident_t();
	// generate the certificate - selfsigned or authsigned depending on auth_cert parameter
	if( ca_cert.is_null() ){
		// generate the selfsigned certificate for the just built request
		crypto_err	= cert.generate_selfsigned(request, privkey);
		if( crypto_err.failed() )	return router_lident_t();
	}else{
		// sanity check - ca_privkey MUST NOT be is_null()
		DBG_ASSERT( !ca_privkey.is_null() );
		// generate the selfsigned certificate for the just built request
		crypto_err	= cert.generate_from_request(request, ca_cert, ca_privkey);
		if( crypto_err.failed() )	return router_lident_t();
	}
	// return the router_lident_t with all the field set
	return router_lident_t(peerid, privkey, cert, dnsname);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		 comparison operator
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 router_lident_t and return value ala strcmp/memcmp
 */
int	router_lident_t::compare(const router_lident_t &other)	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// handle the peerid
	if( peerid() < other.peerid() )			return -1;
	if( peerid() > other.peerid() )			return +1;
	// handle the privkey
	if( privkey() < other.privkey() )		return -1;
	if( privkey() > other.privkey() )		return +1;
	// handle the cert
	if( cert() < other.cert() )			return -1;
	if( cert() > other.cert() )			return +1;
	// handle the name_db
	if( dnsname() < other.dnsname() )		return -1;
	if( dnsname() > other.dnsname() )		return +1;	
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
std::string	router_lident_t::to_canonical_string()	const throw()
{
	std::ostringstream	oss;
	bytearray_t		buffer;
	// handle the null case
	if( is_null() )			return "null";
	// put the router_peerid_t
	oss << peerid();
	// put the private key
	oss << " " << base64_t::encode(privkey().to_der_datum());
	// put the certificate
	oss << " " << base64_t::encode(cert().to_der_datum());
	// put the dnsname
	oss << " " << dnsname();
	// return the just built string	
	return oss.str();
}

/** \brief build the object from a canonical string
 * 
 * @return the built router_lident_t or a null one if an error occured
 */
router_lident_t	router_lident_t::from_canonical_string(const std::string &str)	throw()
{
	std::vector<std::string>	words;
	router_peerid_t			peerid;
	x509_privkey_t			privkey;
	x509_cert_t			cert;
	router_name_t			dnsname;		
	// parse the input string
	words	= string_t::split(str, " ");
	// if the number of parameter is less than 4, return an error
	// - 4 for peerid + privkey + cert + dnsname
	if( words.size() != 4 )		return router_lident_t();
	
	// parse the string
	peerid		= words[0];
	privkey		= x509_privkey_t::from_der_datum( base64_t::decode(words[1]) );
	cert		= x509_cert_t::from_der_datum	( base64_t::decode(words[2]) );
	dnsname		= router_name_t(words[3]);
	
	// build a router_lident_t with the data from the parsed string
	return router_lident_t(peerid, privkey, cert, dnsname);
}


NEOIP_NAMESPACE_END

