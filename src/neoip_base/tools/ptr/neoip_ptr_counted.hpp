/*! \file
    \brief Header of the nunit_gsuite_t
*/


#ifndef __NEOIP_PTR_COUNTED_HPP__ 
#define __NEOIP_PTR_COUNTED_HPP__ 
/* system include */
#include <typeinfo>
/* local include */
#include "neoip_assert.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a reference counting pointer
 * 
 * - it uses an external counter
 *   - as opposed to ptr_linked_t which uses an internal counter
 */
template <typename T> class ptr_counted_t : NEOIP_COPY_CTOR_ALLOW {
private:
	/** \brief Definition of the struct containing the external counter
	 */
	struct counter_t : NEOIP_COPY_CTOR_ALLOW {
		counter_t(T * ptr = NULL, size_t count = 0) throw() : ptr(ptr), count(count) {}
		T *	ptr;
		size_t	count;
	};

	counter_t *	counter;	//!< pointer on the external counter struct

	void		acquire_counter(counter_t *other)	throw();
	void		release_counter()			throw();
public:
	/*************** ctor/dtor	***************************************/
	explicit ptr_counted_t(T *ptr = NULL)	throw();
	~ptr_counted_t()			throw();

	/*************** copy stuff	***************************************/
	ptr_counted_t(const ptr_counted_t &other)		throw();
	ptr_counted_t &	operator=(const ptr_counted_t & other)	throw();

#if 1	// TODO is this stuff needed ? i dont like it
	/*************** Convertion inter-ptr_counter_t	***********************/
	template <class Y> friend class	ptr_counted_t;
	template <class Y> ptr_counted_t(const ptr_counted_t<Y> &other)	throw() : counter(NULL)
			{ acquire_counter( (typename ptr_counted_t<T>::counter_t *)(other.counter));	}
	template <class Y> ptr_counted_t & operator=(const ptr_counted_t<Y> &other)	throw()
	{
		// if it is a self-assignement, do nothing
		if( this == &other )	return *this;
		// release the current counter
		release_counter();
		// acquire the new counter
		acquire_counter( (typename ptr_counted_t<T>::counter_t *)(other.counter));	
		// return the object itself
		return *this;
	}
#endif

	/*************** pointer operation	*******************************/
	T &	operator *()	const throw()	{ return *counter->ptr;			}
	T *	operator->()	const throw()	{ return counter->ptr;			}

	/*************** management function	*******************************/
	T *	get_ptr()	const throw()	{ return counter ? counter->ptr : NULL;	}
	size_t	count()		const throw()	{ return counter ? counter->count : 0;	}
	size_t	unique()	const throw()	{ return count() == 1;			}
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//		ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
template <typename T> ptr_counted_t<T>::ptr_counted_t(T *ptr)	throw()
{
	// if ptr is non null, create a counter pointing on this ptr
	if( ptr )	counter = nipmem_new counter_t(ptr, 1);
	else		counter	= NULL;
}

template <typename T> ptr_counted_t<T>::~ptr_counted_t()	throw()
{
	// release the counter for this ptr_counted_t
	release_counter();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    copy and assignement operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Copy operator
 */
template <typename T>
ptr_counted_t<T>::ptr_counted_t(const ptr_counted_t &other)	throw()
{
	// set the counter to NULL
	counter	= NULL;
	// acquire the other counter
	acquire_counter(other.counter);
}

/** \brief Assignement operator
 */
template <typename T>
ptr_counted_t<T> &	ptr_counted_t<T>::operator=(const ptr_counted_t & other)	throw()
{
	// if it is a self-assignement, do nothing
	if( this == &other )	return *this;
	// release the current counter
	release_counter();
	// acquire the new counter
	acquire_counter(other.counter);
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//		  counter_t handling
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief acquire the counter_t
 */
template <typename T>
void	ptr_counted_t<T>::acquire_counter(counter_t *other)	throw()
{
	// sanity check - the counter MUST be NULL
	DBG_ASSERT( counter == NULL );
	// copy the pointer on counter_t
	counter	= other;
	// increment the counter if present
	if( counter ) counter->count++;
}

/** \brief Release the counter_t
 */
template <typename T>
void 	ptr_counted_t<T>::release_counter()			throw()
{
	// if counter_t is NULL, do nothing
	if( counter == NULL )	return;
	// sanity check - the counter->count MUST be greater than 0
	DBG_ASSERT( counter->count > 0 );
	// decrement the counter
	counter->count--;
	// if the counter is now 0, delete it
	if( counter->count == 0 ){
		nipmem_delete	counter->ptr;
		nipmem_delete	counter;
	}
	// zero the counter_t
	counter	= NULL;
}


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PTR_COUNTED_HPP__  */



