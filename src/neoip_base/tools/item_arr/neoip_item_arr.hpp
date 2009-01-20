/*! \file
    \brief Header of the \ref item_arr_t
  
\par TODO 
- add a append() which take about item_arr_t which allows to merge 2 array
- maybe some other similar function like exlude and co
- add a += operator for a single element and for the item_arr_t too
  
*/


#ifndef __NEOIP_ITEM_ARR_HPP__ 
#define __NEOIP_ITEM_ARR_HPP__ 
/* system include */
#include <vector>
#include <algorithm>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_xmlrpc.hpp"		// for xmlrpc serialization
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;
// forward declaration
template <typename T>	class item_arr_t;
template <typename T>	serial_t& operator << (serial_t& serial, const item_arr_t<T> &item_arr)	throw();
template <typename T>	serial_t& operator >> (serial_t& serial, item_arr_t<T> &item_arr)	throw(serial_except_t);
template <typename T>	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const item_arr_t<T> &item_arr)throw();
template <typename T>	xmlrpc_parse_t& operator >> (xmlrpc_parse_t&, item_arr_t<T> &item_arr)	throw(xml_except_t);


/** \brief class definition for list of nslan_rec_t
 */
template <typename T> class item_arr_t : NEOIP_COPY_CTOR_ALLOW {
protected:
	std::vector<T>	item_db;
public:
	//! return true if the object is considered null, false otherwise
	bool	is_null()			const throw()	{ return item_db.empty();	}
	//! return true if the item_arr_t is empty, false otherwise
	bool	empty()				const throw()	{ return item_db.empty();	}
	//! return the number of node in the list
	size_t	size()				const throw()	{ return item_db.size();	}
	//! append a item to the item_arr_t
	item_arr_t<T> &	append(const T &item)	throw()		{ item_db.push_back(item); return *this;}
	item_arr_t<T> & append(const item_arr_t<T> & other)	throw();
	item_arr_t<T> & assign(size_t nb_item, const T &item)	throw();

	item_arr_t<T> &	remove(size_t idx)	throw();
	item_arr_t<T> &	sort()			throw();
	T		pop_front()		throw();
	
	/*************** search function	*******************************/
	size_t		find(const T &item)				const throw();
	bool		contain(const T &item)				const throw();
	const T *	find_first_common(const item_arr_t<T> &other)	const throw();

	/*************** operator += to act like append()	***************/	
	item_arr_t<T> &	operator += (const T & item)		throw() 	{ this->append(item);
										  return *this;		}
	item_arr_t<T> &	operator += (const item_arr_t<T> & other)throw()	{ this->append(other);	
										  return *this;		}
	item_arr_t<T>	operator + (const T &item)	const throw()
						{ item_arr_t<T> tmp(*this); tmp += item; return tmp;	}
	item_arr_t<T>	operator + (const item_arr_t<T> &other)	const throw()
						{ item_arr_t<T> tmp(*this); tmp += other; return tmp;	}

	/*************** operator []	***************************************/	
	const T &	operator[](size_t idx)	const throw()	{ DBG_ASSERT( idx < size() );
								  return item_db[idx];	}
	T &		operator[](size_t idx)	throw()		{ DBG_ASSERT( idx < size() );
								  return item_db[idx];	}

	/*************** Comparison function	*******************************/
	int	compare(const item_arr_t<T> &other)	  const throw();
	bool 	operator == (const item_arr_t<T> & other) const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const item_arr_t<T> & other) const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const item_arr_t<T> & other) const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const item_arr_t<T> & other) const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const item_arr_t<T> & other) const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const item_arr_t<T> & other) const throw()	{ return compare(other) >= 0;	}

	/*************** display function	*******************************/
	std::string to_string()			const throw();
	friend	std::ostream &	operator << (std::ostream & os, const item_arr_t<T> &item_arr) throw()
					{ return os << item_arr.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t &operator << <> (serial_t& serial, const item_arr_t<T> &item_arr)	throw();
	friend	serial_t &operator >> <> (serial_t& serial, item_arr_t<T> &item_arr) 	throw(serial_except_t);

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << <> (xmlrpc_build_t& xmlrpc_build, const item_arr_t<T> &item_arr)	throw();
	friend	xmlrpc_parse_t &operator >> <> (xmlrpc_parse_t& xmlrpc_parse, item_arr_t<T> &item_arr)	throw(xml_except_t);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Assign this item_arr_t to nb_item element all with the value of item
 */
template <typename T> item_arr_t<T> &item_arr_t<T>::assign(size_t nb_item, const T &item)	throw()
{
	// assign the item_db
	item_db.assign(nb_item, item);
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      append() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief append another item_arr_t to this one
 */
template <typename T> item_arr_t<T> &item_arr_t<T>::append(const item_arr_t<T> &other)	throw()
{
	// append all the item of other into *this
	for( size_t i = 0; i < other.size(); i++ )	this->append( other[i] );
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      remove() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief remove the item at the idx parameters
 */
template <typename T> item_arr_t<T> &item_arr_t<T>::remove(size_t idx)	throw()
{
	// sanity check - the idx MUST < size()
	DBG_ASSERT( idx < size() );
	// remove the item at idx
	item_db.erase( item_db.begin() + idx );
	// return the object itself
	return *this;
}

/** \brief remove the item at the idx parameters
 */
template <typename T> T		item_arr_t<T>::pop_front()	throw()
{
	// get the front item of the item_db
	T	value	= item_db[0];
	// remove the front item
	item_db.erase( item_db.begin() );
	// return the value
	return value;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      sort() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief remove the item at the idx parameters
 */
template <typename T> item_arr_t<T> &item_arr_t<T>::sort()			throw()
{
	// sort the item_db using the default criteria
	std::sort(item_db.begin(), item_db.end());
	// return the object itself
	return *this;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      search function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the idx of the first item of the same value or std::numeric_limits<size_t>::max()
 *         if no matches is found
 */
template <typename T> size_t	item_arr_t<T>::find(const T &item)		const throw()
{
	// go throught the whole item_db
	for(size_t i = 0; i < item_db.size(); i++ ){
		// if this item matches, return true
		if( item_db[i] == item )	return i;
	}
	// no matches has been found, so return std::numeric_limits<size_t>::max()
	return std::numeric_limits<size_t>::max();
}

/** \brief Return true if the item is in the item_arr_t, false otherwise
 */
template <typename T> bool item_arr_t<T>::contain(const T &item)	const throw()
{
	return find(item) != std::numeric_limits<size_t>::max();
}

/** \brief return a pointer on the first element of this item_arr_t present in the other
 */
template <typename T> const T * item_arr_t<T>::find_first_common(const item_arr_t<T> &other) const throw()
{
	// go throught the whole item_db
	for(size_t i = 0; i < item_db.size(); i++ ){
		// if this item matches, return a pointer on it
		if( other.contain(item_db[i]) )	return &item_db[i];
	}
	// if this point is reached, there is no match and return NULL
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      compare() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Compare the object ala memcmp
 */
template <typename T> int  item_arr_t<T>::compare(const item_arr_t<T> &other)	const throw()
{
	// compare the size
	if( this->size() > other.size() )	return +1;
	if( this->size() < other.size() )	return -1;
	// compare all the items
	for( size_t i = 0; i < this->size(); i++ ){
		if( (*this)[i] > other[i] )	return +1;
		if( (*this)[i] < other[i] )	return -1;
	}
	// here both a considered equal
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      to_string() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a string
 */
template <typename T> std::string  item_arr_t<T>::to_string()	const throw()
{
	typename std::vector<T>::const_iterator	iter;
	std::ostringstream			oss;
	// display the begining of the database
	oss << "[";
	// go through the database
	for( iter = item_db.begin(); iter != item_db.end(); iter++ ){
		// display this element
		oss << "[" << *iter << "]";
	}
	// display the begining of the database
	oss << "]";
	// return the built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial item_arr_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a item_arr_t
 */
template <typename T> serial_t& operator << (serial_t& serial, const item_arr_t<T> &item_arr)	throw()
{
	// serialize the number of item as a uint32_t
	serial << uint32_t(item_arr.size());
	// serialize the nodes in the list
	for( size_t i = 0; i < item_arr.size(); i++ )	serial << item_arr[i];		
	// return the serial_t
	return serial;
}

/** \brief unserialze a item_arr_t
 */
template <typename T> serial_t& operator >> (serial_t& serial, item_arr_t<T> &item_arr)	throw(serial_except_t)
{
	T		item;
	uint32_t	nb_elem;
	// reset the item_arr_t
	item_arr	= item_arr_t<T>();
	// unserialize the number of item
	serial >> nb_elem;
	// unserialize each node
	for( size_t i = 0; i < nb_elem; i++ ){
		serial >> item;
		item_arr.append(item);
	}
	// return the serial_t
	return serial;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a kad_addrlist_t into a xmlrpc
 */
template <typename T>
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const item_arr_t<T> &item_arr)	throw()
{
	// begin the array
	xmlrpc_build << xmlrpc_build_t::ARRAY_BEG;
	// serialize all elements of the item_arr_t
	for( size_t i = 0; i < item_arr.size(); i++ )
		xmlrpc_build	<< item_arr[i];
	// end the array 
	xmlrpc_build << xmlrpc_build_t::ARRAY_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a kad_addrlist_t into a xmlrpc
 */
template <typename T>
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, item_arr_t<T> &item_arr)	throw(xml_except_t)
{
	// reset the item_arr_t
	item_arr	= item_arr_t<T>();
	// begin the array
	xmlrpc_parse >> xmlrpc_parse_t::ARRAY_BEG;
	// go thru all the element of this array
	while( xmlrpc_parse.has_more_sibling() ){
		T	item;
		// get a kad_rec_t
		xmlrpc_parse	>> item;
		// put it in the item_arr_t
		item_arr += item;
	}
	// end the array
	xmlrpc_parse >> xmlrpc_parse_t::ARRAY_END;
	// return the object itself
	return xmlrpc_parse;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        #define to standardize the declaration
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define NEOIP_ITEM_ARR_DECLARATION_START(class_name, item_name)			\
	class class_name : public item_arr_t<item_name> {			\
	public: class_name()	throw()	{}					\
		class_name(const item_arr_t<item_name> &item_arr) throw()	\
			: item_arr_t<item_name>(item_arr)	{}
#define NEOIP_ITEM_ARR_DECLARATION_END(class_name, item_name)			\
		};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ITEM_ARR_HPP__  */










