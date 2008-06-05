/*! \file
    \brief Header of the datum0_t
    
*/


#ifndef __NEOIP_DATUM0_HPP__ 
#define __NEOIP_DATUM0_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

typedef	uint32_t	datum0_flag_t;

/** \brief class definition for datum
 * 
 * - A \ref datum0_t is just a way to manipulate memory blobs and keep track 
 *   of their size.
 * - Let me repeat this layer is just to avoid to always do (void *data, int len), 
 *   but do (\ref datum0_t datum) instead. It doesnt intend to do more.
 */
class datum0_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static const datum0_flag_t NOCOPY	= 1 << 0;
	static const datum0_flag_t SECMEM	= 1 << 1;
	static const datum0_flag_t FLAG_DFL	= 0;
	
private:
	datum0_flag_t	flag_bitfield;
	void		*data;	//!< a pointer on the datum's data
	int		len;	//!< the length of the datum

	void		init( const void *orig_data, int orig_len, datum0_flag_t flag )	throw();
	void		free_data_buffer()						throw();
	static bool		flag_get( datum0_flag_t flag_value, datum0_flag_t flag)	throw();
	static datum0_flag_t	flag_set( datum0_flag_t flag_value, datum0_flag_t flag, bool on = true)
											throw();
public:
	/*************** ctor/dtor ********************************************/
	datum0_t()	throw(): flag_bitfield(FLAG_DFL), data(NULL), len(0) {}
	explicit datum0_t(int len, datum0_flag_t flag = FLAG_DFL)	throw();
	datum0_t(const void *data, int len, datum0_flag_t flag = FLAG_DFL)throw();
	datum0_t(const datum0_t &datum)					throw();
	explicit datum0_t(const char *str)				throw();
	explicit datum0_t(const std::string &str)			throw();
	~datum0_t()							throw();

	/*************** assignement and + operators **************************/
	datum0_t &	operator =  (const datum0_t & other);
	friend datum0_t	operator +  (const datum0_t &datum_a, const datum0_t &datum_b)	throw();
	void		operator += (const datum0_t &other) throw() { *this = *this + other; }
	
	/*************** comparison operator  *********************************/
	int	compare(const datum0_t &other)	const throw();
	bool	operator == (const datum0_t & other) const throw() { return compare(other) == 0;	}
	bool	operator != (const datum0_t & other) const throw() { return compare(other) != 0;	}
	bool	operator <  (const datum0_t & other) const throw() { return compare(other) >  0;	}
	bool	operator <= (const datum0_t & other) const throw() { return compare(other) >= 0;	}
	bool	operator >  (const datum0_t & other) const throw() { return compare(other) <  0;	}
	bool	operator >= (const datum0_t & other) const throw() { return compare(other) <= 0;	}

	//! return true if datum0_t is null
	bool		is_null()			const throw()	{ return data == NULL; }

	/*************** query funtion ****************************************/
	datum0_t	get_range(size_t offset, size_t len, datum0_flag_t flag = FLAG_DFL) const throw();
	//! return a pointer on the data
	void *		get_data()	const throw()	{ return data;		}
	void *		get_ptr()	const throw()	{ return data;		}
	void *		ptr_off(size_t offset)	const throw()	{ return (char *)data + offset;	}
	//! return a length on the data
	size_t		get_len()	const throw()	{ return len;		}
	size_t		size()		const throw()	{ return get_len();	}
	bool		empty()		const throw()	{ return size() == 0;	}
	
	std::string	to_stdstring()	const throw()	{ return std::string((char*)get_ptr(),get_len());}

	//! return true if the datum0_t is secure, false otherwise
	bool		is_secmem()	const throw()	{ return flag_get(flag_bitfield,datum0_t::SECMEM);}
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &	operator << ( std::ostream& os, const datum0_t &datum )	throw();

	/*************** serial_t function	*******************************/
	friend	serial_t &	operator << ( serial_t& serial, const datum0_t &datum )	throw();
	friend	serial_t &	operator >> ( serial_t& serial, datum0_t &datum )	throw(serial_except_t);
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DATUM0_HPP__  */



