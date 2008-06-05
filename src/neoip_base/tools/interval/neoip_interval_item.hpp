/*! \file
    \brief Header of the interval_item_t template

*/


#ifndef __NEOIP_INTERVAL_ITEM_HPP__ 
#define __NEOIP_INTERVAL_ITEM_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
template <typename T> class interval_item_t;
template <typename T> serial_t& operator << (serial_t& serial, const interval_item_t<T> &item)throw();
template <typename T> serial_t& operator >> (serial_t& serial, interval_item_t<T> &item)throw(serial_except_t);

/** \brief Store a single item in the interval_t template
 */
template <typename T> class interval_item_t {
private:
	T	min_val;	//!< the minimum value included in this interval_item_t
	T	max_val;	//!< the maximum value included in this interval_item_t
public:
	/*************** ctor/dtor	***************************************/
	interval_item_t()					throw() {}
	interval_item_t(const T &min_value, const T &max_value)	throw();
	explicit interval_item_t(const T &one_value)		throw();

	/*************** Query Function	***************************************/
	const T &	min_value()	const throw()	{ return min_val;		}
	const T &	max_value()	const throw()	{ return max_val;		}
	T 		length()	const throw()	{ return max_val - min_val + 1;	}
	bool		is_distinct(const interval_item_t<T> &other)	const throw();
	bool		fully_include(const interval_item_t<T> &other)	const throw();
	bool		is_contiguous(const interval_item_t<T> &other)	const throw();
	
	interval_item_t<T> &unify(const interval_item_t<T> &other)	throw();

	/*************** Set Function	***************************************/
	interval_item_t<T> &min_value(const T &min_value)		throw();
	interval_item_t<T> &max_value(const T &max_value)		throw();
	
	/*************** Comparison function	*******************************/
	bool 	operator == (const interval_item_t<T> & other)	const throw();
	bool 	operator != (const interval_item_t<T> & other)	const throw()	{ return !(*this == other);	}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &operator << (std::ostream & os, const interval_item_t<T> &item) throw()
					{ return os << item.to_string();	}
		
	/*************** serialization function	*******************************/
	friend	serial_t &operator << <> (serial_t& serial, const interval_item_t<T> &item)	throw();
	friend	serial_t &operator >> <> (serial_t& serial, interval_item_t<T> &item)	 	throw(serial_except_t);
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//		ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor with 2 parameters
 */
template <typename T>
interval_item_t<T>::interval_item_t(const T &min_value, const T &max_value) throw()
{
	// log to debug
	KLOG_DBG("min_value=" << min_value << " max_value=" << max_value);
	// copy the paramter
	this->min_val	= min_value;
	this->max_val	= max_value;
	// sanity check - the min_val MUST BE <= max_val
	DBG_ASSERT( min_val <= max_val );
}

/** \brief Default constructor with 1 parameter (
 */
template <typename T>
interval_item_t<T>::interval_item_t(const T &one_value) throw()
{
	// copy the paramter
	this->min_val	= one_value;
	this->max_val	= one_value;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Set function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Change the min_value
 */
template <typename T>
interval_item_t<T> &	interval_item_t<T>::min_value(const T &min_value)	throw()
{
	// copy the parameter
	this->min_val	= min_value;
	// sanity check - the min_val MUST BE <= max_val
	DBG_ASSERT( min_val <= max_val );	
	// return the object itself
	return *this;
}

/** \brief Change the max_value
 */
template <typename T>
interval_item_t<T> &	interval_item_t<T>::max_value(const T &max_value)	throw()
{
	// copy the parameter
	this->max_val	= max_value;
	// sanity check - the min_val MUST BE <= max_val
	DBG_ASSERT( min_val <= max_val );
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    comparison operator
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief operator ==
 */
template <typename T>
bool	interval_item_t<T>::operator == (const interval_item_t<T> & other)	const throw()
{
	if( min_val != other.min_val )	return false;
	if( max_val != other.max_val )	return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Query function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true if this item item is dictinct from the other, false otherwise
 */
template <typename T>
bool interval_item_t<T>::is_distinct(const interval_item_t<T> &other)	const throw()
{
	// if this item is enterly 'less' than the other, return true
	if( max_val < other.min_val )	return true;
	// if this item is enterly 'more' than the other, return true
	if( min_val > other.max_val )	return true;
	// else return false
	return false;
}

/** \brief return true if this item fully include from the other, false otherwise
 */
template <typename T>
bool interval_item_t<T>::fully_include(const interval_item_t<T> &other)	const throw()
{
	// if the other item is fully included in this item, return true
	if( other.max_val <= max_val && other.min_val >= min_val )	return true;
	// else return false
	return false;
}

/** \brief return true if both item are contiguous, false otherwise
 */
template <typename T>
bool interval_item_t<T>::is_contiguous(const interval_item_t<T> &other)	const throw()
{
	// if the other interval_item_t is contiguous and greater than this interval_item_t, return true
	if( max_val + 1 == other.min_val )		return true;
	// if the other interval_item_t is contiguous and less    than this interval_item_t, return true
	if( min_val     == other.max_val + 1 )	return true;
	// else they are NOT contiguous
	return false;
}

/** \brief unify the other interval_item_t with this interval_item_t
 */
template <typename T>
interval_item_t<T> &interval_item_t<T>::unify(const interval_item_t<T> &other)	throw()
{
	// sanity check - the two interval_item_t MUST be either not distinct or contiguous
	DBG_ASSERT( !is_distinct(other) || is_contiguous(other));
	// this min_val MUST be the least of the two
	if( min_val > other.min_val )	min_val = other.min_val;
	// this min_val MUST be the greatest of the two
	if( max_val < other.max_val )	max_val	= other.max_val;
	// return the object itself
	return *this;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
template <typename T> 
std::string interval_item_t<T>::to_string()	const throw()
{
	std::ostringstream	oss;
	oss << "[";
	oss << min_value();
	oss << ", ";
	oss << max_value();
	oss << "]";
	return oss.str();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial item_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a interval_item_t
 */
template <typename T> 
serial_t& operator << (serial_t& serial, const interval_item_t<T> &item)	throw()
{
	// serialize the object
	serial << item.min_val;
	serial << item.max_val;
	// return the object itself
	return serial;
}

/** \brief unserialze a interval_item_t
 */
template <typename T> 
serial_t& operator >> (serial_t& serial, interval_item_t<T> &item)  		throw(serial_except_t)
{
	T	min_val, max_val;
	// unserialize the object
	serial >> min_val;
	serial >> max_val;
	// set the object
	item	= interval_item_t<T>(min_val, max_val);
	// return the object itself
	return serial;
}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_INTERVAL_ITEM_HPP__  */



