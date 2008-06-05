/*! \file
    \brief Header of the \ref serial2_t
    
*/


#ifndef __NEOIP_SERIAL2_HPP__
#define __NEOIP_SERIAL2_HPP__
/* system include */
#include <iostream>
#include <netinet/in.h>		// for htons etc... in the inline serialization
/* local include */
#include "neoip_serial2_except.hpp"
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// define the #define to optionally compile serial2_stat_t code
#if 1
#	define SERIAL_STAT_CODE(stat_code)	stat_code
#else
#	define SERIAL_STAT_CODE(stat_code)	
#endif

// list of forward declaration
class	serial2_stat_t;

/** \brief class definition for serial2_t
 */
class serial2_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static const size_t	INIT_LENGTH_DFL;
	static const size_t	HEAD_LENGTH_DFL;
	static const size_t	GROW_LENGTH_DFL;
private:
	datum_t		full_data;	//!< the memory zone where the data are stored
	char *		used_ptr;	//!< pointer on the head of the used data (MUST NOT be NULL)
	size_t		used_len;	//!< the length of the used data (may be 0)
	size_t		grow_length;	//!< the length to alloc each time serial2_t needs to grow

	SERIAL_STAT_CODE(serial2_stat_t *serial_stat;)

	/*************** internal function	*******************************/
	void		allocate_ctor(size_t new_init_length, size_t new_head_length
						, size_t new_grow_length)	throw();
	void		reallocate(size_t head_inclen, size_t tail_inclen)	throw();
public:
	/*************** ctor/dtor	***************************************/
	serial2_t()								throw();
	~serial2_t()								throw();
	serial2_t(size_t new_init_length, size_t new_head_length = HEAD_LENGTH_DFL
				, size_t new_grow_length = GROW_LENGTH_DFL)	throw();
	serial2_t(const void *data, int len)					throw();
	explicit serial2_t(const datum_t &datum)				throw();
	void	work_on_datum_nocopy(const datum_t &datum)			throw();
	/*************** copy operator	***************************************/
	serial2_t(const serial2_t &other)					throw();
	serial2_t & operator = (const serial2_t & other)			throw();	
	
	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return full_data.is_null();			}
	bool		empty()		const throw()	{ return used_length() == 0;			}
	size_t		full_length()	const throw()	{ return full_data.length();			}
	size_t		used_length()	const throw()	{ return used_len;				}
	size_t		head_freelen()	const throw()	{ return used_ptr - full_data.char_ptr();	}
	size_t		tail_freelen()	const throw()	{ return full_length() - head_freelen() - used_length();}
	datum_t		range(size_t offset, size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL) const throw(serial2_except_t)
							{ if( offset+len > used_length() ) nthrow_serial2_plain("ByteArray too short.");
							  return datum_t(used_ptr+offset, len, flag);	}
	char *		char_ptr()	const throw()	{ return used_ptr;		}
	void *		void_ptr()	const throw()	{ return used_ptr;		}
	size_t		length()	const throw()	{ return used_length();		}
	size_t		get_grow_length()const throw()	{ return grow_length;		}
	SERIAL_STAT_CODE(const serial2_stat_t &stat()	const throw()	{ return *serial_stat;	}	)

	/*************** Action function	*******************************/
	char *		head_alloc(size_t nbyte)		throw();
	char *		tail_alloc(size_t nbyte)		throw();
	void		head_free(size_t nbyte)			throw();
	void		tail_free(size_t nbyte)			throw();
	serial2_t &	head_add(const void *data, size_t len)	throw();
	serial2_t &	tail_add(const void *data, size_t len)	throw();

	/*************** short cut to add data	*******************************/
	serial2_t &	head_add(const datum_t &datum)		throw()	{ return head_add(datum.void_ptr(), datum.length());	}
	serial2_t &	tail_add(const datum_t &datum)		throw()	{ return tail_add(datum.void_ptr(), datum.length());	}
	serial2_t &	prepend(const void *data, size_t len)	throw()	{ return head_add(data, len);	}
	serial2_t &	prepend(const datum_t &datum)		throw()	{ return head_add(datum);	}
	serial2_t &	append(const void *data, size_t len)	throw()	{ return tail_add(data, len);	}
	serial2_t &	append(const datum_t &datum)		throw()	{ return tail_add(datum);	}

	/*************** short cut to extract data	***********************/
	datum_t		to_datum(const datum_flag_t &flag = datum_t::FLAG_DFL)			const throw(serial2_except_t)
							{ return range(0, used_length(), flag);		}
	// TODO check those DBG_ASSERT()
	datum_t		head_peek(size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL)	const throw(serial2_except_t)
							{ DBG_ASSERT( len <= used_length() );
							  return range(0 , len, flag);			}
	datum_t		tail_peek(size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL)	const throw(serial2_except_t)
							{ DBG_ASSERT( len <= used_length() );
							  return range(used_length()-len , len, flag);	}
	datum_t		head_consume(size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL)	throw(serial2_except_t)
							{ datum_t tmp	= head_peek(len, flag);
							  head_free(len);
							  return tmp;					}
	datum_t		tail_consume(size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL)	throw(serial2_except_t)
							{ datum_t tmp	= tail_peek(len, flag);
							  tail_free(len);
							  return tmp;					}

	/**************** Comparison operator	*******************************/
	int	compare(const serial2_t &other)		const throw();
	bool	operator == (const serial2_t &other)	const throw() { return compare(other) == 0; }
	bool	operator != (const serial2_t &other)	const throw() { return compare(other) != 0; }
	bool	operator <  (const serial2_t &other)	const throw() { return compare(other) <  0; }
	bool	operator <= (const serial2_t &other)	const throw() { return compare(other) <= 0; }
	bool	operator >  (const serial2_t &other)	const throw() { return compare(other) >  0; }
	bool	operator >= (const serial2_t &other)	const throw() { return compare(other) >= 0; }

	/**************** Compatibility layer	*******************************/
	// TODO to remove
	void *		get_data()	const throw()	{ return void_ptr();		}
	size_t		get_len()	const throw()	{ return length();		}
	void *		get_ptr()	const throw()	{ return void_ptr();		}
	size_t		size()		const throw()	{ return length();		}
	void		consume(size_t len)   throw()	{ head_free(len);		}
	void		reset()		      throw()	{ head_free(length());		}
	void		head_remove(size_t len)throw()	{ head_free(len);		}
	void		tail_remove(size_t len)throw()	{ tail_free(len);		}
	void		set_size(size_t len)  throw()	{ tail_alloc(len);		}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const serial2_t &serial )	throw()
					{ return oss << serial.to_string();	}
					
	/*************** handle serialization of basic types	***************/
	friend serial2_t& operator << (serial2_t& serial, const bool &val)	throw();
	friend serial2_t& operator >> (serial2_t& serial, bool &val)		throw(serial2_except_t);
	friend serial2_t& operator << (serial2_t& serial, const uint8_t &val)	throw();
	friend serial2_t& operator >> (serial2_t& serial, uint8_t &val)		throw(serial2_except_t);
	friend serial2_t& operator << (serial2_t& serial, const uint16_t &val)	throw();
	friend serial2_t& operator >> (serial2_t& serial, uint16_t &val)	throw(serial2_except_t);
	friend serial2_t& operator << (serial2_t& serial, const uint32_t &val)	throw();
	friend serial2_t& operator >> (serial2_t& serial, uint32_t &val)	throw(serial2_except_t);
	friend serial2_t& operator << (serial2_t& serial, const uint64_t &val)	throw();
	friend serial2_t& operator >> (serial2_t& serial, uint64_t &val)	throw(serial2_except_t);

/** \brief unserialize a type but dont modify the serial2_t data
 */
template <class T> serial2_t &unserial_peek(T &val) throw(serial2_except_t)
{
#if 1	// TODO safe but slow version - it copies the whole serial2_t every time
	serial2_t	serial(*this);
	serial >> val;
#else
	size_t	old_len	= length();
	// unserialize the type
	*this	>> val;
	// restore the old length - supposed to restore the old data
	// - WARNING: it assume the data wont be modified or moved during this operator >>
	head_alloc(old_len - length());
	// sanity check - now the length is supposed to be back to old_len
	DBG_ASSERT( length() == old_len);
#endif
	return *this;
}

	/************** serializing without << >> operator	***************/
	template <class T> serial2_t &serialize(const T &val)	throw()	{ return *this << val;	}
	template <class T> serial2_t &unserialize(T &val)	throw()	{ return *this >> val;	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     inline serialization of bool
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial2_t& operator << (serial2_t& serial, const bool &val)	throw()
{
	// convert it to internal format
	uint8_t		tmp = val ? 1 : 0;
	// append it to the serial
	serial.append(&tmp, sizeof(tmp));
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial2_t & operator >> (serial2_t& serial, bool &val)		throw(serial2_except_t)
{
	uint8_t		tmp;
	// check the payload length
	if(serial.length() < sizeof(tmp))	nthrow_serial2_plain("bool Payload Too Short");	
	// read the payload
	tmp = *((uint8_t *)serial.void_ptr());
	// free the payload length
	serial.head_free( sizeof(tmp) );
	// convert tmp into val in the serial2_t
	val = tmp ? true : false;
	// return the serial2_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint8_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial2_t& operator << (serial2_t& serial, const uint8_t &val)	throw()
{
	// append it to the serial
	serial.append(&val, sizeof(val));
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial2_t & operator >> (serial2_t& serial, uint8_t &val)	throw(serial2_except_t)
{
	// check the payload length
	if(serial.length() < sizeof(val))	nthrow_serial2_plain("uint8_t Payload Too Short");	
	// read the payload
	val = *((uint8_t *)serial.void_ptr());
	// free the payload length in serial
	serial.head_free( sizeof(val) );
	// return the serial2_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint16_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial2_t& operator << (serial2_t& serial, const uint16_t &val)	throw()
{
	// convert it to internal format
	uint16_t	tmp = htons(val);
	// append it to the serial
	serial.append(&tmp, sizeof(tmp));
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial2_t & operator >> (serial2_t& serial, uint16_t &val)	throw(serial2_except_t)
{
	uint16_t	tmp;
	// check the payload length
	if(serial.length() < sizeof(tmp))	nthrow_serial2_plain("uint16_t Payload Too Short");	
	// read the payload
	tmp = *((uint16_t *)serial.void_ptr());
	// free the payload length in serial
	serial.head_free( sizeof(tmp) );
	// convert tmp into val
	val = ntohs(tmp);
	// return the serial2_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint32_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial2_t& operator << (serial2_t& serial, const uint32_t &val)	throw()
{
	// convert it to internal format
	uint32_t	tmp = htonl(val);
	// append it to the serial
	serial.append(&tmp, sizeof(tmp));
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial2_t & operator >> (serial2_t& serial, uint32_t &val)	throw(serial2_except_t)
{
	uint32_t	tmp;
	// check the payload length
	if(serial.length() < sizeof(tmp))	nthrow_serial2_plain("uint32_t Payload Too Short");	
	// read the payload
	tmp = *((uint32_t *)serial.void_ptr());
	// free the payload length in serial
	serial.head_free( sizeof(tmp) );
	// convert tmp into val
	val = ntohl(tmp);
	// return the serial2_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint64_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial2_t& operator << (serial2_t& serial, const uint64_t &val)	throw()
{
	// convert it to internal format
	uint64_t	tmp = __bswap_64(val);
	// append it to the serial
	serial.append(&tmp, sizeof(tmp));
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial2_t & operator >> (serial2_t& serial, uint64_t &val)	throw(serial2_except_t)
{
	uint64_t	tmp;
	// check the payload length
	if(serial.length() < sizeof(tmp))	nthrow_serial2_plain("uint64_t Payload Too Short");	
	// read the payload
	tmp = *((uint64_t *)serial.void_ptr());
	// free the payload length in serial
	serial.head_free( sizeof(tmp) );
	// convert tmp into val
	val = __bswap_64(tmp);
	// return the serial2_t itself
	return serial;
}


// undefine the #define to optionally compile serial2_stat_t code
#undef SERIAL_STAT_CODE

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_SERIAL2_HPP__  */









