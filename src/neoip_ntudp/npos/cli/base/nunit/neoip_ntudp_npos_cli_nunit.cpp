/*! \file
    \brief Definition of the unit test for the \ref ntudp_npos_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_cli_nunit.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_npos_saddrecho.hpp"
#include "neoip_ntudp_npos_inetreach.hpp"
#include "neoip_ntudp_npos_natlback.hpp"
#include "neoip_ntudp_npos_event.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_nunit_helper.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_npos_cli_testclass_t::ntudp_npos_cli_testclass_t()	throw()
{
	// zero some field
	ntudp_peer1		= NULL;
	ntudp_peer2		= NULL;
	ntudp_npos_saddrecho	= NULL;
	ntudp_npos_inetreach	= NULL;
	ntudp_npos_natlback	= NULL;
}

/** \brief Destructor
 */
ntudp_npos_cli_testclass_t::~ntudp_npos_cli_testclass_t()	throw()
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
nunit_err_t	ntudp_npos_cli_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_DBG("enter");
	// sanity check
	DBG_ASSERT( !ntudp_npos_saddrecho );
	DBG_ASSERT( !ntudp_npos_inetreach );
	DBG_ASSERT( !ntudp_npos_natlback  );
	DBG_ASSERT( !ntudp_peer1 );
	DBG_ASSERT( !ntudp_peer2 );

	// init the ntudp_peer1
	ntudp_peer1	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(ntudp_peer1, "127.0.0.1:4000");	
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// init the ntudp_peer2
	ntudp_peer2	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(ntudp_peer2, "127.0.0.1:4001");	
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ntudp_npos_cli_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the ntudp_npos_saddrecho_t
	if( ntudp_npos_saddrecho ){
		nipmem_delete	ntudp_npos_saddrecho;
		ntudp_npos_saddrecho	= NULL;
	}
	// delete the ntudp_npos_inetreach_t
	if( ntudp_npos_inetreach ){
		nipmem_delete	ntudp_npos_inetreach;
		ntudp_npos_inetreach	= NULL;
	}
	// delete the ntudp_npos_natlback_t
	if( ntudp_npos_natlback ){
		nipmem_delete	ntudp_npos_natlback;
		ntudp_npos_natlback	= NULL;
	}
	// delete the ntudp_peer2
	if( ntudp_peer2 ){
		nipmem_delete	ntudp_peer2;
		ntudp_peer2		= NULL;
	}	
	// delete the ntudp_peer1
	if( ntudp_peer1 ){
		nipmem_delete	ntudp_peer1;
		ntudp_peer1		= NULL;
	}	
}

/** \brief notify a delayed nunit_res_t
 */
bool ntudp_npos_cli_testclass_t::notify_nunit_res(const nunit_res_t &nunit_res)	throw()
{
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*this);
	// notify the caller
	bool tokeep = nunit_ftor(nunit_res);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test for ntudp_npos_saddrecho
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by ntudp_npos_saddrecho_t when the result is known
 */
bool ntudp_npos_cli_testclass_t::neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
					, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
					, const ntudp_npos_event_t &ntudp_npos_event)	throw() 
{
	bool	tokeep;
	// sanity check - the event MUST be cli_saddrecho_ok
	DBG_ASSERT( ntudp_npos_event.is_npos_saddrecho_ok() );
	// display the result
	KLOG_DBG("ntudp_npos_saddrecho notified ntudp_npos_event=" << ntudp_npos_event);

	// report the result
	if( ntudp_npos_event.is_fatal() )	tokeep	= notify_nunit_res(NUNIT_RES_ERROR);
	else					tokeep	= notify_nunit_res(NUNIT_RES_OK);
	if( !tokeep )	return false;

	// delete the client and mark it unused
	nipmem_delete	ntudp_npos_saddrecho;
	ntudp_npos_saddrecho	= NULL;
	// return 'dontkeep'	
	return false;
}

/** \brief Test the ntudp_npos_saddrecho_t
 */
nunit_res_t	ntudp_npos_cli_testclass_t::saddrecho(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// create the ntudp_npos_saddrecho
	ntudp_npos_saddrecho	= nipmem_new ntudp_npos_saddrecho_t();
	// setup the listen_addr and start the ntudp_npos_saddrecho_t
	ntudp_err_t	ntudp_err;
	ntudp_err	= ntudp_npos_saddrecho->start(ntudp_peer2->listen_addr_pview()
								, ntudp_peer1->pserver_pool()
								, this, NULL);
	NUNIT_ASSERT( ntudp_err.succeed() );
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test for ntudp_npos_inetreach
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by ntudp_npos_inetreach_t when the result is known
 */
bool ntudp_npos_cli_testclass_t::neoip_ntudp_npos_inetreach_event_cb(void *cb_userptr
					, ntudp_npos_inetreach_t &cb_ntudp_npos_inetreach
					, const ntudp_npos_event_t &ntudp_npos_event)	throw() 
{
	bool	tokeep;
	// sanity check - the event MUST be cli_inetreach_ok
	DBG_ASSERT( ntudp_npos_event.is_npos_inetreach_ok() );
	// display the result
	KLOG_DBG("ntudp_npos_inetreach notified ntudp_npos_event=" << ntudp_npos_event);

	// report the result
	if( ntudp_npos_event.is_fatal() )	tokeep	= notify_nunit_res(NUNIT_RES_ERROR);
	else					tokeep	= notify_nunit_res(NUNIT_RES_OK);
	if( !tokeep )	return false;

	// delete the client and mark it unused
	nipmem_delete	ntudp_npos_inetreach;
	ntudp_npos_inetreach	= NULL;	

	// return 'dontkeep'	
	return false;
}

/** \brief Test the ntudp_npos_inetreach_t
 */
nunit_res_t	ntudp_npos_cli_testclass_t::inetreach(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// create the ntudp_npos_inetreach
	ntudp_npos_inetreach	= nipmem_new ntudp_npos_inetreach_t();
	// setup the listen_addr and start the ntudp_npos_inetreach_t
	// - NOTE: here both ipport_addr_t points on the same server but in real usage
	//         they obviously points to different server
	ntudp_err_t	ntudp_err;
	ntudp_err	= ntudp_npos_inetreach->start(ntudp_peer2->listen_addr_pview()
						, ntudp_peer1->listen_addr_pview()
						, ntudp_peer1->npos_server()
						, ntudp_peer1->pserver_pool()						
						, this, NULL);
	NUNIT_ASSERT( ntudp_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test for ntudp_npos_natlback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by ntudp_npos_natlback_t when the result is known
 */
bool ntudp_npos_cli_testclass_t::neoip_ntudp_npos_natlback_event_cb(void *cb_userptr
					, ntudp_npos_natlback_t &cb_ntudp_npos_natlback
					, const ntudp_npos_event_t &ntudp_npos_event)	throw() 
{
	bool	tokeep;
	// sanity check - the event MUST be cli_natlback_ok
	DBG_ASSERT( ntudp_npos_event.is_npos_natlback_ok() );
	// display the result
	KLOG_ERR("ntudp_npos_natlback notified ntudp_npos_event=" << ntudp_npos_event);

	// report the result
	if( ntudp_npos_event.is_fatal() )	tokeep	= notify_nunit_res(NUNIT_RES_ERROR);
	else					tokeep	= notify_nunit_res(NUNIT_RES_OK);
	if( !tokeep )	return false;

	// delete the client and mark it unused
	nipmem_delete	ntudp_npos_natlback;
	ntudp_npos_natlback	= NULL;

	// return 'dontkeep'	
	return false;
}

/** \brief Test the ntudp_npos_natlback_t
 */
nunit_res_t	ntudp_npos_cli_testclass_t::natlback(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// create the ntudp_npos_natlback
	ntudp_npos_natlback	= nipmem_new ntudp_npos_natlback_t();
	// setup the listen_addr and start the ntudp_npos_natlback_t
	ntudp_err_t	ntudp_err;
	ntudp_err	= ntudp_npos_natlback->start(ntudp_peer2->listen_addr_pview()
							, ntudp_peer1->pserver_pool()
							, this, NULL);
	NUNIT_ASSERT( ntudp_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

NEOIP_NAMESPACE_END

