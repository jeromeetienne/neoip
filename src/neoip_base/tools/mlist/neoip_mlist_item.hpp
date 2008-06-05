/*! \file
    \brief Header of the mlist_item_t
    
*/


#ifndef __NEOIP_MLIST_ITEM_HPP__ 
#define __NEOIP_MLIST_ITEM_HPP__ 
/* system include */
/* local include */
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declarations
template <typename T>	class mlist_head_t;
template <typename T>	class mlist_iter_t;

/** \brief class to handle a given item in the mlist_head_t
 */
template <typename T> class mlist_item_t : NEOIP_COPY_CTOR_DENY {
private:
	mlist_head_t<T> *	head;	//!< pointer on the mlist_head_t
	T *			object;	//!< pointer on the object for this item
	mlist_item_t<T>	*	prev;	//!< pointer on the prev item
	mlist_item_t<T>	*	next;	//!< pointer on the next item
public:
	/*************** ctor/dtor	***************************************/
	mlist_item_t()		throw();
	~mlist_item_t()		throw();
	
	/*************** query function	***************************************/
	bool	is_null()		const throw()	{ return !head && !object;	}
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const mlist_item_t<T> &obj)
					throw()		{ return os << obj.to_string();	}

	/*************** List of friend class	*******************************/
	friend	class mlist_iter_t<T>;
	friend	class mlist_head_t<T>;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
template <typename T> mlist_item_t<T>::mlist_item_t()	throw()
{
	// zero some fields
	head	= NULL;
	object	= NULL;
	next	= NULL;
	prev	= NULL;
}

/** \brief Destructor
 */
template <typename T> mlist_item_t<T>::~mlist_item_t()	throw()
{
	// sanity check - it MUST be null on dtor
	DBG_ASSERT( is_null() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
template <typename T> std::string mlist_item_t<T>::to_string()		const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "[";
	oss << "head="	<< head;
	oss << " object="<< object;
	oss << " prev="	<< prev;
	oss << " next="	<< next;
	oss << "]";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_MLIST_ITEM_HPP__  */



