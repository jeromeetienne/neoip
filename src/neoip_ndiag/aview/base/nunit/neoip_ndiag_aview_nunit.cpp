/*! \file
    \brief unit test for the ndiag_aview_t unit testt

*/

/* system include */
/* local include */
#include "neoip_ndiag_aview_nunit.hpp"
#include "neoip_ndiag_aview.hpp"
#include "neoip_ndiag_err.hpp"
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
ndiag_aview_testclass_t::ndiag_aview_testclass_t()	throw()
{
	// zero some field
}

/** \brief Destructor
 */
ndiag_aview_testclass_t::~ndiag_aview_testclass_t()	throw()
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
nunit_err_t	ndiag_aview_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ndiag_aview_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the ndiag_aview_db
	while( !ndiag_aview_db.empty() )	nipmem_delete ndiag_aview_db.front();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	ndiag_aview_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ndiag_err_t ndiag_err;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - ndiag_aview_db MUST be empty
	DBG_ASSERT( ndiag_aview_db.empty() );
	
	// create multiple ndiag_aview_t. 
	//- important because some upnp routers fails with simulatenous requests
	for(size_t i = 0; i < 4; i++){
		// determine the ipport_aview_t for the ndiag_aview_t
		uint16_t	port_lview	= 1234 + i;
		ipport_aview_t	ipport_aview_init(std::string(":")+OSTREAMSTR(port_lview)
							, ipport_addr_t());
		
		// create a ndiag_aview_t
		ndiag_aview_t *	ndiag_aview;
		ndiag_aview	= nipmem_new ndiag_aview_t();
		ndiag_err	= ndiag_aview->start(ipport_aview_init, upnp_sockfam_t::TCP
						, "neoip ndiag_aview_nunit", this, NULL);
		if(ndiag_err.failed())	return nunit_err_t(nunit_err_t::ERROR, ndiag_err.to_string());
		
		//KLOG_ERR("ndiag_aview->ipport_pview_curr()=" << ndiag_aview->ipport_pview_curr());
		// put this ndiag_aview_t into ndiag_aview_db
		ndiag_aview_db.push_back(ndiag_aview);
	}
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ndiag_aview_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ndiag_aview_t to notify an event
 */
bool 	ndiag_aview_testclass_t::neoip_ndiag_aview_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
						, const ipport_addr_t &new_ipport_pview)	throw()
{
	ndiag_aview_t *	ndiag_aview	= &cb_ndiag_aview;
	// log to debug
	KLOG_ERR("enter new_ipport_pview=" << new_ipport_pview);
	KLOG_ERR("ipport_lview=" << ndiag_aview->ipport_lview() ); 
	KLOG_ERR("ipport_pview=" << ndiag_aview->ipport_pview() ); 
	
	// if this ndiag_aview_t reported a fully qualified ipport_addr_t, delete it
	if( ndiag_aview->ipport_pview().is_fully_qualified() ){
		ndiag_aview_db.remove(ndiag_aview);
		nipmem_zdelete	ndiag_aview;
	}
	
	// if the ndiag_aview_db is now empty, notify NUNIT_RES_OK
	if( ndiag_aview_db.empty() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// if ndiag_aview has not been deleted, return tokeep
	if( ndiag_aview )	return true;
	// return donkeep
	return false;
}

NEOIP_NAMESPACE_END
