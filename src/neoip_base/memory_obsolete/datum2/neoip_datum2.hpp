/*! \file
    \brief Header of the datum2_t
    
*/


#ifndef __NEOIP_DATUM2_HPP__ 
#define __NEOIP_DATUM2_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_datum2_flag.hpp"
#include "neoip_serial2_except.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

#if 0
#	include "neoip_serial.hpp"	// TODO to remove it is just for the transition
#endif

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for datum2
 * 
 * - A \ref datum2_t is just a way to manipulate memory blobs and keep track 
 *   of their size.
 * - Let me repeat this layer is just to avoid to always do (void *data, int len), 
 *   but do (\ref datum2_t datum2) instead. It doesnt intend to do more.
 */
class datum2_t : NEOIP_COPY_CTOR_ALLOW {
public:	// constant copied from datum2_flag_t for easier usage
	static const datum2_flag_t NOCOPY;	//!< not to copy the data while creating (IS NOT inherited)
	static const datum2_flag_t SECMEM;	//!< store the data in 'secure memory' (IS inherited)
	static const datum2_flag_t FLAG_DFL;	//!< the default datum2_flag_t
private:
	datum2_flag_t	flag;	//!< the datum2_flag_t for this datum2_t
	void *		data;	//!< a pointer on the datum2's data
	int		len;	//!< the length of the datum2


	/**************	internal function	*******************************/
	void	internal_ctor(const void *orig_data, int orig_len, datum2_flag_t flag)	throw();
	void	internal_dtor()								throw();
public:
	/*************** ctor/dtor ********************************************/
	datum2_t()					throw(): flag(FLAG_DFL), data(NULL), len(0) {}
	datum2_t(const void *data, int len, const datum2_flag_t &flag = FLAG_DFL)
							throw()	{ internal_ctor(data, len, flag);	}
	~datum2_t()					throw()	{ internal_dtor();			}
	explicit datum2_t(int len, const datum2_flag_t &flag = FLAG_DFL)
							throw()	{ internal_ctor(NULL, len, flag);	}
	explicit datum2_t(const char *str)		throw()	{ internal_ctor(str, strlen(str), FLAG_DFL);		}
	explicit datum2_t(const std::string &str)	throw()	{ internal_ctor(str.c_str(), str.size(), FLAG_DFL);	}

	/*************** copy operator	***************************************/
	datum2_t(const datum2_t &other)			throw()	{ internal_ctor(other.data, other.len, other.flag.inheritance() );}
	datum2_t & operator = (const datum2_t & other)	throw();
	void	swap(datum2_t &other)			throw();

	/*************** arithmetic operator	*******************************/
	datum2_t &	operator +=(const datum2_t &other)	throw();
	datum2_t	operator + (const datum2_t &other)	const throw()
						{ datum2_t tmp(*this); tmp += other; return tmp;	}

	/*************** comparison operator  *********************************/
	int	compare(const datum2_t &other)		const throw();
	bool	operator == (const datum2_t & other)	const throw() { return compare(other) == 0;	}
	bool	operator != (const datum2_t & other)	const throw() { return compare(other) != 0;	}
	bool	operator <  (const datum2_t & other)	const throw() { return compare(other) >  0;	}
	bool	operator <= (const datum2_t & other)	const throw() { return compare(other) >= 0;	}
	bool	operator >  (const datum2_t & other)	const throw() { return compare(other) <  0;	}
	bool	operator >= (const datum2_t & other)	const throw() { return compare(other) <= 0;	}

	/*************** query funtion ****************************************/
	bool		is_null()			const throw()	{ return data == NULL;		}
	bool		empty()				const throw()	{ return length() == 0;		}
	size_t		length()			const throw()	{ return len;			}
	uint8_t *	uint8_ptr(size_t offset = 0)	const throw()	{ return (uint8_t*)data +offset;}
	char *		char_ptr(size_t offset = 0)	const throw()	{ return (char *)data + offset;	}
	void *		void_ptr(size_t offset = 0)	const throw()	{ return uint8_ptr(offset);	}

	/**************	datum2_flag_t testing	*******************************/
	bool		is_secmem()			const throw()	{ return flag.is_secmem();	}
	bool		is_nocopy()			const throw()	{ return flag.is_nocopy();	}

	/**************	convertion function	*******************************/
	std::string	to_stdstring()			const throw()	{ return std::string(char_ptr(),length());	}
	datum2_t	range(size_t offset, size_t len, const datum2_flag_t &flag = FLAG_DFL) const throw();
	
#if 1
	/**************	bad API for compatibility	***********************/
	void *		get_data()		const throw()	{ return data;		}
	void *		get_ptr()		const throw()	{ return data;		}
	void *		ptr_off(size_t offset)	const throw()	{ return (char *)data + offset;	}
	size_t		get_len()		const throw()	{ return length();	}
	size_t		size()			const throw()	{ return length();	}
	datum2_t	get_range(size_t offset, size_t len, const datum2_flag_t &flag = FLAG_DFL) const throw()
						{ return range(offset, len, flag);	}
#endif
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream& oss, const datum2_t &datum2)	throw();

	/*************** serial_t function	*******************************/
	friend serial2_t & operator << (serial2_t& serial, const datum2_t &datum)	throw();
	friend serial2_t & operator >> (serial2_t& serial, datum2_t &datum)		throw(serial2_except_t);

#if 0
	/*************** serial_t function	*******************************/
	friend	serial_t & operator << (serial_t& serial, const datum2_t &datum)	throw();
	friend	serial_t & operator >> ( serial_t& serial, datum2_t &datum )		throw(serial_except_t);
#endif
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DATUM2_HPP__  */



