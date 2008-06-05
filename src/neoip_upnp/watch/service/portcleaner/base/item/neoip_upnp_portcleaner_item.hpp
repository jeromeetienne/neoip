/*! \file
    \brief Header of the \ref upnp_portcleaner_item_t

*/


#ifndef __NEOIP_UPNP_PORTCLEANER_ITEM_HPP__ 
#define __NEOIP_UPNP_PORTCLEANER_ITEM_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_portcleaner_item_wikidbg.hpp"
#include "neoip_upnp_portdesc.hpp"
#include "neoip_date.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	strvar_db_t;

/** \brief to store the parameters for a upnp_portcleaner_item_t
 */
class upnp_portcleaner_item_t : NEOIP_COPY_CTOR_ALLOW
		, private wikidbg_obj_t<upnp_portcleaner_item_t, upnp_portcleaner_item_wikidbg_init>
		{
public:
	/*************** ctor/dtor	***************************************/
	upnp_portcleaner_item_t()	throw()	{}
	upnp_portcleaner_item_t(const upnp_portdesc_t &a_portdesc
					, const date_t &a_creation_date)	throw();

	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return creation_date().is_null();	}
	date_t		expire_date()	const throw()	{ return creation_date() + portdesc().portcleaner_lease();	}
	bool		is_obsolete()	const throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	upnp_portcleaner_item_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_STRUCT( upnp_portdesc_t	, portdesc);
	RES_VAR_DIRECT( date_t		, creation_date);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** Comparison Operator	*******************************/
	int	compare(const upnp_portcleaner_item_t &other)	   const throw();
	bool	operator == (const upnp_portcleaner_item_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const upnp_portcleaner_item_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const upnp_portcleaner_item_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const upnp_portcleaner_item_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const upnp_portcleaner_item_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const upnp_portcleaner_item_t &other) const throw() { return compare(other) >= 0; }
	
	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const upnp_portcleaner_item_t &upnp_portcleaner_item) throw()
					{ return oss << upnp_portcleaner_item.to_string();	}

	/*************** List of friend class	*******************************/
	friend class	upnp_portcleaner_item_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_PORTCLEANER_ITEM_HPP__  */



