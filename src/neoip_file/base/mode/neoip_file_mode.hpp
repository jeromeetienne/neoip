/*! \file
    \brief Header of the \ref file_mode_t
    
*/


#ifndef __NEOIP_FILE_MODE_HPP__ 
#define __NEOIP_FILE_MODE_HPP__ 
/* system include */
#include <stdint.h>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for remote node
 */
class file_mode_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	static const uint32_t	READ;
	static const uint32_t	WRITE;
	static const uint32_t	APPEND;
	static const uint32_t	CREATE;
	static const uint32_t	TRUNCATE;
	
	static const uint32_t	RW;		//!< just an alias on READ|WRITE
	static const uint32_t	WPLUS;		//!< just an alias on WRITE|CREATE|TRUNCATE
private:
	uint32_t	mode;
public:
	/*************** ctor/dtor	***************************************/
	file_mode_t()					throw() { mode	= 0;		}
	file_mode_t(const uint32_t &other_mode)		throw() { mode	= other_mode;	}

	/*************** query function	***************************************/
	bool	is_null()				const throw();
	bool	include(const file_mode_t &other)	const throw();
	int	to_open_flags()				const throw();

	/*************** action function	*******************************/
	file_mode_t &	insert(const file_mode_t &other)throw();
	file_mode_t &	remove(const file_mode_t &other)throw();

	/*************** arithmetic operator on file_mode_t	***************/
	void		operator +=(const file_mode_t &other)	throw()	{ insert(other);	}
	void		operator -=(const file_mode_t &other)	throw()	{ remove(other);	}
	file_mode_t	operator + (const file_mode_t &other)	const throw()
						{ file_mode_t tmp(*this); tmp += other; return tmp;	}
	file_mode_t	operator - (const file_mode_t &other)	const throw()
						{ file_mode_t tmp(*this); tmp -= other; return tmp;	}
	
	/*************** comparison operator	*******************************/
	int	compare(const file_mode_t &other)	const throw();
	bool	operator == (const file_mode_t & other) const throw() { return compare(other) == 0;	}
	bool	operator != (const file_mode_t & other) const throw() { return compare(other) != 0;	}
	bool	operator <  (const file_mode_t & other) const throw() { return compare(other) >  0;	}
	bool	operator <= (const file_mode_t & other) const throw() { return compare(other) >= 0;	}
	bool	operator >  (const file_mode_t & other) const throw() { return compare(other) <  0;	}
	bool	operator >= (const file_mode_t & other) const throw() { return compare(other) <= 0;	}
	
	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const file_mode_t &file_mode)	throw()
				{ return os << file_mode.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_MODE_HPP__  */










