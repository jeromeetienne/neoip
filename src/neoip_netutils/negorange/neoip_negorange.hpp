/*! \file
    \brief Header of the negorange_t
*/


#ifndef __NEOIP_NEGORANGE_HPP__ 
#define __NEOIP_NEGORANGE_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_assert.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
template <class T> class negorange_t;
template <class T> std::ostream& operator << ( std::ostream& os, const negorange_t<T> &negorange )
										throw();
template <class T> serial_t& operator << ( serial_t& serial, const negorange_t<T> &negorange )
										throw();
template <class T> serial_t& operator >> ( serial_t& serial, negorange_t<T> &negorange )
										throw(serial_except_t);

/** \brief class to handle the negociation of a range
 */
template <typename T> class negorange_t {
private:
	T	min_value;
	T	max_value;
	T	wished_value;
	bool	is_not_null;		//!< only here to answer the is_null() function
	
	negorange_t<T>	get_common(const negorange_t<T> &other, int wish_pref ) const throw();
public:
	//! default constructor
	negorange_t()		throw()	{ nullify(); };
	//! Constructor from values
	negorange_t(const T &min_value, const T &max_value, const T &wished_value)	throw() {
		// set local variable
		this->min_value		= min_value;
		this->max_value 	= max_value;
		this->wished_value	= wished_value;		
		is_not_null		= true;
		// sanity check - the wished value MUST be between the min and max included
		DBG_ASSERT( min_value <= wished_value );
		DBG_ASSERT( max_value >= wished_value );
	}

	T	get_min()	const throw()	{ DBG_ASSERT( !is_null() );	return min_value;	}
	T	get_max()	const throw()	{ DBG_ASSERT( !is_null() );	return max_value;	}
	T	get_wished()	const throw()	{ DBG_ASSERT( !is_null() );	return wished_value;	}

	bool	is_null()	const throw()	{ return is_not_null == false;	}
	void	nullify()	throw()		{ is_not_null = false;		}
	

	negorange_t<T>	get_common_local_wish(const negorange_t<T> &other)	const throw();
	negorange_t<T>	get_common_remote_wish(const negorange_t<T> &other)	const throw();
	negorange_t<T>	get_common_average_wish(const negorange_t<T> &other)	const throw();
	
	
friend std::ostream& operator << <> (std::ostream& os, const negorange_t<T> &negorange)	throw();
friend serial_t& operator << <> (serial_t& serial, const negorange_t<T> &negorange)	throw();
friend serial_t& operator >> <> (serial_t& serial, negorange_t<T> &negorange) 	throw(serial_except_t);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          get_common functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the common range between *this and other. may be null if no common range exists
 * 
 * - the common wished_value the closed to the local wish as possible
 */
template <class T> 
negorange_t<T>	negorange_t<T>::get_common_local_wish(const negorange_t<T> &other)	const throw()
{
	return get_common(other, +1);
}

/** \brief return the common range between *this and other. may be null if no common range exists
 * 
 * - the common wished_value the closed to the remote wish as possible
 */
template <class T> 
negorange_t<T>	negorange_t<T>::get_common_remote_wish(const negorange_t<T> &other)	const throw()
{
	return get_common(other, -1);
}

/** \brief return the common range between *this and other. may be null if no common range exists
 * 
 * - the common wished_value is an average between the two and then clamped by the 
 *   common min and max.
 */
template <class T> 
negorange_t<T>	negorange_t<T>::get_common_average_wish(const negorange_t<T> &other)	const throw()
{
	return get_common(other,  0);
}
	
/** \brief return the common range between *this and other. may be null if no common range exists
 * 
 * @param wish_pref determine how the common wished value is computed
 *                  >  0 means to prefer the local value
 *                  == 0 means to average both wishes
 *                  <  0 means to prefer the remote value
 * @return the common range with wished_value being the negociated one. if there is 
 *         no common range, return a null one
 */
template <class T> 
negorange_t<T>	negorange_t<T>::get_common(const negorange_t<T> &other, int wish_pref ) const throw()
{
	T	common_min, common_max, common_wished;

	// sanity check - local and other range MUST NOT be null
	DBG_ASSERT( !is_null() );
	DBG_ASSERT( !other.is_null() );

	// if there are no intersection between the range, return null
	if( other.max_value < min_value )	return negorange_t<T>();
	if( max_value < other.min_value )	return negorange_t<T>();
	
	// compute the common range
	common_min	= min_value < other.min_value ? other.min_value : min_value;
	common_max	= max_value < other.max_value ? max_value : other.max_value;
	
	// compute the common wished value
	if( wish_pref == 0 ){
		// compute a wished value as middle ground between the *this range and the other range
		common_wished	= (wished_value + other.wished_value) / T(2);
	}else if( wish_pref < 0 ){
		// compute a wished value prefering remote wish
		common_wished	= other.wished_value;
	}else{
		// compute a wished value prefering local wish
		common_wished	= wished_value;
	}
	// clamp the common_wished value with the common min/max
	if( common_wished > common_max )	common_wished = common_max;
	if( common_wished < common_min )	common_wished = common_min;
	// return the result
	return negorange_t<T>(common_min, common_max, common_wished);		
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
template <class T> std::ostream& operator << ( std::ostream& os, const negorange_t<T> &negorange )
										throw()
{
	os << "whishing "	<< negorange.wished_value;
	os << " but accepting";
	os << " from "		<< negorange.min_value;
	os << " to "		<< negorange.max_value;
	return os;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          SERIALIZATION
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a negorange_t
 */
template <class T>
serial_t& operator << ( serial_t& serial, const negorange_t<T> &negorange )	throw()
{
	serial	<< negorange.min_value;
	serial	<< negorange.max_value;
	serial	<< negorange.wished_value;
	return serial;
}

/** \brief unserialize a negorange_t
 */
template <class T>
serial_t& operator >> ( serial_t& serial, negorange_t<T> &negorange )		throw(serial_except_t)
{
	T	min_value, max_value, wished_value;
	serial	>> min_value;
	serial	>> max_value;
	serial	>> wished_value;
	// copy to the negorange the unserialize values
	negorange = negorange_t<T>( min_value, max_value, wished_value );
	return serial;
}

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_NEGORANGE_HPP__  */



