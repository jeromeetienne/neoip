/*! \file
    \brief Header of the \ref bt_peersrc_http_profile_t

*/


#ifndef __NEOIP_BT_PEERSRC_HTTP_PROFILE_HPP__ 
#define __NEOIP_BT_PEERSRC_HTTP_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the fields
#include "neoip_bt_tracker_client_profile.hpp"
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for a bt_peersrc_http_t
 */
class bt_peersrc_http_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default delay before retrying a http tracker connection after a local error
	static const delay_t	RETRY_DELAY_LOCAL_ERR;
	//! the default delay before retrying a http tracker connection after a http error
	static const delay_t	RETRY_DELAY_CNX_ERR;
	//! the default delay before retrying a http tracker connection after a bt error (faillure field)
	static const delay_t	RETRY_DELAY_BT_ERR;
	//! if true the bt_tracker_client_t will issue only compact request
	static const bool	USE_COMPACT_REQUEST;
public:
	/*************** ctor/dtor	***************************************/
	bt_peersrc_http_profile_t()	throw();
	~bt_peersrc_http_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_peersrc_http_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, retry_delay_local_err);
	PROFILE_VAR_PLAIN( delay_t	, retry_delay_cnx_err);
	PROFILE_VAR_PLAIN( delay_t	, retry_delay_bt_err);
	PROFILE_VAR_PLAIN( bool		, use_compact_request);
	PROFILE_VAR_SPROF( bt_tracker_client_profile_t	, tracker_client);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_HTTP_PROFILE_HPP__  */



