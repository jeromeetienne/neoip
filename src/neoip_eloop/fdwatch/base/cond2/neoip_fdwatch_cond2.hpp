/*! \file
    \brief Header of the datum_t
    
*/


#ifndef __NEOIP_FDWATCH_COND2_HPP__ 
#define __NEOIP_FDWATCH_COND2_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for fdwatch_cond2_t
 */
class fdwatch_cond2_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static const fdwatch_cond2_t NONE;	//!< the default fdwatch_cond2_t
	static const fdwatch_cond2_t INPUT;	//!< to watch the input on this fd
	static const fdwatch_cond2_t OUTPUT;	//!< to watch the output on this fd
	static const fdwatch_cond2_t ERROR;	//!< to watch the error on this fd
private:
	uint32_t	flag_value;	//!< the value of the fdwatch_cond2_t
public:
	/**************	ctor/dtor ********************************************/
	fdwatch_cond2_t()				throw()	: flag_value(NONE.to_uint32())	{}
	explicit fdwatch_cond2_t(uint32_t value)	throw()	: flag_value(value)		{}

	/**************	query function	***************************************/
	uint32_t	to_uint32()		const throw()	{ return flag_value;	}
	bool		fully_include(const fdwatch_cond2_t &other)	const throw()
					{ return (to_uint32() & other.to_uint32()) == other.to_uint32();}

	/**************	alias to query each flag	***********************/
	bool		is_null()		const throw()	{ return fully_include(NONE);		}
	bool		is_input()		const throw()	{ return fully_include(INPUT);		}
	bool		is_output()		const throw()	{ return fully_include(OUTPUT);		}
	bool		is_error()		const throw()	{ return fully_include(ERROR);		}

	/*************** arithmetic operator	*******************************/
	fdwatch_cond2_t&operator ~ ()		throw()		{ flag_value = ~flag_value; return *this;}
	fdwatch_cond2_t&operator |=(const fdwatch_cond2_t &other)	throw();
	fdwatch_cond2_t	operator | (const fdwatch_cond2_t &other)	const throw()
						{ fdwatch_cond2_t tmp(*this); tmp |= other; return tmp;	}
	fdwatch_cond2_t&operator &=(const fdwatch_cond2_t &other)	throw();
	fdwatch_cond2_t	operator & (const fdwatch_cond2_t &other)	const throw()
						{ fdwatch_cond2_t tmp(*this); tmp &= other; return tmp;	}
	fdwatch_cond2_t&operator ^=(const fdwatch_cond2_t &other)	throw();
	fdwatch_cond2_t	operator ^ (const fdwatch_cond2_t &other)	const throw()
						{ fdwatch_cond2_t tmp(*this); tmp &= other; return tmp;	}

	/*************** comparison operator  *********************************/
	bool	operator == (const fdwatch_cond2_t & other) const throw() { return to_uint32() == other.to_uint32();}
	bool	operator != (const fdwatch_cond2_t & other) const throw() { return to_uint32() != other.to_uint32();}
	bool	operator >  (const fdwatch_cond2_t & other) const throw() { return to_uint32() >  other.to_uint32();}
	bool	operator >= (const fdwatch_cond2_t & other) const throw() { return to_uint32() >= other.to_uint32();}
	bool	operator <  (const fdwatch_cond2_t & other) const throw() { return to_uint32() <  other.to_uint32();}
	bool	operator <= (const fdwatch_cond2_t & other) const throw() { return to_uint32() <= other.to_uint32();}

	/*************** Compatibility layer	*******************************/
	// TODO to remove and use boolean function
	fdwatch_cond2_t &set_flag(const fdwatch_cond_t &other, bool on)	throw()
	{
		*this &= ~fdwatch_cond_t(other);
		if( on )	*this |= other;
		return *this;
	}
	fdwatch_cond2_t &set_input(bool on=true)	throw()	{ return set_flag(INPUT, on);	}
	fdwatch_cond2_t &set_output(bool on=true)	throw()	{ return set_flag(OUTPUT, on);	}
	fdwatch_cond2_t &set_error(bool on=true)	throw()	{ return set_flag(ERROR, on);	}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream& oss, const fdwatch_cond2_t &datum_flag)throw()
					{ return oss << datum_flag.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FDWATCH_COND2_HPP__  */



