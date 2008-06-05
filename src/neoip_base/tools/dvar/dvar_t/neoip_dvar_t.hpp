/*! \file
    \brief Declaration of the dvar_t

*/


#ifndef __NEOIP_DVAR_T_HPP__
#define __NEOIP_DVAR_T_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_dvar_vapi.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	dvar_int_t;
class	dvar_dbl_t;
class	dvar_str_t;
class	dvar_arr_t;
class	dvar_map_t;

/** \brief a dvar_t of any dvar_type_t
 */ 
class dvar_t : NEOIP_COPY_CTOR_ALLOW {
private:
	dvar_vapi_t *	dvar_vapi;	//!< pointer on the dvar_vapi of this variable
public:
	/*************** ctor/dtor	***************************************/
	dvar_t()	throw();
	~dvar_t()	throw();

	/*************** copy stuff	***************************************/
	dvar_t(const dvar_t &other)			throw();
	dvar_t(const dvar_vapi_t &other_vapi)		throw();
	dvar_t &operator = (const dvar_t & other)	throw();

	/*************** Query function	***************************************/
	bool		is_null()	const throw()	{ return dvar_vapi == NULL;	}
	dvar_type_t	type()		const throw();

	/*************** Comparison function	*******************************/
	int	compare(const dvar_t &other)		const throw();
	bool 	operator == (const dvar_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const dvar_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const dvar_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const dvar_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const dvar_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const dvar_t & other)	const throw()	{ return compare(other) >= 0;	}
		
	/*************** Convertion to subtype	*******************************/
	const dvar_int_t &	integer()	const throw();
	dvar_int_t &		integer()	throw();
	const dvar_dbl_t &	dbl()		const throw();
	dvar_dbl_t &		dbl()		throw();
	const dvar_str_t &	str()		const throw();
	dvar_str_t &		str()		throw();
	const dvar_arr_t &	arr()		const throw();
	dvar_arr_t &		arr()		throw();
	const dvar_map_t &	map()		const throw();
	dvar_map_t &		map()		throw();

	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend std::ostream & operator << ( std::ostream& os, const dvar_t &dvar )
						{ return os << dvar.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_T_HPP__  */



