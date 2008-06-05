/*! \file
    \brief definition of the \ref socket_nunit_ntudp_t
*/

/* system include */
/* local include */
#include "neoip_socket_nunit_ntudp.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_log.hpp"

// include to access the domain specific profile
#include "neoip_socket_profile_ntudp.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_nunit_ntudp_t::socket_nunit_ntudp_t()	throw()
{
	socket_domain	= socket_domain_t::NTUDP;
	socket_type	= socket_type_t::DGRAM;
	socket_profile	= socket_profile_t(socket_domain);

	
// TODO bug the peerid of the server_addr doesnt match the one of the ntudp_peer
	
	// start the ntudp_peer
	ntudp_err_t	ntudp_err;
	ntudp_peer	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_peer->start(ipport_aview_t::ANY_IP4, ntudp_npos_res_t::get_default()
								, ntudp_peerid_t::build_random());
	KLOG_ERR(" ntudp_err=" << ntudp_err);
	DBG_ASSERT( ntudp_err.succeed() );
	
	// set the ntudp_peer in the socket_profile
	socket_profile_ntudp_t & profile_dom	= socket_profile_ntudp_t::from_socket(socket_profile);
	profile_dom.ntudp_peer( ntudp_peer );

	// build the server address
	ntudp_peerid_t	peerid	= ntudp_peer->local_peerid();
	ntudp_portid_t	portid	= ntudp_portid_t::build_random();
	server_addr	= "ntudp://" + peerid.to_string() + ":" + portid.to_string();
}

/** \brief destructor
 */
socket_nunit_ntudp_t::~socket_nunit_ntudp_t()	throw()
{
	// delete the ntudp_peer_t
	nipmem_zdelete	ntudp_peer;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	
/** \brief Return a socket_resp_arg_t for this domain
 */
socket_resp_arg_t	socket_nunit_ntudp_t::get_resp_arg()	const throw()
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
socket_itor_arg_t	socket_nunit_ntudp_t::get_itor_arg()	const throw()
{
	socket_itor_arg_t	itor_arg;
	// build the itor_arg
	itor_arg	= socket_itor_arg_t().profile(socket_profile).domain(socket_domain)
							.type(socket_type).remote_addr(server_addr);	
	// return the itor_arg
	return itor_arg;
}

NEOIP_NAMESPACE_END




