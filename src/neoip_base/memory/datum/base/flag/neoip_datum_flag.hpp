/*! \file
    \brief Header of the datum_t
    
*/


#ifndef __NEOIP_DATUM_FLAG_HPP__ 
#define __NEOIP_DATUM_FLAG_HPP__ 
/* system include */
#include <iostream>
#include <stdint.h>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for datum_flag_t
 */
class datum_flag_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static const datum_flag_t NOCOPY;	//!< not to copy the data while creating (IS NOT inherited)
	static const datum_flag_t SECMEM;	//!< store the data in 'secure memory' (IS inherited)
	static const datum_flag_t FLAG_DFL;	//!< the default datum_flag_t
private:
	uint32_t	flag_value;	//!< the value of the datum_flag_t
public:
	/**************	ctor/dtor ********************************************/
	datum_flag_t()				throw()	: flag_value(FLAG_DFL.to_uint32())	{}
	explicit datum_flag_t(uint32_t value)	throw()	: flag_value(value)			{}

	/**************	query function	***************************************/
	uint32_t	to_uint32()		const throw()	{ return flag_value;	}
	datum_flag_t	inheritance()		const throw();
	bool		fully_include(const datum_flag_t &other)	const throw()
					{ return (to_uint32() & other.to_uint32()) == other.to_uint32();}

	/**************	alias to query each flag	***********************/
	bool		is_secmem()		const throw()	{ return fully_include(SECMEM);		}
	bool		is_nocopy()		const throw()	{ return fully_include(NOCOPY);		}

	/*************** arithmetic operator	*******************************/
	datum_flag_t&	operator |=(const datum_flag_t &other)	throw();
	datum_flag_t	operator | (const datum_flag_t &other)	const throw()
						{ datum_flag_t tmp(*this); tmp |= other; return tmp;	}
	datum_flag_t&	operator &=(const datum_flag_t &other)	throw();
	datum_flag_t	operator & (const datum_flag_t &other)	const throw()
						{ datum_flag_t tmp(*this); tmp &= other; return tmp;	}
	datum_flag_t&	operator ^=(const datum_flag_t &other)	throw();
	datum_flag_t	operator ^ (const datum_flag_t &other)	const throw()
						{ datum_flag_t tmp(*this); tmp &= other; return tmp;	}

	/*************** comparison operator  *********************************/
	bool	operator == (const datum_flag_t & other) const throw() { return to_uint32() == other.to_uint32();}
	bool	operator != (const datum_flag_t & other) const throw() { return to_uint32() != other.to_uint32();}
	bool	operator >  (const datum_flag_t & other) const throw() { return to_uint32() >  other.to_uint32();}
	bool	operator >= (const datum_flag_t & other) const throw() { return to_uint32() >= other.to_uint32();}
	bool	operator <  (const datum_flag_t & other) const throw() { return to_uint32() <  other.to_uint32();}
	bool	operator <= (const datum_flag_t & other) const throw() { return to_uint32() <= other.to_uint32();}
				
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream& oss, const datum_flag_t &datum_flag)throw()
					{ return oss << datum_flag.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DATUM_FLAG_HPP__  */



