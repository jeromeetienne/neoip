/*! \file
    \brief Definition of the unit test for the \ref ntudp_npos_eval_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_eval_nunit.hpp"
#include "neoip_ntudp_npos_eval.hpp"
#include "neoip_ntudp_peer.hpp"
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
ntudp_npos_eval_testclass_t::ntudp_npos_eval_testclass_t()	throw()
{
	// zero some field
	ntudp_peer1	= NULL;
	ntudp_peer2	= NULL;
	ntudp_peer3	= NULL;
	ntudp_npos_eval	= NULL;
}

/** \brief Destructor
 */
ntudp_npos_eval_testclass_t::~ntudp_npos_eval_testclass_t()	throw()
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
nunit_err_t	ntudp_npos_eval_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_DBG("enter");
	
	// init the ntudp_peer1
	DBG_ASSERT( !ntudp_peer1 );
	ntudp_peer1	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_default(ntudp_peer1);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// init the ntudp_peer2
	DBG_ASSERT( !ntudp_peer2 );
	ntudp_peer2	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(ntudp_peer2, "127.0.0.1:4001");
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());


	// init the ntudp_peer3
	DBG_ASSERT( !ntudp_peer3 );
	ntudp_peer3	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(ntudp_peer3, "127.0.0.1:4002");
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ntudp_npos_eval_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// delete the ntudp_npos_eval
	if( ntudp_npos_eval ){
		nipmem_delete	ntudp_npos_eval;
		ntudp_npos_eval	= NULL;
	}
	
	// delete the ntudp_peer1
	if( ntudp_peer1 ){
		nipmem_delete	ntudp_peer1;
		ntudp_peer1	= NULL;
	}	
	// delete the ntudp_peer2
	if( ntudp_peer2 ){
		nipmem_delete	ntudp_peer2;
		ntudp_peer2	= NULL;
	}
	// delete the ntudp_peer3
	if( ntudp_peer3 ){
		nipmem_delete	ntudp_peer3;
		ntudp_peer3	= NULL;
	}	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief General test
 */
nunit_res_t	ntudp_npos_eval_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_ERR("enter");
	
	// start the ntudp_npos_eval_t
	DBG_ASSERT( ntudp_npos_eval == NULL );
	ntudp_npos_eval= nipmem_new ntudp_npos_eval_t();
	ntudp_err	= ntudp_npos_eval->start( ntudp_peer1->pserver_pool()
						, ntudp_peer1->npos_server()
						, ntudp_peer1->listen_aview_cfg().pview()
						, this, NULL);
	NUNIT_ASSERT( ntudp_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified by \ref ntudp_npos_eval_t when to notify an event
 */
bool ntudp_npos_eval_testclass_t::neoip_ntudp_npos_eval_cb(void *cb_userptr
					, ntudp_npos_eval_t &cb_ntudp_npos_eval
					, const ntudp_npos_res_t &ntudp_npos_res)	throw()
{
	// log the result
	KLOG_ERR("ntudp_npos_res=" << ntudp_npos_res);
	
#if 1
	// if the ntudp_npos_eval_t failed, report the event
	bool	tokeep;
	if( !ntudp_npos_res.is_null() )	tokeep = nunit_ftor(NUNIT_RES_OK);
	else				tokeep = nunit_ftor(NUNIT_RES_ERROR);	
	if( !tokeep )	return false;
#endif

	// delete the object
	nipmem_delete ntudp_npos_eval;
	ntudp_npos_eval	= false;
	// return 'dontkeep'
	return false;
}

NEOIP_NAMESPACE_END

