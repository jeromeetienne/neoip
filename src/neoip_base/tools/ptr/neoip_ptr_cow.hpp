/*! \file
    \brief Header of the ptr_cow_t
*/


#ifndef __NEOIP_PTR_COW_HPP__ 
#define __NEOIP_PTR_COW_HPP__ 
/* system include */
/* local include */
#include "neoip_ptr_counted.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a copy-on-write pointer
 * 
 * - based on ptr_counted_t
 */
template <typename T> class ptr_cow_t : NEOIP_COPY_CTOR_ALLOW {
private:
	ptr_counted_t<T>	ptr_counted;
	
	/** \brief copy the pointed object - called in case of non-const reference
	 */
	void copy()	throw()
	{
		// if ptr_counted is unique, do nothing
		if( ptr_counted.unique() )	return;
		// copy the value
		T *	old_ptr	= ptr_counted.get_ptr();
		if( old_ptr )	ptr_counted	= ptr_counted_t<T>( nipmem_new T(*old_ptr) );
		else		ptr_counted	= ptr_counted_t<T>( NULL );
	}
public:
	/*************** ctor/dtor	***************************************/
	explicit ptr_cow_t(T *ptr = NULL)	throw() : ptr_counted(ptr)	{}

	/*************** copy stuff	***************************************/
	ptr_cow_t(const ptr_cow_t &other)	throw()	: ptr_counted(other.ptr_counted)	{}
	ptr_cow_t &	operator =(const ptr_cow_t & other)	throw()
	{
		// if it is a self-assignement, do nothing
		if( this == &other )	return *this;
		// copy the ptr_counted_t
		ptr_counted	= other.ptr_counted;
		// return the object itself
		return *this;
	}

	/*************** non-const pointer operation	***********************/
	T &		operator *()	throw()		{ copy(); return *ptr_counted;		}
	T *		operator->()	throw()		{ copy(); return ptr_counted.get_ptr();	}
	T *		get_ptr()	throw()		{ copy(); return ptr_counted.get_ptr();	}
	/*************** const pointer operation	***********************/
	const T &	operator *()	const throw()	{ return *ptr_counted;			}
	const T *	operator->()	const throw()	{ return ptr_counted.get_ptr();		}
	const T *	get_ptr()	const throw()	{ return ptr_counted.get_ptr();		}

	/*************** management function	*******************************/
	size_t		count()		const throw()	{ return ptr_counted.count();	}
	size_t		unique()	const throw()	{ return ptr_counted.unique();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PTR_COW_HPP__  */



