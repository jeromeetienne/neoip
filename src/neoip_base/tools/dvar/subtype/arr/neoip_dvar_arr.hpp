/*! \file
    \brief Declaration of the dvar_arr_t

*/


#ifndef __NEOIP_DVAR_ARR_HPP__ 
#define __NEOIP_DVAR_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_dvar_t.hpp"
#include "neoip_dvar_type.hpp"
#include "neoip_dvar_vapi.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief a dvar_t containing a dvar_type_t::INTEGER
 */ 
class dvar_arr_t : NEOIP_COPY_CTOR_ALLOW, public dvar_vapi_t {
private:
	std::vector<dvar_t>	array;
	
	/*************** clone() to copy via dvar_vapi_t	***************/
	dvar_vapi_t *	clone()	const throw();
public:
	/*************** ctor/dtor	***************************************/
	dvar_arr_t()		throw()	{}
	~dvar_arr_t()		throw()	{}

	/*************** Query function	***************************************/
	dvar_type_t	type()	const throw()	{ return dvar_type_t::ARRAY;	}
	size_t		size()	const throw()	{ return array.size();		}
	size_t		empty()	const throw()	{ return array.empty();		}
	const dvar_t &	operator[](size_t idx)	const throw()	{ DBG_ASSERT( idx < size() );
								  return array[idx];	}
	
	/*************** Action function	*******************************/
	void		operator += (const dvar_t &dvar)throw() { array.push_back(dvar);		}
	dvar_arr_t	append(const dvar_t &dvar)	throw()	{ *this += dvar; return *this;		}							  
	dvar_t &	operator[](size_t idx)	throw()		{ DBG_ASSERT( idx < size() );
								  return array[idx];	}

	/*************** Comparison function	*******************************/
	int	compare(const dvar_vapi_t &other_vapi)	const throw();
	bool 	operator == (const dvar_arr_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const dvar_arr_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const dvar_arr_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const dvar_arr_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const dvar_arr_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const dvar_arr_t & other)	const throw()	{ return compare(other) >= 0;	}
		
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream & operator << ( std::ostream& os, const dvar_arr_t &dvar_arr )
						{ return os << dvar_arr.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_ARR_HPP__  */



