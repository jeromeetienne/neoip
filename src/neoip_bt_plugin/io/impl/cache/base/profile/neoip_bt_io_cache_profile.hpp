/*! \file
    \brief Header of the \ref bt_io_cache_profile_t

*/


#ifndef __NEOIP_BT_IO_CACHE_PROFILE_HPP__ 
#define __NEOIP_BT_IO_CACHE_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_file_size.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for bt_io_cache_pool_t
 */
class bt_io_cache_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! true if this bt_io_cache_t MUST be write-thru, else it is write delayed
	static const bool	WRITE_THRU;
	//! if no cleaning happened during CLEANING_MAX_DELAY, start cleaning
	static const delay_t	CLEANING_MAX_DELAY;
public:
	/*************** ctor/dtor	***************************************/
	bt_io_cache_profile_t()		throw();
	~bt_io_cache_profile_t()		throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_io_cache_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( bool		, write_thru);
	PROFILE_VAR_PLAIN( delay_t	, cleaning_max_delay);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_CACHE_PROFILE_HPP__  */



