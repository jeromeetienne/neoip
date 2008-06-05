/*! \file
    \brief Header of the \ref bt_http_ecnx_cnx_profile_t

*/


#ifndef __NEOIP_BT_HTTP_ECNX_CNX_PROFILE_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_CNX_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the fields of the 
#include "neoip_file_size.hpp"
#include "neoip_http_sclient_profile.hpp"
#include "neoip_rate_estim_arg.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for bt_http_ecnx_pool_t
 */
class bt_http_ecnx_cnx_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the delay for which request are queued
	//! - it is a manual and fixed estimation of the latency between the 2 peers. 
	//! - tcp doesnt allow to measure the latency and it is not possible in the bt protocol
	//! - additionnaly sending a BLOCK_REQ is not possible if a BLOCK_REP is currently
	//!   being sent, increasing the 'latency' even more
	static const delay_t		REQ_QUEUE_DELAY;
	//! the minimum length of the request queue
	//! - it should be >= to bt_swarm_profile_t::xmit_req_maxlen()
	static const size_t		REQ_QUEUE_MINLEN;
	//! the maximum length of the request queue
	static const size_t		REQ_QUEUE_MAXLEN;
	//! the multiplier for the request queue minimum length if the underlying
	//! http has_connection_close. it allow to 'absorb' the overhead due to 
	//! the connection establishement at every bt_http_ecnx_iov_t
	static const size_t		REQ_QUEUE_MINLEN_CNXCLOSE_MULT;
	//! the maximum length of the outter request
	static const file_size_t	OUTTER_REQ_MAXLEN;
	//! the rate_estim_t argument for the recv_rate
	static const rate_estim_arg_t	RATE_ESTIM_ARG;	
public:
	/*************** ctor/dtor	***************************************/
	bt_http_ecnx_cnx_profile_t()	throw();
	~bt_http_ecnx_cnx_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_http_ecnx_cnx_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t			, req_queue_delay);
	PROFILE_VAR_PLAIN( size_t			, req_queue_minlen);
	PROFILE_VAR_PLAIN( size_t			, req_queue_maxlen);
	PROFILE_VAR_PLAIN( size_t			, req_queue_minlen_cnxclose_mult);
	PROFILE_VAR_PLAIN( file_size_t			, outter_req_maxlen);
	PROFILE_VAR_SPROF( http_sclient_profile_t	, http_sclient);
	PROFILE_VAR_SPROF( rate_estim_arg_t		, rate_estim_arg);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_CNX_PROFILE_HPP__  */



