/*! \file
    \brief Header of the \ref bt_jamstd_profile_t

*/


#ifndef __NEOIP_BT_JAMSTD_PROFILE_HPP__ 
#define __NEOIP_BT_JAMSTD_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_bt_jamstd_negoflag.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile for get_t
 */
class bt_jamstd_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//!< the diffie-hellman public key length in byte
	static const size_t		DH_PUBKEY_LEN;
	//!< the string of the group used for diffie-hellman computation
	static const std::string	DH_GROUP;
	//!< the string of the generator used for diffie-hellman computation
	static const std::string	DH_GENERATOR;
	//!< the string for the verification constant used in the protocol
	static const std::string	VERIF_CST;
	//!< the padding maximum length in byte for the CNXESTA packet
	static const size_t		PADESTA_MAXLEN;
	//!< the padding maximum length in byte for the CNXAUTH packet
	static const size_t		PADAUTH_MAXLEN;
	//!< the allowed bt_jamstd_negoflag_t
	static const bt_jamstd_negoflag_t	NEGOFLAG_ALLOWED;
public:
	/*************** ctor/dtor	***************************************/
	bt_jamstd_profile_t()		throw();
	~bt_jamstd_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_jamstd_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( size_t		, dh_pubkey_len		);
	PROFILE_VAR_PLAIN( std::string		, dh_group		);
	PROFILE_VAR_PLAIN( std::string		, dh_generator		);
	PROFILE_VAR_PLAIN( std::string		, verif_cst		);
	PROFILE_VAR_PLAIN( size_t		, padesta_maxlen	);
	PROFILE_VAR_PLAIN( size_t		, padauth_maxlen	);
	PROFILE_VAR_PLAIN( bt_jamstd_negoflag_t	, negoflag_allowed	);
	

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMSTD_PROFILE_HPP__  */



