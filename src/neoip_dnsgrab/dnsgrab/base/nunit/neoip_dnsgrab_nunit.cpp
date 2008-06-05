/*! \file
    \brief Definition of the unit test for the ntudp socket with DIRECT connection
    
*/

/* system include */
/* local include */
#include "neoip_dnsgrab_nunit.hpp"
#include "neoip_dnsgrab.hpp"
#include "neoip_dnsgrab_arg.hpp"
#include "neoip_dnsgrab_request.hpp"
#include "neoip_host2ip.hpp"
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
dnsgrab_testclass_t::dnsgrab_testclass_t()	throw()
{
	// zero some field
	dnsgrab	= NULL;
	host2ip	= NULL;
}

/** \brief Destructor
 */
dnsgrab_testclass_t::~dnsgrab_testclass_t()	throw()
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
nunit_err_t	dnsgrab_testclass_t::neoip_nunit_testclass_init()	throw()
{
	dnsgrab_err_t	dnsgrab_err;
	// create the dnsgrab_t
	dnsgrab		= nipmem_new dnsgrab_t(this, NULL);
	// start the dnsgrab_t
	dnsgrab_err	= dnsgrab->start("127.0.0.1", "user", 999, delay_t::from_sec(30),"dnsgrab_utest");	
	if( dnsgrab_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, dnsgrab_err.to_string() );

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	dnsgrab_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// delete host2ip_t if needed
	nipmem_zdelete host2ip;
	// delete dnsgrab_t if needed
	nipmem_zdelete dnsgrab;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      stuff for unknown_host error
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the dnsgrab_t thru a host2ip_t
 */
nunit_res_t	dnsgrab_testclass_t::test_thru_host2ip(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	inet_err_t	inet_err;
	// start the host2ip_t
	host2ip		= nipmem_new host2ip_t();
	inet_err	= host2ip->start("dnsgrab_nunit.neoip", this, NULL);
	NUNIT_ASSERT( inet_err.succeed() );
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief dnsgrab callback to received request	
 */
bool dnsgrab_testclass_t::neoip_dnsgrab_cb(void *cb_userptr, dnsgrab_t &cb_dnsgrab
						, dnsgrab_request_t &request)	throw()
{
	// log to debug
	KLOG_DBG("Received a request of " << request.get_request_name() 
			<< " From " << request.get_addr_family()
			<< " by " << (request.is_request_by_name() ? "name" : "address"));

	// if the dnsgrab_request_t is not the expected one, notify a not_found
	if( request.get_addr_family() != "AF_INET" || !request.is_request_by_name()
				|| request.get_request_name() != "dnsgrab_nunit.neoip"){
		// make the reply as not found
		request.get_reply_present()	= true;
		request.get_reply_name()	= std::string();	
		return true;
	}
	
	// set the reply with dummy data before notifying it
	request.get_reply_present()	= true;	
	request.get_reply_name()	= "dnsgrab_nunit.neoip";
	request.get_reply_aliases().push_back("wonderalias.neoip");
	request.get_reply_addresses().push_back("1.2.3.4");
	request.get_reply_addresses().push_back("5.6.7.8");

	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 host2ip_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by host2ip_t when the result is known
 */
bool	dnsgrab_testclass_t::neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
			, const inet_err_t &inet_err, const std::vector<ip_addr_t> &result_arr)	throw()
{
	// display the result
	KLOG_DBG("enter host2ip returned err=" << inet_err << " with " << result_arr.size() << " ip_addr_t"
					<< " for hostname=" << host2ip->hostname());
	// delete the host2ip_t
	nipmem_zdelete	host2ip;

	// if the hostent contain the proper data, the test is positive
	if( result_arr.size() == 2 && result_arr[0] == "1.2.3.4" && result_arr[1] == "5.6.7.8" ){
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}

	// else it is negative
	nunit_ftor(NUNIT_RES_ERROR);
	return false;
}

NEOIP_NAMESPACE_END

