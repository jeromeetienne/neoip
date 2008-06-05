/*! \file
    \brief Header of the unit test of the diffie-hellman layer
*/


#ifndef __NEOIP_SKEY_CIPH_NUNIT_HPP__ 
#define __NEOIP_SKEY_CIPH_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the gen_id_t
 */
class skey_ciph_testclass_t : public nunit_testclass_api_t {
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	aes_ecb(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	aes_cbc(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	arcfour(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_CIPH_NUNIT_HPP__  */



