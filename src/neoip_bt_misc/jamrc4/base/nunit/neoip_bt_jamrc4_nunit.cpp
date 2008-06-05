/*! \file
    \brief Definition of the unit test for the \ref bt_jamrc4_t

*/

/* system include */
/* local include */
#include "neoip_bt_jamrc4_nunit.hpp"
#include "neoip_bt_jamrc4_itor.hpp"
#include "neoip_bt_jamrc4_resp.hpp"
#include "neoip_bt_jamrc4_full.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_jamrc4_testclass_t::cnx_nojam2nojam(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	// log to debug
	KLOG_DBG("enter");
	
	// build bt_jamrc4_profile_t
	bt_jamrc4_profile_t	profile;
	profile.itor_type_default	( bt_jamrc4_type_t::NOJAM );
	profile.resp_type_accept_arr	( bt_jamrc4_type_arr_t()
						.append(bt_jamrc4_type_t::NOJAM)
					);
	
	// launch a do_cnx using those parameters
	nunit_res	= do_cnx(profile);
	if( nunit_res.error().failed() )	return nunit_res;
	
	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_jamrc4_testclass_t::cnx_dojam2dojam(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	// log to debug
	KLOG_DBG("enter");

	// build bt_jamrc4_profile_t
	bt_jamrc4_profile_t	profile;
	profile.itor_type_default	( bt_jamrc4_type_t::DOJAM );
	profile.resp_type_accept_arr	( bt_jamrc4_type_arr_t()
						.append(bt_jamrc4_type_t::NOJAM)
						.append(bt_jamrc4_type_t::DOJAM)
					);
	
	// launch a do_cnx using those parameters
	nunit_res	= do_cnx(profile);
	if( nunit_res.error().failed() )	return nunit_res;
	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_jamrc4_testclass_t::cnx_nojam2dojam(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");
	// build bt_jamrc4_profile_t
	bt_jamrc4_profile_t	profile;
	profile.itor_type_default	( bt_jamrc4_type_t::NOJAM );
	profile.resp_type_accept_arr	( bt_jamrc4_type_arr_t()
						.append(bt_jamrc4_type_t::DOJAM)
					);
	
	// create the bt_jamrc4_resp_t
	bt_jamrc4_resp_t*	jamrc4_resp;
	jamrc4_resp	= nipmem_new bt_jamrc4_resp_t();
	bt_err		= jamrc4_resp->profile(profile).start();
	NUNIT_ASSERT( bt_err.succeed() );
	
	// create a fake packet which is supposed to come from bt_jamrc4_itor_t in NOJAM
	// - NOTE: it MUST be equal to and greater than bt_jamrc4_type_t::DOJAM header
	pkt_t	pkt_inp("dummy data itor to resp and other blablablabla");

	// notify the pkt_inp to jamrc4_resp
	bt_jamrc4_full_t *	resp_jamrc4_full;
	bt_err = jamrc4_resp->notify_recved_data(pkt_inp, &resp_jamrc4_full);
	NUNIT_ASSERT( resp_jamrc4_full == NULL );
	NUNIT_ASSERT( jamrc4_resp->xmit_buffer().empty() );
	// it SHOULD fails, as it is NOT bt_jamrc4_type_t::DOJAM header
	NUNIT_ASSERT( bt_err.failed() );


	// delete jamrc4_resp_t
	nipmem_zdelete	jamrc4_resp;	
	// delete resp_jamrc4_full if needed
	nipmem_zdelete	resp_jamrc4_full;
	
	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_jamrc4_testclass_t::cnx_nojam2anyjam(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");
	
	// build bt_jamrc4_profile_t
	bt_jamrc4_profile_t	profile;
	profile.itor_type_default	( bt_jamrc4_type_t::NOJAM );
	profile.resp_type_accept_arr	( bt_jamrc4_type_arr_t()
						.append(bt_jamrc4_type_t::NOJAM)
						.append(bt_jamrc4_type_t::DOJAM)
					);
	
	// create the bt_jamrc4_resp_t
	bt_jamrc4_resp_t*	jamrc4_resp;
	jamrc4_resp	= nipmem_new bt_jamrc4_resp_t();
	bt_err		= jamrc4_resp->profile(profile).start();
	NUNIT_ASSERT( bt_err.succeed() );
	

	// create a fake packet which is supposed to come from bt_jamrc4_itor_t in NOJAM
	// - NOTE: it MUST be equal to and greater than bt_jamrc4_type_t::DOJAM header
	datum_t		i2r_data("dummy data itor to resp and other blablablabla");	

	// notify the pkt_inp to jamrc4_resp
	bt_jamrc4_full_t *	resp_jamrc4_full;
	bt_err = jamrc4_resp->notify_recved_data(pkt_t(i2r_data), &resp_jamrc4_full);

	// it SHOULD succeed, as it is jamrc4_resp accept bt_jamrc4_type_t::NOJAM
	NUNIT_ASSERT( bt_err.succeed() );
	// resp_jamrc4_full MUST have been notified
	NUNIT_ASSERT( resp_jamrc4_full );
	// bt_jamrc4_resp_t MUST NOT send anything at this point
	NUNIT_ASSERT( jamrc4_resp->xmit_buffer().empty() );


	// pass no data to resp_jamrc4_full - it should return i2r_data
	pkt_t	pkt_out;
	bt_err	= resp_jamrc4_full->notify_recved_data(pkt_t(), pkt_out);
	NUNIT_ASSERT( bt_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// sanity check - pkt_out MUST be equal to i2r_data
	NUNIT_ASSERT( pkt_out.to_datum() == i2r_data);
	

	// delete jamrc4_resp_t
	nipmem_zdelete	jamrc4_resp;	
	// delete resp_jamrc4_full if needed
	nipmem_zdelete	resp_jamrc4_full;

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_jamrc4_testclass_t::do_cnx(const bt_jamrc4_profile_t &profile)throw()
{
	bt_jamrc4_full_t *	resp_jamrc4_full	= NULL;
	bt_jamrc4_full_t *	itor_jamrc4_full	= NULL;
	pkt_t			pkt_inp, pkt_out;
	bt_err_t		bt_err;
	// log to debug
	KLOG_DBG("enter");
	
	/*************** create itor and resp	*******************************/
	// create the bt_jamrc4_resp_t
	bt_jamrc4_resp_t*	jamrc4_resp;
	jamrc4_resp	= nipmem_new bt_jamrc4_resp_t();
	bt_err		= jamrc4_resp->profile(profile).start();
	NUNIT_ASSERT( bt_err.succeed() );
	// create the bt_jamrc4_itor_t
	bt_jamrc4_itor_t*	jamrc4_itor;
	jamrc4_itor	= nipmem_new bt_jamrc4_itor_t();
	bt_err		= jamrc4_itor->profile(profile).start();
	NUNIT_ASSERT( bt_err.succeed() );
	
	// log to debug
	KLOG_DBG("itor xmit_buffer=" << jamrc4_itor->xmit_buffer());
	KLOG_DBG("resp xmit_buffer=" << jamrc4_resp->xmit_buffer());

	// loop until itor *and* resp consider themselves connected
	while( !itor_jamrc4_full || !resp_jamrc4_full ){
		// pass data from resp to itor - if ito_jamrc4_full doesnt exist
		// - first iteration, pkt_inp is empty and start jamrc4_itor_t
		if( !itor_jamrc4_full ){
			// notify a empty pkt_t to jamrc4_itor_t to get it started
			bt_err = jamrc4_itor->notify_recved_data(pkt_inp, &itor_jamrc4_full);
			NUNIT_ASSERT( bt_err.succeed() );
			KLOG_DBG("pkt_inp="	<< pkt_inp);
			KLOG_DBG("xmit_buffer=" << jamrc4_itor->xmit_buffer());
			// copy jamrc4_itor->xmit_buffer into pkt_inp - to pass it to resp
			pkt_inp = jamrc4_itor->xmit_buffer();
			// flush jamrc4_itor->xmit_buffer();
			jamrc4_itor->xmit_buffer().head_free( jamrc4_itor->xmit_buffer().length() );		
		}
		
		// pass data from itor to resp - if resp_jamrc4_full doesnt exist
		if( !resp_jamrc4_full ){
			bt_err = jamrc4_resp->notify_recved_data(pkt_inp, &resp_jamrc4_full);
			NUNIT_ASSERT( bt_err.succeed() );
			KLOG_DBG("pkt_inp="	<< pkt_inp);
			KLOG_DBG("xmit_buffer=" << jamrc4_resp->xmit_buffer());
			// copy jamrc4_resp->xmit_buffer into pkt_inp - to pass it to itor
			pkt_inp = jamrc4_resp->xmit_buffer();
			// flush jamrc4_resp->xmit_buffer();
			jamrc4_resp->xmit_buffer().head_free( jamrc4_resp->xmit_buffer().length() );		
		}
	} 
	// delete jamrc4_itor_t
	nipmem_zdelete jamrc4_itor;
	// delete jamrc4_resp_t
	nipmem_zdelete jamrc4_resp;

	// TODO test the initpkt in the itor->resp
	// TODO test for remaining data at the end of cnx establishment
	
	/*************** Send data from itor_jamrc4_full to resp_jamrc4_full ******/
	datum_t		i2r_data("dummy data itor to resp");	
	pkt_inp		= pkt_t(i2r_data);
	bt_err	= itor_jamrc4_full->notify_toxmit_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( bt_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// swap pkt_inp and pkt_out
	pkt_inp	= pkt_t(pkt_out); pkt_out = pkt_t();
	// pass data from itor_jamrc4_full to resp_jamrc4_full
	bt_err	= resp_jamrc4_full->notify_recved_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( bt_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// sanity check - pkt_out MUST be equal to i2r_data
	NUNIT_ASSERT( pkt_out.to_datum() == i2r_data);


	/*************** Send data from resp_jamrc4_full to itor_jamrc4_full ******/
	datum_t		r2i_data("dummy data resp to itor");	
	pkt_inp		= pkt_t(r2i_data);
	bt_err	= resp_jamrc4_full->notify_toxmit_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( bt_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// swap pkt_inp and pkt_out
	pkt_inp	= pkt_t(pkt_out); pkt_out = pkt_t();
	// pass data from resp_jamrc4_full to itor_jamrc4_full
	bt_err	= itor_jamrc4_full->notify_recved_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( bt_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// sanity check - pkt_out MUST be equal to r2i_data
	NUNIT_ASSERT( pkt_out.to_datum() == r2i_data);
	
	// delete itor_jamrc4_full if needed
	nipmem_zdelete	itor_jamrc4_full;
	// delete resp_jamrc4_full if needed
	nipmem_zdelete	resp_jamrc4_full;
	
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

