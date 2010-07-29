/*! \file
    \brief unit test for the host2ip_t unit testt

\par Possible improvement
- to implement a timeout
- to return a inet_err_t and not a bool for the start functoin

*/

/* system include */
/* local include */
#include "neoip_host2ip_nunit.hpp"
#include "neoip_host2ip.hpp"
#include "neoip_ip_addr.hpp"
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
host2ip_testclass_t::host2ip_testclass_t()	throw()
{
}

/** \brief Destructor
 */
host2ip_testclass_t::~host2ip_testclass_t()	throw()
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
nunit_err_t	host2ip_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	host2ip_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// delete all pending host2ip_t
	while( !host2ip_db.empty() ){
		nipmem_delete	*host2ip_db.begin();
		host2ip_db.erase(host2ip_db.begin());
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	host2ip_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");
	
	// set the nb_concurrent
	nb_concurrent	= 20;

	// set the hostname_db
#if 1
	hostname_db.push_back("localhost");
#else
	// set the hostname_db
	for(size_t i = 0; i < 40; i++ ){
		hostname_db.push_back("localhost");
		hostname_db.push_back("google.com");
		hostname_db.push_back("jmebox");
		hostname_db.push_back("kde.org");
	}
#endif
	// start launching the host2ip_t	
	launch_host2ip();
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the host2ip_t
 */
void	host2ip_testclass_t::launch_host2ip()	throw()
{
	host2ip_t *	host2ip;
	inet_err_t	inet_err;
	// launch host2ip_t until there is nb_concurrent running
	while( host2ip_db.size() < nb_concurrent ){
		// if the hostname_db is empty, return now
		if( hostname_db.empty() )	return;
		
		// dequeue the first hostname
		std::string	hostname	= hostname_db.front();
		hostname_db.pop_front();
		
		// start the host2ip_t for this hostname
		host2ip		= nipmem_new host2ip_t();
		inet_err	= host2ip->start(hostname, this, NULL);
		DBG_ASSERT( inet_err.succeed() );
		
		// queue the host2ip_t in the host2ip_db
		host2ip_db.push_back(host2ip);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 host2ip_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by host2ip_t when the result is known
 */
bool	host2ip_testclass_t::neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
			, const inet_err_t &inet_err, const std::vector<ip_addr_t> &ipaddr_arr)	throw()
{
	host2ip_t *	host2ip	= dynamic_cast <host2ip_t *>(&cb_host2ip_vapi);
	// display the result
	KLOG_ERR("host2ip returned err=" << inet_err << " with " << ipaddr_arr.size() << " ip_addr_t"
					<< " for hostname=" << host2ip->hostname());
DBG_ASSERT(inet_err == inet_err_t::OK);
	// remove the host2ip_t from the host2ip_db
	host2ip_db.remove(host2ip);
	// delete the notifier
	nipmem_zdelete	host2ip;

	// display the resulting ip_addr_t
	for( size_t i = 0; i < ipaddr_arr.size(); i++ )	KLOG_DBG("result=" << ipaddr_arr[i]);

	// launch more host2ip_t if needed
	launch_host2ip();
	
	// if now the host2ip_db is empty, the test is completed
	if( host2ip_db.empty() )	nunit_ftor(NUNIT_RES_OK);
	
	// return dontkeep - host2ip_t has just been deleted
	return false;
}

NEOIP_NAMESPACE_END
