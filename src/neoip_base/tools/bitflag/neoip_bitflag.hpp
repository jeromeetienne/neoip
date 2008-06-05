/*! \file
    \brief Header of the datum_t
    
*/


#ifndef __NEOIP_BITFLAG_HPP__ 
#define __NEOIP_BITFLAG_HPP__ 
/* system include */
#include <iostream>
#include <stdint.h>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
template <typename T> class bitflag_t;
template <typename T> serial_t& operator << (serial_t& serial, const bitflag_t<T> &bitflag)	throw();
template <typename T> serial_t& operator >> (serial_t& serial, bitflag_t<T> &bitflag)		throw(serial_except_t);

/** \brief class definition for bitflag_t
 */
template <typename T>
class bitflag_t : NEOIP_COPY_CTOR_ALLOW {
private:
	T		flag_value;	//!< the value of the bitflag_t
public:
	/**************	ctor/dtor ********************************************/
	bitflag_t()				throw()	: flag_value(0)		{}
	explicit bitflag_t(T value)		throw()	: flag_value(value)	{}

	/**************	query function	***************************************/
	T		value()					const throw()	{ return flag_value;					}
	bool		fully_include(const bitflag_t<T> &other)const throw()	{ return (value() & other.value()) == other.value();	}
	bool		is_distinct(const bitflag_t<T> &other)	const throw()	{ return (value() & other.value()) == T(0);		}

	/*************** arithmetic operator	*******************************/
	bitflag_t<T> &	operator |=(const bitflag_t<T> &other)	throw()		{ flag_value |= other.value(); return *this;		}
	bitflag_t<T>	operator | (const bitflag_t<T> &other)	const throw()	{ bitflag_t tmp(*this); tmp |= other; return tmp;	}
	bitflag_t<T> &	operator &=(const bitflag_t<T> &other)	throw()		{ flag_value &= other.value(); return *this;		}
	bitflag_t<T>	operator & (const bitflag_t<T> &other)	const throw()	{ bitflag_t tmp(*this); tmp &= other; return tmp;	}
	bitflag_t<T> &	operator ^=(const bitflag_t<T> &other)	throw()		{ flag_value ^= other.value(); return *this;		}
	bitflag_t<T>	operator ^ (const bitflag_t<T> &other)	const throw()	{ bitflag_t tmp(*this); tmp ^= other; return tmp;	}
	bitflag_t<T>	operator ~ ()				const throw()	{ return bitflag_t<T>(~flag_value);			}

	/*************** comparison operator  *********************************/
	bool		operator == (const bitflag_t<T> &other)	const throw()	{ return value() == other.value();	}
	bool		operator != (const bitflag_t<T> &other)	const throw()	{ return value() != other.value();	}
	bool		operator >  (const bitflag_t<T> &other)	const throw()	{ return value() >  other.value();	}
	bool		operator >= (const bitflag_t<T> &other)	const throw()	{ return value() >= other.value();	}
	bool		operator <  (const bitflag_t<T> &other)	const throw()	{ return value() <  other.value();	}
	bool		operator <= (const bitflag_t<T> &other)	const throw()	{ return value() <= other.value();	}

	/*************** serialization function	*******************************/
	friend	serial_t &operator << <> (serial_t& serial, const bitflag_t<T> &bitflag)	throw();
	friend	serial_t &operator >> <> (serial_t& serial, bitflag_t<T> &bitflag)	 	throw(serial_except_t);

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial bitflag_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bitflag_t
 */
template <typename T> 
serial_t& operator << (serial_t& serial, const bitflag_t<T> &bitflag)		throw()
{
	// serialize the value
	serial << bitflag.flag_value;
	// return the serial_t itself
	return serial;
}

/** \brief unserialze a bitflag_t
 */
template <typename T> 
serial_t& operator >> (serial_t& serial, bitflag_t<T> &bitflag)  		throw(serial_except_t)
{
	// unserialize the value
	serial >> bitflag.flag_value;
	// return the serial_t itself
	return serial;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 the #define's for the declaration
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// define for the begining of the declaration
#define	NEOIP_BITFLAG_DECLARATION_START(class_name, store_type, cmd_list)				\
class	class_name : public bitflag_t<store_type> {							\
	public:	class_name()				throw()	{}					\
		explicit class_name(store_type value)	throw()	: bitflag_t<store_type>(value) {}	\
		class_name(const bitflag_t<store_type> &bitflag)throw():bitflag_t<store_type>(bitflag){}\
		std::string	to_string()	const throw();						\
		friend std::ostream &operator << (std::ostream& oss, const class_name &instance)throw()	\
					{ return oss << instance.to_string();	}			\
	cmd_list (NEOIP_BITFLAG_DECLARATION_ITEM)


// define for each item in the declaration
#define NEOIP_BITFLAG_DECLARATION_ITEM(class_name, store_type, opt_locaps, opt_hicaps, opt_bshift)	\
	static const store_type	opt_hicaps ## _VAL	= store_type(1) << (opt_bshift);		\
	static const class_name	opt_hicaps;								\
	bool	is_##opt_locaps()	const throw()	{ return this->fully_include(opt_hicaps); }

// NOTE: it is possible to add other methods to <class_name> between DECLARATION_START and DECLARATION_END

// define for the end of the declaration
#define	NEOIP_BITFLAG_DECLARATION_END(class_name, store_type, cmd_list)					\
	};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 the #define's for the definition
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// define for the begining of the to_string() function
#define	NEOIP_BITFLAG_DEF_TOSTRING_START(class_name, store_type, cmd_list)				\
std::string class_name::to_string()	const throw() 							\
{	std::ostringstream	oss;									\
	oss << "(";
// define for each item in the to_string() function
#define	NEOIP_BITFLAG_DEF_TOSTRING_ITEM(class_name, store_type, opt_locaps, opt_hicaps, opt_bshift)	\
	if( is_##opt_locaps() ){ oss << "(" << #opt_locaps << ")";	}
// define for the end of the to_string() function
#define	NEOIP_BITFLAG_DEF_TOSTRING_END(class_name, store_type, cmd_list)				\
	oss << ")";											\
	return oss.str();										\
}

// define to define all the static objects for each item
#define	NEOIP_BITFLAG_DEF_STATIC_ITEM(class_name, store_type, opt_locaps, opt_hicaps, opt_bshift)	\
	const class_name class_name::opt_hicaps	= class_name(class_name::opt_hicaps##_VAL);

// define for all the definition stuff - to_string and static object
#define	NEOIP_BITFLAG_DEFINITION(class_name, store_type, cmd_list)					\
	cmd_list			( NEOIP_BITFLAG_DEF_STATIC_ITEM		)			\
	NEOIP_BITFLAG_DEF_TOSTRING_START( class_name, store_type, cmd_list	)			\
	cmd_list			( NEOIP_BITFLAG_DEF_TOSTRING_ITEM 	)			\
	NEOIP_BITFLAG_DEF_TOSTRING_END	( class_name, store_type, cmd_list	)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BITFLAG_HPP__  */



