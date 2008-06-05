/*! \file
    \brief Header of the interval_t template

*/


#ifndef __NEOIP_INTERVAL_HPP__ 
#define __NEOIP_INTERVAL_HPP__ 
/* system include */
#include <vector>
#include <functional>
/* local include */
#include "neoip_interval_item.hpp"
#include "neoip_serial.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
template <typename T>	class interval_t;
template <typename T>	serial_t& operator << (serial_t& serial, const interval_t<T> &interval)	throw();
template <typename T>	serial_t& operator >> (serial_t& serial, interval_t<T> &interval)	throw(serial_except_t);


//! shortcut to get the interval_item_db_t type
template <typename T> class interval_item_db_t : public std::vector<interval_item_t<T> > {};

/** \brief Template class to handle interval
 * 
 * - aka interval of presence/absence
 * - the templated type T MUST support all comparison operator (aka not only '<')
 *   - it could be done with only '<' but im lazy
 * - the templated type T MUST support operator + and operator - for int(1)
 *   - it could be done thru operator ++ and -- but it make the interval_t code less
 *     clean
 */
template <typename T> class interval_t {
private:
	interval_item_db_t<T>	item_db;	//!< the database of all the interval_item_t
	
	/*************** Internal Function	*******************************/
	bool		is_sane()	const throw();
public:
	/*************** ctor/dtor	***************************************/
	interval_t()						throw() {}
	explicit interval_t(const interval_item_t<T> &item)	throw();

	/*************** Query Function	***************************************/
	bool				empty()			const throw()	{ return item_db.empty();}
	size_t				size()			const throw()	{ return item_db.size();}
	const interval_item_t<T> &	operator[](size_t idx)	const throw()	{ DBG_ASSERT(idx<size());
										  return item_db[idx];	}
	T				total_length()		const throw();
	T				sum()			const throw()	{ return total_length();}

	/*************** Query Set Function	*******************************/
	bool		is_distinct(const interval_item_t<T> &other)	const throw();
	bool		is_distinct(const interval_t<T> &other)		const throw();
	bool		fully_include(const interval_item_t<T> &other)	const throw();
	bool		fully_include(const interval_t<T> &other)	const throw();
	bool		contain(const T &element)			const throw()
					{ return this->fully_include(interval_item_t<T>(element));	}
	const interval_item_t<T> *	get_item_containing(const T &element)	const throw();
	/*************** Comparison function	*******************************/
	int	compare(const interval_t<T> &other)	  const throw();
	bool 	operator == (const interval_t<T> & other) const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const interval_t<T> & other) const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const interval_t<T> & other) const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const interval_t<T> & other) const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const interval_t<T> & other) const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const interval_t<T> & other) const throw()	{ return compare(other) >= 0;	}

	/*************** arithmetic operator on interval_t	***************/
	void 		operator +=(const interval_t<T> &other)			throw();
	void 		operator -=(const interval_t<T> &other)			throw();
	interval_t<T>	operator + (const interval_t<T> &other)			const throw()
						{ interval_t<T> tmp(*this); tmp += other; return tmp;	}
	interval_t<T>	operator - (const interval_t<T> &other)			const throw()
						{ interval_t<T> tmp(*this); tmp -= other; return tmp;	}

	/*************** arithmetic operator on interval_item_t	***************/
	void		operator +=(const interval_item_t<T> &item)		throw();
	void		operator -=(const interval_item_t<T> &item)		throw();
	interval_t<T>	operator + (const interval_item_t<T> &item)		const throw()
						{ interval_t<T> tmp(*this); tmp += item; return tmp;	}
	interval_t<T>	operator - (const interval_item_t<T> &item)		const throw()
						{ interval_t<T> tmp(*this); tmp -= item; return tmp;	}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &operator << (std::ostream & os, const interval_t<T> &interval) throw()
						{ return os << interval.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t &operator << <> (serial_t& serial, const interval_t<T> &item)	throw();
	friend	serial_t &operator >> <> (serial_t& serial, interval_t<T> &item)	throw(serial_except_t);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Explicit contructor from a interval_item_t
 */
template <typename T>
interval_t<T>::interval_t(const interval_item_t<T> &item)	throw()
{
	*this += item;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Interval function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a sum of all the interval_item_t::lenght()
 */
template <typename T>
T	interval_t<T>::total_length()		const throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;
	T						total	= 0;
	// go thru the whole item_db
	for( iter = item_db.begin(); iter != item_db.end(); iter++ ){
		const interval_item_t<T> &	item = *iter;
		// update the total
		total	+= item.length();
	}
	// return the result
	return total;
}

/** \brief Return true if the interval_t is sane, false otherwise
 * 
 * - this function is used internally to detect bugs via assert()
 *   - aka check if the interval_t is sane when it is supposed to be
 */
template <typename T>
bool	interval_t<T>::is_sane()	const throw()
{
	typename interval_item_db_t<T>::const_iterator	prev	= item_db.end();	
	typename interval_item_db_t<T>::const_iterator	iter;
	// log to debug
	KLOG_DBG("before testing=" << *this);
	// go thru the whole item_db
	for( iter = item_db.begin(); iter != item_db.end(); prev = iter, iter++ ){
		const interval_item_t<T> &	cur = *iter;
		// if the min_value is > the max_value, it is NOT sane
		if( cur.min_value() > cur.max_value() )		return false;
		// if there are no previous item yet, goto the next item
		if( prev == item_db.end() )			continue;
		// the prev max_value MUST be < than the current min_value
		if( prev->max_value() >= cur.min_value() )	return false;
		// the prev mac_value+1 MUST be < than the current min value
		if( prev->max_value()+1 >= cur.min_value() )	return false;
	}
	// if all the tests passed, return true
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare function - retuning result ala memcmp 
 */
template <typename T>
int interval_t<T>::compare(const interval_t<T> &other)	const throw()
{
	// sanity check - both interval_t MUST be sane
	DBG_ASSERT( is_sane() && other.is_sane() );	

	// compare the size() - the smaller size is less
	if( size() < other.size() )	return -1;
	if( size() > other.size() )	return +1;
	// NOTE: here both have the same size()
	
	// compare each item for both interval_t
	typename interval_item_db_t<T>::const_iterator	iter1	 = this->item_db.begin();	
	typename interval_item_db_t<T>::const_iterator	iter2	 = other.item_db.begin();	
	for(; iter1 != this->item_db.end(); iter1++, iter2++){
		const interval_item_t<T> &	item1	= *iter1;
		const interval_item_t<T> &	item2	= *iter2;
		// if both item are equal, goto the next
		if( item1 == item2 )				continue;
		// the smaller min_value is less
		if( item1.min_value() < item2.min_value() )	return -1;
		if( item1.min_value() > item2.min_value() )	return +1;
		// the greater max_value is more
		if( item1.max_value() < item2.max_value() )	return -1;
		if( item1.max_value() > item2.max_value() )	return +1;
		// this point MUST never be reached
		DBG_ASSERT( 0 );
	}
	// here both are considered equal
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         Query Set Function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief return true if the interval_item_t is_distinct from the interval_t
 */
template <typename T>
bool	interval_t<T>::is_distinct(const interval_item_t<T> &item)		const throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;
	// for thru the whole item_db of other
	for( iter = item_db.begin(); iter != item_db.end(); iter++ ){
		// if all remaining iter have no chance to overlap item, exit the loop
		if( iter->min_value() > item.max_value() )	break;
		// if this iter is not distinct from iter, return false
		if( !iter->is_distinct(item) )			return false;
	}
	// if this point is reached, both interval are distinct
	return true;
}

/** \brief return true if both interval_t are distinct, false otherwise
 */
template <typename T>
bool	interval_t<T>::is_distinct(const interval_t<T> &other)		const throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;
	// for thru the whole item_db of other
	for( iter = other.item_db.begin(); iter != other.item_db.end(); iter++ ){
		if( !is_distinct(*iter) )	return false;
	}	
	// if this point is reached, both interval are distinct
	return true;
}

/** \brief return true if the interval_item_t is fully included in the interval_t
 * 
 */
template <typename T>
bool	interval_t<T>::fully_include(const interval_item_t<T> &item)		const throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;
	// for thru the whole item_db of other
	for( iter = item_db.begin(); iter != item_db.end(); iter++ ){
		// if all remaining iter have no chance to overlap item, exit the loop
		if( iter->min_value() > item.max_value() )	break;
		// if iter fully include item, return true
		if( iter->fully_include(item) )			return true;
	}
	// if this point is reached, the item is not fully included in the interval_t
	return false;
}


/** \brief return true if this interval_t fully include the other, false otherwise
 */
template <typename T>
bool	interval_t<T>::fully_include(const interval_t<T> &other)	const throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;
	// for thru the whole item_db of other
	for( iter = other.item_db.begin(); iter != other.item_db.end(); iter++ ){
		if( !fully_include(*iter) )	return false;
	}	
	// if this point is reached, this interval_t fully include the other
	return true;
}

/** \brief Return the iterval_item_t containing this element, or NULL if none matches
 */
template <typename T>
const interval_item_t<T> *	interval_t<T>::get_item_containing(const T &element)	const throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;
	// for thru the whole item_db of other
	for( iter = item_db.begin(); iter != item_db.end(); iter++ ){
		const interval_item_t<T> &	item = *iter;
		// if this item.min_value is greater than element, leave the loop		
		if( element < item.min_value() )	break;
		// if the element is < item.max_value(), goto the next item
		if( element > item.max_value() )	continue;
		// if this point is reached, the element is contained by this item, so return it
		return &item;
	}	
	// return NULL - as no interval_item_t containing this element has been found
	return NULL;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         arithmetic with interval_item_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief operator += with a interval_t
 */
template <typename T>
void interval_t<T>::operator +=(const interval_t<T> &other)			throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;
	// for thru the whole item_db of other
	for( iter = other.item_db.begin(); iter != other.item_db.end(); iter++ ){
		// add this item to this object
		*this += *iter;
	}
	// sanity check - the interval_t MUST be sane
	DBG_ASSERT( is_sane() );
}

/** \brief operator -= with a interval_t
 */
template <typename T>
void interval_t<T>::operator -=(const interval_t<T> &other)			throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;
	// for thru the whole item_db of other
	for( iter = other.item_db.begin(); iter != other.item_db.end(); iter++ ){
		// remove this item to this object
		*this -= *iter;
	}
	// sanity check - the interval_t MUST be sane
	DBG_ASSERT( is_sane() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         arithmetic with interval_item_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief operator += with a interval_item_t
 */
template <typename T>
void interval_t<T>::operator +=(const interval_item_t<T> &item)			throw()
{
	typename interval_item_db_t<T>::iterator	iter;
	interval_item_t<T>				base = item;
	// first pass: find first
	for( iter = item_db.begin();	iter != item_db.end()
					&& base.min_value() > iter->max_value()
					&& !base.is_contiguous(*iter)
					; iter++);
	// second pass: absorbe all redundant
	while( iter != item_db.end() && (base.max_value() >= iter->min_value()
					|| base.is_contiguous(*iter)) ){
		// unify the iter in the base
		base.unify(*iter);
		// remove and update the iter
		iter = item_db.erase(iter);
	}
	// third pass: insert base
	item_db.insert( iter, base );
	// sanity check - the interval_t MUST be sane
	DBG_ASSERT( is_sane() );
}

/** \brief operator -= with a interval_item_t
 */
template <typename T>
void interval_t<T>::operator -=(const interval_item_t<T> &item)			throw()
{
	typename interval_item_db_t<T>::iterator	iter;
	interval_item_t<T>				base = item;	
	// first pass: find first
	for(iter = item_db.begin(); iter!=item_db.end() && base.min_value() > iter->max_value(); iter++);

	// second pass: exlude base
	while( iter != item_db.end() && base.max_value() >= iter->min_value()){
		// sanity check - iter MUST NOT be distinct from base
		DBG_ASSERT( !iter->is_distinct(base) );
		// backup the item at the iter position
		interval_item_t<T>	temp = *iter;
		// remove it from the database
		iter = item_db.erase(iter);

		// include the remaining temp item less than base, if any
		if( temp.min_value() < base.min_value() ){
			iter = item_db.insert( iter, interval_item_t<T>(temp.min_value(),base.min_value() - 1 ));
			iter++;
		}

		// include the remaining temp item greater than base, if any
		if( base.max_value() < temp.max_value() ){
			iter = item_db.insert( iter, interval_item_t<T>(base.max_value() + 1, temp.max_value()));
			iter++;
		}
	}
	// sanity check - the interval_t MUST be sane
	DBG_ASSERT( is_sane() );
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a std::string
 */
template <typename T> 
std::string interval_t<T>::to_string()	const throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;	
	std::ostringstream				oss;
	// display the begining of the database
	oss << "[";
	// go through the database
	for( iter = item_db.begin(); iter != item_db.end(); iter++ ){
		const interval_item_t<T> &	item = *iter;
		// display this element
		oss << item;
	}
	// display the end of the database
	oss << "]";
	return oss.str();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial interval_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a interval_item_t
 */
template <typename T> 
serial_t& operator << (serial_t& serial, const interval_t<T> &interval)	throw()
{
	typename interval_item_db_t<T>::const_iterator	iter;	
	// sanity check - the interval_t MUST be sane
	DBG_ASSERT( interval.is_sane() );
	// serialize the number of item as a uint32_t
	serial << uint32_t(interval.item_db.size());
	// for thru the whole item_db of other
	for( iter = interval.item_db.begin(); iter != interval.item_db.end(); iter++ )
		serial << *iter;
	// return the object itself
	return serial;
}

/** \brief unserialze a interval_item_t
 */
template <typename T> 
serial_t& operator >> (serial_t& serial, interval_t<T> &interval)  		throw(serial_except_t)
{	
	interval_item_t<T>	item;
	uint32_t		nb_elem;	
	// zero the destination object
	interval	= interval_t<T>();
	// unserialize the number of item
	serial >> nb_elem;
	// unserialize each item
	for(size_t i = 0; i < nb_elem; i++){
		serial	>> item;
		interval += item;
	}
	// return the object itself
	return serial;
}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_INTERVAL_HPP__  */



