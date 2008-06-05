/*! \file
    \brief Definition of the unit test for the \ref timeout_t

*/

/* system include */
/* local include */
#include "neoip_expireset2_nunit.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
expireset2_testclass_t::expireset2_testclass_t()	throw()
{
	// zero some field
	expireset	= NULL;
}

/** \brief Destructor
 */
expireset2_testclass_t::~expireset2_testclass_t()	throw()
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
nunit_err_t	expireset2_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");	
	// init the expireset_t
	expireset	= nipmem_new expireset2_t<std::string>(delay_t::from_sec(42));
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	expireset2_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// stop the timeout if it is running
	if( timeout.is_running() )	timeout.stop();
	// delete the expireset
	nipmem_zdelete	expireset;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



/** \brief general test
 */
nunit_res_t	expireset2_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// get the default ttl
	NUNIT_ASSERT( expireset->default_ttl() == delay_t::from_sec(42) );

	// test if the size/empty reflect an empty expireset2_t
	NUNIT_ASSERT( expireset->size() == 0 );
	NUNIT_ASSERT( expireset->empty() == true );
	
	// insert an element with the default ttl
	expireset->insert("hello");
	NUNIT_ASSERT( expireset->contain("hello") );

	// test if the size/empty reflect the insertion
	NUNIT_ASSERT( expireset->size() == 1 );
	NUNIT_ASSERT( expireset->empty() == false );
	
	// test if the element 0 is "hello"
	NUNIT_ASSERT( (*expireset)[0] == "hello" );
	
	// erase the "hello" from the expireset2_t
	expireset->erase("hello");
	// check it is no more here
	NUNIT_ASSERT( expireset->empty() );
	
	// insert an element which expire in 10ms
	expireset->insert("todisappear", delay_t::from_msec(10));	
	// Start the timeout in 20ms to test is the data did dispeared
	timeout.start(delay_t::from_msec(20), this, NULL);

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback called when the timeout_t expire
 */
bool 	expireset2_testclass_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// report OK if the expireset2 is now empty, ERROR otherwise	
	bool	tokeep;
	if( expireset->empty() )	tokeep	= nunit_ftor(NUNIT_RES_OK);
	else				tokeep	= nunit_ftor(NUNIT_RES_ERROR);
	if( !tokeep )	return false;

	// stop the timeout
	timeout.stop();
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the serial consistency
 */
nunit_res_t	expireset2_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	expireset2_t<std::string>	expireset_toserial;
	expireset2_t<std::string>	expireset_unserial;
	serial_t			serial;
	// populate the expireset_toserial
	expireset_toserial.insert("hello", delay_t::from_sec(2));
	expireset_toserial.insert("world");
	// do the serial/unserial
	serial << expireset_toserial;
	serial >> expireset_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( expireset_toserial == expireset_unserial );
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

