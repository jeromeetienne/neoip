/*! \file
    \brief Definition of the unit test for the \ref bt_jamstd_t

\par TODO
- have a full exchange done
- up to resp send hello world and itor receive it successfully



*/

/* system include */
/* local include */
#include "neoip_bt_jamstd_nunit.hpp"
#include "neoip_bt_jamstd_itor.hpp"
#include "neoip_bt_jamstd_resp.hpp"
#include "neoip_bt_jamstd_full.hpp"
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
nunit_res_t	bt_jamstd_testclass_t::connection_esta(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_id_t			cnx_shsecret		= "supersecret";
	bt_jamstd_full_t *	resp_jamstd_full	= NULL;
	bt_jamstd_full_t *	itor_jamstd_full	= NULL;
	pkt_t			pkt_inp, pkt_out;
	bt_err_t		bt_err;
	// log to debug
	KLOG_DBG("enter");
	
	/*************** create itor and resp	*******************************/
	// create the bt_jamstd_resp_t
	bt_jamstd_resp_t*	jamstd_resp;
	jamstd_resp	= nipmem_new bt_jamstd_resp_t();
	bt_err		= jamstd_resp->start(cnx_shsecret);
	NUNIT_ASSERT( bt_err.succeed() );
	// create the bt_jamstd_itor_t
	bt_jamstd_itor_t*	jamstd_itor;
	jamstd_itor	= nipmem_new bt_jamstd_itor_t();
	bt_err		= jamstd_itor->start(cnx_shsecret);
	NUNIT_ASSERT( bt_err.succeed() );
	
	// log to debug
	KLOG_DBG("itor xmit_buffer=" << jamstd_itor->xmit_buffer());
	KLOG_DBG("resp xmit_buffer=" << jamstd_resp->xmit_buffer());

	// loop until itor *and* resp consider themselves connected
	while( !itor_jamstd_full || !resp_jamstd_full ){
		// pass data from resp to itor - if ito_jamstd_full doesnt exist
		// - first iteration, pkt_inp is empty and start jamstd_itor_t
		if( !itor_jamstd_full ){
			// notify a empty pkt_t to jamstd_itor_t to get it started
			bt_err = jamstd_itor->notify_recved_data(pkt_inp, &itor_jamstd_full);
			NUNIT_ASSERT( bt_err.succeed() );
			KLOG_DBG("pkt_inp="	<< pkt_inp);
			KLOG_DBG("xmit_buffer=" << jamstd_itor->xmit_buffer());
			// copy jamstd_itor->xmit_buffer into pkt_inp - to pass it to resp
			pkt_inp = jamstd_itor->xmit_buffer();
			// flush jamstd_itor->xmit_buffer();
			jamstd_itor->xmit_buffer().head_free( jamstd_itor->xmit_buffer().length() );		
		}
		
		// pass data from itor to resp - if resp_jamstd_full doesnt exist
		if( !resp_jamstd_full ){
			bt_err = jamstd_resp->notify_recved_data(pkt_inp, &resp_jamstd_full);
			NUNIT_ASSERT( bt_err.succeed() );
			KLOG_DBG("pkt_inp="	<< pkt_inp);
			KLOG_DBG("xmit_buffer=" << jamstd_resp->xmit_buffer());
			// copy jamstd_resp->xmit_buffer into pkt_inp - to pass it to itor
			pkt_inp = jamstd_resp->xmit_buffer();
			// flush jamstd_resp->xmit_buffer();
			jamstd_resp->xmit_buffer().head_free( jamstd_resp->xmit_buffer().length() );		
		}
	} 
	// delete jamstd_itor_t
	nipmem_zdelete jamstd_itor;
	// delete jamstd_resp_t
	nipmem_zdelete jamstd_resp;

	// TODO test the initpkt in the itor->resp
	// TODO test for remaining data at the end of cnx establishment
	
	/*************** Send data from itor_jamstd_full to resp_jamstd_full ******/
	datum_t		i2r_data("dummy data itor to resp");	
	pkt_inp		= pkt_t(i2r_data);
	bt_err	= itor_jamstd_full->notify_toxmit_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( bt_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// swap pkt_inp and pkt_out
	pkt_inp	= pkt_t(pkt_out); pkt_out = pkt_t();
	// pass data from itor_jamstd_full to resp_jamstd_full
	bt_err	= resp_jamstd_full->notify_recved_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( bt_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// sanity check - pkt_out MUST be equal to i2r_data
	NUNIT_ASSERT( pkt_out.to_datum() == i2r_data);


	/*************** Send data from resp_jamstd_full to itor_jamstd_full ******/
	datum_t		r2i_data("dummy data resp to itor");	
	pkt_inp		= pkt_t(r2i_data);
	bt_err	= resp_jamstd_full->notify_toxmit_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( bt_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// swap pkt_inp and pkt_out
	pkt_inp	= pkt_t(pkt_out); pkt_out = pkt_t();
	// pass data from resp_jamstd_full to itor_jamstd_full
	bt_err	= itor_jamstd_full->notify_recved_data(pkt_inp, pkt_out);
	NUNIT_ASSERT( bt_err.succeed() );
	// log to debug
	KLOG_DBG("pkt_out=" << pkt_out);
	// sanity check - pkt_out MUST be equal to r2i_data
	NUNIT_ASSERT( pkt_out.to_datum() == r2i_data);
	
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
nunit_res_t	bt_jamstd_testclass_t::resp_detect_plainbt(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_id_t		cnx_shsecret	= "supersecret";
	bt_err_t	bt_err;
	bt_jamstd_full_t*	resp_jamstd_full;
	// log to debug
	KLOG_DBG("enter");
	
	// create the bt_jamstd_resp_t
	bt_jamstd_resp_t*	jamstd_resp;
	jamstd_resp	= nipmem_new bt_jamstd_resp_t();
	bt_err		= jamstd_resp->start(cnx_shsecret);
	NUNIT_ASSERT( bt_err.succeed() );

	/*************** test sending data from itor to resp	***************/
	pkt_t	test_pkt(datum_t("\x13""BitTorrent protocol424242424242"));
	// pass the jamstd_itor->xmit_buffer() to bt_jamstd_resp_t
	// - NOTE: this pass itor cnxauth to resp
	bt_err	= jamstd_resp->notify_recved_data(test_pkt, &resp_jamstd_full);
	NUNIT_ASSERT( bt_err.failed() );
	NUNIT_ASSERT( bt_err.reason() == "PLAINBT SIGNATURE DETECTED");
	NUNIT_ASSERT( resp_jamstd_full == NULL );
	
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

