/*! \file
    \brief unit test for the nudp_t unit testt

*/

/* system include */
/* local include */
#include "neoip_nudp_nunit.hpp"
#include "neoip_nudp.hpp"
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
nudp_testclass_t::nudp_testclass_t()	throw()
{
	// zero some field
	nudp		= NULL;
	xmit_count	= 0;
}

/** \brief Destructor
 */
nudp_testclass_t::~nudp_testclass_t()	throw()
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
nunit_err_t	nudp_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check
	DBG_ASSERT( !nudp );
	


	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	nudp_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// delete the nudp_t
	nipmem_zdelete	nudp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	nudp_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// init the nudp
	inet_err_t	inet_err;	
	nudp		= nipmem_new nudp_t();
	inet_err	= nudp->start("0.0.0.0:4000", this, NULL );
	if( inet_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, inet_err.to_string());
	
	// start the rxmit timeout
	rxmit_timeout.start(delay_t::from_sec(0), this, NULL);

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool nudp_testclass_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// send a packet thru nudp_t
	ipport_addr_t	remote_addr	= "255.255.255.255:4000";
	datum_t		data		= datum_t("Hello " + OSTREAMSTR(xmit_count));
	inet_err_t	inet_err	= nudp->send_to(data.void_ptr(), data.length(), remote_addr);
	KLOG_ERR("inet_err=" << inet_err );
	// update the rxmit_timeout
	rxmit_timeout.change_period( delay_t::from_sec(1) );
	
	// increase the xmit_count
	xmit_count++;

	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     nudp callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref nudp_t receive a packet
 */
bool	nudp_testclass_t::neoip_inet_nudp_event_cb(void *cb_userptr, nudp_t &cb_nudp, pkt_t &pkt
					, const ipport_addr_t &local_addr
					, const ipport_addr_t &remote_addr)	throw()
{
	// log to debug
	KLOG_ERR("enter local_addr=" << local_addr << " remote_addr=" << remote_addr
					<< " pkt=" << pkt );
	// return tokeep	
	return true;
}


NEOIP_NAMESPACE_END
