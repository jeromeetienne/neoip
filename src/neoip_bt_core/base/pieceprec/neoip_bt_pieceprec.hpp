/*! \file
    \brief Header of the \ref bt_pieceprec_t
    
*/


#ifndef __NEOIP_BT_PIECEPREC_HPP__ 
#define __NEOIP_BT_PIECEPREC_HPP__ 
/* system include */
#include <stdio.h>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for bt_pieceprec_t
 */
class bt_pieceprec_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the precedence of a piece_idx when it is not needed (MUST be < to LOWEST)
	static const size_t	NOTNEEDED;
	//! the precedence of a piece_idx when it is to be the lowest precedence
	static const size_t	LOWEST;
	//! the precedence of a piece_idx when it is to be the highest precedence
	static const size_t	HIGHEST;
	//! the precedence of a piece_idx by default
	static const size_t	DEFAULT;
private:
	size_t	prec_value;
public:
	/*************** ctor/dtor	***************************************/
	bt_pieceprec_t(size_t value = NOTNEEDED)	throw() : prec_value(value) {}
	
	/*************** Query function	***************************************/
	size_t	value()		const throw()	{ return prec_value;			}
	bool	is_notneeded()	const throw()	{ return prec_value == NOTNEEDED;	}
	bool	is_needed()	const throw()	{ return !is_notneeded();		}
	bool	is_lowest()	const throw()	{ return prec_value == LOWEST;		}
	bool	is_highest()	const throw()	{ return prec_value == HIGHEST;		}
	bool	is_default()	const throw()	{ return prec_value == DEFAULT;		}

	/*************** Comparison function	*******************************/
	int	compare(const bt_pieceprec_t &other)	  const throw();
	bool 	operator == (const bt_pieceprec_t & other) const throw() { return compare(other) == 0;	}
	bool 	operator != (const bt_pieceprec_t & other) const throw() { return compare(other) != 0;	}
	bool 	operator <  (const bt_pieceprec_t & other) const throw() { return compare(other) <  0;	}
	bool 	operator <= (const bt_pieceprec_t & other) const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const bt_pieceprec_t & other) const throw() { return compare(other) >  0;	}
	bool 	operator >= (const bt_pieceprec_t & other) const throw() { return compare(other) >= 0;	}
	
	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bt_pieceprec_t &bt_pieceprec) throw()
				{ return os << bt_pieceprec.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_pieceprec_t &pieceprec)	throw();
	friend	serial_t& operator >> (serial_t & serial, bt_pieceprec_t &pieceprec) 		throw(serial_except_t);	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PIECEPREC_HPP__  */










