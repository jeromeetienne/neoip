/*! \file
    \brief Header of the \ref bytearray0_t
    
*/


#ifndef __NEOIP_BYTEARRAY0_HPP__ 
#define __NEOIP_BYTEARRAY0_HPP__ 
/* system include */
#include <iostream>
#include <glib.h>
/* local include */
#include "neoip_datum.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for id
 */
class bytearray0_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static const size_t	RESERVED_SIZE;
	static const size_t	START_OFFSET;
	static const size_t	GROW_CHUNK_LEN;
private:
	GByteArray *	glib_barray;

	void		internal_ctor(const void *ptr, size_t len)		throw();
	void		internal_dtor()						throw();
public:
	/*************** ctor/dtor	***************************************/
	bytearray0_t(const size_t reserved_size = bytearray0_t::RESERVED_SIZE
			, const size_t start_offset = bytearray0_t::START_OFFSET
			, const size_t grow_chunk_len = bytearray0_t::GROW_CHUNK_LEN)	throw();
	bytearray0_t(const datum_t &datum)	throw()	{ internal_ctor(datum.get_ptr(), datum.get_len());	}
	bytearray0_t(const bytearray0_t &other)	throw()	{ internal_ctor(other.get_ptr(), other.get_len());	}
	bytearray0_t(const void *data, int len)	throw() { internal_ctor(data, len);				}
	~bytearray0_t()				throw()	{ internal_dtor();					}

	// assignement operator
	bytearray0_t &	operator = (const bytearray0_t &other)			throw();

	/**************** Comparison operator	*******************************/
	int	compare(const bytearray0_t &other)	const throw();
	bool	operator == (const bytearray0_t &other)	const throw() { return compare(other) == 0; }
	bool	operator != (const bytearray0_t &other)	const throw() { return compare(other) != 0; }
	bool	operator <  (const bytearray0_t &other)	const throw() { return compare(other) <  0; }
	bool	operator <= (const bytearray0_t &other)	const throw() { return compare(other) <= 0; }
	bool	operator >  (const bytearray0_t &other)	const throw() { return compare(other) >  0; }
	bool	operator >= (const bytearray0_t &other)	const throw() { return compare(other) >= 0; }
	
	/**************** add/remove data at head/tail	***********************/
	bytearray0_t &	head_add(const void *data, size_t len)	throw();
	bytearray0_t &	head_remove(size_t len)			throw(serial_except_t);
	bytearray0_t &	tail_add(const void *data, size_t len)	throw();
	bytearray0_t &	tail_remove(size_t len)			throw(serial_except_t);

	bytearray0_t &	set_size(size_t len)			throw();

	/**************** Query function	*******************************/
	bool		is_null()	const throw()	{ return glib_barray->len == 0;	}
	void *		get_data()	const throw()	{ return glib_barray->data;	}
	size_t		get_len()	const throw()	{ return glib_barray->len;	}
	void *		get_ptr()	const throw()	{ return get_data();	}
	size_t		size()		const throw()	{ return get_len();	}
	bool		empty()		const throw()	{ return size() == 0;	}
	
	/**************** Convertion function	*******************************/
	datum_t			to_datum(datum_flag_t flag = datum_t::FLAG_DFL)	const throw();
	std::string		to_base64()					const throw();
	static bytearray0_t	from_base64(const std::string &str)		throw();

	/*************** consume functions	*******************************/
	datum_t		head_consume(size_t len)	throw(serial_except_t);
	datum_t		tail_consume(size_t len)	throw(serial_except_t);
	/*************** peek functions	***************************************/
	datum_t		head_peek(size_t len)		throw(serial_except_t);
	datum_t		tail_peek(size_t len)		throw(serial_except_t);

	/*************** short cut	***************************************/
	bytearray0_t &	head_add(const datum_t &datum)		throw()	{ return head_add(datum.get_data(), datum.get_len());	}
	bytearray0_t &	tail_add(const datum_t &datum)		throw()	{ return tail_add(datum.get_data(), datum.get_len());	}
	bytearray0_t &	prepend(const void *data, size_t len)	throw()	{ return head_add(data, len);	}
	bytearray0_t &	prepend(const datum_t &datum)		throw()	{ return head_add(datum);	}
	bytearray0_t &	append(const void *data, size_t len)	throw()	{ return tail_add(data, len);	}
	bytearray0_t &	append(const datum_t &datum)		throw()	{ return tail_add(datum);	}
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bytearray0_t &bytearray0 )	throw()
					{ return oss << bytearray0.to_string();	}

/** \brief template to have all the \ref neoip_serial stuff automaticaly in \ref bytearray0_t
 */
template <class T> bytearray0_t &unserial_peek(T &val) throw(serial_except_t)
{
	// init the serial_t
	// - this copy the whole bytearray0 everytime this is used. it is very inefficient
	// - TODO to change
	serial_t	serial(get_data(), get_len());
	serial >> val;
	return *this;
}

	template <class T> bytearray0_t &serialize(T &val)	throw()	{ return *this << val;	}
};


/** \brief template to have all the \ref neoip_serial stuff automaticaly in \ref bytearray0_t
 */
template <class T> bytearray0_t & operator >> ( bytearray0_t &bytearray0, T &val ) throw(serial_except_t)
{
	// init the serial_t
	// - this copy the whole bytearray0 everytime this is used. it is very inefficient
	// - TODO to change
	serial_t	serial(bytearray0.get_data(), bytearray0.get_len());
	serial >> val;
	// remove the unserialized data at the head
	bytearray0.head_remove( bytearray0.get_len() - serial.get_len() );	
	return bytearray0;
}

/** \brief template to have all the \ref neoip_serial stuff automaticaly in \ref bytearray0_t
 */
template <class T> bytearray0_t & operator << ( bytearray0_t &bytearray0, const T &val )
{
	serial_t	serial;
	serial << val;
	bytearray0.tail_add( serial.get_data(), serial.get_len() );
	return bytearray0;
}
NEOIP_NAMESPACE_END


#endif	/* __NEOIP_BYTEARRAY0_HPP__  */










