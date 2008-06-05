/*! \file
    \brief definition of the \ref socket_nunit_stcp_t
*/

/* system include */
/* local include */
#include "neoip_socket_nunit_stcp.hpp"
#include "neoip_log.hpp"

// include to access the domain specific profile
#include "neoip_socket_profile_stcp.hpp"
#include "neoip_slay_profile.hpp"
#include "neoip_slay_profile_btjamstd.hpp"
#include "neoip_slay_profile_btjamrc4.hpp"
#include "neoip_bt_jamrc4_profile.hpp"
#include "neoip_slay_profile_tls.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_nunit_stcp_t::socket_nunit_stcp_t()	throw()
{
	server_addr	= "stcp://127.0.0.1:4000";
#if 0	// TODO to remove - only for experiencing with remote server and tls	
	server_addr	= "stcp://192.168.1.3:443";
#endif

	socket_domain	= socket_domain_t::STCP;
	socket_type	= socket_type_t::STREAM;
	socket_profile	= socket_profile_t(socket_domain);

#if 0	// build a slay_profile_t for slay_domain_t::TLS
	slay_profile_t		slay_profile(slay_domain_t::TLS);
	slay_profile_tls_t &	profile_tls	= slay_profile_tls_t::from_slay(slay_profile);
	profile_tls.tls().authtype(tls_authtype_t::ANON);
#endif
	
#if 0	// build a slay_profile_t for slay_domain_t::BTJAMSTD
	slay_profile_t		slay_profile(slay_domain_t::BTJAMSTD);
	slay_profile_btjamstd_t & profile_btjamstd	= slay_profile_btjamstd_t::from_slay(slay_profile);
	profile_btjamstd.shared_secret("my very own shared secret");
#endif

#if 1	// build a slay_profile_t for slay_domain_t::JAMRC4

	// build the bt_jamrc4_profile_t
	bt_jamrc4_profile_t	jamrc4_profile;
	jamrc4_profile.itor_type_default	( bt_jamrc4_type_t::NOJAM );
	jamrc4_profile.resp_type_accept_arr	( bt_jamrc4_type_arr_t()
							.append(bt_jamrc4_type_t::NOJAM)
//							.append(bt_jamrc4_type_t::DOJAM)
						);
	// set bt_jamrc4_profile_t into slay_profile_t
	slay_profile_t		slay_profile(slay_domain_t::BTJAMRC4);
	slay_profile_btjamrc4_t&slay_profile_dom= slay_profile_btjamrc4_t::from_slay(slay_profile);
	slay_profile_dom.bt_jamrc4	(jamrc4_profile);
#endif

	// set the slay_profile_t into the socket_profile_t
	socket_profile_stcp_t &	socket_profile_dom	= socket_profile_stcp_t::from_socket(socket_profile);
	socket_profile_dom.slay		(slay_profile);
}

/** \brief destructor
 */
socket_nunit_stcp_t::~socket_nunit_stcp_t()	throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	
/** \brief Return a socket_resp_arg_t for this domain
 */
socket_resp_arg_t	socket_nunit_stcp_t::get_resp_arg()	const throw()
{
	socket_resp_arg_t	resp_arg;
	// build the resp_arg
	resp_arg	= socket_resp_arg_t().profile(socket_profile).domain(socket_domain)
						.type(socket_type).listen_addr(server_addr);	
	// return the resp_arg
	return resp_arg;
}

/** \brief Return a socket_itor_arg_t for this domain
 */
socket_itor_arg_t	socket_nunit_stcp_t::get_itor_arg()	const throw()
{
	socket_itor_arg_t	itor_arg;
	// build the itor_arg
	itor_arg	= socket_itor_arg_t().profile(socket_profile).domain(socket_domain)
						.type(socket_type).remote_addr(server_addr);	
	// return the itor_arg
	return itor_arg;
}

NEOIP_NAMESPACE_END




