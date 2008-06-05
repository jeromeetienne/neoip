/*! \file
    \brief Header of the mlist_iter_t

*/


#ifndef __NEOIP_MLIST_ITER_HPP__ 
#define __NEOIP_MLIST_ITER_HPP__ 
/* system include */
/* local include */
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declarations
template <typename T>	class mlist_item_t;

/** \brief class to handle a iterator on mlist_head_t
 */
template <typename T> class mlist_iter_t : NEOIP_COPY_CTOR_ALLOW {
private:
	mlist_item_t<T> *	item;	//!< pointer on the current mlist_item_t
public:
	/*************** ctor/dtor	***************************************/
	mlist_iter_t()					throw()	{ item = NULL;	}
	explicit mlist_iter_t(mlist_item_t<T> * item)	throw()	: item(item) {}
	~mlist_iter_t()					throw()	{}

	/**************	query function	***************************************/
	bool	is_null()	const throw()	{ return item == NULL;	}
	T *	object()	const throw()	{ DBG_ASSERT( !is_null() );
						  return item->object;	}

	/************** Arithmetic operator	*******************************/
	mlist_iter_t<T>&operator ++ ()		throw()	{ DBG_ASSERT( !is_null() );
							  item	= item->next; return *this;		}
	mlist_iter_t<T>&operator -- ()		throw()	{ DBG_ASSERT( !is_null() );
							  item	= item->prev; return *this;		}
	mlist_iter_t<T> operator ++ (int dummy)	throw()	{ mlist_iter_t<T> tmp(*this); ++(*this); return tmp;}
	mlist_iter_t<T>	operator -- (int dummy)	throw()	{ mlist_iter_t<T> tmp(*this); --(*this); return tmp;}

	/*************** Comparison function	*******************************/
	bool 	operator == (const mlist_iter_t<T> & other)	const throw()	{ return item == other.item;	}
	bool 	operator != (const mlist_iter_t<T> & other)	const throw()	{ return item != other.item;	}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream &	operator << (std::ostream & os, const mlist_iter_t<T> &obj)
					throw()		{ return os << obj.to_string();	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
template <typename T> std::string mlist_iter_t<T>::to_string()		const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "[";
	oss << "item="	<< item;
	oss << "]";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_MLIST_ITER_HPP__  */



