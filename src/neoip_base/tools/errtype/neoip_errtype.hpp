/*! \file
    \brief Bunch of define for type based on associated type with a string

*/
#ifndef __NEOIP_ERRTYPE_HPP__ 
#define __NEOIP_ERRTYPE_HPP__ 

/* system include */
#include <iostream>
#include <map>
/* local include */
#include "neoip_assert.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class to ease the definition of error type object
 */
template <typename TYPE> class errtype_t : public TYPE {
private:
	typename TYPE::errtype_enum	type_val;	//!< the type of the error itself
	std::string *			m_reason;	//!< the reason associated with the error

	/*************** static function for string_map	***********************/
	static void type_register(const typename TYPE::errtype_enum &key, const std::string str) throw();
	static std::map<typename TYPE::errtype_enum, std::string>	string_map;
public:
	/*************** ctor/dtor	***************************************/
	errtype_t(typename TYPE::errtype_enum type_val = TYPE::NONE)
						throw() : type_val(type_val), m_reason(NULL) {}
	errtype_t(typename TYPE::errtype_enum type_val, const std::string &m_reason)	throw();
	errtype_t(const std::string &val_str)	throw()	{ type_val = from_string_nocase(val_str);	}
	errtype_t(const char *val_str)		throw()	{ type_val = from_string_nocase(val_str);	}
	~errtype_t()									throw();
	
	/*************** copy stuff	***************************************/
	errtype_t(const errtype_t &other)			throw();
	errtype_t &	operator=(const errtype_t& other)	throw();
	
	/*************** this function init the string_map	***************/
	static void	init_string_map()	throw();

	/*************** string convertion	*******************************/
	static typename TYPE::errtype_enum from_string(const std::string &str)		throw();
	static typename TYPE::errtype_enum from_string_nocase(const std::string &str)	throw();

	/*************** query function	***************************************/
	std::string 			reason()	const throw()	{ if(m_reason) return *m_reason;
									  else	return std::string();	}
	typename TYPE::errtype_enum	value()		const throw()	{ return type_val; 		}
	bool				is_null()	const throw()	{ return type_val == TYPE::NONE;}
	void				nullify() 	throw()		{ type_val = TYPE::NONE; 	}

	/*************** compatibility layer	*******************************/	
	std::string 			get_reason()	const throw()	{ return reason();		}
	typename TYPE::errtype_enum	get_value()	const throw()	{ return value(); 		}

	/*************** Comparison operator	*******************************/
	bool operator == (const errtype_t<TYPE> & other) const throw(){return type_val == other.type_val;}
	bool operator != (const errtype_t<TYPE> & other) const throw(){return type_val != other.type_val;}
	bool operator <  (const errtype_t<TYPE> & other) const throw(){return type_val <  other.type_val;}
	bool operator <= (const errtype_t<TYPE> & other) const throw(){return type_val <= other.type_val;}
	bool operator >  (const errtype_t<TYPE> & other) const throw(){return type_val >  other.type_val;}
	bool operator >= (const errtype_t<TYPE> & other) const throw(){return type_val >= other.type_val;}

	/*************** display function	*******************************/
	std::string	to_string()		const throw();
	friend	std::ostream & operator << (std::ostream & os, const errtype_t<TYPE> &type_var)
						{ return os << type_var.to_string();	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      ctor/dtor function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor with a reason
 */
template <typename TYPE>
errtype_t<TYPE>::errtype_t(typename TYPE::errtype_enum type_val, const std::string &m_reason) throw()
{
	// copy the parameter
	this->type_val	= type_val;
	this->m_reason	= nipmem_new std::string(m_reason);
}

/** \brief Destructor
 */
template <typename TYPE>
errtype_t<TYPE>::~errtype_t()			throw()
{
	// if the m_reason is set, delete it
	if(m_reason)	nipmem_delete m_reason;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      copy stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Copy operator
 */
template <typename TYPE>
errtype_t<TYPE>::errtype_t(const errtype_t &other) throw()
{
	// ctor the new version
	this->type_val	= other.type_val;
	if( other.m_reason )	m_reason	= nipmem_new std::string(*other.m_reason);
	else			m_reason	= NULL;
}

/** \brief assignement operator
 */
template <typename TYPE>
errtype_t<TYPE> &	errtype_t<TYPE>::operator=(const errtype_t& other)	throw()
{
	// Gracefully handle self assignment
	if( this == &other )	return *this;
	// dtor the old version
	if(m_reason)		nipmem_delete m_reason;	
	// ctor the new version
	this->type_val	= other.type_val;
	if(other.m_reason)	m_reason	= nipmem_new std::string(*other.m_reason);
	else			m_reason	= NULL;
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief To register a given type with its associated string
 */
template <typename TYPE> 
void errtype_t<TYPE>::type_register(const typename TYPE::errtype_enum &key, const std::string str)
										throw()
{
	// setup the string/type association
	bool	succeed = string_map.insert(std::make_pair(key, str)).second;
	DBG_ASSERT( succeed );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief To convert the object into a string
 */
template <typename TYPE> 
std::string errtype_t<TYPE>::to_string()					const throw()
{
	typename std::map<typename TYPE::errtype_enum, std::string>::const_iterator	iter;
	// if the string_map is not yet initialized, do it now
	if( string_map.empty() )	init_string_map();	
	// try to find the element in the string_map
	iter = string_map.find(type_val);
	// sanity check - type_val MUST be in the string_map
	DBG_ASSERT( iter != string_map.end() );
	// if no m_reason is specified in this particular error, return only the error string
	if( !m_reason )	return iter->second;
	// else return the error string appended with the given m_reason
	return iter->second + " (m_reason=" + *m_reason + ")";
}

/** \brief convert a string into a errtype_t with string case are NOT ignored
 */
template <typename TYPE> 
typename TYPE::errtype_enum errtype_t<TYPE>::from_string(const std::string &str)	throw()
{
	typename std::map<typename TYPE::errtype_enum, std::string>::const_iterator	iter;
	// if the string_map is not yet initialized, do it now
	if( string_map.empty() )	init_string_map();
	// go thru all the registered string
	for( iter = string_map.begin(); iter != string_map.end(); iter++ ){
		// if it equals, return now
		if( iter->second == str )	return iter->first;
	}
	// if none if found, return TYPE::NONE
	return TYPE::NONE;
}

/** \brief convert a string into a errtype_t with string case ARE ignored
 */
template <typename TYPE> 
typename TYPE::errtype_enum errtype_t<TYPE>::from_string_nocase(const std::string &str)	throw()
{
	typename std::map<typename TYPE::errtype_enum, std::string>::const_iterator	iter;
	// if the string_map is not yet initialized, do it now
	if( string_map.empty() )	init_string_map();
	// go thru all the registered string
	for( iter = string_map.begin(); iter != string_map.end(); iter++ ){
		// if it equals, return now
		if( string_t::casecmp(iter->second, str) == 0 )
			return iter->first;
	}
	// if none if found, return TYPE::NONE
	return TYPE::NONE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                ERRTYPE_DECLARATION macro
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define	NEOIP_ERRTYPE_DECLARATION_START(class_name)				\
	class errtype_##class_name {						\
	public:	enum errtype_enum {						\
			NONE,
#define	NEOIP_ERRTYPE_DECLARATION_ITEM(class_name, item_name)			\
			item_name,
#define	NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(class_name)				\
			MAX,							\
			};							\
	virtual errtype_enum	get_value()	const throw() = 0;
// NOTE: it is allowed to add stuff here (e.g. additionnal methods)
#define	NEOIP_ERRTYPE_DECLARATION_END(class_name)				\
	virtual ~errtype_##class_name()	{}					\
	};									\
	class	class_name : public errtype_t<errtype_##class_name> {		\
	public:									\
	class_name(errtype_##class_name::errtype_enum type_val = errtype_##class_name::NONE)	\
				throw() : errtype_t<errtype_##class_name>(type_val) {}		\
	class_name(errtype_##class_name::errtype_enum type_val, const std::string &m_reason)	\
				throw()	: errtype_t<errtype_##class_name>(type_val, m_reason) {}	\
	};
	


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                ERRTYPE_DEFINITION macro
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define NEOIP_ERRTYPE_DEFINITION_START(class_name)						\
	template <> std::map<errtype_##class_name::errtype_enum, std::string>			\
			errtype_t<errtype_##class_name>::string_map = 			\
				std::map<errtype_##class_name::errtype_enum, std::string>();	\
	template <> void errtype_t<errtype_##class_name>::init_string_map()	throw()		\
	{											\
		type_register( class_name::NONE, "NONE" );
#define NEOIP_ERRTYPE_DEFINITION_ITEM(class_name, item_constant, item_str)			\
		type_register( class_name::item_constant, item_str );
#define NEOIP_ERRTYPE_DEFINITION_END(class_name)						\
	}


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ERRTYPE_HPP__  */





