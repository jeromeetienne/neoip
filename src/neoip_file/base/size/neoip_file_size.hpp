/*! \file
    \brief Header of the \ref file_size_t
    
*/


#ifndef __NEOIP_FILE_SIZE_HPP__ 
#define __NEOIP_FILE_SIZE_HPP__ 
/* system include */
#include <limits>
#include <ostream>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for remote node
 */
class file_size_t : NEOIP_COPY_CTOR_ALLOW {
public:
	// constant to use delay_t in static ctor
	// NOTE: the actual numeric value are important as they are related to compare() function
	static const uint64_t		MAX_VAL	= 0xFFFFFFFFFFFFFFEULL;
	static const uint64_t		NONE_VAL= 0xFFFFFFFFFFFFFFFULL;
	// constant which MUST NOT be used for static ctor - due to static ctor order issue
	static const file_size_t	MAX;	//!< the maximum acceptable value for a file_size;
	static const file_size_t	NONE;	//!< the 'null' value
private:
	uint64_t	offset;
public:
	/*************** ctor/dtor	***************************************/
	file_size_t()				throw() { *this		= NONE;		}
	file_size_t(const uint64_t &offset)	throw() { this->offset	= offset;	}

	/*************** query function	***************************************/
	uint64_t	get_value()	const throw()	{ return offset;		}
	bool		is_null()	const throw()	{ return *this == NONE;		}
	
	/*************** type convertion	*******************************/
	bool		is_size_t_ok()	const throw()	{ return offset <= std::numeric_limits<size_t>::max();		}
	size_t		to_size_t()	const throw()	{ DBG_ASSERT(is_size_t_ok()); 
							  return size_t(offset);	}
	bool		is_off_t_ok()	const throw()	{ return offset <= (uint64_t)std::numeric_limits<off_t>::max();	} 
	off_t		to_off_t()	const throw()	{ DBG_ASSERT(is_off_t_ok());
							  return off_t(offset);		}
	bool		is_ssize_t_ok()	const throw()	{ return offset <= (uint64_t)std::numeric_limits<ssize_t>::max();} 
	ssize_t		to_ssize_t()	const throw()	{ DBG_ASSERT(is_ssize_t_ok());
							  return ssize_t(offset);	}
	bool		is_uint64_ok()	const throw()	{ return true;			}
	uint64_t	to_uint64()	const throw()	{ return offset;		}
	bool		is_int64_ok()	const throw()	{ return true;			}
	int64_t		to_int64()	const throw()	{ return offset;		}
	bool		is_ldouble_ok()	const throw()	{ return true;			}
	long double	to_ldouble()	const throw()	{ return (long double)offset;	}
	bool		is_double_ok()	const throw()	{ return true;			}
	double		to_double()	const throw()	{ return (double)offset;	}
	static file_size_t from_str(const std::string &str)	throw();
	
	/*************** arithmetic operator	*******************************/
	file_size_t &	operator +=(const file_size_t &other)	throw()	{ offset += other.offset; return *this;	}
	file_size_t &	operator -=(const file_size_t &other)	throw()	{ offset -= other.offset; return *this;	}
	file_size_t &	operator *=(const file_size_t &other)	throw()	{ offset *= other.offset; return *this;	}
	file_size_t &	operator /=(const file_size_t &other)	throw()	{ offset /= other.offset; return *this;	}
	file_size_t &	operator %=(const file_size_t &other)	throw()	{ offset %= other.offset; return *this;	}
	file_size_t	operator + (const file_size_t &other)	const throw()
						{ file_size_t tmp(*this); tmp += other; return tmp;	}
	file_size_t	operator - (const file_size_t &other)	const throw()
						{ file_size_t tmp(*this); tmp -= other; return tmp;	}	
	file_size_t	operator * (const file_size_t &other)	const throw()
						{ file_size_t tmp(*this); tmp *= other; return tmp;	}
	file_size_t	operator / (const file_size_t &other)	const throw()
						{ file_size_t tmp(*this); tmp /= other; return tmp;	}	
	file_size_t	operator % (const file_size_t &other)	const throw()
						{ file_size_t tmp(*this); tmp %= other; return tmp;	}	

	/*************** comparison operator	*******************************/
	int	compare(const file_size_t &other)	const throw()	{ if( offset < other.offset )	return -1;
									  if( offset > other.offset )	return +1;
									  return 0;			}
	bool 	operator == (const file_size_t & other)	const throw()	{ return offset == other.offset;}
	bool 	operator != (const file_size_t & other)	const throw()	{ return offset != other.offset;}
	bool 	operator <  (const file_size_t & other)	const throw()	{ return offset <  other.offset;}
	bool 	operator <= (const file_size_t & other)	const throw()	{ return offset <= other.offset;}
	bool 	operator >  (const file_size_t & other)	const throw()	{ return offset >  other.offset;}
	bool 	operator >= (const file_size_t & other)	const throw()	{ return offset >= other.offset;}
	
	/*************** display function	*******************************/	
	std::string		to_string()		const throw();
	std::string		to_human_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & oss, const file_size_t &file_offset) throw()
				{ return oss << file_offset.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const file_size_t &file_size)	throw();
	friend	serial_t& operator >> (serial_t & serial, file_size_t &file_size) 	throw(serial_except_t);			

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const file_size_t &file_size)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, file_size_t &file_size)		throw(xml_except_t);
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_SIZE_HPP__  */










