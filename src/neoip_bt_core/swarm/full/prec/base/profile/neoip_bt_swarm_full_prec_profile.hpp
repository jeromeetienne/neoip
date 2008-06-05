/*! \file
    \brief Header of the \ref bt_swarm_full_prec_profile_t

*/


#ifndef __NEOIP_BT_SWARM_FULL_PREC_PROFILE_HPP__ 
#define __NEOIP_BT_SWARM_FULL_PREC_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the bt_swarm_full_prec_t
 */
class bt_swarm_full_prec_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default precedence 'base' aka the value which is fixed independantly of subprec
	static const double	XMIT_PREC_BASE;
	//! the default range which gonna be used in xmit according to the bt_reqauth_type_t
	static const double	XMIT_REQAUTH_RANGE;
	//! the default precedence 'base' aka the value which is fixed independantly of subprec
	static const double	RECV_PREC_BASE;
	//! the default range which gonna be used in recv according to the bt_reqauth_type_t
	static const double	RECV_REQAUTH_RANGE;
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_full_prec_profile_t()	throw();
	~bt_swarm_full_prec_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_swarm_full_prec_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( double	, xmit_prec_base);
	PROFILE_VAR_PLAIN( double	, xmit_reqauth_range);
	PROFILE_VAR_PLAIN( double	, recv_prec_base);
	PROFILE_VAR_PLAIN( double	, recv_reqauth_range);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_FULL_PREC_PROFILE_HPP__  */



