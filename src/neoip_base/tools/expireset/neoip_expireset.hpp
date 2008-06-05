/*! \file
    \brief Header of the expireset_t

\par TODO
- remove the function bodies from the class declaration
- add a function to get the expiration time of the n-th elements
  - do a function returning the n-th element
  - return elem->get_expire_delay()
  - this function is called expire_delay_at(idx)
- think about a way to test it
- this stuff act like a multi set allowing multiple element of the same value
  - change that
  - have a std::set<> instead of a std::list<>

*/


#ifndef __NEOIP_EXPIRESET_HPP__ 
#define __NEOIP_EXPIRESET_HPP__ 
/* system include */
#include <set>
#include <iostream>
/* local include */
#include "neoip_timeout.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief class to handle a negative cache
 */
template <typename T> class expireset_t : NEOIP_COPY_CTOR_DENY {
private:
	/** \brief The element store in the expireset_t
	 */
	template <typename T1> class elem_t : public timeout_cb_t, NEOIP_COPY_CTOR_DENY {
	private:
		T1		value;		//!< the value itself store in the expireset_t
		expireset_t<T1>*expireset;	//!< backpointer on the expireset_t
		timeout_t	timeout;	//!< the expliration timeout_t
		//! callback called when the timeout_t expire
		bool	neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw(){
			// autodelete
			nipmem_delete this;
			// return 'dontkeep'remove
			return false;
		}
	public:	
		// constructor
		elem_t(const T1 &value, const delay_t &elem_ttl, expireset_t<T1> *expireset ){
			// copy the data
			this->value	= value;
			this->expireset	= expireset;
			// put the element in the database
			bool	succeed = expireset->elem_db.insert(this).second;
			DBG_ASSERT(succeed);
			// start the timeout
			timeout.start(elem_ttl, this, NULL);
		}
		//! destructor
		~elem_t(){
			// remove the element from the database
			expireset->elem_db.erase(this);		
		}
		//! return the value store in this element
		const T1 &	get_value()		const throw() { return value;	}
		//! return the amount of time before this element expires
		delay_t		get_expire_delay()	const throw() { return timeout.get_expire_delay();}
	};
	
	/*************** internal data	***************************************/
	delay_t			default_ttl;	//!< the default ttl for the elements
	std::set<elem_t<T> *>	elem_db;	//!< to store the elements
public:
	/*************** ctor/dtor	***************************************/
	//! default constructor
	expireset_t(const delay_t &default_ttl = delay_t::ALWAYS)	throw();
	~expireset_t()							throw();
	
	//!< return the default_ttl
	const delay_t &	get_default_ttl()	const throw()	{ return default_ttl;		}
	expireset_t<T> &set_default_ttl(const delay_t &default_ttl)	throw()
					{ this->default_ttl = default_ttl; return *this;	}
	//!< return true if the expireset_t is empty, true otherwise
	bool		empty()			const throw()	{ return elem_db.empty();	}
	//! return the number of element currectly in the set
	size_t		size()			const throw()	{ return elem_db.size();	}	
	const T &	operator[](size_t idx)	const throw();
	delay_t		expire_delay_at(size_t idx)	const throw();

	/*************** query function	***************************************/
	void	insert(const T &value, delay_t elem_ttl = delay_t())	throw();
	bool	contain(const T &value)					const throw();
	void	erase(const T &value)					throw();
	void	update(const T &value, delay_t elem_ttl = delay_t())	throw();
	
	/*************** display function	*******************************/
	std::string to_string()			const throw();
	friend std::ostream & operator << ( std::ostream& os, const expireset_t<T> &expireset )
			{ return os << expireset.to_string();	}	
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//		ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
template <typename T> expireset_t<T>::expireset_t(const delay_t &default_ttl)	throw()
{
	// copy the default_ttl
	this->default_ttl	= default_ttl;
}

/** \brief Destructor
 */
template <typename T> expireset_t<T>::~expireset_t()	throw()
{
	// delete all the element of the database
	while( !elem_db.empty() ){
		typename std::set<elem_t<T> *>::iterator	iter	= elem_db.begin();
		nipmem_delete *iter;
		elem_db.erase(*iter);
	}
}

/** \brief Insert a new element in the expireset_t
 * 
 * - if the elem_ttl is null, the default_ttl of the expireset_t is used
 * 
 */
template <typename T> void expireset_t<T>::insert(const T &value, delay_t elem_ttl) throw()
{
	// sanity check - the expireset_t MUST NOT already contains the element
	DBG_ASSERT( !contain(value) );
	// if expire element is null, 
	if( elem_ttl.is_null() )	elem_ttl = default_ttl;
	// createan element with this value
	nipmem_new elem_t<T>(value, elem_ttl, this);
}

/** \brief return true if value is contained in the expireset_t, false otherwise
 */
template <typename T> bool expireset_t<T>::contain(const T &value)	const throw()
{
	typename std::set<elem_t<T> *>::const_iterator	iter;
	// go through the database
	for( iter = elem_db.begin(); iter != elem_db.end(); iter++ ){
		const elem_t<T> *	elem = *iter;
		// if the value of this element is the same, return true
		if( value == elem->get_value() )	return true;
	}
	// if no match has been found, return false
	return false;
}

/** \brief erase an value from the expireset_t
 */
template <typename T> void expireset_t<T>::erase(const T &value)	throw()
{
	typename std::set<elem_t<T> *>::iterator	iter;
	// go through the database
	for( iter = elem_db.begin(); iter != elem_db.end(); iter++ ){
		elem_t<T> *	elem = *iter;
		// if the value of this element is different, skip it
		if( value != elem->get_value() )	continue;
		// delete this element
		nipmem_delete	elem;
		// exit
		return;
	}
}

/** \brief update a element in the expireset_t (if it already exist, the timeout is refreshed)
 * 
 * ALGO:
 * -# if the element is already contained, remove it.
 * -# in anycase, insert the element.
 */
template <typename T> void expireset_t<T>::update(const T &value, delay_t elem_ttl)	throw()
{
	// if the value is already contained, erase it
	if( contain(value) )	erase(value);
	// insert the value 
	insert(value, elem_ttl);
}

/** \brief return the expire delay of the idx-th element
 */
template <typename T> delay_t expireset_t<T>::expire_delay_at(size_t idx)	const throw()
{
	typename std::set<elem_t<T> *>::const_iterator	iter;
	// sanity check - idx MUST be < size()
	DBG_ASSERT( idx < size() );
	// go through the database
	for( iter = elem_db.begin(); iter != elem_db.end(); iter++, idx-- ){
		const elem_t<T> *	elem = *iter;
		if( idx == 0 )	return	elem->get_expire_delay();
	}
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return delay_t();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   operator []
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief allow the [] operator to access in readonly the single elements
 */
template <typename T> const T &expireset_t<T>::operator[](size_t idx)	const throw()
{
	typename std::set<elem_t<T> *>::const_iterator	iter;
	// sanity check - idx MUST be < size()
	DBG_ASSERT( idx < size() );
	// go through the database
	for( iter = elem_db.begin(); iter != elem_db.end(); iter++, idx-- ){
		const elem_t<T> *	elem = *iter;
		if( idx == 0 )	return	elem->get_value();
	}
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return (*elem_db.begin())->get_value();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      to_string() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a string
 */
template <typename T> std::string  expireset_t<T>::to_string()	const throw()
{
	typename std::set<elem_t<T> *>::const_iterator	iter;
	std::ostringstream				oss;
	// display the begining of the database
	oss << "[";
	// go through the database
	for( iter = elem_db.begin(); iter != elem_db.end(); iter++ ){
		const elem_t<T> *	elem = *iter;
		// disply this element
		oss << "[" << elem->get_value() << "]";
	}
	// display the end of the database
	oss << "]";
	return oss.str();
}

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_EXPIRESET_HPP__  */



