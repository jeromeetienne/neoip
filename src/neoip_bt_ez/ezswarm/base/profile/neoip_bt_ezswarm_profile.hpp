/*! \file
    \brief Header of the \ref bt_ezswarm_profile_t

*/


#ifndef __NEOIP_BT_EZSWARM_PROFILE_HPP__ 
#define __NEOIP_BT_EZSWARM_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal field
#include "neoip_bt_io_pfile_profile.hpp"
#include "neoip_bt_alloc_profile.hpp"
#include "neoip_bt_alloc_policy.hpp"
#include "neoip_bt_check_policy.hpp"
#include "neoip_bt_peerpick_mode.hpp"
#include "neoip_bt_peerpick_profile.hpp"
#include "neoip_file_path.hpp"
#include "neoip_delay.hpp"
#include "neoip_bt_swarm_profile.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;

/** \brief to handle the profile for bt_ezswarm_t
 */
class bt_ezswarm_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the bt_alloc_policy_t to use in this bt_ezswarm_t
	static const bt_alloc_policy_t	ALLOC_POLICY;
	//! the bt_check_policy_t to use in this bt_ezswarm_t
	static const bt_check_policy_t	CHECK_POLICY;
	//! the bt_peerpick_mode_t to use in this bt_ezswarm_t
	static const bt_peerpick_mode_t	PEERPICK_MODE;
	//! the maximum amount of time allowed to stop bt_peersrc_kad_t in bt_ezswarm_stopping_t
	static const delay_t		PEERSRC_KAD_STOPPING_TIMEOUT;
public:
	/*************** ctor/dtor	***************************************/
	bt_ezswarm_profile_t()	throw();
	~bt_ezswarm_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_ezswarm_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( bt_alloc_policy_t	, alloc_policy);
	PROFILE_VAR_SPROF( bt_alloc_profile_t	, alloc_profile);
	PROFILE_VAR_SPROF( bt_swarm_profile_t	, swarm);
	
	PROFILE_VAR_PLAIN( bt_check_policy_t	, check_policy);
	
	PROFILE_VAR_PLAIN( bt_peerpick_mode_t	, peerpick_mode);
	PROFILE_VAR_SPROF( bt_peerpick_profile_t, peerpick);
	
	PROFILE_VAR_SPROF( bt_io_pfile_profile_t, io_pfile);
	
	typedef	kad_peer_t *	kad_peer_ptr_t;	// trick to full the macro because it is a pointer
	PROFILE_VAR_PLAIN( kad_peer_ptr_t	, peersrc_kad_peer);
	PROFILE_VAR_PLAIN( delay_t		, peersrc_kad_stopping_timeout);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_PROFILE_HPP__  */



