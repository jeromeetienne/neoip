/*! \file
    \brief Header of the mlist_head_t

\par Brief Description
mlist_head_t is the head of a list. mlist stands for fast-list.
- it allow to store pointer on object
  - this is a very common case in the neoip code
- it allow O(1) mlist_head_t::size()
- it allow O(1) mlist_head_t::remove(mlist_iter_t)
- this is a 'intrusive' list, aka the linked object needs to be modified and 
  contains a mlist_item_t
- the API is intended to be as close as possible to the STL iterator

\par Original purpose: have a fast LRU
- originally it has been coded to have a fast LRU
  - std::list<> is not suitable because the std::list::remove() is O(size/2)
  - and in a LRU *each* operation requires a remove+push_front so std::list is not 
    possible
  - with this mlist_head_t, remove is now O(1)


*/


#ifndef __NEOIP_MLIST_HEAD_HPP__ 
#define __NEOIP_MLIST_HEAD_HPP__ 
/* system include */
/* local include */
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declarations
template <typename T>	class mlist_item_t;
template <typename T>	class mlist_iter_t;

/** \brief class to handle the head of the mlist
 */
template <typename T> class mlist_head_t : NEOIP_COPY_CTOR_DENY {
private:
	size_t			count;	//!< the current number of item in the list
	mlist_item_t<T>	*	first;	//!< pointer on the first item
	mlist_item_t<T>	*	last;	//!< pointer on the last item
public:
	/*************** ctor/dtor	***************************************/
	mlist_head_t()		throw();
	~mlist_head_t()		throw();
	
	/*************** query function	***************************************/
	bool		empty()		const throw()	{ return count == 0;		}
	size_t		size()		const throw()	{ return count;			}
	mlist_iter_t<T>	begin()		const throw()	{ return mlist_iter_t<T>(first);}
	mlist_iter_t<T>	end()		const throw()	{ return mlist_iter_t<T>(NULL);	}
	T *		front()		const throw()	{ DBG_ASSERT( !empty() );
							  return first->object;		}
	T *		back()		const throw()	{ DBG_ASSERT( !empty() );
							  return last->object;		}
	void		pop_front()	const throw()	{ remove( *first );		} 
	void		pop_back()	const throw()	{ remove( *last ); 		}
	mlist_iter_t<T>	find(T *object)	const throw();

	/*************** Action function	*******************************/
	void		push_front(mlist_item_t<T> &item, T *object)	throw();
	void		remove(mlist_item_t<T> &item)			throw();
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const mlist_head_t<T> &obj)
					throw()		{ return os << obj.to_string();	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
template <typename T> mlist_head_t<T>::mlist_head_t()	throw()
{
	// init some fields
	count	= 0;
	first	= NULL;
	last	= NULL;
}

/** \brief Destructor
 */
template <typename T> mlist_head_t<T>::~mlist_head_t()	throw()
{
	// sanity check - mlist_head_t MUST be empty
	DBG_ASSERT( empty() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Push this object in front of the mlist
 * 
 * - TODO make a similar function push_back 
 *   - basically reverse all the first/next with last/prev
 */ 
template <typename T> void mlist_head_t<T>::push_front(mlist_item_t<T> &item, T *object)
										throw()
{
	// log to debug
	KLOG_DBG("enter head=" << *this << " item=" << item << " object=" << object); 

	// sanity check - mlist_item_t MUST be null
	DBG_ASSERT( item.is_null() );

	// update the count
	count++;

// update the main stuff of the item
	item.head	= this;
	item.object	= object;

// handle the next case
	item.next	= first;
	first		= &item;
	if( item.next ){
		DBG_ASSERT( item.next->prev == NULL );
		item.next->prev	= &item;
	}
	
// handle the prev case
	item.prev	= NULL;
	if( !last )	last	= &item;

	// log to debug
	KLOG_DBG("leave head=" << *this << " item=" << item << " object=" << object); 
}

/** \brief Push this object in front of the mlist
 */ 
template <typename T> void mlist_head_t<T>::remove(mlist_item_t<T> &item)	throw()
{
	// log to debug
	KLOG_DBG("enter head=" << *this << " item=" << item);
	
	// sanity check - mlist_item_t MUST NOT be null
	DBG_ASSERT( !item.is_null() );
	
	// update the count
	count--;
	
	// handle the item.next case (with and without item *after* this one)
	if( item.next ){
		// if there is a next item after this one, link it to the previous one
		item.next->prev	= item.prev;
	}else{
		// if there is no next item, mlist_head_t::last MUST be this item
		DBG_ASSERT( last == &item );
		// link the last to the item before this one
		last	= item.prev;
	}
	
	// handle the item.prev case (with and without item *before* this one)
	if( item.prev ){
		// if there is a previous item before this one, link it to the next one
		item.prev->next	= item.next;
	}else{
		// if there is no prev item, mlist_head_t::first MUST be this item
		DBG_ASSERT( first == &item );
		// link the last to the item after this one
		first	= item.next;
	}
	
	
	// reset the item
	item.head	= NULL;
	item.object	= NULL;
	// sanity check - mlist_item_t MUST be null
	DBG_ASSERT( item.is_null() );
	// log to debug
	KLOG_DBG("leave head=" << *this << " item=" << item); 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a mlist_iter_t pointing on the object passed in parameter
 */
template <typename T>	mlist_iter_t<T>	mlist_head_t<T>::find(T *object)	const throw()
{
	mlist_iter_t<T>	iter;
	// go thru the list
	for(iter = begin(); iter != end(); iter++){
		// if this iter points on a item with this object, return this iter
		if( object == iter.object() )	return iter;
	}
	// if no item points on this object, return end()
	return end();	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
template <typename T> std::string mlist_head_t<T>::to_string()		const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "[";
	oss << "count=" << count;
	oss << " first="<< first;
	oss << " last="	<< last;
	oss << "]";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_MLIST_HEAD_HPP__  */



