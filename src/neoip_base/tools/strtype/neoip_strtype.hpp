/*! \file
    \brief Template for type based on associated type with a string

*/

#ifndef __NEOIP_STRTYPE_HPP__
#define __NEOIP_STRTYPE_HPP__ 

/* system include */
#include <iostream>
#include <map>
/* local include */
#include "neoip_assert.hpp"
#include "neoip_string.hpp"
#include "neoip_serial.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

template <typename T, typename TS> class strtype_t : public T {
private:
	typename T::strtype_enum	type_val;	//!< internal type to this particular strtype_t

	static void	type_register(const typename T::strtype_enum &key, const std::string str)throw();
	//! the std::map which contains all the registered type
	static std::map<typename T::strtype_enum, std::string>	string_map;
public:
	/*************** ctor/dtor	***************************************/
	strtype_t(typename T::strtype_enum type_val = T::NONE )	throw() : type_val(type_val){}
	strtype_t(const char *val_str)		throw()	{ type_val = from_string_nocase(val_str);	}
	strtype_t(const std::string &val_str)	throw()	{ type_val = from_string_nocase(val_str);	}

	/*************** init the string_map - based on the static ctor	*******/
	static void	init_string_map()					throw();

	/*************** string convertion	*******************************/
	static typename T::strtype_enum	from_string(const std::string &str)		throw();
	static typename T::strtype_enum	from_string_nocase(const std::string &str)	throw();

	typename T::strtype_enum	value()		const throw()	{ return type_val; 		}
	bool				is_null()	const throw()	{ return type_val == T::NONE;	}
	void				nullify() 	throw()		{ type_val = T::NONE; 		}

	/*************** compatibility layer	*******************************/	
	typename T::strtype_enum	get_value()	const throw()	{ return value(); 		}

	/*************** comparison operator	*******************************/
	bool operator == (const strtype_t<T, TS> & other) const throw(){return type_val == other.type_val;}
	bool operator != (const strtype_t<T, TS> & other) const throw(){return type_val != other.type_val;}
	bool operator <  (const strtype_t<T, TS> & other) const throw(){return type_val <  other.type_val;}
	bool operator <= (const strtype_t<T, TS> & other) const throw(){return type_val <= other.type_val;}
	bool operator >  (const strtype_t<T, TS> & other) const throw(){return type_val >  other.type_val;}
	bool operator >= (const strtype_t<T, TS> & other) const throw(){return type_val >= other.type_val;}

	/*************** display function	*******************************/
	std::string	to_string()						const throw();
	friend	std::ostream & operator << (std::ostream & os, const strtype_t<T, TS> &type_var)
			{ return os << type_var.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t& operator << ( serial_t& serial, const strtype_t<T, TS> &strtype)	throw()
			{ return serial	<< (TS)strtype.type_val; }
	friend	serial_t& operator >> (serial_t& serial, strtype_t<T, TS> &strtype)   throw(serial_except_t){
		TS	value;
		serial	>> value;
		strtype.type_val	= (typename T::strtype_enum)value;
		return serial;	
	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        type_register()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert a new type in this strtype_t
 */
template <typename T, typename TS> 
void strtype_t<T, TS>::type_register(const typename T::strtype_enum &key, const std::string str)
										throw()
{
	bool	succeed = string_map.insert(std::make_pair(key, str)).second;
	DBG_ASSERT( succeed );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        string convertion
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Convert the object to a std::string
 */
template <typename T, typename TS> 
std::string strtype_t<T, TS>::to_string()					const throw()
{
	typename std::map<typename T::strtype_enum, std::string>::const_iterator	iter;
	// if the string_map is not yet initialized, do it now
	if( string_map.empty() )	init_string_map();
	// try to find the type_val into the string_map
	iter = string_map.find(type_val);
	// if none is found, return a empty string
	if( iter == string_map.end() )	return std::string();
	// else return the actual string
	return iter->second;
}

/** \brief convert a string into a strtype_t with string case are NOT ignored
 */
template <typename T, typename TS> 
typename T::strtype_enum strtype_t<T, TS>::from_string(const std::string &str)	throw()
{
	typename std::map<typename T::strtype_enum, std::string>::const_iterator	iter;
	// if the string_map is not yet initialized, do it now
	if( string_map.empty() )	init_string_map();
	// go thru all the registered string
	for( iter = string_map.begin(); iter != string_map.end(); iter++ ){
		// if it equals, return now
		if( iter->second == str )	return iter->first;
	}
	// if none if found, return T::NONE
	return T::NONE;
}

/** \brief convert a string into a strtype_t with string case ARE ignored
 */
template <typename T, typename TS> 
typename T::strtype_enum strtype_t<T, TS>::from_string_nocase(const std::string &str)	throw()
{
	typename std::map<typename T::strtype_enum, std::string>::const_iterator	iter;
	// if the string_map is not yet initialized, do it now
	if( string_map.empty() )	init_string_map();
	// go thru all the registered string
	for( iter = string_map.begin(); iter != string_map.end(); iter++ ){
		// if it equals, return now
		if( string_t::casecmp(iter->second, str) == 0 )
			return iter->first;
	}
	// if none if found, return T::NONE
	return T::NONE;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                STRTYPE_DECLARATION macro
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define	NEOIP_STRTYPE_DECLARATION_START(class_name, offset)			\
	class strtype_##class_name {						\
	public:	enum strtype_enum {						\
			NONE = offset,
#define	NEOIP_STRTYPE_DECLARATION_ITEM(class_name, item_name)			\
			item_name,
#define	NEOIP_STRTYPE_DECLARATION_ITEM_IDX(class_name, item_name, item_idx)	\
			item_name = item_idx,
#define	NEOIP_STRTYPE_DECLARATION_ITEM_LAST(class_name)				\
			MAX,							\
			};							\
	virtual strtype_enum	get_value()	const throw() = 0;
// NOTE: it is allowed to add stuff here (e.g. additionnal methods)
#define	NEOIP_STRTYPE_DECLARATION_END(class_name, serial_type)			\
	virtual ~strtype_##class_name()	{}					\
	};									\
	class	class_name : public strtype_t<strtype_##class_name, serial_type> {		\
	public:											\
	class_name(strtype_##class_name::strtype_enum type_val = strtype_##class_name::NONE)	\
				throw() 							\
				: strtype_t<strtype_##class_name, serial_type>(type_val) {}	\
	class_name(const char *val_str) throw()							\
				: strtype_t<strtype_##class_name, serial_type>(val_str) {}	\
	class_name(const std::string &val_str) throw()						\
				: strtype_t<strtype_##class_name, serial_type>(val_str) {}	\
	static size_t		size()				throw();			\
	static class_name	at(size_t idx)			throw();			\
	};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                STRTYPE_DEFINITION macro
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define NEOIP_STRTYPE_DEFINITION_START(class_name, serial_type)				\
	template <> std::map<strtype_##class_name::strtype_enum, std::string>			\
			strtype_t<strtype_##class_name, serial_type>::string_map = 		\
				std::map<strtype_##class_name::strtype_enum, std::string>();	\
	size_t class_name::size()	throw()							\
	{											\
		return int(class_name::MAX) - int(class_name::NONE) - 1; /* to remove NONE */	\
	}											\
	class_name class_name::at(size_t idx)	throw()						\
	{											\
		return (strtype_##class_name::strtype_enum)(idx + int(class_name::NONE) + 1);	\
	}											\
	template <> void strtype_t<strtype_##class_name, serial_type>::init_string_map()throw()	\
	{											\
		type_register( class_name::NONE, "NONE" );
#define NEOIP_STRTYPE_DEFINITION_ITEM(class_name, item_constant, item_str)			\
		type_register( class_name::item_constant, item_str );
#define NEOIP_STRTYPE_DEFINITION_END(class_name)						\
	}


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_STRTYPE_HPP__  */





