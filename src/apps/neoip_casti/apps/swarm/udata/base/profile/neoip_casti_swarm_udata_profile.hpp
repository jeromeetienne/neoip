/*! \file
    \brief Header of the \ref casti_swarm_udata_profile_t

*/


#ifndef __NEOIP_CASTI_SWARM_UDATA_PROFILE_HPP__ 
#define __NEOIP_CASTI_SWARM_UDATA_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal field
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile for casti_swarm_t
 */
class casti_swarm_udata_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the maximum amount of time between 2 xmit of bt_cast_udata_t
	static const delay_t	XMIT_MAXDELAY;
	//! the maximum of piece between 2 xmit of bt_cast_udata_t
	static const size_t	XMIT_MAXPIECE;
	//! the maximum amount of time between 2 recv of bt_cast_udata_t
	static const delay_t	RECV_UDATA_MAXDELAY;
public:
	/*************** ctor/dtor	***************************************/
	casti_swarm_udata_profile_t()	throw();
	~casti_swarm_udata_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		casti_swarm_udata_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, xmit_maxdelay);
	PROFILE_VAR_PLAIN( size_t		, xmit_maxpiece);
	PROFILE_VAR_PLAIN( delay_t		, recv_udata_maxdelay);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_SWARM_UDATA_PROFILE_HPP__  */



