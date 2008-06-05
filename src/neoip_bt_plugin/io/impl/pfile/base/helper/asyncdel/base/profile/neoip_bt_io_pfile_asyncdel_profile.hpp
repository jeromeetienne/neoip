/*! \file
    \brief Header of the \ref bt_io_pfile_asyncdel_profile_t

*/


#ifndef __NEOIP_BT_IO_PFILE_ASYNCDEL_PROFILE_HPP__ 
#define __NEOIP_BT_IO_PFILE_ASYNCDEL_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delaygen_arg.hpp"


NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile_t of a router_peer_t
 */
class bt_io_pfile_asyncdel_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the delaygen_arg_t used to trigger the delete
	static const delaygen_arg_t	DELETOR_DELAYGEN;
public:
	/*************** ctor/dtor	***************************************/
	bt_io_pfile_asyncdel_profile_t()		throw();

	/*************** query function	***************************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define NEOIP_VAR_DIRECT(var_type, var_name)							\
	private:var_type		var_name ## _val;						\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_io_pfile_asyncdel_profile_t&	var_name(const var_type &value)		throw()			\
						{ var_name ## _val = value; return *this;	}
#	define NEOIP_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &		var_name()	throw()		{ return var_name ## _val; }	\
		NEOIP_VAR_DIRECT(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	NEOIP_VAR_DIRECT(delaygen_arg_t	, deletor_delaygen);

	/*************** #undef to ease the declaration	***********************/
#	undef NEOIP_VAR_DIRECT
#	undef NEOIP_VAR_STRUCT
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_PFILE_ASYNCDEL_PROFILE_HPP__  */



