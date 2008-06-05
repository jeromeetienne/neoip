/*! \file
    \brief Header of the datum_t
    
*/


#ifndef __NEOIP_DATUM_HPP__ 
#define __NEOIP_DATUM_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_datum_flag.hpp"
#include "neoip_serial_except.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for datum
 * 
 * - A \ref datum_t is just a way to manipulate memory blobs and keep track 
 *   of their size.
 * - Let me repeat this layer is just to avoid to always do (void *data, int len), 
 *   but do (\ref datum_t datum) instead. It doesnt intend to do more.
 * - when datum_t is null, 
 */
class datum_t : NEOIP_COPY_CTOR_ALLOW {
public:	// constant copied from datum_flag_t for easier usage
	static const datum_flag_t NOCOPY;	//!< not to copy the data while creating (IS NOT inherited)
	static const datum_flag_t SECMEM;	//!< store the data in 'secure memory' (IS inherited)
	static const datum_flag_t FLAG_DFL;	//!< the default datum_flag_t
private:
	datum_flag_t	flag;	//!< the datum_flag_t for this datum_t
	void *		data;	//!< a pointer on the datum's data (== NULL if is_null())
	size_t		len;	//!< the length of the datum (== 0 if is null())

	/**************	internal function	*******************************/
	void	internal_ctor(const void *orig_data, size_t orig_len, datum_flag_t flag)throw();
	void	internal_dtor()								throw();
public:
	/*************** ctor/dtor ********************************************/
	datum_t()					throw(): flag(FLAG_DFL), data(NULL), len(0) {}
	datum_t(const void *data, size_t len, const datum_flag_t &flag = FLAG_DFL)
							throw()	{ internal_ctor(data, len, flag);	}
	~datum_t()					throw()	{ internal_dtor();			}
	explicit datum_t(size_t len, const datum_flag_t &flag = FLAG_DFL)
							throw()	{ internal_ctor(NULL, len, flag);	}
	explicit datum_t(const char *str)		throw()	{ internal_ctor(str, strlen(str), FLAG_DFL);		}
	explicit datum_t(const std::string &str)	throw()	{ internal_ctor(str.c_str(), str.size(), FLAG_DFL);	}

	/*************** copy operator	***************************************/
	datum_t(const datum_t &other)			throw()	{ internal_ctor(other.data, other.len, other.flag.inheritance() );}
	datum_t&operator = (const datum_t & other)	throw();
	void	swap(datum_t &other)			throw();

	/*************** arithmetic operator	*******************************/
	datum_t&operator +=(const datum_t &other)	throw();
	datum_t	operator + (const datum_t &other)	const throw()
						{ datum_t tmp(*this); tmp += other; return tmp;	}

	/*************** comparison operator  *********************************/
	int	compare(const datum_t &other)		const throw();
	bool	operator == (const datum_t & other)	const throw() { return compare(other) == 0;	}
	bool	operator != (const datum_t & other)	const throw() { return compare(other) != 0;	}
	bool	operator <  (const datum_t & other)	const throw() { return compare(other) >  0;	}
	bool	operator <= (const datum_t & other)	const throw() { return compare(other) >= 0;	}
	bool	operator >  (const datum_t & other)	const throw() { return compare(other) <  0;	}
	bool	operator >= (const datum_t & other)	const throw() { return compare(other) <= 0;	}

	/*************** query funtion ****************************************/
	bool		is_null()			const throw()	{ return data == NULL;		}
	bool		empty()				const throw()	{ return length() == 0;		}
	size_t		length()			const throw()	{ return len;			}
	uint8_t *	uint8_ptr(size_t offset = 0)	const throw()	{ return (uint8_t*)data +offset;}
	char *		char_ptr(size_t offset = 0)	const throw()	{ return (char *)data + offset;	}
	void *		void_ptr(size_t offset = 0)	const throw()	{ return uint8_ptr(offset);	}

	/**************	datum_flag_t testing	*******************************/
	bool		is_secmem()			const throw()	{ return flag.is_secmem();	}
	bool		is_nocopy()			const throw()	{ return flag.is_nocopy();	}

	/**************	convertion function	*******************************/
	std::string	to_stdstring()			const throw()	{ return std::string(char_ptr(),length());	}
	datum_t		range(size_t offset, size_t len, const datum_flag_t &flag = FLAG_DFL)
							const throw();
	
#if 1
	/**************	bad API for compatibility	***********************/
	void *		get_data()		const throw()	{ return data;				}
	void *		get_ptr()		const throw()	{ return data;				}
	void *		ptr_off(size_t offset)	const throw()	{ return (char *)data + offset;		}
	size_t		get_len()		const throw()	{ return length();			}
	size_t		size()			const throw()	{ return length();			}
	datum_t		get_range(size_t offset, size_t len, const datum_flag_t &flag = FLAG_DFL)
						const throw()	{ return range(offset, len, flag);	}
#endif
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream& oss, const datum_t &datum)	throw();

	/*************** serial_t function	*******************************/
	friend	serial_t & operator << (serial_t& serial, const datum_t &datum)	throw();
	friend	serial_t & operator >> (serial_t& serial, datum_t &datum)	throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DATUM_HPP__  */



