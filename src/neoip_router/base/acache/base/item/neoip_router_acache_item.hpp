/*! \file
    \brief Header of the \ref router_acache_item_t

*/


#ifndef __NEOIP_ROUTER_ACACHE_ITEM_HPP__ 
#define __NEOIP_ROUTER_ACACHE_ITEM_HPP__ 
/* system include */
/* local include */
#include "neoip_router_name.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters for a router_acache_item_t
 */
class router_acache_item_t : NEOIP_COPY_CTOR_ALLOW {
public:
	/*************** ctor/dtor	***************************************/
	router_acache_item_t()		throw()		{}
	router_acache_item_t(const router_name_t &m_remote_dnsname, const ip_addr_t &m_local_iaddr
					, const ip_addr_t &m_remote_iaddr)	throw();

	/*************** query function	***************************************/
	bool	is_null()		const throw()	{ return remote_dnsname().is_null();	}
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	router_acache_item_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( router_name_t	, remote_dnsname);
	RES_VAR_DIRECT( ip_addr_t	, local_iaddr);
	RES_VAR_DIRECT( ip_addr_t	, remote_iaddr);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** Comparison Operator	*******************************/
	int	compare(const router_acache_item_t &other)			const throw();
	bool	operator == (const router_acache_item_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const router_acache_item_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const router_acache_item_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const router_acache_item_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const router_acache_item_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const router_acache_item_t &other) const throw() { return compare(other) >= 0; }
	
	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const router_acache_item_t &acache_item) throw()
						{ return oss << acache_item.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const router_acache_item_t &acache_item)	throw();
	friend	serial_t& operator >> (serial_t & serial, router_acache_item_t &acache_item)		throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ACACHE_ITEM_HPP__  */
