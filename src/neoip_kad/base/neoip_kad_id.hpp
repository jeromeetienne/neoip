/*! \file
    \brief Declaration of all the gen_id_t of the kademlia layer

*/


#ifndef __NEOIP_KAD_ID_HPP__ 
#define __NEOIP_KAD_ID_HPP__ 
/* system include */
/* local include */
#include "neoip_gen_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


NEOIP_GEN_ID_DECLARATION_START	(kad_recid_t	, skey_auth_algo_t::SHA1, 20);
NEOIP_GEN_ID_DECLARATION_END	(kad_recid_t	, skey_auth_algo_t::SHA1, 20);

NEOIP_GEN_ID_DECLARATION_START	(kad_realmid_t	, skey_auth_algo_t::SHA1, 20);
NEOIP_GEN_ID_DECLARATION_END	(kad_realmid_t	, skey_auth_algo_t::SHA1, 20);

NEOIP_GEN_ID_DECLARATION_START	(kad_nonceid_t	, skey_auth_algo_t::SHA1, 8);
NEOIP_GEN_ID_DECLARATION_END	(kad_nonceid_t	, skey_auth_algo_t::SHA1, 8);

NEOIP_GEN_ID_DECLARATION_START	(kad_targetid_t	, skey_auth_algo_t::SHA1, 20);
NEOIP_GEN_ID_DECLARATION_END	(kad_targetid_t	, skey_auth_algo_t::SHA1, 20);

/** \brief Class definition for kad_peerid_t
 * 
 * - this is just a forward-declarable 'typedef' on top of kad_targetid_t
 * - the kademlia dht assumes that the kad_peerid_t and the kad_keyid_t have the same length
 */
class	kad_peerid_t : public kad_targetid_t {
public:	kad_peerid_t(const char *id_str)		throw(): kad_targetid_t(id_str) {}
	kad_peerid_t(const std::string &id_str)		throw(): kad_targetid_t(id_str) {}
	kad_peerid_t(const kad_targetid_t &targetid)	throw(): kad_targetid_t(targetid) {}
	kad_peerid_t()					throw(): kad_targetid_t()	{}
};

/** \brief Class definition for kad_keyid_t
 * 
 * - this is just a forward-declarable 'typedef' on top of kad_targetid_t
 * - the kademlia dht assumes that the kad_peerid_t and the kad_keyid_t have the same length
 */
class	kad_keyid_t : public kad_targetid_t {
public:	kad_keyid_t(const char *id_str)			throw(): kad_targetid_t(id_str) {}
	kad_keyid_t(const std::string &id_str)		throw(): kad_targetid_t(id_str) {}
	kad_keyid_t(const kad_targetid_t &targetid)	throw(): kad_targetid_t(targetid) {}
	kad_keyid_t()					throw(): kad_targetid_t()	{}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_ID_HPP__  */



