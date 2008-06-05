/*! \file
    \brief Header of the \ref bt_io_pfile_profile_t

*/


#ifndef __NEOIP_BT_IO_PFILE_PROFILE_HPP__ 
#define __NEOIP_BT_IO_PFILE_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for bt_io_pfile_t
 */
class bt_io_pfile_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! true if the pieceidx are considered as circular - false otherwise
	static const bool	HAS_CIRCULARIDX;
public:
	/*************** ctor/dtor	***************************************/
	bt_io_pfile_profile_t()		throw();
	~bt_io_pfile_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_io_pfile_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( file_path_t	, dest_dirpath);
	PROFILE_VAR_PLAIN( bool		, has_circularidx);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_PFILE_PROFILE_HPP__  */



