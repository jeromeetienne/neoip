/*! \file
    \brief Header of the \ref serial_t
    
*/


#ifndef __NEOIP_SERIAL_HPP__
#define __NEOIP_SERIAL_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_serial_except.hpp"
#include "neoip_assert.hpp"
#include "neoip_endian.hpp"
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// define the #define to optionally compile serial_stat_t code
#if 0
#	define SERIAL_STAT_CODE(stat_code)	stat_code
#else
#	define SERIAL_STAT_CODE(stat_code)	
#endif

// list of forward declaration
class	serial_stat_t;

/** \brief class definition for serial_t
 */
class serial_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static const size_t	INIT_LENGTH_DFL;
	static const size_t	HEAD_LENGTH_DFL;
	static const size_t	GROW_LENGTH_DFL;
private:
	datum_t		full_data;	//!< the memory zone where the data are stored
	char *		used_ptr;	//!< pointer on the head of the used data (MUST NOT be NULL)
	size_t		used_len;	//!< the length of the used data (may be 0)
	size_t		grow_length;	//!< the length to alloc each time serial_t needs to grow

	SERIAL_STAT_CODE(serial_stat_t *serial_stat;)

	/*************** internal function	*******************************/
	void		allocate_ctor(size_t new_init_length, size_t new_head_length
						, size_t new_grow_length)	throw();
	void		reallocate(size_t head_inclen, size_t tail_inclen)	throw();
	void		internal_ctor_docopy(const void *data, size_t len)	throw();
	bool		is_sane()						const throw();
public:
	/*************** ctor/dtor	***************************************/
	serial_t()								throw();
	~serial_t()								throw();
	serial_t(size_t new_init_length, size_t new_head_length = HEAD_LENGTH_DFL
				, size_t new_grow_length = GROW_LENGTH_DFL)	throw();
	serial_t(const void *data, size_t len)			throw()	{ internal_ctor_docopy(data, len);				}
	explicit serial_t(const datum_t &datum)			throw()	{ internal_ctor_docopy(datum.void_ptr(), datum.length());	}
	explicit serial_t(const char *str)			throw()	{ internal_ctor_docopy(str, strlen(str));			}
	explicit serial_t(const std::string &str)		throw()	{ internal_ctor_docopy(str.c_str(), str.size());		}
	/*************** copy operator	***************************************/
	serial_t(const serial_t &other)				throw();
	serial_t & operator = (const serial_t & other)		throw();
	
	/*************** nocopy 'ctor'	***************************************/
	void	work_on_data_nocopy(const void *data, size_t len)throw();
	void	work_on_data_nocopy(const datum_t &datum)	throw()	{ work_on_data_nocopy(datum.void_ptr(), datum.length());	}

	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return empty();				}
	bool		empty()		const throw()	{ return used_length() == 0;			}
	size_t		full_length()	const throw()	{ return full_data.length();			}
	size_t		used_length()	const throw()	{ return used_len;				}
	size_t		head_freelen()	const throw()	{ return used_ptr - full_data.char_ptr();	}
	size_t		tail_freelen()	const throw()	{ return full_length() - head_freelen() - used_length();}
	char *		char_ptr()	const throw()	{ return used_ptr;		}
	void *		void_ptr()	const throw()	{ return used_ptr;		}
	size_t		length()	const throw()	{ return used_length();		}
	size_t		get_grow_length()const throw()	{ return grow_length;		}
	datum_t		range(size_t offset, size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL) const throw(serial_except_t)
							{ if( offset+len > used_length() ) nthrow_serial_plain("ByteArray too short.");
							  return datum_t(used_ptr+offset, len, flag);	}
	SERIAL_STAT_CODE(const serial_stat_t &stat()	const throw()	{ return *serial_stat;	}	)

	/*************** Action function	*******************************/
	char *		head_alloc(size_t nbyte)		throw();
	char *		tail_alloc(size_t nbyte)		throw();
	void		head_free(size_t nbyte)			throw();
	void		tail_free(size_t nbyte)			throw();
	serial_t &	head_add(const void *data, size_t len)	throw();
	serial_t &	tail_add(const void *data, size_t len)	throw();

	/*************** short cut to add data	*******************************/
	serial_t &	head_add(const datum_t &datum)		throw()	{ return head_add(datum.void_ptr(), datum.length());	}
	serial_t &	tail_add(const datum_t &datum)		throw()	{ return tail_add(datum.void_ptr(), datum.length());	}
	serial_t &	prepend(const void *data, size_t len)	throw()	{ return head_add(data, len);	}
	serial_t &	prepend(const datum_t &datum)		throw()	{ return head_add(datum);	}
	serial_t &	append(const void *data, size_t len)	throw()	{ return tail_add(data, len);	}
	serial_t &	append(const datum_t &datum)		throw()	{ return tail_add(datum);	}

	/*************** short cut to extract data	***********************/
	std::string	to_stdstring()	const throw()	{ return std::string(char_ptr(),length());	}
	datum_t		to_datum(const datum_flag_t &flag = datum_t::FLAG_DFL)			const throw()
							{ return datum_t(void_ptr(), length(), flag);	}
	// TODO check those DBG_ASSERT()
	datum_t		head_peek(size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL)	const throw(serial_except_t)
							{ DBGNET_ASSERT( len <= used_length() );
							  return range(0 , len, flag);			}
	datum_t		tail_peek(size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL)	const throw(serial_except_t)
							{ DBGNET_ASSERT( len <= used_length() );
							  return range(used_length()-len , len, flag);	}
	datum_t		head_consume(size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL)	throw(serial_except_t)
							{ datum_t tmp	= head_peek(len, flag);
							  head_free(len);
							  return tmp;					}
	datum_t		tail_consume(size_t len, const datum_flag_t &flag = datum_t::FLAG_DFL)	throw(serial_except_t)
							{ datum_t tmp	= tail_peek(len, flag);
							  tail_free(len);
							  return tmp;					}

	/**************** Comparison operator	*******************************/
	int	compare(const serial_t &other)		const throw();
	bool	operator == (const serial_t &other)	const throw() { return compare(other) == 0; }
	bool	operator != (const serial_t &other)	const throw() { return compare(other) != 0; }
	bool	operator <  (const serial_t &other)	const throw() { return compare(other) <  0; }
	bool	operator <= (const serial_t &other)	const throw() { return compare(other) <= 0; }
	bool	operator >  (const serial_t &other)	const throw() { return compare(other) >  0; }
	bool	operator >= (const serial_t &other)	const throw() { return compare(other) >= 0; }

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
	friend	std::ostream & operator << (std::ostream & oss, const serial_t &serial )	throw()
					{ return oss << serial.to_string();	}
					
	/*************** handle serialization of basic types	***************/
	friend serial_t& operator << (serial_t& serial, const bool &val)	throw();
	friend serial_t& operator >> (serial_t& serial, bool &val)		throw(serial_except_t);
	friend serial_t& operator << (serial_t& serial, const uint8_t &val)	throw();
	friend serial_t& operator >> (serial_t& serial, uint8_t &val)		throw(serial_except_t);
	friend serial_t& operator << (serial_t& serial, const uint16_t &val)	throw();
	friend serial_t& operator >> (serial_t& serial, uint16_t &val)		throw(serial_except_t);
	friend serial_t& operator << (serial_t& serial, const uint32_t &val)	throw();
	friend serial_t& operator >> (serial_t& serial, uint32_t &val)		throw(serial_except_t);
	friend serial_t& operator << (serial_t& serial, const uint64_t &val)	throw();
	friend serial_t& operator >> (serial_t& serial, uint64_t &val)		throw(serial_except_t);
	friend serial_t& operator << (serial_t& serial, const float &val)	throw();
	friend serial_t& operator >> (serial_t& serial, float &val)		throw(serial_except_t);
	friend serial_t& operator << (serial_t& serial, const double &val)	throw();
	friend serial_t& operator >> (serial_t& serial, double &val)		throw(serial_except_t);
	friend serial_t& operator << (serial_t& serial, const std::string &val)	throw();
	friend serial_t& operator >> (serial_t& serial, std::string &val)	throw(serial_except_t);
#ifdef __APPLE__	// just a workaround because i forgot many serialisation of size_t
	friend serial_t& operator << (serial_t& serial, const size_t &val)	throw();
	friend serial_t& operator >> (serial_t& serial, size_t &val)		throw(serial_except_t);
#endif
        
/** \brief unserialize a type but dont modify the serial_t data
 */
template <class T> serial_t &unserial_peek(T &val) throw(serial_except_t)
{
#if 0	// TODO safe but slow version - it copies the whole serial_t every time
	serial_t	serial(*this);
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
	template <class T> serial_t &serialize(const T &val)	throw()	{ return *this << val;	}
	template <class T> serial_t &unserialize(T &val)	throw()	{ return *this >> val;	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     inline serialization of bool
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial_t& operator << (serial_t& serial, const bool &val)	throw()
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
inline serial_t & operator >> (serial_t& serial, bool &val)		throw(serial_except_t)
{
	uint8_t		tmp;
	// check the payload length
	if(serial.length() < sizeof(tmp))	nthrow_serial_plain("bool Payload Too Short");	
	// read the payload
	tmp = *((uint8_t *)serial.void_ptr());
	// free the payload length
	serial.head_free( sizeof(tmp) );
	// convert tmp into val in the serial_t
	val = tmp ? true : false;
	// return the serial_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint8_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial_t& operator << (serial_t& serial, const uint8_t &val)	throw()
{
	// append it to the serial
	serial.append(&val, sizeof(val));
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial_t & operator >> (serial_t& serial, uint8_t &val)	throw(serial_except_t)
{
	// check the payload length
	if(serial.length() < sizeof(val))	nthrow_serial_plain("uint8_t Payload Too Short");	
	// read the payload
	val = *((uint8_t *)serial.void_ptr());
	// free the payload length in serial
	serial.head_free( sizeof(val) );
	// return the serial_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint16_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial_t& operator << (serial_t& serial, const uint16_t &val)	throw()
{
	// convert it to internal format
	uint16_t	tmp = NEOIP_HTOBE16(val);
	// append it to the serial
	serial.append(&tmp, sizeof(tmp));
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial_t & operator >> (serial_t& serial, uint16_t &val)	throw(serial_except_t)
{
	uint16_t	tmp;
	// check the payload length
	if(serial.length() < sizeof(tmp))	nthrow_serial_plain("uint16_t Payload Too Short");	
	// read the payload
	tmp = *((uint16_t *)serial.void_ptr());
	// free the payload length in serial
	serial.head_free( sizeof(tmp) );
	// convert tmp into val
	val = NEOIP_BETOH16(tmp);
	// return the serial_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint32_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial_t& operator << (serial_t& serial, const uint32_t &val)	throw()
{
	// convert it to internal format
	uint32_t	tmp = NEOIP_HTOBE32(val);
	// append it to the serial
	serial.append(&tmp, sizeof(tmp));
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial_t & operator >> (serial_t& serial, uint32_t &val)	throw(serial_except_t)
{
	uint32_t	tmp;
	// check the payload length
	if(serial.length() < sizeof(tmp))	nthrow_serial_plain("uint32_t Payload Too Short");	
	// read the payload
	tmp = *((uint32_t *)serial.void_ptr());
	// free the payload length in serial
	serial.head_free( sizeof(tmp) );
	// convert tmp into val
	val = NEOIP_BETOH32(tmp);
	// return the serial_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint64_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial_t& operator << (serial_t& serial, const uint64_t &val)	throw()
{
	// convert it to internal format
	uint64_t	tmp = NEOIP_HTOBE64(val);
	// append it to the serial
	serial.append(&tmp, sizeof(tmp));
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial_t & operator >> (serial_t& serial, uint64_t &val)	throw(serial_except_t)
{
	uint64_t	tmp;
	// check the payload length
	if(serial.length() < sizeof(tmp))	nthrow_serial_plain("uint64_t Payload Too Short");	
	// read the payload
	tmp = *((uint64_t *)serial.void_ptr());
	// free the payload length in serial
	serial.head_free( sizeof(tmp) );
	// convert tmp into val
	val = NEOIP_BETOH64(tmp);
	// return the serial_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of float
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial_t& operator << (serial_t& serial, const float &val)	throw()
{
	// sanity check - number of byte of the float MUST be the same as in uint32_t
	DBG_ASSERT( sizeof(float) == sizeof(uint32_t) ); 
	// define an union for the type-punning
	union { uint32_t i; float f; } tmp;
	// make it appears as an uint32_t (BUT NOT convert it)
	tmp.f	= val; 
	// use the uint32_t serialization
	return serial << tmp.i;
}

/** \brief overload the >> operator
 */
inline serial_t & operator >> (serial_t& serial, float &val)	throw(serial_except_t)
{
	// sanity check - number of byte of the float MUST be the same as in uint32_t
	DBG_ASSERT( sizeof(float) == sizeof(uint32_t) ); 
	// define an union for the type-punning
	union { uint32_t i; float f; } tmp;
	// unserialize it as a uint32_t
	serial	>> tmp.i;
	// set val using the data as float
	val	= tmp.f;
	// return the serial_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of double
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial_t& operator << (serial_t& serial, const double &val)	throw()
{
	// sanity check - number of byte of the double MUST be the same as in uint64_t
	DBG_ASSERT( sizeof(double) == sizeof(uint64_t) ); 
	// define an union for the type-punning
	union { uint64_t i; double d; } tmp;
	// make it appears as an uint64_t (BUT NOT convert it)
	tmp.d	= val; 
	// use the uint64_t serialization
	return serial << tmp.i;
}

/** \brief overload the >> operator
 */
inline serial_t & operator >> (serial_t& serial, double &val)	throw(serial_except_t)
{
	// sanity check - number of byte of the double MUST be the same as in uint64_t
	DBG_ASSERT( sizeof(double) == sizeof(uint64_t) ); 
	// define an union for the type-punning
	union { uint64_t i; double d; } tmp;
	// unserialize it as a uint64_t
	serial	>> tmp.i;
	// set val using the data as double
	val	= tmp.d;
	// return the serial_t itself
	return serial;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         inline serialization of std::string
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial_t& operator << (serial_t& serial, const std::string &val)	throw()
{
	// serialize the length of the std::string
	serial	<< (uint32_t)(val.size());
	// append the data in the std::string
	serial.append(val.c_str(), val.size());
	// return the object itself
	return serial;
}

/** \brief overload the >> operator
 */
inline serial_t & operator >> (serial_t& serial, std::string &val)	throw(serial_except_t)
{
	// get the length in the payload
	uint32_t	len;
	serial		>> len;
	// check the length
	if( serial.get_len() < len )	throw serial_except_t("std::string Payload Too Short");
	// build the datum
	val	= std::string(serial.char_ptr(), len);
	// consume the buffer
	serial.consume( len );
	// return the serial_t itself
	return serial;
}

#ifdef __APPLE__	// just a workaround because i forgot many serialisation of size_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of size_t
// - NOTE: this is only a kudge because i got many size_t serialization i forgot
//   - it triggered no error on linux/win32 but cause compilation error on macos
//   - so this is just a workaround... this is a temporary fix
//   - the trick is to handle it as a uint32_t serialisation
//   - same thing on xmlrpc_build_t/xmlrpc_parse_t and serial_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline serial_t& operator << (serial_t& serial, const size_t &val)	throw()
{
	// return the object itself
	return serial << uint32_t(val);
}

/** \brief overload the >> operator
 */
inline serial_t & operator >> (serial_t& serial, size_t &val)	throw(serial_except_t)
{
	uint32_t	tmp;
        serial >> tmp;
        val = tmp;
	// return the serial_t itself
	return serial;
}
#endif

// undefine the #define to optionally compile serial_stat_t code
#undef SERIAL_STAT_CODE

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//        #define to standardize the declaration of an inheriting object
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define NEOIP_SERIAL_INHERITANCE_START(class_name)							\
class class_name : public serial_t {									\
public:													\
	class_name(size_t new_init_length = INIT_LENGTH_DFL, size_t new_head_length = HEAD_LENGTH_DFL	\
			, size_t new_grow_length = GROW_LENGTH_DFL)	throw() 			\
			: serial_t(new_init_length, new_head_length, new_grow_length) 		{}	\
	class_name(const void *data, int len)			throw() : serial_t(data, len)	{}	\
	explicit class_name(const datum_t &datum)		throw() : serial_t(datum)	{}	\
	explicit class_name(const char *str)			throw() : serial_t(str)		{}	\
	explicit class_name(const std::string &str)		throw() : serial_t(str)		{}	\
	template <class T> class_name &unserial_peek(T &val)	throw(serial_except_t)			\
							{ serial_t::unserial_peek(val); return *this;}	\
	template <class T> class_name &serialize(const T &val)	throw()					\
								{ *this << val; return *this;	}	\
	template <class T> class_name &unserialize(T &val)	throw(serial_except_t)			\
								{ *this >> val; return *this;	}
		
#define NEOIP_SERIAL_INHERITANCE_END(class_name)							\
};


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_SERIAL_HPP__  */









