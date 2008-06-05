/*! \file
    \brief Header of the compress_t
*/


#ifndef __NEOIP_COMPRESS_HPP__ 
#define __NEOIP_COMPRESS_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_compress_vapi.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \ref class to perform compression of various type (found in compress_type_t)
 */
class compress_t {
private:
	compress_type_t		compress_type;	//!< the type of compression performed
	compress_vapi_t *	compress_vapi;	//!< pointer on the virtual API implementation

public:
	/*************** ctor/dtor	***************************************/
	compress_t(const compress_type_t &compress_type)			throw();

	
	/*************** query function	***************************************/
	const compress_type_t &	get_type()	const throw() { return compress_type;	}

	/*************** compress/uncompress function	***********************/
	datum_t			compress(const datum_t &datum, size_t max_len)	throw();
	datum_t			uncompress(const datum_t &datum, size_t max_len)throw();
	
	/*************** canonical object management stuff	***************/
private:
	void		zeroing()			throw();
	void		copy(const compress_t &other)	throw();
public:	
	/*************** ctor/dtor	***************************************/
	compress_t()				throw()	{ zeroing();			}
	compress_t(const compress_t &other)	throw()	{ zeroing(); copy(other);	}
	~compress_t()			 	throw()	{ nullify();			}
	//! assignement operator
	compress_t &	operator = (const compress_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}
	// canonical object management stuff
	int		compare(const compress_t & other)	const throw();
	std::string	to_string()				const throw();
	void		nullify()				throw();
	bool		is_null()				const throw();	

	// comparison operator
	bool	operator ==(const compress_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const compress_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const compress_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const compress_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const compress_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const compress_t & other)  const throw(){ return compare(other) >= 0; }
	// display functions
	friend std::ostream & operator << (std::ostream & os, const compress_t & compress) throw()
		{ return os << compress.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_COMPRESS_HPP__  */



