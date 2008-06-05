/*! \file
    \brief Declaration of the dvar_int_t

*/


#ifndef __NEOIP_DVAR_INT_HPP__ 
#define __NEOIP_DVAR_INT_HPP__ 
/* system include */
/* local include */
#include "neoip_dvar_type.hpp"
#include "neoip_dvar_vapi.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief a dvar_t containing a dvar_type_t::INTEGER
 * 
 * - bencode has the same trouble as xmlrpc aka impossible to determine the length 
 *   or signeness of the number.
 *   - so it is handled as a signed integer of 64bit
 * 
 * - TODO big issue as how big is the integer ? 64bit 16bit ? so likely a good idea to handle it as 64bit
 *   - is it signed or not
 *   - it can be known from the format itself 
 *   - for bittorrent it need to support 64bit
 *   - dont do get in itself... more like get_uint32 get_uint64 etc...
 */ 
class dvar_int_t : NEOIP_COPY_CTOR_ALLOW, public dvar_vapi_t {
private:
	int64_t		value;
	
	/*************** clone() to copy via dvar_vapi_t	***************/
	dvar_vapi_t *	clone()	const throw();
public:
	/*************** ctor/dtor	***************************************/
	dvar_int_t(int64_t newval)	throw()	{ value = newval;	}
	dvar_int_t()			throw()	{}
	~dvar_int_t()			throw()	{}

	/*************** Query function	***************************************/
	int64_t		get()		const throw()	{ return value;			}
	dvar_int_t &	set(int64_t newval) throw()	{ value = newval; return *this;	}
	dvar_type_t	type()		const throw()	{ return dvar_type_t::INTEGER;	}

	/*************** Alias to get the value	per type	***************/
	bool		is_int32_ok()	const throw()	{ return value <= std::numeric_limits<int32_t>::max();	}
	bool		is_uint32_ok()	const throw()	{ return value <= std::numeric_limits<uint32_t>::max();	}
	bool		is_size_t_ok()	const throw()	{ return value <= std::numeric_limits<size_t>::max();	}
	bool		is_int64_ok()	const throw()	{ return value <= std::numeric_limits<int64_t>::max();	}
	bool		is_uint64_ok()	const throw()	{ return true;						}
	size_t		to_size_t()	const throw()	{ DBG_ASSERT( is_size_t_ok() );  return value;		}
	int32_t		to_int32()	const throw()	{ DBG_ASSERT( is_int32_ok() );  return value;		}
	uint32_t	to_uint32()	const throw()	{ DBG_ASSERT( is_uint32_ok() );  return value;		}
	int64_t		to_int64()	const throw()	{ DBG_ASSERT( is_int64_ok() );  return value;		}
	uint64_t	to_uint64()	const throw()	{ DBG_ASSERT( is_uint64_ok() );  return value;		}

	/*************** Comparison function	*******************************/
	int	compare(const dvar_vapi_t &other_vapi)	const throw();
	bool 	operator == (const dvar_int_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const dvar_int_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const dvar_int_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const dvar_int_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const dvar_int_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const dvar_int_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream & operator << ( std::ostream& os, const dvar_int_t &dvar_int )
						{ return os << dvar_int.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_INT_HPP__  */



