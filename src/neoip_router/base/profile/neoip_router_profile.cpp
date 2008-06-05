/*! \file
    \brief Definition of the \ref router_profile_t

- TODO split this router_profile_t into several sub one
  - e.g. there are 3 field only for itor

*/


/* system include */
/* local include */
#include "neoip_router_profile.hpp"

NEOIP_NAMESPACE_BEGIN;


// definition of \ref router_profile_t constant
#if 1
	const delay_t		router_profile_t::RESP_CNX_TIMEOUT		= delay_t::from_sec(60);
	const delay_t		router_profile_t::PEER_RECORD_TTL		= delay_t::from_sec(3*60);
	const delay_t		router_profile_t::ACACHE_TTL			= delay_t::from_min(24*60);
	const delay_t		router_profile_t::ITOR_NEGCACHE_TTL		= delay_t::from_sec(5);
	const size_t		router_profile_t::ITOR_INNERPKT_QUEUE_MAXLEN	= 3;
	const delay_t		router_profile_t::ITOR_EXPIRE_DELAY		= delay_t::from_sec(30);
	const delaygen_arg_t	router_profile_t::ADDRNEGO_DELAYGEN		= delaygen_expboff_arg_t()
										.min_delay(delay_t::from_sec(1))
										.max_delay(delay_t::from_sec(10))
										.timeout_delay(delay_t::from_sec(60));
	const router_name_t	router_profile_t::SELFSIGNED_DOMAIN		= router_name_t("*.neoip");
	const std::string	router_profile_t::SELFSIGNED_DOMAIN_STR		= "neoip";
	const std::string	router_profile_t::NONESIGNED_DOMAIN_STR		= "unauthenticated";
	const bool		router_profile_t::TUNNEL_STUB			= false;
#else	// debug value only
	const delay_t		router_profile_t::RESP_CNX_TIMEOUT		= delay_t::from_sec(60);
	const delay_t		router_profile_t::PEER_RECORD_TTL		= delay_t::from_sec(3*60);
	const delay_t		router_profile_t::ACACHE_TTL			= delay_t::from_min(24*60);
	const delay_t		router_profile_t::ITOR_NEGCACHE_TTL		= delay_t::from_sec(5);
	const size_t		router_profile_t::ITOR_INNERPKT_QUEUE_MAXLEN	= 3;
	const delay_t		router_profile_t::ITOR_EXPIRE_DELAY		= delay_t::from_sec(3*60);
	const delaygen_arg_t	router_profile_t::ADDRNEGO_DELAYGEN		= delaygen_expboff_arg_t()
										.min_delay(delay_t::from_sec(1))
										.max_delay(delay_t::from_sec(10))
										.timeout_delay(delay_t::from_sec(60));
	const router_name_t	router_profile_t::SELFSIGNED_DOMAIN		= router_name_t("*.neoip");
	const std::string	router_profile_t::SELFSIGNED_DOMAIN_STR		= "neoip";
	const std::string	router_profile_t::NONESIGNED_DOMAIN_STR		= "unauthenticated";
	const bool		router_profile_t::TUNNEL_STUB			= false;
#endif
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_profile_t::router_profile_t()	throw()
{
	// set the constant fields
	resp_cnx_timeout		(RESP_CNX_TIMEOUT);
	peer_record_ttl			(PEER_RECORD_TTL);
	acache_ttl			(ACACHE_TTL);
	itor_negcache_ttl		(ITOR_NEGCACHE_TTL);
	itor_innerpkt_queue_maxlen	(ITOR_INNERPKT_QUEUE_MAXLEN);
	itor_expire_delay		(ITOR_EXPIRE_DELAY);
	addrnego_delaygen		(ADDRNEGO_DELAYGEN);
	selfsigned_domain		(SELFSIGNED_DOMAIN);
	selfsigned_domain_str		(SELFSIGNED_DOMAIN_STR);
	nonesigned_domain_str		(NONESIGNED_DOMAIN_STR);
	tunnel_stub			(TUNNEL_STUB);
		
	// sanity check - the profile MUST succeed check() by default
	DBG_ASSERT( check().succeed() );
}
/** \brief Destructor
 */
router_profile_t::~router_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
router_err_t	router_profile_t::check()	const throw()
{
	// check that selfsigned_domain() is_domain_only
	if( !selfsigned_domain().is_domain_only() )
		return router_err_t(router_err_t::ERROR, "selfsigned_domain MUST be is_domain_only."); 
	// return no error
	return router_err_t::OK;
}

NEOIP_NAMESPACE_END

