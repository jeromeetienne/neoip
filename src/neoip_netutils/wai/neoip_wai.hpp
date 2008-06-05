/*! \file
    \brief Header of the wai_t

\par Possible Improvement
- Coded from memory. there is an rfc about it "Serial Number Arithmetic" rfc1982
  - review this code with the rfc - the rfc is more likely be right than this one
  - possibly some reference in the code

*/


#ifndef __NEOIP_WAI_HPP__ 
#define __NEOIP_WAI_HPP__ 
/* system include */
#include <list>
#include <iostream>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_assert.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Warp Around Integer (aka wai) using sequential number arithmetic
 * 
 * - Ts = signed type and Tu = unsigned type
 * - it ASSUMES the difference between 2 numbers is lower than 2^31
 */
template <typename Ts, typename Tu> class wai_t {
private:
	Tu		value;
	//! main comparison function
	Ts wai_cmp(const wai_t<Ts, Tu> & other)	const throw() { return (Ts)(value - other.value);}
public:
	// Constructor with implicit convertion
	wai_t(const Tu value)			throw()	{ this->value = value;		}
	wai_t(const Ts value)			throw()	{ this->value = (Tu)value;	}
	// Constructor
	wai_t()					throw()	{ this->value = 0;		}
	wai_t(const wai_t<Ts, Tu> & other)	throw()	{ value	= other.value;		}

	// operator = for signed and unsigned type
	wai_t<Ts, Tu> & operator = (const Tu uval)	throw()	{ value = uval;		return *this;	}
	wai_t<Ts, Tu> & operator = (const Ts sval)	throw()	{ value = (Tu)sval;	return *this;	}

	// Definition of all the +/- operators
	wai_t<Ts, Tu> operator - (const wai_t<Ts, Tu> & other) const throw()	{ return (Ts)(value - other.value);	}
	wai_t<Ts, Tu> operator + (const wai_t<Ts, Tu> & other) const throw()	{ return (Tu)(value + other.value);	}
	void operator += (const wai_t<Ts, Tu> & other)		throw() { *this = *this + other;}
	void operator -= (const wai_t<Ts, Tu> & other)		throw() { *this = *this - other;}

	wai_t<Ts, Tu> & operator ++ ()		throw()	{ *this += (Tu)1; return *this;	}
	wai_t<Ts, Tu>	operator ++ (int dummy)	throw()	{ wai_t<Ts, Tu> tmp(*this); *this += (Tu)1; return tmp;	}
	wai_t<Ts, Tu> &	operator -- ()		throw()	{ *this -= (Tu)1; return *this;	}
	wai_t<Ts, Tu>	operator -- (int dummy)	throw()	{ wai_t<Ts, Tu> tmp(*this); *this -= (Tu)1; return tmp;	}
	
	// Type conversion operator
	operator Tu()	const	{ return value;		}

	// comparison operator
	bool operator == (const wai_t<Ts, Tu> & other) const throw() { return wai_cmp(other) == 0; }
	bool operator != (const wai_t<Ts, Tu> & other) const throw() { return wai_cmp(other) != 0; }
	bool operator <  (const wai_t<Ts, Tu> & other) const throw() { return wai_cmp(other) <  0; }
	bool operator <= (const wai_t<Ts, Tu> & other) const throw() { return wai_cmp(other) <= 0; }
	bool operator >  (const wai_t<Ts, Tu> & other) const throw() { return wai_cmp(other) >  0; }
	bool operator >= (const wai_t<Ts, Tu> & other) const throw() { return wai_cmp(other) >= 0; }

	Ts	get_max_diff()	const throw()	{ return std::numeric_limits<Ts>::max()-1;	}

	//! serialize a wai_t
	friend serial_t& operator << (serial_t& serial, const wai_t<Ts, Tu> &wai) throw(){
		return serial << wai.value;
	}
	//! unserialize a wai_t
	friend serial_t& operator >> ( serial_t& serial, wai_t<Ts, Tu> &wai )	throw(serial_except_t) {
		serial >> wai.value;
		return serial;
	}
	//! convert the number to a string
	std::string to_string()	const throw() {
		std::ostringstream	oss;
		oss << value;
		return oss.str();
	}
	//! ostream redirection (mainly for debug)
	friend std::ostream & operator << ( std::ostream& os, wai_t<Ts, Tu> &wai ) {
		return os << wai.to_string();
	}
};

// Some predefinitions to ease readability
typedef wai_t<int8_t , uint8_t>		wai8_t;
typedef wai_t<int16_t, uint16_t>	wai16_t;
typedef wai_t<int32_t, uint32_t>	wai32_t;
typedef wai_t<int64_t, uint64_t>	wai64_t;

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_WAI_HPP__  */



