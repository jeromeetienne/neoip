/*! \file
    \brief Header of the \ref oload_swarm_profile_t

*/


#ifndef __NEOIP_OLOAD_SWARM_PROFILE_HPP__ 
#define __NEOIP_OLOAD_SWARM_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal field
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile for get_t
 */
class oload_swarm_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the amount of time to wait after all httpo_full_t got closed before autodelete
	static const delay_t	IDLE_TIMEOUT;
	//! the maximum length of the bt_pselect_slide_curs_t in byte (aka not in piece)
	static const size_t	SLIDE_CURS_MAXLEN;
public:
	/*************** ctor/dtor	***************************************/
	oload_swarm_profile_t()		throw();
	~oload_swarm_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		oload_swarm_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, idle_timeout);
	PROFILE_VAR_PLAIN( size_t	, slide_curs_maxlen);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_SWARM_PROFILE_HPP__  */



