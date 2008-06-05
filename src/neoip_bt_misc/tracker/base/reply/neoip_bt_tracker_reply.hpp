/*! \file
    \brief Header of the \ref bt_tracker_reply_t

*/


#ifndef __NEOIP_BT_TRACKER_REPLY_HPP__ 
#define __NEOIP_BT_TRACKER_REPLY_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_delay.hpp"
#include "neoip_bt_tracker_peer_arr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_tracker_request_t;
class	dvar_t;

/** \brief to store the parameters replied by a tracker
 */
class bt_tracker_reply_t : NEOIP_COPY_CTOR_ALLOW {
private:
	dvar_t		peer_arr_to_dvar(const bt_tracker_request_t &request)		const throw();
	dvar_t		peer_arr_to_dvar_nocompact(const bt_tracker_request_t &request)	const throw();
	dvar_t		peer_arr_to_dvar_docompact(const bt_tracker_request_t &request)	const throw();

	static bt_tracker_peer_arr_t	dvar_to_peer_arr(const dvar_t &peers_dvar)		throw();
	static bt_tracker_peer_arr_t	dvar_to_peer_arr_nocompact(const dvar_t &peers_dvar)	throw();
	static bt_tracker_peer_arr_t	dvar_to_peer_arr_docompact(const dvar_t &peers_dvar)	throw();
public:
	/*************** query function	***************************************/
	bool		is_null()	const throw();
	
	/*************** Convertion function	*******************************/
	datum_t				to_bencode(const bt_tracker_request_t &request)	const throw();
	static bt_tracker_reply_t	from_bencode(const datum_t &datum)		throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_tracker_reply_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( std::string		, failure_reason);
	RES_VAR_DIRECT( delay_t			, request_period);
	RES_VAR_DIRECT( size_t			, nb_seeder);
	RES_VAR_DIRECT( size_t			, nb_leecher);
	RES_VAR_STRUCT( bt_tracker_peer_arr_t	, peer_arr);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const bt_tracker_reply_t &reply ) throw()
						{ return os << reply.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_TRACKER_REPLY_HPP__  */



