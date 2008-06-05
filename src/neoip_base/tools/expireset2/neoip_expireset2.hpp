/*! \file
    \brief Header of the expireset2_t

\par TODO
- have a wikidbg
  - i thought about it a little and failed to find a way to get a wikidbg
    for a template
  - it would require the wikidbg function itself to be a template
  - POSSIBLE SOLUTION: 
    - to have a template function for _init
    - a specially implemented template function
- NOT DONE - because it would cause trouble with the timeout_t callback
  - avoid the pointer in item_t to get a fully normal std::set
  - use this in the code to avoid custom function
- DONE remove the function bodies from the class declaration
- DONE nunit the serialization
- DONE pass the item_t out of the declaration
- DONE handle the advance via std::advance
 

*/


#ifndef __NEOIP_EXPIRESET2_HPP__ 
#define __NEOIP_EXPIRESET2_HPP__ 
/* system include */
#include <set>
#include <iostream>
/* local include */
#include "neoip_expireset2_item.hpp"
#include "neoip_timeout.hpp"
#include "neoip_serial.hpp"		// for binary serialization
#include "neoip_nipmem_alloc.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
template <typename T> class expireset2_t;
template <typename T> class expireset2_item_t;
template <typename T> serial_t& operator << (serial_t& serial, const expireset2_t<T> &expireset)	throw();
template <typename T> serial_t& operator >> (serial_t& serial, expireset2_t<T> &expireset)		throw(serial_except_t);

/** \brief class to handle a negative cache
 */
template <typename T> class expireset2_t : NEOIP_COPY_CTOR_ALLOW {
private:
	delay_t			m_default_ttl;	//!< the default ttl for the itements

	/*************** item_db_t	***************************************/
	typedef std::list<expireset2_item_t<T> *>	item_db_t;
	item_db_t		item_db;	//!< to store the item
	
	void		zeroing()				throw();
	void		nullify()				throw();
	void		copy(const expireset2_t<T> &other)	throw();
public:
	/*************** ctor/dtor	***************************************/
	expireset2_t()							throw()	{ zeroing();		}
	~expireset2_t()							throw()	{ nullify();		}
	expireset2_t(const expireset2_t<T> &other)			throw()	{ zeroing();copy(other);}
	expireset2_t<T> &operator = (const expireset2_t<T> & other)	throw();
	expireset2_t(const delay_t &default_ttl)			throw();
	
	/*************** query function	***************************************/
	bool		is_null()		const throw()	{ return default_ttl().is_null();	}
	bool		empty()			const throw()	{ return item_db.empty();		}
	size_t		size()			const throw()	{ return item_db.size();		}	
	const delay_t &	default_ttl()		const throw()	{ return m_default_ttl;			}
	const T &	operator[](size_t idx)	const throw();
	delay_t		expire_delay_at(size_t idx)				const throw();	
	bool		contain(const T &value)					const throw();

	/*************** update function	*******************************/
	void		insert(const T &value, delay_t item_ttl = delay_t())	throw();
	void		erase(const T &value)					throw();
	void		update(const T &value, delay_t item_ttl = delay_t())	throw();
	expireset2_t<T>&default_ttl(const delay_t &m_new_ttl)			throw()
					{ this->m_default_ttl = m_new_ttl; return *this;	}

	/*************** Compatibility layer	*******************************/
	expireset2_t<T>&set_default_ttl(const delay_t &m_new_ttl)	throw()	{ return default_ttl(m_new_ttl);}
	const delay_t &	get_default_ttl()				throw()	{ return default_ttl();		}

	/*************** Comparison operator	***********************/
	bool	operator == (const expireset2_t<T> & other)	const throw();
	bool	operator != (const expireset2_t<T> & other)	const throw()	{ return !(*this == other);	}

	/*************** display function	*******************************/
	std::string to_string()		const throw();
	friend std::ostream & operator << (std::ostream& oss, const expireset2_t<T> &expireset)
					{ return oss << expireset.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << <> (serial_t& serial, const expireset2_t<T> &expireset)	throw();
	friend	serial_t& operator >> <> (serial_t & serial, expireset2_t<T> &expireset)	throw(serial_except_t);
	
	/*************** List of friend function	***********************/
	friend class	expireset2_item_t<T>;	
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
template <typename T> expireset2_t<T>::expireset2_t(const delay_t &m_default_ttl)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the default_ttl
	this->m_default_ttl	= m_default_ttl;
}


/** \brief zeroing the object
 */
template <typename T> void expireset2_t<T>::zeroing()	throw()
{
	// set the default_ttl
	this->m_default_ttl	= delay_t::INFINITE;
}

/** \brief Nullify the object
 */
template <typename T> void expireset2_t<T>::nullify()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all the itement of the database
	while( !item_db.empty() )	nipmem_delete item_db.front();
}

/** \brief copy the object
 */
template <typename T> void expireset2_t<T>::copy(const expireset2_t<T> & other)	throw()
{
	// copy the default ttl
	default_ttl	( other.default_ttl() );
	// copy all the item from the other object to this one
	for(size_t i = 0; i < other.size(); i++ )
		insert( other[i], other.expire_delay_at(i) );
}

/** \brief assignement operator
 */
template <typename T> expireset2_t<T> &expireset2_t<T>::operator=(const expireset2_t<T> & other) throw()
{
	// if it is a self assignement, return now
	if( this == &other )	return *this;
	// nullify this object
	nullify();
	// copy the other value to this one
	copy(other);
	// return the object itself
	return *this;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the expire delay of the idx-th itement
 */
template <typename T> delay_t expireset2_t<T>::expire_delay_at(size_t idx)	const throw()
{
	typename item_db_t::const_iterator	iter	= item_db.begin();
	// sanity check - idx MUST be < size()
	DBG_ASSERT( idx < size() );
	// go through the database
	std::advance(iter, idx);
	// return the expire_delay()
	return (*iter)->expire_delay();
}

/** \brief allow the [] operator to access in readonly the single itements
 */
template <typename T> const T &expireset2_t<T>::operator[](size_t idx)	const throw()
{
	typename item_db_t::const_iterator	iter	= item_db.begin();
	// sanity check - idx MUST be < size()
	DBG_ASSERT( idx < size() );
	// go through the database
	std::advance(iter, idx);
	// return the value()
	return (*iter)->value();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert a new itement in the expireset2_t
 * 
 * - if the item_ttl is null, the default_ttl of the expireset2_t is used
 * 
 */
template <typename T> void expireset2_t<T>::insert(const T &value, delay_t item_ttl) throw()
{
	// sanity check - the expireset2_t MUST NOT already contains the itement
	DBG_ASSERT( !contain(value) );
	// if expire itement is null, 
	if( item_ttl.is_null() )	item_ttl = default_ttl();
	// createan itement with this value
	nipmem_new expireset2_item_t<T>(value, item_ttl, this);
}

/** \brief return true if value is contained in the expireset2_t, false otherwise
 */
template <typename T> bool expireset2_t<T>::contain(const T &value)	const throw()
{
	typename item_db_t::const_iterator	iter;
	// go through the database
	for( iter = item_db.begin(); iter != item_db.end(); iter++ ){
		const expireset2_item_t<T> *	item = *iter;
		// if the value of this itement is the same, return true
		if( value == item->value() )	return true;
	}
	// if no match has been found, return false
	return false;
}

/** \brief erase an value from the expireset2_t
 */
template <typename T> void expireset2_t<T>::erase(const T &value)	throw()
{
	typename item_db_t::iterator	iter;
	// go through the database
	for( iter = item_db.begin(); iter != item_db.end(); iter++ ){
		expireset2_item_t<T> *	item = *iter;
		// if the value of this itement is different, skip it
		if( value != item->value() )	continue;
		// delete this itement
		nipmem_zdelete	item;
		// exit
		return;
	}
}

/** \brief update a itement in the expireset2_t (if it already exist, the timeout is refreshed)
 * 
 * ALGO:
 * -# if the itement is already contained, remove it.
 * -# in anycase, insert the itement.
 */
template <typename T> void expireset2_t<T>::update(const T &value, delay_t item_ttl)	throw()
{
	// if the value is already contained, erase it
	if( contain(value) )	erase(value);
	// insert the value 
	insert(value, item_ttl);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			comparison operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Overload the operator ==
 */
template <typename T>
bool	expireset2_t<T>::operator == (const expireset2_t<T> & other)	const throw()
{
	// if the size() is different, return false
	if( this->size() != other.size() )	return false;

	// compare each value	
	for(size_t i = 0; i < size(); i++){
		// if this item is present in the other expireset2_t, goto the next
		if( other.contain( (*this)[i] ) )	continue;
		// if this item IS NOT present in the other expireset2_t return false
		return false; 
	}
	// if all previous tests passed, return true
	return true;
	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      to_string() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a string
 */
template <typename T> std::string  expireset2_t<T>::to_string()	const throw()
{
	typename item_db_t::const_iterator	iter;
	std::ostringstream			oss;
	// display the begining of the database
	oss << "[";
	// go through the database
	for( iter = item_db.begin(); iter != item_db.end(); iter++ ){
		const expireset2_item_t<T> *	item = *iter;
		// disply this itement
		oss << "[" << item->value() << " expires in " << item->expire_delay()<< "]";
	}
	// display the end of the database
	oss << "]";
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			serialization
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a expireset2_t
 * 
 * - support null/empty object
 */
template <typename T>
serial_t& operator << (serial_t& serial, const expireset2_t<T> &expireset)	throw()
{
	const typename expireset2_t<T>::item_db_t &	item_db = expireset.item_db;
	date_t						present	= date_t::present();
	typename expireset2_t<T>::item_db_t::const_iterator	iter;
	// serialize the number of item
	serial << uint32_t(item_db.size());

	// serialize each item
	for(iter = item_db.begin(); iter != item_db.end(); iter++){
		const expireset2_item_t<T> *item	= *iter;
		// serialize the item
		serial << item->value();
		// serialize the expire_date of this item
		serial << date_t( present + item->expire_delay() );
	}

	// return serial
	return serial;
}

/** \brief unserialze a expireset2_t
 * 
 * - support null/empty expireset2_t
 */
template <typename T>
serial_t& operator >> (serial_t & serial, expireset2_t<T> &expireset)		throw(serial_except_t)
{
	date_t		present	= date_t::present();
	uint32_t	nb_item;
	T		value;
	date_t		expire_date;
	// unserialize the number of item
	serial >> nb_item;
	// unserialize each item
	for(size_t i = 0; i < nb_item; i++){
		// unserialize the value
		serial >> value;
		// unserialize the expire_date
		serial >> expire_date;
	
		// if this item is already expired, discard it and goto the next
		if( expire_date < present )	continue;
		// add it to the expireset2_t
		expireset.insert(value, expire_date - present);
	}
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_EXPIRESET2_HPP__  */



