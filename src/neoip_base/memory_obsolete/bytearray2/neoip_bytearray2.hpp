/*! \file
    \brief Header of the \ref bytearray2_t
    
*/


#ifndef __NEOIP_BYTEARRAY2_HPP__
#define __NEOIP_BYTEARRAY2_HPP__
/* system include */
#include <iostream>
#include <netinet/in.h>		// for htons etc... in the inline serialization
/* local include */
#include "neoip_serial2_except.hpp"
#include "neoip_datum2.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for bytearray2_t
 */
class bytearray2_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static const size_t	FULL_LENGTH_DFL;
	static const size_t	HEAD_FREELEN_DFL;
	static const size_t	GROW_CHUNKLEN_DFL;
private:
	datum2_t	full_data;	//!< the memory zone where the data are stored
	char *		used_ptr;	//!< pointer on the head of the used data (MUST NOT be NULL)
	size_t		used_len;	//!< the length of the used data (may be 0)
	size_t		grow_chunklen;	//!< the length to alloc each time bytearray2_t needs to grow
	
	/*************** internal function	*******************************/
	void		allocate_ctor(size_t new_full_length, size_t new_head_freelen
						, size_t new_grow_chunklen)	throw();
	void		reallocate(size_t head_inclen, size_t tail_inclen)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bytearray2_t(size_t new_full_length = FULL_LENGTH_DFL, size_t new_head_freelen = HEAD_FREELEN_DFL
				, size_t new_grow_chunklen = GROW_CHUNKLEN_DFL)	throw();
	explicit bytearray2_t(const datum2_t &datum)				throw();
	void	work_on_datum_nocopy(const datum2_t &datum)			throw();
	/*************** copy operator	***************************************/
	bytearray2_t(const bytearray2_t &other)			throw();
	bytearray2_t & operator = (const bytearray2_t & other)	throw();	
	
	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return full_data.is_null();			}
	bool		empty()		const throw()	{ return used_length() == 0;			}
	size_t		full_length()	const throw()	{ return full_data.length();			}
	size_t		used_length()	const throw()	{ return used_len;				}
	size_t		head_freelen()	const throw()	{ DBG_ASSERT( !is_null() );
							  return used_ptr - full_data.char_ptr();	}
	size_t		tail_freelen()	const throw()	{ DBG_ASSERT( !is_null() );
							  return full_length() - head_freelen() - used_length();}
	datum2_t	range(size_t offset, size_t len, const datum2_flag_t &flag = datum2_t::FLAG_DFL) const throw(serial2_except_t)
							{ if( offset+len > used_length() ) nthrow_serial2_plain("ByteArray too short.");
							  return datum2_t(used_ptr+offset, len, flag);	}
	char *		char_ptr()	const throw()	{ return used_ptr;		}
	void *		void_ptr()	const throw()	{ return used_ptr;		}
	size_t		length()	const throw()	{ return used_length();		}
	size_t		get_grow_chunklen()const throw(){ return grow_chunklen;		}

	/*************** Action function	*******************************/
	char *		head_alloc(size_t nbyte)		throw();
	char *		tail_alloc(size_t nbyte)		throw();
	void		head_free(size_t nbyte)			throw();
	void		tail_free(size_t nbyte)			throw();
	bytearray2_t &	head_add(const void *data, size_t len)	throw();
	bytearray2_t &	tail_add(const void *data, size_t len)	throw();

	/*************** short cut to add data	*******************************/
	bytearray2_t &	head_add(const datum2_t &datum)		throw()	{ return head_add(datum.void_ptr(), datum.length());	}
	bytearray2_t &	tail_add(const datum2_t &datum)		throw()	{ return tail_add(datum.void_ptr(), datum.length());	}
	bytearray2_t &	prepend(const void *data, size_t len)	throw()	{ return head_add(data, len);	}
	bytearray2_t &	prepend(const datum2_t &datum)		throw()	{ return head_add(datum);	}
	bytearray2_t &	append(const void *data, size_t len)	throw()	{ return tail_add(data, len);	}
	bytearray2_t &	append(const datum2_t &datum)		throw()	{ return tail_add(datum);	}

	/*************** short cut to extract data	***********************/
	datum2_t	to_datum(const datum2_flag_t &flag = datum2_t::FLAG_DFL)		const throw(serial2_except_t)
							{ return range(0, used_length(), flag);		}
	datum2_t	head_peek(size_t len, const datum2_flag_t &flag = datum2_t::FLAG_DFL)	const throw(serial2_except_t)
							{ DBG_ASSERT( len <= used_length() );
							  return range(0 , len, flag);			}
	datum2_t	tail_peek(size_t len, const datum2_flag_t &flag = datum2_t::FLAG_DFL)	const throw(serial2_except_t)
							{ DBG_ASSERT( len <= used_length() );
							  return range(used_length()-len , len, flag);	}
	datum2_t	head_consume(size_t len, const datum2_flag_t &flag = datum2_t::FLAG_DFL)throw(serial2_except_t)
							{ datum2_t tmp	= head_peek(len, flag);
							  head_free(len);
							  return tmp;					}
	datum2_t	tail_consume(size_t len, const datum2_flag_t &flag = datum2_t::FLAG_DFL)throw(serial2_except_t)
							{ datum2_t tmp	= tail_peek(len, flag);
							  tail_free(len);
							  return tmp;					}

	/**************** Comparison operator	*******************************/
	int	compare(const bytearray2_t &other)	const throw();
	bool	operator == (const bytearray2_t &other)	const throw() { return compare(other) == 0; }
	bool	operator != (const bytearray2_t &other)	const throw() { return compare(other) != 0; }
	bool	operator <  (const bytearray2_t &other)	const throw() { return compare(other) <  0; }
	bool	operator <= (const bytearray2_t &other)	const throw() { return compare(other) <= 0; }
	bool	operator >  (const bytearray2_t &other)	const throw() { return compare(other) >  0; }
	bool	operator >= (const bytearray2_t &other)	const throw() { return compare(other) >= 0; }

	/**************** Compatibility layer	*******************************/
	void *		get_data()	const throw()	{ return void_ptr();	}
	size_t		get_len()	const throw()	{ return length();	}
	void *		get_ptr()	const throw()	{ return void_ptr();	}
	size_t		size()		const throw()	{ return length();	}
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bytearray2_t &bytearray )	throw()
					{ return oss << bytearray.to_string();	}
					
	/*************** handle serialization of basic types	***************/
	friend bytearray2_t& operator << (bytearray2_t& bytearray, const bool &val)	throw();
	friend bytearray2_t& operator >> (bytearray2_t& bytearray, bool &val)		throw(serial2_except_t);
	friend bytearray2_t& operator << (bytearray2_t& bytearray, const uint8_t &val)	throw();
	friend bytearray2_t& operator >> (bytearray2_t& bytearray, uint8_t &val)	throw(serial2_except_t);
	friend bytearray2_t& operator << (bytearray2_t& bytearray, const uint16_t &val)	throw();
	friend bytearray2_t& operator >> (bytearray2_t& bytearray, uint16_t &val)	throw(serial2_except_t);
	friend bytearray2_t& operator << (bytearray2_t& bytearray, const uint32_t &val)	throw();
	friend bytearray2_t& operator >> (bytearray2_t& bytearray, uint32_t &val)	throw(serial2_except_t);
	friend bytearray2_t& operator << (bytearray2_t& bytearray, const uint64_t &val)	throw();
	friend bytearray2_t& operator >> (bytearray2_t& bytearray, uint64_t &val)	throw(serial2_except_t);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     inline serialization of bool
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline bytearray2_t& operator << (bytearray2_t& bytearray, const bool &val)	throw()
{
	// convert it to internal format
	uint8_t		tmp = val ? 1 : 0;
	// append it to the bytearray
	bytearray.append(&tmp, sizeof(tmp));
	// return the object itself
	return bytearray;
}

/** \brief overload the >> operator
 */
inline bytearray2_t & operator >> (bytearray2_t& bytearray, bool &val)		throw(serial2_except_t)
{
	uint8_t		tmp;
	// check the payload length
	if(bytearray.length() < sizeof(tmp))	nthrow_serial2_plain("bool Payload Too Short");	
	// read the payload
	tmp = *((uint8_t *)bytearray.void_ptr());
	// free the payload length
	bytearray.head_free( sizeof(tmp) );
	// convert tmp into val in the bytearray2_t
	val = tmp ? true : false;
	// return the bytearray2_t itself
	return bytearray;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint8_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline bytearray2_t& operator << (bytearray2_t& bytearray, const uint8_t &val)	throw()
{
	// append it to the bytearray
	bytearray.append(&val, sizeof(val));
	// return the object itself
	return bytearray;
}

/** \brief overload the >> operator
 */
inline bytearray2_t & operator >> (bytearray2_t& bytearray, uint8_t &val)	throw(serial2_except_t)
{
	// check the payload length
	if(bytearray.length() < sizeof(val))	nthrow_serial2_plain("uint8_t Payload Too Short");	
	// read the payload
	val = *((uint8_t *)bytearray.void_ptr());
	// free the payload length in bytearray
	bytearray.head_free( sizeof(val) );
	// return the bytearray2_t itself
	return bytearray;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint16_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline bytearray2_t& operator << (bytearray2_t& bytearray, const uint16_t &val)	throw()
{
	// convert it to internal format
	uint16_t	tmp = htons(val);
	// append it to the bytearray
	bytearray.append(&tmp, sizeof(tmp));
	// return the object itself
	return bytearray;
}

/** \brief overload the >> operator
 */
inline bytearray2_t & operator >> (bytearray2_t& bytearray, uint16_t &val)	throw(serial2_except_t)
{
	uint16_t	tmp;
	// check the payload length
	if(bytearray.length() < sizeof(tmp))	nthrow_serial2_plain("uint16_t Payload Too Short");	
	// read the payload
	tmp = *((uint16_t *)bytearray.void_ptr());
	// free the payload length in bytearray
	bytearray.head_free( sizeof(tmp) );
	// convert tmp into val
	val = ntohs(tmp);
	// return the bytearray2_t itself
	return bytearray;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint32_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline bytearray2_t& operator << (bytearray2_t& bytearray, const uint32_t &val)	throw()
{
	// convert it to internal format
	uint32_t	tmp = htonl(val);
	// append it to the bytearray
	bytearray.append(&tmp, sizeof(tmp));
	// return the object itself
	return bytearray;
}

/** \brief overload the >> operator
 */
inline bytearray2_t & operator >> (bytearray2_t& bytearray, uint32_t &val)	throw(serial2_except_t)
{
	uint32_t	tmp;
	// check the payload length
	if(bytearray.length() < sizeof(tmp))	nthrow_serial2_plain("uint32_t Payload Too Short");	
	// read the payload
	tmp = *((uint32_t *)bytearray.void_ptr());
	// free the payload length in bytearray
	bytearray.head_free( sizeof(tmp) );
	// convert tmp into val
	val = ntohl(tmp);
	// return the bytearray2_t itself
	return bytearray;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         inline serialization of uint64_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
inline bytearray2_t& operator << (bytearray2_t& bytearray, const uint64_t &val)	throw()
{
	// convert it to internal format
	uint64_t	tmp = __bswap_64(val);
	// append it to the bytearray
	bytearray.append(&tmp, sizeof(tmp));
	// return the object itself
	return bytearray;
}

/** \brief overload the >> operator
 */
inline bytearray2_t & operator >> (bytearray2_t& bytearray, uint64_t &val)	throw(serial2_except_t)
{
	uint64_t	tmp;
	// check the payload length
	if(bytearray.length() < sizeof(tmp))	nthrow_serial2_plain("uint64_t Payload Too Short");	
	// read the payload
	tmp = *((uint64_t *)bytearray.void_ptr());
	// free the payload length in bytearray
	bytearray.head_free( sizeof(tmp) );
	// convert tmp into val
	val = __bswap_64(tmp);
	// return the bytearray2_t itself
	return bytearray;
}

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_BYTEARRAY2_HPP__  */










