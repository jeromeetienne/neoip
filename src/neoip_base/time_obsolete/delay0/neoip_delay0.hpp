/*! \file
    \brief Header of the neoip_delay
    
*/


#ifndef __NEOIP_DELAY0_HPP__ 
#define __NEOIP_DELAY0_HPP__ 
/* system include */
#include <sys/time.h>
#include <stdint.h>
#include <limits>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_namespace.hpp"
#include "neoip_copy_ctor_checker.hpp"
NEOIP_NAMESPACE_BEGIN

class delay0_t : NEOIP_COPY_CTOR_ALLOW {
public:	
	static const uint64_t	NONE	= 0xFFFFFFFFFFFFFFFULL;
	//!< Symbolize the ALWAYS case (MUST be greater than NEVER)
	static const uint64_t	ALWAYS	= 0xFFFFFFFFFFFFFFEULL;
	//!< Symbolize the NEVER case (MUST be less than ALWAYS)
	static const uint64_t	NEVER	= 0xFFFFFFFFFFFFFFDULL;
	
	static const uint64_t	USEC_PER_SEC = 1000000ULL;
private:
	uint64_t	val_ms;	//!< nb ms since epoch
public:
	// ctor/dtor
	delay0_t()					throw() : val_ms(NONE) {}
	// TODO this should be explicit
	delay0_t(uint64_t val_ms)			throw() : val_ms(val_ms) {}
	~delay0_t()					throw()	{}
	delay0_t(const delay0_t & other)			throw() : val_ms(other.val_ms){};
	delay0_t &operator = (const delay0_t & other)	throw()	{ val_ms = other.val_ms; return *this;	}
	

	uint32_t	to_msec_32bit()	  const throw() { return val_ms;			}
	uint32_t	to_sec_32bit()	  const throw()	{ return (val_ms+500ULL)/1000ULL;	}
	double		to_msec_double()  const throw() { return (double)val_ms;		}
	double		to_sec_double()	  const throw()	{ return double(val_ms)/1000.0;		}
	uint64_t	to_uint64()	  const throw() { return val_ms;			}
	uint32_t	to_uint32()	  const throw() { return val_ms;			}
	double		to_double()	  const throw() { return (double)val_ms;		}
	static delay0_t	from_msec(uint64_t ms)  throw()	{ return delay0_t(ms);			}
	static delay0_t	from_sec(uint64_t sec)  throw()	{ return from_msec(sec * 1000ULL);	}
	static delay0_t	from_min(uint64_t min)  throw()	{ return from_sec(min * 60ULL);		}

	// utility function
	bool	is_null()	const throw() { return val_ms == NONE;		}
	void	nullify()	      throw() { val_ms = NONE;			}
	bool	is_always()	const throw() { return val_ms == ALWAYS;	}
	bool	is_never()	const throw() { return val_ms == NEVER;		}
	bool	is_special()	const throw() { return is_always() || is_never(); };
	delay0_t	a_bit_less(const delay0_t &delta_max = delay0_t::from_sec(1)
						, double delta_ratio = 0.8)	const throw();
	
	// operator + and -
	delay0_t	operator+ (const delay0_t & other)	const;
	delay0_t	operator- (const delay0_t & other)	const;
	delay0_t	operator/ (const delay0_t & other)	const;
	delay0_t	operator* (const delay0_t & other)	const;
	delay0_t	operator* (const double    factor)	const;
	delay0_t	operator% (const delay0_t & other)	const;
	void		operator+=(const delay0_t & other)	throw() { *this = *this + other; }
	void		operator-=(const delay0_t & other)	throw() { *this = *this - other; }	
	delay0_t&	operator ++ ()				throw()	{ *this += delay0_t(1); return *this;		 	}
	delay0_t	operator ++ (int dummy)			throw()	{ delay0_t tmp(*this); *this += delay0_t(1); return tmp;	}
	delay0_t&	operator -- ()				throw()	{ *this -= delay0_t(1); return *this;			}
	delay0_t	operator -- (int dummy)			throw()	{ delay0_t tmp(*this); *this += delay0_t(1); return tmp;	}

	/*************** comparison operator	*******************************/
	int	compare(const delay0_t &other)	    const throw();
	bool	operator ==(const delay0_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const delay0_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const delay0_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const delay0_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const delay0_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const delay0_t & other)  const throw(){ return compare(other) >= 0; }

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream & operator << (std::ostream & os, const delay0_t & delay)	throw()
					{ return os << delay.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t& operator << ( serial_t& serial, const delay0_t &delay )throw();
	friend	serial_t& operator >> ( serial_t& serial, delay0_t &delay )	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAY0_HPP__  */



