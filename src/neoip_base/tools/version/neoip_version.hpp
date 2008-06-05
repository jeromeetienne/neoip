/*! \file
    \brief Header of the version_t
    
*/


#ifndef __NEOIP_VERSION_HPP__ 
#define __NEOIP_VERSION_HPP__ 
/* system include */
/* local include */
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief handle version 
 * 
 * - it store/serialize/check the compatibility of version
 */
class version_t : NEOIP_COPY_CTOR_ALLOW {
private:
	uint8_t		v_major;
	uint8_t		v_minor;
public:
	version_t(void)			throw() : v_major(0), v_minor(0)	{};
	version_t(const version_t &other)throw(): v_major(other.v_major), v_minor(other.v_minor){};
	version_t(const char *str)	throw();

	uint8_t		get_major()	const throw() { return v_major;	}
	uint8_t		get_minor()	const throw() { return v_minor;	}
	
	bool		is_compatible(const version_t &other)	const throw();
	
	std::string		to_string()				const throw();
	static version_t	from_string(const std::string &str)	throw();
	
	// comparison operator
	int	compare( const version_t &other )	const throw();
	bool	operator == (const version_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const version_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const version_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const version_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const version_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const version_t &other) const throw() { return compare(other) >= 0; }	
	
friend std::ostream & operator << (std::ostream & os, const version_t &version )
		{ return os << version.to_string();	}
friend serial_t & operator << ( serial_t& serial, const version_t &version )	throw();
friend serial_t & operator >> ( serial_t& serial, version_t &version )		throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_VERSION_HPP__  */



