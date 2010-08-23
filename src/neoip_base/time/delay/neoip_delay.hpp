/*! \file
    \brief Header of the neoip_delay
    
*/


#ifndef __NEOIP_DELAY_HPP__ 
#define __NEOIP_DELAY_HPP__ 
/* system include */
#include <sys/time.h>
#include <stdint.h>
#include <limits>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_namespace.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN

class delay_t : NEOIP_COPY_CTOR_ALLOW {
public:
	// constant to use delay_t in static ctor - use delay_t(INFINITE_VAL) instead of delay_t::INFINITE
	static const uint64_t	INFINITE_VAL	= 0xFFFFFFFFFFFFFFEULL;
	static const uint64_t	NONE_VAL	= 0xFFFFFFFFFFFFFFFULL;
	// constant which MUST NOT be used for static ctor - due to static ctor order issue
	static const delay_t	INFINITE;	//!< when delay_t is infinite
	static const delay_t	NONE;		//!< when delay_t is null
	static const delay_t	ALWAYS;		//!< only for backward compatibility
	static const delay_t	NEVER;		//!< only for backward compatibility
private:
	uint64_t	val_ms;	//!< nb millisend since epoch (jan 1 1970)
public:
	/*************** ctor/dtor	***************************************/
	delay_t()					throw() : val_ms(NONE_VAL)	{}
	explicit delay_t(uint64_t val_ms)		throw() : val_ms(val_ms)	{}
	~delay_t()					throw()				{}
	delay_t(const delay_t & other)			throw() : val_ms(other.val_ms)	{};
	delay_t &operator = (const delay_t & other)	throw()	{ val_ms = other.val_ms; return *this;	}

	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return *this == NONE;		}
	bool		is_infinite()	const throw()	{ return *this == INFINITE;	}

	/*************** action function	*******************************/
	delay_t		a_bit_less(const delay_t &delta_max = delay_t::from_sec(1)
						, double delta_ratio = 0.8)	const throw();

	/*************** compatibility layer	*******************************/
	bool		is_always()	const throw()	{ return *this == ALWAYS;		}
	bool		is_never()	const throw()	{ return *this == NEVER;		}
	bool		is_special()	const throw()	{ return is_always() || is_never();	}

	/*************** arithmetic operator	*******************************/
	delay_t &	operator +=(const delay_t &other)	throw();
	delay_t &	operator -=(const delay_t &other)	throw();
	delay_t &	operator *=(const delay_t &other)	throw();
	delay_t &	operator /=(const delay_t &other)	throw();
	delay_t &	operator %=(const delay_t &other)	throw();
	delay_t		operator + (const delay_t &other)	const throw()	{ delay_t tmp(*this); tmp += other; return tmp;	}
	delay_t		operator - (const delay_t &other)	const throw()	{ delay_t tmp(*this); tmp -= other; return tmp;	}	
	delay_t		operator * (const delay_t &other)	const throw()	{ delay_t tmp(*this); tmp *= other; return tmp;	}
	delay_t		operator / (const delay_t &other)	const throw()	{ delay_t tmp(*this); tmp /= other; return tmp;	}	
	delay_t		operator % (const delay_t &other)	const throw()	{ delay_t tmp(*this); tmp %= other; return tmp;	}	
	delay_t&	operator ++()				throw()		{ *this += delay_t(1); return *this;			}
	delay_t		operator ++(int dummy)			throw()		{ delay_t tmp(*this); *this += delay_t(1); return tmp;	}
	delay_t&	operator --()				throw()		{ *this -= delay_t(1); return *this;			}
	delay_t		operator --(int dummy)			throw()		{ delay_t tmp(*this); *this -= delay_t(1); return tmp;	}
	delay_t		operator * (const double   &factor)	const throw();
	
	/*************** convertion function	*******************************/
	uint32_t	to_msec_32bit()	  const throw() { return val_ms;			}
	uint32_t	to_sec_32bit()	  const throw()	{ return (val_ms+500ULL)/1000ULL;	}
	double		to_msec_double()  const throw() { return (double)val_ms;		}
	double		to_sec_double()	  const throw()	{ return double(val_ms)/1000.0;		}
	uint64_t	to_uint64()	  const throw() { return val_ms;			}
	uint32_t	to_uint32()	  const throw() { return val_ms;			}
	double		to_double()	  const throw() { return (double)val_ms;		}
	static delay_t	from_msec(uint64_t ms)  throw()	{ return delay_t(ms);			}
	static delay_t	from_sec(uint64_t sec)  throw()	{ return from_msec(sec * 1000ULL);	}
	static delay_t	from_min(uint64_t min)  throw()	{ return from_sec(min * 60ULL);		}

	/*************** comparison operator	*******************************/
	int	compare(const delay_t &other)		const throw();
	bool	operator ==(const delay_t & other)	const throw()	{ return val_ms == other.val_ms;}
	bool	operator !=(const delay_t & other)	const throw()	{ return val_ms != other.val_ms;}
	bool	operator < (const delay_t & other)	const throw()	{ return val_ms <  other.val_ms;}
	bool	operator <=(const delay_t & other)	const throw()	{ return val_ms <= other.val_ms;}
	bool	operator > (const delay_t & other)	const throw()	{ return val_ms >  other.val_ms;}
	bool	operator >=(const delay_t & other)	const throw()	{ return val_ms >= other.val_ms;}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream & operator << (std::ostream & oss, const delay_t & delay)	throw()
						{ return oss << delay.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const delay_t &delay)	throw();
	friend	serial_t& operator >> (serial_t& serial, delay_t &delay)	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAY_HPP__  */



