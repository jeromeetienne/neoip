/*! \file
    \brief Header of the \ref file_aio_profile_t

*/


#ifndef __NEOIP_FILE_AIO_PROFILE_HPP__ 
#define __NEOIP_FILE_AIO_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_file_err.hpp"
#include "neoip_file_size.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class file_aio_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the number of byte to read in a single iteration of the event loop
	static const file_size_t	AREAD_ITERLEN;
	//! the number of byte to write in a single iteration of the event loop
	static const file_size_t	AWRITE_ITERLEN;
public:
	/*************** ctor/dtor	***************************************/
	file_aio_profile_t()	throw();
	~file_aio_profile_t()	throw();

	/*************** validity function	*******************************/
	file_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		file_aio_profile_t 	&var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( file_size_t	, aread_iterlen);
	PROFILE_VAR_PLAIN( file_size_t	, awrite_iterlen);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_AIO_PROFILE_HPP__  */



