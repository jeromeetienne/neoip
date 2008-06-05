/*! \file
    \brief Header of the serial0_t

- see \ref neoip_serial.cpp

*/


#ifndef __NEOIP_SERIAL0_HPP__ 
#define __NEOIP_SERIAL0_HPP__ 
/* system include */
#include <iostream>
#include <glib.h>
/* local include */
#include "neoip_except.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

class serial0_except_t : public neoip_except {
public:	
	serial0_except_t(std::string reason)
			: neoip_except(reason ){}
	serial0_except_t(std::string reason, std::string filename, int line, std::string fctname )
			: neoip_except(reason, filename, line, fctname ){}
};
#define serial0_except_l(reason)		serial0_except_t(reason, __FILE__, __LINE__, __func__)
#define nthrow_serial0_errno(str)	NTHROW serial0_except_l(std::string(str) \
							+ ". errno=" + neoip_strerror(errno) )
#define nthrow_serial0_plain(str)	NTHROW serial0_except_l(std::string(str))

/** \brief class definition for serialization
 */
class serial0_t {
private:
	GByteArray *	buf;
public:
	serial0_t(int reserved_size = 2*1024)	throw();
	serial0_t(const void *data, int len)	throw();
	~serial0_t()				throw();

	void		append( const void *buf_data, int buf_len )	throw();
	void		consume( size_t length )			throw();
	void * const	get_data()	const throw()	{ return (void * const)buf->data; }
	size_t		get_len()	const throw()	{ return buf->len;		}
	void		reset()		throw()		{ consume(get_len());		}


friend	serial0_t& operator << ( serial0_t& serial, const bool &val )	throw();
friend	serial0_t& operator >> ( serial0_t& serial, bool &val )	  	throw(serial0_except_t);

friend	serial0_t& operator << ( serial0_t& serial, const uint8_t &val )	throw();
friend	serial0_t& operator >> ( serial0_t& serial, uint8_t &val )  	throw(serial0_except_t);

friend	serial0_t& operator << ( serial0_t& serial, const uint16_t &val )	throw();
friend	serial0_t& operator >> ( serial0_t& serial, uint16_t &val )  	throw(serial0_except_t);

friend	serial0_t& operator << ( serial0_t& serial, const uint32_t &val )	throw();
friend	serial0_t& operator >> ( serial0_t& serial, uint32_t &val )  	throw(serial0_except_t);

friend	serial0_t& operator << ( serial0_t& serial, const uint64_t &val )	throw();
friend	serial0_t& operator >> ( serial0_t& serial, uint64_t &val )  	throw(serial0_except_t);

};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SERIAL0_HPP__  */



