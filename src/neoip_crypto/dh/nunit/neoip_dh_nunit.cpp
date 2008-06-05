/*! \file
    \brief unit test for the diffie-hellman unit test

*/

/* system include */
/* local include */
#include "neoip_dh_nunit.hpp"
#include "neoip_dh.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 * 
 * -# generate a shared secret and test both side got the same
 * -# test the serial_t consitency of the public key on serial_t
 */
nunit_res_t	dh_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// a ike group
	std::string	group =	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
				"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
				"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
				"E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
				"EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE65381"
				"FFFFFFFFFFFFFFFF";
	std::string	generator = "2";
	
	// build the diffie hellman parameter
	dh_param_t	dh_param(group, generator);
	// build alice private key
	dh_privkey_t	dh_privkey_alice( dh_param );
	// build bob private key
	dh_privkey_t	dh_privkey_bob( dh_param );
	// build alice shared secret based on alice private key and bob's public key
	dh_shsecret_t	dh_shsecret_alice( dh_privkey_alice, dh_privkey_bob.get_pubkey(), dh_param );
	// build bob's shared secret based on bob's private key and alice's public key
	dh_shsecret_t	dh_shsecret_bob( dh_privkey_bob, dh_privkey_alice.get_pubkey(), dh_param );

	// compare if both shared secret are equal	
	NUNIT_ASSERT( dh_shsecret_alice == dh_shsecret_bob );

	// test dh_pubkey_t serialization
	serial_t	serial;
	dh_pubkey_t	dh_pubkey_unserial;
	serial << dh_privkey_alice.get_pubkey();
	serial >> dh_pubkey_unserial;
	NUNIT_ASSERT( dh_privkey_alice.get_pubkey() == dh_pubkey_unserial );

	// return no error
	return NUNIT_RES_OK;
}



NEOIP_NAMESPACE_END
