/*! \file
    \brief unit test for the diffie-hellman unit test

\par Possible Improvements
- this unit test is way to basic.
  - a lot of thing may go wrong and this test will still pass
  - TODO find a good test and code it

*/

/* system include */
/* local include */
#include "neoip_ipport_strlist_src_nunit.hpp"
#include "neoip_ipport_strlist_src.hpp"
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
ipport_strlist_src_testclass_t::ipport_strlist_src_testclass_t()	throw()
{
	// zero some field
	strlist_src	= NULL;
}

/** \brief Destructor
 */
ipport_strlist_src_testclass_t::~ipport_strlist_src_testclass_t()	throw()
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
nunit_err_t	ipport_strlist_src_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// sanity check
	DBG_ASSERT( !strlist_src );
	
	// init the strlist_src
	strlist_src	= nipmem_new ipport_strlist_src_t();
	// populate the strlist_src
	strlist_src->append("127.0.0.1:1");
	strlist_src->append("127.0.0.2:2");
	strlist_src->append("jmehost.dyndns.org:3");
	strlist_src->append("127.0.0.3:4");
	strlist_src->append("hostnameWhichDoesntExist:99");
	strlist_src->append("127.0.0.4:5");
	strlist_src->append("google.com:6");
	strlist_src->append("127.0.0.5:7");

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ipport_strlist_src_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// delete the ipport_strlist_src
	if( strlist_src ){
		nipmem_delete	strlist_src;
		strlist_src	= NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     ipport_strlist_src callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback notified by \ref ipport_strlist_src_t when to notify an ipport_addr_t
 */
bool ipport_strlist_src_testclass_t::neoip_ipport_strlist_src_cb(void *cb_userptr
					, ipport_strlist_src_t &cb_ipport_strlist_src
					, const ipport_addr_t &ipport_addr)	throw()
{
	// log to debug
	KLOG_ERR("enter notified ipport_addr=" << ipport_addr);

	// if the source reached its end, exit the test
	if( ipport_addr.is_null() ){
		// delete the strlist_src and mark it unused
		nipmem_delete	strlist_src;
		strlist_src	= NULL;
		// notify the caller of the end of this testfunction
		nunit_ftor(NUNIT_RES_OK);
		// return 'dontkeep'
		return false;
	}

	// ask for more ipport_addr_t
	strlist_src->get_more();
	
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	ipport_strlist_src_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// Start the strlist_src
	inet_err_t	inet_err;
	inet_err	= strlist_src->start(this, NULL);
	if( inet_err.failed() )	return NUNIT_RES_ERROR;
	// request one address
	strlist_src->get_more();
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}



NEOIP_NAMESPACE_END
