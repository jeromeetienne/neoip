/*! \file
    \brief Definition of the unit test for the \ref socket_client_t

\par Brief Description
this nunit performs a 'in-memory' slay connection establishement between
slay_itor_t and slay_resp_t. then exchange data in both direction thru
the 2 slay_full_t created.

*/

/* system include */
/* local include */
#include "neoip_slay_nunit.hpp"
#include "neoip_slay.hpp"
#include "neoip_slay_profile_tls.hpp"
#include "neoip_slay_profile_btjamstd.hpp"
#include "neoip_slay_profile_btjamrc4.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function - perform a slay connection 'in memory'
 */
nunit_res_t	slay_testclass_t::standalone_tls(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	// log to debug
	KLOG_DBG("enter");
	
	// build the slay_domain_t profile
	slay_profile_t	slay_profile(slay_domain_t::TLS);
	// build the itor_arg and resp_arg
	slay_itor_arg_t	itor_arg = slay_itor_arg_t().domain(slay_domain_t::TLS)
					.profile(slay_profile);
	slay_resp_arg_t	resp_arg = slay_resp_arg_t().domain(slay_domain_t::TLS)
					.profile(slay_profile);
	// launch a standalone_cnx using those parameters
	nunit_res	= standalone_cnx(resp_arg, itor_arg);
	if( nunit_res.error().failed() )	return nunit_res;

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function - perform a slay connection 'in memory'
 */
nunit_res_t	slay_testclass_t::standalone_btjamstd(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	// log to debug
	KLOG_DBG("enter");

	// build the slay_domain_t profile
	slay_profile_t		slay_profile(slay_domain_t::BTJAMSTD);
	slay_profile_btjamstd_t	profile_dom	= slay_profile_btjamstd_t::from_slay(slay_profile);
	profile_dom.shared_secret("hello, world!");	
	// build the itor_arg and resp_arg
	slay_itor_arg_t	itor_arg = slay_itor_arg_t().domain(slay_domain_t::BTJAMSTD)
					.profile(slay_profile);
	slay_resp_arg_t	resp_arg = slay_resp_arg_t().domain(slay_domain_t::BTJAMSTD)
					.profile(slay_profile);
	// launch a standalone_cnx using those parameters
	nunit_res	= standalone_cnx(resp_arg, itor_arg);
	if( nunit_res.error().failed() )	return nunit_res;

	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function - perform a slay connection 'in memory'
 */
nunit_res_t	slay_testclass_t::standalone_btjamrc4(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	// log to debug
	KLOG_DBG("enter");

	// build the slay_domain_t profile
	slay_profile_t		slay_profile(slay_domain_t::BTJAMRC4);
	// build the itor_arg and resp_arg
	slay_itor_arg_t	itor_arg = slay_itor_arg_t().domain(slay_domain_t::BTJAMRC4)
					.profile(slay_profile);
	slay_resp_arg_t	resp_arg = slay_resp_arg_t().domain(slay_domain_t::BTJAMRC4)
					.profile(slay_profile);
	// launch a standalone_cnx using those parameters
	nunit_res	= standalone_cnx(resp_arg, itor_arg);
	if( nunit_res.error().failed() )	return nunit_res;

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function - perform a slay connection 'in memory'
 */
nunit_res_t	slay_testclass_t::standalone_cnx(const slay_resp_arg_t &resp_arg
						, const slay_itor_arg_t &itor_arg) throw()
{
	slay_full_t *	itor_slay_full	= NULL;
	slay_full_t *	resp_slay_full	= NULL;
	slay_err_t	slay_err;
	pkt_t		pkt_inp, pkt_out;
	// log to debug
	KLOG_DBG("enter");

	// start a slay_resp_t
	slay_resp_t *	slay_resp;
	slay_resp	= nipmem_new slay_resp_t();
	slay_err	= slay_resp->start(resp_arg);
	NUNIT_ASSERT( slay_err.succeed() );

	// start a slay_itor_t
	slay_itor_t *	slay_itor;
	slay_itor	= nipmem_new slay_itor_t();
	slay_err	= slay_itor->start(itor_arg);
	NUNIT_ASSERT( slay_err.succeed() );
	
	// loop until itor *and* resp consider themselves connected
	while( !itor_slay_full || !resp_slay_full ){
		// pass data from resp to itor - if itor_slay_full doenst exist
		// - first iteration, pkt_inp is empty and start slay_itor_t
		if( !itor_slay_full ){
			// notify a empty pkt_t to slay_itor_t to get it started
			slay_err = slay_itor->notify_recved_data(pkt_inp, &itor_slay_full);
			NUNIT_ASSERT( slay_err.succeed() );
			KLOG_DBG("pkt_inp="	<< pkt_inp);
			KLOG_DBG("xmit_buffer=" << slay_itor->xmit_buffer());
			// copy slay_itor->xmit_buffer into pkt_inp - to pass it to resp
			pkt_inp = slay_itor->xmit_buffer();
			// flush slay_itor->xmit_buffer();
			slay_itor->xmit_buffer().head_free( slay_itor->xmit_buffer().length() );		
		}
		
		// pass data from itor to resp - if resp_slay_full doesnt exist
		if( !resp_slay_full ){
			slay_err = slay_resp->notify_recved_data(pkt_inp, &resp_slay_full);
			NUNIT_ASSERT( slay_err.succeed() );
			KLOG_DBG("pkt_inp="	<< pkt_inp);
			KLOG_DBG("xmit_buffer=" << slay_resp->xmit_buffer());
			// copy slay_resp->xmit_buffer into pkt_inp - to pass it to itor
			pkt_inp = slay_resp->xmit_buffer();
			// flush slay_resp->xmit_buffer();
			slay_resp->xmit_buffer().head_free( slay_resp->xmit_buffer().length() );		
		}
	} 
	// delete slay_itor_t
	nipmem_zdelete slay_itor;
	// delete slay_resp_t
	nipmem_zdelete slay_resp;

	/*************** Send data from itor_slay_full to resp_slay_full ******/
	datum_t		i2r_data("dummy data itor to resp");	
	pkt_inp		= pkt_t(i2r_data);
	slay_err	= itor_slay_full->notify_toxmit_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( slay_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// swap pkt_inp and pkt_out
	pkt_inp	= pkt_t(pkt_out); pkt_out = pkt_t();
	// pass data from itor_slay_full to resp_slay_full
	slay_err	= resp_slay_full->notify_recved_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( slay_err.succeed() );
	// sanity check - pkt_out MUST be equal to i2r_data
	NUNIT_ASSERT( pkt_out.to_datum() == i2r_data);


	/*************** Send data from resp_slay_full to itor_slay_full ******/
	datum_t		r2i_data("dummy data resp to itor");	
	pkt_inp		= pkt_t(r2i_data);
	slay_err	= resp_slay_full->notify_toxmit_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( slay_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// swap pkt_inp and pkt_out
	pkt_inp	= pkt_t(pkt_out); pkt_out = pkt_t();
	// pass data from resp_slay_full to itor_slay_full
	slay_err	= itor_slay_full->notify_recved_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( slay_err.succeed() );
	// sanity check - pkt_out MUST be equal to r2i_data
	NUNIT_ASSERT( pkt_out.to_datum() == r2i_data);
	
	// delete slay_full_t
	nipmem_zdelete itor_slay_full;
	// delete slay_full_t
	nipmem_zdelete resp_slay_full;
	// return a delayed nunit_res_t
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

