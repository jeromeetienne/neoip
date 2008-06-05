/*! \file
    \brief Header of the test of socket_client_t
*/


#ifndef __NEOIP_SLAY_NUNIT_HPP__ 
#define __NEOIP_SLAY_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_socket_client_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	slay_resp_arg_t;
class	slay_itor_arg_t;

/** \brief Class which implement a nunit for the slay
 */
class slay_testclass_t : public nunit_testclass_api_t {
private:
	nunit_res_t	standalone_cnx(const slay_resp_arg_t &resp_arg
					, const slay_itor_arg_t &itor_arg)	throw();
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	standalone_tls(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	standalone_btjamstd(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	standalone_btjamrc4(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_NUNIT_HPP__  */



