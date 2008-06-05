/*! \file
    \brief Header of the \ref http_sclient_profile_t

*/


#ifndef __NEOIP_HTTP_SCLIENT_PROFILE_HPP__ 
#define __NEOIP_HTTP_SCLIENT_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_http_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// include for the profile field
#include "neoip_http_client_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class http_sclient_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default maximum length of the whole reply (0 means unlimited)
	static const size_t		REPLY_MAXLEN;
	//! the size preallocated for the reply message body if http_rephd_t doesnt give it explicitly
	static const size_t		DFL_MSGLEN_PREALLOC;
	//! size of progress_chunk to be notified as they are downloaded (0 means no notification)
	static const size_t		PROGRESS_CHUNK_LEN;
public:
	/*************** ctor/dtor	***************************************/
	http_sclient_profile_t()	throw();

	/*************** validity function	*******************************/
	http_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		http_sclient_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( size_t			, reply_maxlen);
	PROFILE_VAR_PLAIN( size_t			, dfl_msglen_prealloc);
	PROFILE_VAR_PLAIN( size_t			, progress_chunk_len);	
	PROFILE_VAR_SPROF( http_client_profile_t	, http_client);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SCLIENT_PROFILE_HPP__  */



