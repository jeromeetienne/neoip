/*! \file
    \brief Header of the \ref ndiag_cacheport_item_t

*/


#ifndef __NEOIP_NDIAG_CACHEPORT_ITEM_HPP__ 
#define __NEOIP_NDIAG_CACHEPORT_ITEM_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_upnp_sockfam.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters for a ndiag_cacheport_item_t
 */
class ndiag_cacheport_item_t : NEOIP_COPY_CTOR_ALLOW {
public:
	/*************** ctor/dtor	***************************************/
	ndiag_cacheport_item_t()		throw()		{}
	ndiag_cacheport_item_t(const std::string &m_key, const upnp_sockfam_t &m_sockfam, uint16_t m_port)	throw()
							{ key(m_key).sockfam(m_sockfam).port(m_port);	}
	/*************** query function	***************************************/
	bool	is_null()		const throw()	{ return !is_fully_qualified();	}
	bool	is_fully_qualified()	const throw()	{ return !key().empty() && !sockfam().is_null()
									&& port();	}
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	ndiag_cacheport_item_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( std::string	, key);
	RES_VAR_DIRECT( upnp_sockfam_t	, sockfam);
	RES_VAR_DIRECT( uint16_t	, port);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** Comparison Operator	*******************************/
	int	compare(const ndiag_cacheport_item_t &other)			const throw();
	bool	operator == (const ndiag_cacheport_item_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const ndiag_cacheport_item_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const ndiag_cacheport_item_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const ndiag_cacheport_item_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const ndiag_cacheport_item_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const ndiag_cacheport_item_t &other) const throw() { return compare(other) >= 0; }
	
	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const ndiag_cacheport_item_t &ndiag_cacheport_item) throw()
						{ return oss << ndiag_cacheport_item.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const ndiag_cacheport_item_t &cacheport_item)	throw();
	friend	serial_t& operator >> (serial_t & serial, ndiag_cacheport_item_t &cacheport_item)	throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NDIAG_CACHEPORT_ITEM_HPP__  */



