/*! \file
    \brief Template for packet type with a flexible offset and serialization type

*/

#ifndef __NEOIP_PKTTYPE_HPP__ 
#define __NEOIP_PKTTYPE_HPP__ 

/* system include */
#include <map>
/* local include */
#include "neoip_pkttype_profile.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Template used a base class to define pkttype 
 * 
 * - it allow a flexible pkttype offset and serialization type.
 * - it is tunnable at running time.
 * - this kind of flexbility allows to easily reuse protocol implementation in
 *   multiple context.
 */
template <typename T> class pkttype_t {
private:
	pkttype_profile_t	profile;
	size_t			pkttype_idx;	//!< the current pkttype_idx already 'offsetized'

	static std::map<std::string, size_t>	pkttype_map;	//!< the std::map which contains all the registered type
	static void	pkttype_register(const std::string &pkttype_str)	throw();
	static void	init_pkttype_map()					throw();
public:
	/*************** ctor/dtor	***************************************/
	pkttype_t()					throw();
	pkttype_t(const pkttype_profile_t &profile)	throw();
	

	/*************** string convertion	*******************************/
	pkttype_t<T>	get_pkttype(const std::string &pkttype_str)		const throw();
	size_t		get_value()	const throw()	{ return pkttype_idx; 		}
	bool		is_null()	const throw()	{ return pkttype_idx == std::numeric_limits<size_t>::max(); }
	
	/*************** Array operator	***************************************/
	static size_t	size()		throw()	{ return pkttype_map.size();	}
	pkttype_t<T>	at(size_t idx)	throw() { DBG_ASSERT( idx < size() );
						  pkttype_t<T> pkttype(profile);
						  pkttype.pkttype_idx = idx + profile.offset();
						  return pkttype;		}
	/*************** comparison operator	*******************************/
	bool operator == (const pkttype_t<T> & other) const throw(){return pkttype_idx == other.pkttype_idx;}
	bool operator != (const pkttype_t<T> & other) const throw(){return pkttype_idx != other.pkttype_idx;}
	bool operator <  (const pkttype_t<T> & other) const throw(){return pkttype_idx <  other.pkttype_idx;}
	bool operator <= (const pkttype_t<T> & other) const throw(){return pkttype_idx <= other.pkttype_idx;}
	bool operator >  (const pkttype_t<T> & other) const throw(){return pkttype_idx >  other.pkttype_idx;}
	bool operator >= (const pkttype_t<T> & other) const throw(){return pkttype_idx >= other.pkttype_idx;}

	/*************** display function	*******************************/
	std::string	to_string()						const throw();
	friend	std::ostream & operator << (std::ostream & os, const pkttype_t<T> &pkttype)
						{ return os << pkttype.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t& operator << ( serial_t& serial, const pkttype_t<T> &pkttype)	throw()
				{ return pkttype.profile.serialize(serial, pkttype.get_value());	}
	friend	serial_t& operator >> (serial_t& serial, pkttype_t<T> &pkttype)   throw(serial_except_t)
				{ pkttype.pkttype_idx = pkttype.profile.unserialize(serial); return serial;}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor by pkttype_profile_t by default
 */
template <typename T> 
pkttype_t<T>::pkttype_t()	throw()
{
	// if the pkttype_map is not already initialized, do it now
	if( pkttype_map.empty() )	init_pkttype_map(); 	
	// set the pkttype_idx to the null value
	pkttype_idx	= std::numeric_limits<size_t>::max();
}


/** \brief Constructor by pkttype_profile_t with a profile
 */
template <typename T> 
pkttype_t<T>::pkttype_t(const pkttype_profile_t &profile)	throw()
{
	// if the pkttype_map is not already initialized, do it now
	if( pkttype_map.empty() )	init_pkttype_map(); 	
	// copy the parameter
	this->profile	= profile;
	// set the pkttype_idx to the null value
	pkttype_idx	= std::numeric_limits<size_t>::max();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        type_register()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert a new type in this pkttype_t
 */
template <typename T> 
void pkttype_t<T>::pkttype_register(const std::string &pkttype_str)		throw()
{
	std::map<std::string, size_t>::const_iterator	iter;
	size_t						max_val = 0;
	// find the maximum value of pkttype_idx within the map
	for( iter = pkttype_map.begin(); iter != pkttype_map.end(); iter++ ){
		if( max_val < iter->second)
			max_val = iter->second;
	}
	// compute the pkttype index for this pkttype_str
	size_t	idx;
	if( pkttype_map.empty() )	idx = 0;
	else				idx = max_val + 1;
	// log to debug
	KLOG_DBG("insert " << pkttype_str << " at index " << idx);
	// insert the couple pkttype_str, pkttype_idx in the pkttype_map
	bool	succeed = pkttype_map.insert(std::make_pair(pkttype_str, idx)).second;
	DBG_ASSERT( succeed );
}

/** \brief Return a pkttype_t from a string
 */
template <typename T> 
pkttype_t<T> pkttype_t<T>::get_pkttype(const std::string &pkttype_str)		const throw()
{
	// find the pkttype_str in the pkttype_map
	std::map<std::string, size_t>::const_iterator	iter	= pkttype_map.find(pkttype_str);
	// sanity check - the profile MUST be set
	DBG_ASSERT( !profile.is_null() );
	// sanity check - the pkttype_str MUST be present
	DBG_ASSERT( iter != pkttype_map.end() );
	// return the result
	pkttype_t<T>	pkttype(profile);
	pkttype.pkttype_idx	= iter->second + profile.offset();
	return pkttype;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        string convertion
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Convert the object to a std::string
 */
template <typename T> 
std::string pkttype_t<T>::to_string()					const throw()
{
	std::map<std::string, size_t>::const_iterator	iter;
	// handle the null case
	if( is_null() )	return "null";
	// sanity check - the profile MUST be set
	DBG_ASSERT( !profile.is_null() );
	// find the maximum value of pkttype_idx within the map
	for( iter = pkttype_map.begin(); iter != pkttype_map.end(); iter++ ){
		if( pkttype_idx - profile.offset() == iter->second) break;
	}
	// sanity check - the pkttype_idx MUST be found
	DBG_ASSERT( iter != pkttype_map.end() );
	// return the string
	return iter->first;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                PKTTYPE_DECLARATION macro
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define	NEOIP_PKTTYPE_DECLARATION_START(class_name)					\
	class	class_name : NEOIP_COPY_CTOR_ALLOW, public pkttype_t<class_name> {	\
	public:	class_name(const pkttype_profile_t &profile)	throw(): pkttype_t<class_name>(profile){}\
		class_name(const pkttype_t<class_name> &pkttype)throw(): pkttype_t<class_name>(pkttype){}\
		class_name()					throw() {}
#define	NEOIP_PKTTYPE_DECLARATION_ITEM(class_name, item_name)					\
	public:	class_name item_name ()		const throw()					\
			{ return pkttype_t<class_name>::get_pkttype( # item_name); }		\
		bool is_ ## item_name()	const throw()	{ return *this == item_name(); }
// NOTE: it is allowed to add stuff here (e.g. additionnal methods)
#define	NEOIP_PKTTYPE_DECLARATION_END(class_name)					\
	}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                PKTTYPE_DEFINITION macro
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define NEOIP_PKTTYPE_DEFINITION_START(class_name)						\
	template <> std::map<std::string, size_t>	pkttype_t<class_name>::pkttype_map 	\
							= std::map<std::string, size_t>();	\
	template <> void pkttype_t<class_name>::init_pkttype_map()	throw()			\
	{
#define NEOIP_PKTTYPE_DEFINITION_ITEM(class_name, item_name)					\
		pkttype_register( #item_name );
#define NEOIP_PKTTYPE_DEFINITION_END(class_name)						\
	}


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PKTTYPE_HPP__  */





