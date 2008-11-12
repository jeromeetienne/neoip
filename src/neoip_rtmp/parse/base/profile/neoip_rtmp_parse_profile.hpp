/*! \file
    \brief Header of the \ref rtmp_parse_profile_t

*/


#ifndef __NEOIP_RTMP_PARSE_PROFILE_HPP__
#define __NEOIP_RTMP_PARSE_PROFILE_HPP__
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for rtmp_parse_t
 */
class rtmp_parse_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the maximum size of the received buffer before declaring it illegal
	//! - used to prevent a DOS which would allocate a very large amount of memory
	static const size_t	BUFFER_MAXLEN;
public:
	/*************** ctor/dtor	***************************************/
	rtmp_parse_profile_t()	throw();
	~rtmp_parse_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		rtmp_parse_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( size_t	, buffer_maxlen);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_PARSE_PROFILE_HPP__  */



