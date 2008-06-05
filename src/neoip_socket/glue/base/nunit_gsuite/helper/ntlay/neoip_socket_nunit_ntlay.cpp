/*! \file
    \brief definition of the \ref socket_nunit_ntlay_t
*/

/* system include */
/* local include */
#include "neoip_socket_nunit_ntlay.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_log.hpp"

// include to access the domain specific profile
#include "neoip_socket_profile_ntlay.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_nunit_ntlay_t::socket_nunit_ntlay_t()	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// init some variable
	socket_domain	= socket_domain_t::NTLAY;
	socket_type	= socket_type_t::DGRAM;
	socket_profile	= socket_profile_t(socket_domain);

	// start the ntudp_peer
	ntudp_err_t	ntudp_err;
	ntudp_peer	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_peer->start(ipport_aview_t::ANY_IP4, ntudp_npos_res_t::get_default()
								, ntudp_peerid_t::build_random());
	DBG_ASSERT( ntudp_err.succeed() );
	// set the ntudp_peer in the socket_profile
	socket_profile_ntlay_t &	profile_dom	= socket_profile_ntlay_t::from_socket(socket_profile);
	profile_dom.ntudp_peer( ntudp_peer );

	// build the server address
	ntudp_peerid_t	peerid	= ntudp_peer->local_peerid();
	ntudp_portid_t	portid	= ntudp_portid_t::build_random();
	server_addr	= "ntlay://" + peerid.to_string() + ":" + portid.to_string();

	// generate the authentication certificate and private key
	x509_privkey_t	ident_privkey;
	x509_request_t	ident_request;
	x509_cert_t	ident_cert;
	crypto_err_t	crypto_err;
	crypto_err	= ident_privkey.generate(512);
	DBG_ASSERT( crypto_err.succeed() );
	crypto_err	= ident_request.generate("dummy cert for socket nunit", ident_privkey);
	DBG_ASSERT( crypto_err.succeed() );
	crypto_err	= ident_cert.generate_selfsigned(ident_request, ident_privkey);
	DBG_ASSERT( crypto_err.succeed() );
	// - set the socket_profile with the ident_privkey, ident_cert and the callback to authenticate
	//   remote peer
	profile_dom.scnx().ident_privkey	( ident_privkey 		);
	profile_dom.scnx().ident_cert		( ident_cert 			);
	profile_dom.scnx().scnx_auth_ftor	( scnx_auth_ftor_t(this, NULL)	);
}

/** \brief destructor
 */
socket_nunit_ntlay_t::~socket_nunit_ntlay_t()	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// delete the ntudp_peer_t
	nipmem_zdelete	ntudp_peer;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      socket certificate authentication
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called to authenticate a x509_cert_t received by the scnx layer
 * 
 * @return scnx_err_t::OK if the certificate is allowed, any other value mean denied
 */
scnx_err_t	socket_nunit_ntlay_t::scnx_auth_ftor_cb(void *userptr, const x509_cert_t &cert) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// accept all certificate
	return scnx_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	
/** \brief Return a socket_resp_arg_t for this domain
 */
socket_resp_arg_t	socket_nunit_ntlay_t::get_resp_arg()	const throw()
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
socket_itor_arg_t	socket_nunit_ntlay_t::get_itor_arg()	const throw()
{
	socket_itor_arg_t	itor_arg;
	// build the itor_arg
	itor_arg	= socket_itor_arg_t().profile(socket_profile).domain(socket_domain)
							.type(socket_type).remote_addr(server_addr);	
	// return the itor_arg
	return itor_arg;
}

NEOIP_NAMESPACE_END




