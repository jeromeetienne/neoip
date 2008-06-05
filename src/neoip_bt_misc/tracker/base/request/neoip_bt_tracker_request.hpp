/*! \file
    \brief Header of the \ref bt_tracker_request_t

*/


#ifndef __NEOIP_BT_TRACKER_REQUEST_HPP__ 
#define __NEOIP_BT_TRACKER_REQUEST_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_http_uri.hpp"
#include "neoip_file_size.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	httpd_request_t;

/** \brief to store the parameters replied by a tracker
 */
class bt_tracker_request_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	bt_tracker_request_t()	throw();

	/*************** query function	***************************************/
	bool		is_null()					const throw();
	
	/*************** action function	*******************************/
	http_uri_t	build_full_uri()				const throw();
	static bt_tracker_request_t from_httpd_request(const httpd_request_t &httpd_request) throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_tracker_request_t &var_name(const var_type &var_name)	throw()			\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( http_uri_t	, announce_uri);
	RES_VAR_DIRECT( bt_id_t		, infohash);	
	RES_VAR_DIRECT( bt_id_t		, peerid);
	RES_VAR_DIRECT( uint16_t	, port);
	RES_VAR_DIRECT( file_size_t	, uploaded);
	RES_VAR_DIRECT( file_size_t	, downloaded);
	RES_VAR_DIRECT( file_size_t	, left);
	RES_VAR_DIRECT( bool		, compact);
	RES_VAR_DIRECT( bool		, nopeerid);
	RES_VAR_DIRECT( std::string	, event);
	RES_VAR_DIRECT( ip_addr_t	, ipaddr);
	RES_VAR_DIRECT( size_t		, nb_peer_wanted);
	RES_VAR_DIRECT( std::string	, key);
	RES_VAR_DIRECT( bool		, jamstd_support);
	RES_VAR_DIRECT( bool		, jamstd_require);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const bt_tracker_request_t &client_res ) throw()
						{ return os << client_res.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_TRACKER_REQUEST_HPP__  */



