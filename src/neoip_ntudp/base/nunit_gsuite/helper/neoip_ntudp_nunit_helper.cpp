/*! \file
    \brief Definition of the helper for nunit_suite_t

\par Brief description
This module is a list of functions to help start a ntudp_peer_t in fashion
common to nunit. it is not intended to be used in normal usage of ntudp_peer_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_nunit_helper.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ipport_addr.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Just a short cut to start the ntudp_peer_t as if it was inetreach
 * 
 * - with listen_addr_pview_cfg giving the listen_addr public view
 * 
 * - NOTE: it should be used only for nunit stuff
 */
ntudp_err_t	ntudp_nunit_peer_start_inetreach(ntudp_peer_t *ntudp_peer
					, const ipport_addr_t &listen_addr_pview_cfg
					, const ntudp_peerid_t &local_peerid_orig) throw()
{
	// set the local_peerid
	ntudp_peerid_t	local_peerid	= local_peerid_orig;
	if( local_peerid.is_null() )	local_peerid	= ntudp_peerid_t::build_random();	
	
	// sanity check - the listen_addr_pview_cfg MUST be fully_qualified
	DBG_ASSERT( listen_addr_pview_cfg.is_fully_qualified() );
	DBG_ASSERT( listen_addr_pview_cfg.ipaddr() == "127.0.0.1" );

	// set the ntudp_npos_res_t
	ntudp_npos_res_t	npos_res;
	npos_res.inetreach(true);
	npos_res.local_ipaddr_lview( listen_addr_pview_cfg.ipaddr() );
	npos_res.local_ipaddr_pview( listen_addr_pview_cfg.ipaddr() );
	npos_res.natted(false);
	// sanity check - the ntudp_npos_res_t MUST be completed
	DBG_ASSERT( npos_res.completed() );

	// setup ntudp_profile_t to disable ntudp_npos_watch_t - leaving the original npos_res for ever
	ntudp_profile_t	profile;
	profile.npos_watch().eval_trigger(delaygen_regular_arg_t().first_delay(delay_t::INFINITE));
	ntudp_peer->set_profile(profile);

	// Start the ntudp_peer_t
	return ntudp_peer->start(ipport_aview_t(listen_addr_pview_cfg, listen_addr_pview_cfg), npos_res, local_peerid);	
}

/** \brief Just a short cut to start the ntudp_peer_t as if it was natsym and non inetreach
 * 
 * - NOTE: it should be used only for nunit stuff
 */
ntudp_err_t	ntudp_nunit_peer_start_default(ntudp_peer_t *ntudp_peer)		throw()
{
	ntudp_peerid_t		local_peerid	= ntudp_peerid_t::build_random();
	ntudp_npos_res_t	npos_res	= ntudp_npos_res_t::get_default();
	// setup ntudp_profile_t to disable ntudp_npos_watch_t - leaving the original npos_res for ever
	ntudp_profile_t	profile;
	profile.npos_watch().eval_trigger(delaygen_regular_arg_t().first_delay(delay_t::ALWAYS));
	ntudp_peer->set_profile(profile);
	// Start the ntudp_peer_t
	return ntudp_peer->start(ipport_aview_t::ANY_IP4, npos_res, local_peerid);
}

/** \brief Just a short cut to start the ntudp_peer_t as if it was natsym and non inetreach
 * 
 * - NOTE: it should be used only for nunit stuff
 */
ntudp_err_t	ntudp_nunit_peer_start_natsym(ntudp_peer_t *ntudp_peer)		throw()
{
	ntudp_peerid_t		local_peerid	= ntudp_peerid_t::build_random();
	ntudp_npos_res_t	npos_res;
	npos_res.inetreach(false).natted(true).natsym(true).natlback(false);
	// sanity check - the ntudp_npos_res_t MUST be completed
	DBG_ASSERT( npos_res.completed() );

	// setup ntudp_profile_t to disable ntudp_npos_watch_t - leaving the original npos_res for ever
	ntudp_profile_t	profile;
	profile.npos_watch().eval_trigger(delaygen_regular_arg_t().first_delay(delay_t::ALWAYS));
	ntudp_peer->set_profile(profile);
	
	// Start the ntudp_peer_t
	return ntudp_peer->start(ipport_aview_t::ANY_IP4, npos_res, local_peerid);	
}

/** \brief Just a short cut to start the ntudp_peer_t as if it was non inetreach, natted and non natlsym
 * 
 * - NOTE: it should be used only for nunit stuff
 */
ntudp_err_t	ntudp_nunit_peer_start_non_natsym(ntudp_peer_t *ntudp_peer)	throw()
{
	ntudp_peerid_t		local_peerid	= ntudp_peerid_t::build_random();
	ntudp_npos_res_t	npos_res;
	npos_res.inetreach(false).natted(true).natsym(false).natlback(false);
	// sanity check - the ntudp_npos_res_t MUST be completed
	DBG_ASSERT( npos_res.completed() );

	// setup ntudp_profile_t to disable ntudp_npos_watch_t - leaving the original npos_res for ever
	ntudp_profile_t	profile;
	profile.npos_watch().eval_trigger(delaygen_regular_arg_t().first_delay(delay_t::ALWAYS));
	ntudp_peer->set_profile(profile);
	
	// Start the ntudp_peer_t
	return ntudp_peer->start(ipport_aview_t::ANY_IP4, npos_res, local_peerid);	
}

NEOIP_NAMESPACE_END




