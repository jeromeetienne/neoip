/*! \file
    \brief Declaration of the dvar_dbl_t

*/


#ifndef __NEOIP_DVAR_DBL_HPP__ 
#define __NEOIP_DVAR_DBL_HPP__ 
/* system include */
/* local include */
#include "neoip_dvar_type.hpp"
#include "neoip_dvar_vapi.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief a dvar_t containing a dvar_type_t::DOUBLE
 */ 
class dvar_dbl_t : NEOIP_COPY_CTOR_ALLOW, public dvar_vapi_t {
private:
	double		value;
	
	/*************** clone() to copy via dvar_vapi_t	***************/
	dvar_vapi_t *	clone()	const throw();
public:
	/*************** ctor/dtor	***************************************/
	dvar_dbl_t(double newval)	throw()	{ value = newval;	}
	dvar_dbl_t()			throw()	{}
	~dvar_dbl_t()			throw()	{}

	/*************** Query function	***************************************/
	double		get()		const throw()	{ return value;			}
	dvar_dbl_t &	set(double newval) throw()	{ value = newval; return *this;	}
	dvar_type_t	type()		const throw()	{ return dvar_type_t::DOUBLE;	}

	/*************** Alias to get the value	per type	***************/
	bool		is_float_ok()	const throw()	{ return value <= (double)std::numeric_limits<float>::max();	}
	float		to_float()	const throw()	{ DBG_ASSERT( is_float_ok() );  return value;		}

	/*************** Comparison function	*******************************/
	int	compare(const dvar_vapi_t &other_vapi)	const throw();
	bool 	operator == (const dvar_dbl_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const dvar_dbl_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const dvar_dbl_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const dvar_dbl_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const dvar_dbl_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const dvar_dbl_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream & operator << ( std::ostream& os, const dvar_dbl_t &dvar_dbl )
						{ return os << dvar_dbl.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_DBL_HPP__  */



