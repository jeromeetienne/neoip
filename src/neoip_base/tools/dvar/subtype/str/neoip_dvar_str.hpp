/*! \file
    \brief Declaration of the dvar_str_t

*/


#ifndef __NEOIP_DVAR_STR_HPP__ 
#define __NEOIP_DVAR_STR_HPP__ 
/* system include */
/* local include */
#include "neoip_dvar_type.hpp"
#include "neoip_dvar_vapi.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief a dvar_t containing a dvar_type_t::INTEGER
 */ 
class dvar_str_t : NEOIP_COPY_CTOR_ALLOW, public dvar_vapi_t {
private:
	std::string	value;
	/*************** clone() to copy via dvar_vapi_t	***************/	
	dvar_vapi_t *	clone()	const throw();
public:
	/*************** ctor/dtor	***************************************/
	dvar_str_t(const std::string newval)	throw()	{ value = newval;	}
	dvar_str_t()				throw()	{}
	~dvar_str_t()				throw()	{}

	/*************** Query function	***************************************/
	size_t		size()				const throw()	{ return value.size();		}
	bool		empty()				const throw()	{ return value.empty();		}
	const std::string &get()			const throw()	{ return value;			}
	dvar_str_t &	set(const std::string &newval)	throw()		{ value = newval; return *this;	}
	dvar_type_t	type()				const throw()	{ return dvar_type_t::STRING;	}

	/*************** Comparison function	*******************************/
	int	compare(const dvar_vapi_t &other_vapi)	const throw();
	bool 	operator == (const dvar_str_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const dvar_str_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const dvar_str_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const dvar_str_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const dvar_str_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const dvar_str_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream & operator << ( std::ostream& os, const dvar_str_t &dvar_str )
						{ return os << dvar_str.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_STR_HPP__  */



