/*! \file
    \brief unit test for the agethostbyname_t unit testt

\par Possible improvement
- to implement a timeout
- to return a inet_err_t and not a bool for the start functoin

*/

/* system include */
/* local include */
#include "neoip_agethostbyname_nunit.hpp"
#include "neoip_agethostbyname.hpp"
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
agethostbyname_testclass_t::agethostbyname_testclass_t()	throw()
{
	// zero some field
	agethostbyname	= NULL;
}

/** \brief Destructor
 */
agethostbyname_testclass_t::~agethostbyname_testclass_t()	throw()
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
nunit_err_t	agethostbyname_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check
	DBG_ASSERT( !agethostbyname );

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	agethostbyname_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// delete the agethostbyname
	nipmem_zdelete	agethostbyname;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	agethostbyname_testclass_t::query_localhost(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	inet_err_t	inet_err;
	// start the agethostbyname_t
	agethostbyname	= nipmem_new agethostbyname_t();
	inet_err	= agethostbyname->start("localhost", this, NULL);
	NUNIT_ASSERT( inet_err.succeed() );
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     agethostbyname callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by agethostbyname_t when the result is known
 */
void agethostbyname_testclass_t::neoip_agethostbyname_cb(void *cb_userptr
					, agethostbyname_t &agethostbyname
					, const hostent_t &hostent)	throw()
{
	// display the result
	KLOG_DBG("agethostbyname returned " << hostent);

	// if the hostent contain 127.0.0.1 ip_addr_t, the test is positive
	for( size_t i = 0; i < hostent.get_ip_addr_db().size(); i++ ){
		if( hostent.get_ip_addr_db()[i] == "127.0.0.1" ){
			nunit_ftor(NUNIT_RES_OK);
			return;
		}
	}
	// else it is negative
	nunit_ftor(NUNIT_RES_ERROR);
}
NEOIP_NAMESPACE_END
