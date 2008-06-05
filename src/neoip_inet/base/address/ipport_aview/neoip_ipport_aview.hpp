/*! \file
    \brief Header of the \ref ipport_aview_t

*/


#ifndef __NEOIP_IPPORT_AVIEW_HPP__ 
#define __NEOIP_IPPORT_AVIEW_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_ipport_addr.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters for a ipport_aview_t
 */
class ipport_aview_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the ANY ipport_aview_t aka lview("0.0.0.0:0").pview(ipport_addr_t) for ipv4
	static const ipport_aview_t	ANY_IP4;
private:
	/*************** Internal function	*******************************/
	bool		is_sane_common()	const throw();
public:
	/*************** ctor/dtor	***************************************/
	ipport_aview_t()	throw()	{}
	ipport_aview_t(const ipport_addr_t &local_view, const ipport_addr_t &public_view)	throw()
						{ lview(local_view).pview(public_view);	}
	/*************** query function	***************************************/
	bool		is_null()		const throw();
	// TODO to clearly explain all those states
	// - to rename sane_run and sane_cfg
	// - is_run_ok and is_cfg_ok
	bool		is_run_ok()		const throw();
	bool		is_cfg_ok()		const throw();
	bool		is_inetreach_ok()	const throw()	{ return !pview().is_null();		}
	bool		is_fully_qualified()	const throw()	{ return lview().is_fully_qualified() && pview().is_fully_qualified();	}
	bool		is_natted()		const throw()	{ DBG_ASSERT(is_fully_qualified());
								  return lview() != pview();		}

	/*************** update function	*******************************/
	ipport_aview_t &update_once_bound(const ipport_addr_t &bound_lview)	throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	ipport_aview_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( ipport_addr_t	, lview);
	RES_VAR_DIRECT( ipport_addr_t	, pview);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** Comparison Operator	*******************************/
	int	compare(const ipport_aview_t &other)			const throw();
	bool	operator == (const ipport_aview_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const ipport_aview_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const ipport_aview_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const ipport_aview_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const ipport_aview_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const ipport_aview_t &other) const throw() { return compare(other) >= 0; }
	
	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const ipport_aview_t &ipport_aview) throw()
						{ return oss << ipport_aview.to_string();	}

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const ipport_aview_t &ipport_aview)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, ipport_aview_t &ipport_aview)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPPORT_AVIEW_HPP__  */



