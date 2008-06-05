/*! \file
    \brief Header of the test of socket_client_t
*/


#ifndef __NEOIP_TLS_NUNIT_HPP__ 
#define __NEOIP_TLS_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_socket_client_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	tls_profile_t;

/** \brief Class which implement a nunit for the tls
 */
class tls_testclass_t : public nunit_testclass_api_t {
private:
	nunit_res_t	standalone_cnx(const tls_profile_t &resp_profile
						, const tls_profile_t &itor_profile)	throw();
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	cnx_anon2anon(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	cnx_cert2cert(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TLS_NUNIT_HPP__  */



