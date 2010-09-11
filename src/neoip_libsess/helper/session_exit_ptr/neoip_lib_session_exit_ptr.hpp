/*! \file
    \brief Header of the lib_session_exit_ptr_t

\par Brief Description
lib_session_exit_ptr_t implements a very specific type of pointer. The pointer
is allocated dynamically from an external code, stored here, and when the 
lib_session_exit_t is called, the pointer is deleted.
- this is a very raw/specific implementation just made for a specific case
  - namely when a apps needs to be deleted before the lib_session_t starts
    exiting itself.
  - e.g. neoip-btcli exiting process is 
    -# stop all swarm to unregister the peer from all the peersrc
    -# delete the bt_ezsession_t to delete the upnp_bindport_t to be able to 
       do the exiting of the libneoip_upnp
    -# do the exiting of the libneoip_upnp, aka lets all the upnp_delport_t complete
       before exiting 

\par About deletion
- To delete the contained pointer MUST NOT be done by a nipmem_delete exit_ptr.value()
- this will leave the lib_session_exit_t
- instead either delete exit_ptr or do exit_ptr.delete_ptr()

*/


#ifndef __NEOIP_LIB_SESSION_EXIT_PTR_HPP__ 
#define __NEOIP_LIB_SESSION_EXIT_PTR_HPP__ 
/* system include */
/* local include */
#include "neoip_lib_session_exit_cb.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_exit.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a copy-on-write pointer
 * 
 * - based on lib_session_exit_ptr_t
 */
template <typename T> class lib_session_exit_ptr_t : NEOIP_COPY_CTOR_DENY
					, private lib_session_exit_cb_t {
private:
	T *			ptr_value;

	/*************** lib_session_exit_t	*******************************/
	lib_session_exit_t *	lib_session_exit;
	bool			neoip_lib_session_exit_cb(void *cb_userptr
						, lib_session_exit_t &session_exit)	throw();
	
public:
	/*************** ctor/dtor	***************************************/
	lib_session_exit_ptr_t(T *_ptr_value, size_t exit_order)	throw();
	~lib_session_exit_ptr_t()					throw();

	/*************** Query function	***************************************/
	T * &		value()		throw()		{ return ptr_value;	}

	/*************** Action function	*******************************/
	void		delete_ptr()	throw();

	/*************** non-const pointer operation	***********************/
	T &		operator *()	throw()		{ return *ptr_value;	}
	T *		operator->()	throw()		{ return ptr_value;	}
	/*************** const pointer operation	***********************/
	const T &	operator *()	const throw()	{ return *ptr_value;	}
	const T *	operator->()	const throw()	{ return ptr_value;	}
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//		ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
template <typename T> 
lib_session_exit_ptr_t<T>::lib_session_exit_ptr_t(T *_ptr_value, size_t exit_order)	throw()
{
	// copy the ptr_value
	ptr_value	= _ptr_value;
	// start the lib_session_exit_t
	lib_session_exit= nipmem_new lib_session_exit_t();
	lib_session_exit->start(lib_session_get(), exit_order, this, NULL);
}

/** \brief Destructor
 */
template <typename T> 
lib_session_exit_ptr_t<T>::~lib_session_exit_ptr_t()	throw()
{
	// delete the ptr_value itself
	nipmem_zdelete	ptr_value;
	// delete the lib_session_exit_t if needed
	nipmem_zdelete	lib_session_exit;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Do a delete on the contained pointer and delete the associated lib_session_exit
 * 
 * - NOTE: just deleting the ptr externally WILL NOT delete the lib_session_exit_t
 *   - aka this is error prone
 *   - DO NOT lib_session_exit_ptr_t<bla_t> bla(); nipmem_delete bla.value()
 *   - INSTEAD do bla.delete_ptr() or simply destruct the lib_session_exit_ptr_t 
 */
template <typename T> 
void		lib_session_exit_ptr_t<T>::delete_ptr()	throw()
{
	// log to debug
	KLOG_DBG("enter ptr_value=" << ptr_value << " lib_session_exit=" << lib_session_exit << " this=" << this);
	// delete the ptr_value itself
	nipmem_zdelete	ptr_value;
	// delete the lib_session_exit_t if needed
	nipmem_zdelete	lib_session_exit;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			lib_session_exit_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref lib_session_exit_t when to notify an event
 */
template <typename T> 
bool	lib_session_exit_ptr_t<T>::neoip_lib_session_exit_cb(void *cb_userptr
					, lib_session_exit_t &session_exit)	throw()
{
	// log to debug
	KLOG_DBG("enter ptr_value=" << ptr_value << " lib_session_exit=" << lib_session_exit);

	// delete the ptr_value itself
	nipmem_zdelete	ptr_value;
	
	// else delete the exit procedure
	nipmem_zdelete	lib_session_exit;
	
	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_SESSION_EXIT_PTR_HPP__  */



