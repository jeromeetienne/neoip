/*! \file
    \brief Header of the \ref upnp_portdesc_t

*/


#ifndef __NEOIP_UPNP_PORTDESC_HPP__ 
#define __NEOIP_UPNP_PORTDESC_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_ipport_addr.hpp"
#include "neoip_upnp_sockfam.hpp"
#include "neoip_delay.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	strvar_db_t;

/** \brief to store the parameters for a upnp_portdesc_t
 */
class upnp_portdesc_t : NEOIP_COPY_CTOR_ALLOW {
public:
	/*************** ctor/dtor	***************************************/
	upnp_portdesc_t()	throw()		{}

	/*************** query function	***************************************/
	bool		is_null()		const throw()	{ return ipport_lview().is_null();	}
	bool		has_portcleaner_tag()	const throw()	{ return !portcleaner_lease().is_null();}

	/*************** update function	*******************************/
	static upnp_portdesc_t	from_strvar_db(const strvar_db_t &strvar_db, bool revendian)	throw();

	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	upnp_portdesc_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( ipport_addr_t	, ipport_pview);
	RES_VAR_DIRECT( upnp_sockfam_t	, sockfam);
	RES_VAR_DIRECT( ipport_addr_t	, ipport_lview);
	RES_VAR_DIRECT( bool		, map_enabled);
	RES_VAR_DIRECT( std::string	, desc_str);
	RES_VAR_DIRECT( delay_t		, lease_delay);
	RES_VAR_DIRECT( delay_t		, portcleaner_lease);
	RES_VAR_DIRECT( std::string	, portcleaner_nonce);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** Comparison Operator	*******************************/
	int	compare(const upnp_portdesc_t &other)			const throw();
	bool	operator == (const upnp_portdesc_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const upnp_portdesc_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const upnp_portdesc_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const upnp_portdesc_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const upnp_portdesc_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const upnp_portdesc_t &other) const throw() { return compare(other) >= 0; }
	
	/************** display function	*******************************/
	std::string	to_string()	const throw();
	std::string	to_human_str()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const upnp_portdesc_t &upnp_portdesc) throw()
						{ return oss << upnp_portdesc.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_PORTDESC_HPP__  */



