/*! \file
    \brief Header of the \ref rate_prec_t
    
*/


#ifndef __NEOIP_RATE_PREC_HPP__ 
#define __NEOIP_RATE_PREC_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for rate_prec_t
 */
class rate_prec_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the precedence when it is not set
	static const size_t	NONE;
	//! the precedence when it is to be the lowest precedence
	static const size_t	LOWEST;
	//! the precedence when it is to be the highest precedence
	static const size_t	HIGHEST;
	//! the precedence by default
	static const size_t	DEFAULT;
private:
	size_t			value;
public:
	/*************** ctor/dtor	***************************************/
	explicit rate_prec_t(size_t value = NONE)	throw() : value(value) {}
	
	/*************** Query function	***************************************/
	bool	is_null()	const throw()	{ return value == NONE;		}
	size_t	to_size_t()	const throw()	{ return value;			}
	double	to_double()	const throw()	{ return value;			}
	
	bool	is_lowest()	const throw()	{ return value == LOWEST;	}
	bool	is_highest()	const throw()	{ return value == HIGHEST;	}
	bool	is_default()	const throw()	{ return value == DEFAULT;	}

	/*************** arithmetic operator	*******************************/
	rate_prec_t &	operator +=(const rate_prec_t &other)	throw()	{ value += other.value; return *this;	}
	rate_prec_t &	operator -=(const rate_prec_t &other)	throw()	{ value -= other.value; return *this;	}
	rate_prec_t &	operator *=(const rate_prec_t &other)	throw()	{ value *= other.value; return *this;	}
	rate_prec_t &	operator /=(const rate_prec_t &other)	throw()	{ value /= other.value; return *this;	}
	rate_prec_t &	operator %=(const rate_prec_t &other)	throw()	{ value %= other.value; return *this;	}
	rate_prec_t	operator + (const rate_prec_t &other)	const throw()
						{ rate_prec_t tmp(*this); tmp += other; return tmp;	}
	rate_prec_t	operator - (const rate_prec_t &other)	const throw()
						{ rate_prec_t tmp(*this); tmp -= other; return tmp;	}	
	rate_prec_t	operator * (const rate_prec_t &other)	const throw()
						{ rate_prec_t tmp(*this); tmp *= other; return tmp;	}
	rate_prec_t	operator / (const rate_prec_t &other)	const throw()
						{ rate_prec_t tmp(*this); tmp /= other; return tmp;	}
						
	/*************** Comparison function	*******************************/
	int	compare(const rate_prec_t &other)	  const throw();
	bool 	operator == (const rate_prec_t & other) const throw() { return compare(other) == 0;	}
	bool 	operator != (const rate_prec_t & other) const throw() { return compare(other) != 0;	}
	bool 	operator <  (const rate_prec_t & other) const throw() { return compare(other) <  0;	}
	bool 	operator <= (const rate_prec_t & other) const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const rate_prec_t & other) const throw() { return compare(other) >  0;	}
	bool 	operator >= (const rate_prec_t & other) const throw() { return compare(other) >= 0;	}
	
	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const rate_prec_t &rate_prec) throw()
				{ return os << rate_prec.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_PREC_HPP__  */










