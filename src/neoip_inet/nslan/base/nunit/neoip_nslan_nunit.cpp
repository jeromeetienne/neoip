/*! \file
    \brief unit test for the nslan_t unit testt

*/

/* system include */
/* local include */
#include "neoip_nslan_nunit.hpp"
#include "neoip_nslan.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nslan_testclass_t::nslan_testclass_t()	throw()
{
	// zero some field
	nslan_listener	= NULL;
	nslan_peer	= NULL;
	nslan_publish	= NULL;
	nslan_query	= NULL;
}

/** \brief Destructor
 */
nslan_testclass_t::~nslan_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	nslan_testclass_t::neoip_nunit_testclass_init()	throw()
{
	inet_err_t inet_err;
	// log to debug
	KLOG_DBG("enter");
	// sanity check
	DBG_ASSERT( !nslan_listener );
	
	// create a nslan_listener_t
	nslan_listener	= nipmem_new nslan_listener_t();
	inet_err	= nslan_listener->start("255.255.255.255:4000");
	if( inet_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, inet_err.to_string());
	
	// create a nslan_peer_t
	nslan_realmid_t	realmid	= "nunit_realmid";
	nslan_peer	= nipmem_new nslan_peer_t(nslan_listener, "nunit_realm");

	// create a nslan_rec_t
	nslan_rec_t	nslan_rec("nunit_keyid", datum_t("bonjour", 7), delay_t::from_sec(20));

	// publish the record
	nslan_publish	= nipmem_new nslan_publish_t(nslan_peer, nslan_rec);
	

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	nslan_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// delete the nslan_query
	nipmem_zdelete	nslan_query;
	// delete the nslan_publish
	nipmem_zdelete	nslan_publish;
	// delete the nslan_peer
	nipmem_zdelete	nslan_peer;
	// delete the nslan_listener
	nipmem_zdelete	nslan_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	nslan_testclass_t::basic_query(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nslan_keyid_t	keyid		= "nunit_keyid";
	inet_err_t	inet_err;
	// do a query on the pusblished record
	nslan_query	= nipmem_new nslan_query_t(nslan_peer, keyid, delay_t::from_sec(30), this,NULL);
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    nslan_query_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when nslan_query_t has an event to report
 */
bool nslan_testclass_t::neoip_nslan_query_cb(void *cb_userptr, nslan_query_t &cb_nslan_query
					, const nslan_event_t &nslan_event)	throw()
{
	nunit_res_t	nunit_res;
	// log to debug
	KLOG_ERR("enter nslan_event=" << nslan_event);

	// handle the event depending on its value
	switch(nslan_event.get_value()){
	case nslan_event_t::TIMEDOUT:
			// if the nslan_query timeout, report an negative
			nunit_res	= NUNIT_RES_ERROR;
			break;
	case nslan_event_t::GOT_RECORD:
			// if the received record is NOT the expected one, continue the query
			if( nslan_event.get_got_record(NULL) != nslan_publish->get_record() )
				return true;
			// if the recieved record IS the expected one, report a positive
			nunit_res	= NUNIT_RES_OK;
			break;
	default:	DBG_ASSERT( 0 );
	}
	
	// delete the nslan_query and mark it unused
	nipmem_delete	nslan_query;
	nslan_query	= NULL;

	// notify the caller of the end of this testfunction
	nunit_ftor(nunit_res);	
	// return tokeep
	return false;	
}

NEOIP_NAMESPACE_END
