/*! \file
    \brief Header of the neoip_delay
    
*/


#ifndef __NEOIP_DELAY2_HPP__ 
#define __NEOIP_DELAY2_HPP__ 
/* system include */
#include <sys/time.h>
#include <stdint.h>
#include <limits>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_namespace.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN

class delay2_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static const delay2_t	INFINITE;	//!< when delay2_t is infinite
	static const delay2_t	NONE;		//!< when delay2_t is null
	static const delay2_t	ALWAYS;		//!< only for backward compatibility
	static const delay2_t	NEVER;		//!< only for backward compatibility
private:
	uint64_t	val_ms;	//!< nb millisend since epoch (jan 1 1970)
public:
	/*************** ctor/dtor	***************************************/
	delay2_t()					throw() : val_ms(NONE.val_ms)	{}
	explicit delay2_t(uint64_t val_ms)		throw() : val_ms(val_ms){}
	~delay2_t()					throw()	{}
	delay2_t(const delay2_t & other)		throw() : val_ms(other.val_ms){};
	delay2_t &operator = (const delay2_t & other)	throw()	{ val_ms = other.val_ms; return *this;	}

	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return *this == NONE;		}
	bool		is_infinite()	const throw()	{ return *this == INFINITE;	}

	/*************** action function	*******************************/
	delay2_t	a_bit_less(const delay2_t &delta_max = delay2_t::from_sec(1)
						, double delta_ratio = 0.8)	const throw();

	/*************** compatibility layer	*******************************/
	bool		is_always()	const throw()	{ return *this == ALWAYS;		}
	bool		is_never()	const throw()	{ return *this == NEVER;		}
	bool		is_special()	const throw()	{ return is_always() || is_never();	}

	/*************** arithmetic operator	*******************************/
	delay2_t &	operator +=(const delay2_t &other)	throw();
	delay2_t &	operator -=(const delay2_t &other)	throw();
	delay2_t &	operator *=(const delay2_t &other)	throw();
	delay2_t &	operator /=(const delay2_t &other)	throw();
	delay2_t &	operator %=(const delay2_t &other)	throw();
	delay2_t	operator + (const delay2_t &other)	const throw()	{ delay2_t tmp(*this); tmp += other; return tmp;	}
	delay2_t	operator - (const delay2_t &other)	const throw()	{ delay2_t tmp(*this); tmp -= other; return tmp;	}	
	delay2_t	operator * (const delay2_t &other)	const throw()	{ delay2_t tmp(*this); tmp *= other; return tmp;	}
	delay2_t	operator / (const delay2_t &other)	const throw()	{ delay2_t tmp(*this); tmp /= other; return tmp;	}	
	delay2_t	operator % (const delay2_t &other)	const throw()	{ delay2_t tmp(*this); tmp %= other; return tmp;	}	
	delay2_t&	operator ++()				throw()		{ *this += delay2_t(1); return *this;			}
	delay2_t	operator ++(int dummy)			throw()		{ return *this + delay2_t(1);				}
	delay2_t&	operator --()				throw()		{ *this -= delay2_t(1); return *this;			}
	delay2_t	operator --(int dummy)			throw()		{ return *this - delay2_t(1);				}
	delay2_t	operator * (const double   &factor)	const throw();
	
	/*************** convertion function	*******************************/
	uint32_t	to_msec_32bit()	  const throw() { return val_ms;			}
	uint32_t	to_sec_32bit()	  const throw()	{ return (val_ms+500ULL)/1000ULL;	}
	double		to_msec_double()  const throw() { return (double)val_ms;		}
	double		to_sec_double()	  const throw()	{ return double(val_ms)/1000.0;		}
	uint64_t	to_uint64()	  const throw() { return val_ms;			}
	uint32_t	to_uint32()	  const throw() { return val_ms;			}
	double		to_double()	  const throw() { return (double)val_ms;		}
	static delay2_t	from_msec(uint64_t ms)  throw()	{ return delay2_t(ms);			}
	static delay2_t	from_sec(uint64_t sec)  throw()	{ return from_msec(sec * 1000ULL);	}
	static delay2_t	from_min(uint64_t min)  throw()	{ return from_sec(min * 60ULL);		}

	/*************** comparison operator	*******************************/
	bool	operator ==(const delay2_t & other)	const throw()	{ return val_ms == other.val_ms;}
	bool	operator !=(const delay2_t & other)	const throw()	{ return val_ms != other.val_ms;}
	bool	operator < (const delay2_t & other)	const throw()	{ return val_ms <  other.val_ms;}
	bool	operator <=(const delay2_t & other)	const throw()	{ return val_ms <= other.val_ms;}
	bool	operator > (const delay2_t & other)	const throw()	{ return val_ms >  other.val_ms;}
	bool	operator >=(const delay2_t & other)	const throw()	{ return val_ms >= other.val_ms;}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream & operator << (std::ostream & oss, const delay2_t & delay)	throw()
						{ return oss << delay.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const delay2_t &delay)	throw();
	friend	serial_t& operator >> (serial_t& serial, delay2_t &delay)	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAY2_HPP__  */



