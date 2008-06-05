/*! \file
    \brief Declaration of the dvar_map_t

*/


#ifndef __NEOIP_DVAR_MAP_HPP__ 
#define __NEOIP_DVAR_MAP_HPP__ 
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
class dvar_map_t : NEOIP_COPY_CTOR_ALLOW, public dvar_vapi_t {
private:
	std::map<std::string, dvar_t>	map;

	/*************** clone() to copy via dvar_vapi_t	***************/	
	dvar_vapi_t *	clone()	const throw();
public:
	/*************** ctor/dtor	***************************************/
	dvar_map_t()		throw()	{}
	~dvar_map_t()		throw()	{}

	/*************** Query function	***************************************/
	dvar_type_t	type()	const throw()	{ return dvar_type_t::MAP;	}
	size_t		size()	const throw()	{ return map.size();		}
	size_t		empty()	const throw()	{ return map.empty();		}
	bool		contain(const std::string &key
				, const dvar_type_t &dvar_type = dvar_type_t::NONE) const throw();
	const dvar_t &	operator[](const std::string &key)	const throw();

	/*************** Action function	*******************************/
	dvar_map_t &	insert(const std::string &key, const dvar_t &dvar)	throw();				  
	dvar_t &	operator[](const std::string &key)	throw();
	const std::map<std::string, dvar_t> &	get_inmap()	const throw()	{ return map;	}

	/*************** Comparison function	*******************************/
	int	compare(const dvar_vapi_t &other_vapi)	const throw();
	bool 	operator == (const dvar_map_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const dvar_map_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const dvar_map_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const dvar_map_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const dvar_map_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const dvar_map_t & other)	const throw()	{ return compare(other) >= 0;	}
		
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream & operator << ( std::ostream& os, const dvar_map_t &dvar_map )
						{ return os << dvar_map.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_MAP_HPP__  */



