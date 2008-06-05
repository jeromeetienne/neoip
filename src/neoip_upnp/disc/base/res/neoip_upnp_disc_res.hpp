/*! \file
    \brief Header of the \ref upnp_disc_res_t

*/


#ifndef __NEOIP_UPNP_DISC_RES_HPP__ 
#define __NEOIP_UPNP_DISC_RES_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_upnp_disc_res_wikidbg.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters replied by a tracker
 */
class upnp_disc_res_t : NEOIP_COPY_CTOR_ALLOW
				, private wikidbg_obj_t<upnp_disc_res_t, upnp_disc_res_wikidbg_init> {
private:
public:
	/*************** ctor/dtor	***************************************/
	upnp_disc_res_t()	throw();

	/*************** query function	***************************************/
	bool		is_null()		const throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	upnp_disc_res_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( std::string	, service_name);
	RES_VAR_DIRECT( std::string	, server_name);
	RES_VAR_DIRECT( ip_addr_t	, local_ipaddr);
	RES_VAR_DIRECT( ip_addr_t	, server_ipaddr);
	RES_VAR_DIRECT( http_uri_t	, location_uri);
	RES_VAR_DIRECT( http_uri_t	, control_uri);
	RES_VAR_DIRECT( bool		, getport_endianbug);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** Comparison function	*******************************/
	int	compare(const upnp_disc_res_t &other)		const throw();
	bool 	operator == (const upnp_disc_res_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const upnp_disc_res_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const upnp_disc_res_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const upnp_disc_res_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const upnp_disc_res_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const upnp_disc_res_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/************** display function	*******************************/
	std::string	to_string()	const throw();
	std::string	to_human_str()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const upnp_disc_res_t &upnp_disc_res) throw()
						{ return oss << upnp_disc_res.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t &operator << (serial_t& serial, const upnp_disc_res_t &disc_res)	throw();
	friend	serial_t &operator >> (serial_t& serial, upnp_disc_res_t &disc_res)		throw(serial_except_t);	

	/*************** list of friend class	*******************************/
	friend class	upnp_disc_res_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_DISC_RES_HPP__  */



