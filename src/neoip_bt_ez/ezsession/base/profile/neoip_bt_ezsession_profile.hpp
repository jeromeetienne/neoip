/*! \file
    \brief Header of the \ref bt_ezsession_profile_t

*/


#ifndef __NEOIP_BT_EZSESSION_PROFILE_HPP__ 
#define __NEOIP_BT_EZSESSION_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal field
#include "neoip_ipport_addr.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_nslan_id.hpp"
#include "neoip_rate_prec.hpp"
#include "neoip_bt_io_cache_pool_profile.hpp"
#include "neoip_bt_session_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for bt_ezsession_t
 */
class bt_ezsession_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the address to use for the nslan_peer_t for the kad_peer_t stuff
	static const std::string	NSLAN_ADDR;
	//! the realmid to use for the nslan_peer_t stuff
	static const std::string	NSLAN_REALMID;
	//! the realmid to use for the kad_peer_t stuff
	static const std::string	KAD_REALMID;
	//! the maximum xmit rate for the bt_session_t
	static const size_t		XMIT_MAXRATE;
	//! the maximum recv rate for the bt_session_t
	static const size_t		RECV_MAXRATE;
	//! the default rate_prec_t used for rate_sched_t
	static const rate_prec_t	DFL_RATE_PREC;
public:
	/*************** ctor/dtor	***************************************/
	bt_ezsession_profile_t()	throw();
	~bt_ezsession_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_ezsession_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( ipport_addr_t	, nslan_addr	);
	PROFILE_VAR_PLAIN( nslan_realmid_t	, nslan_realmid	);
	PROFILE_VAR_PLAIN( kad_realmid_t	, kad_realmid	);
	PROFILE_VAR_PLAIN( size_t		, xmit_maxrate	);
	PROFILE_VAR_PLAIN( size_t		, recv_maxrate	);
	PROFILE_VAR_PLAIN( rate_prec_t		, dfl_rate_prec	);
	PROFILE_VAR_SPROF( bt_session_profile_t		, session	);
	PROFILE_VAR_SPROF( bt_io_cache_pool_profile_t	, io_cache_pool	);
	

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSESSION_PROFILE_HPP__  */



