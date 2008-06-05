/*! \file
    \brief Declaration of the memory allocation functions
  
*/


/* system include */
#include <iostream>
#ifndef _WIN32
#	include <sys/mman.h>
#endif
/* local include */
#include "neoip_nipmem_alloc.hpp"
#include "neoip_nipmem_tracker.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       nipmem_malloc_ function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/** \brief allocate a zone of memory
 */
void *	nipmem_malloc_(size_t len, bool secure_f, const char * filename, int lineno
						, const char *fct_name)		throw()
{
	// call the standard malloc
	void *	ptr	= malloc(len);
	// sanity check - the malloc MUST succeed
	DBG_ASSERT( ptr );
#ifndef _WIN32
	// lock the memory if the memory is secured
	if( secure_f && mlock(ptr, len) )	EXP_ASSERT( 0 );
#else
	//KLOG_STDOUT("malloc with secure_f but ignored on _WIN32, ignored for going on\n");
//	EXP_ASSERT( 0 );
#endif
#if !NO_NIPMEM_TRACKING
	// tracker the memory zone in the memory tracker
	get_global_nipmem_tracker()->zone_insert(ptr, len, secure_f, filename, lineno
							, fct_name,"malloced");
#endif

#if !NO_NIPMEM_SMASHING
	// fill the memory with a pattern
	memset( (void *)ptr, 0x42, len);
#endif
	// return a pointer on the allocated memory
	return ptr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       nipmem_zalloc_ function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief allocate and zero a zone of memory
 */
void *	nipmem_zalloc_(size_t len, bool secure_f, const char * filename, int lineno
						, const char *fct_name)		throw()
{
	// allocate memory
	void	*ptr	= nipmem_malloc_(len, secure_f, filename, lineno, fct_name);
	// zero the memory
	memset( ptr, 0, len );
	// return a pointer on the allocated memory
	return ptr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       nipmem_free_ function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief allocate and zero a zone of memory
 */
void 	nipmem_free_(void *ptr, const char * filename, int lineno, const char *fct_name)	throw()
{
#if !NO_NIPMEM_TRACKING
	// unregister the memory zone in the memory tracker
	size_t zone_size = get_global_nipmem_tracker()->zone_remove(ptr, false, filename, lineno
									, fct_name, "malloced");
#endif

#if !NO_NIPMEM_SMASHING && !NO_NIPMEM_TRACKING
	// fill the memory with a pattern
	memset( (void *)ptr, 0x43, zone_size );
#endif
	// free the memory
	free(ptr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       nipmem_free_sec_ function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief allocate and zero a zone of memory
 */
void 	nipmem_free_sec_(void *buffer_ptr, size_t len, const char * filename, int lineno
						, const char *fct_name)		throw()
{
#if !NO_NIPMEM_TRACKING
	// unregister the memory zone in the memory tracker
	size_t	zone_size = get_global_nipmem_tracker()->zone_remove(buffer_ptr, true, filename
						, lineno, fct_name, "malloced");
	// test if the zone_size to remove match the parameter len
	if( zone_size != len ){
		KLOG_STDOUT("Try to free_sec a allocated zone of " << zone_size << "-byte "
				<< " but declaring it is " << len << "-byte long"
				<< " at " << filename << ":" << lineno
				<< ":" << fct_name << "()\n");
		EXP_ASSERT( 0 );
	}	
#endif
	// NOTE: no smashing as the memory is already zeroed for security

	// zero the memory to erase potentially sensitive info
	memset(buffer_ptr, 0, len);
#ifndef _WIN32
	// unlock the memory
	if( munlock(buffer_ptr, len) )	EXP_ASSERT( 0 );
#else
	//KLOG_STDOUT("free with secure_f but ignored on _WIN32, ignored for going on\n");
	//EXP_ASSERT( 0 );
#endif
	// free the memory
	free(buffer_ptr);
}

NEOIP_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//              Global redefinition of new/delete for smashing memory
//
// - the redefined operator new fill the allocated memory with a non null pattern (i.e 0x42).
//   It is an efficient manner to detect the use of non zeroed memory
//   - especially the variables in object which are not initialized before usage
//   - additionnaly as the pattern is known, it is recognizable in a debugger.
//   - BUT it doesnt catch all the bugs like efence stuff could
// - it works in conjonction with the nipmem_delete which fill the free memory with 0x43
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if !NO_NIPMEM_SMASHING	// NOTE: to enable during debug only 

/** \brief Global redefinition of the new operator
 * 
 * - fill the memory with 0x42 pattern after the allocation
 */
void* operator new(size_t size)
{
	// log to debug - dont use any c++ stuff as it may create an infinite loop
	// 1. dump the output of the executable in /tmp/out
	// 2. grep operator /tmp/out | awk '{ N[$3]++ } END { for (i in N) print i " appeared " N[i] " times" }' | sort
	//    - it will display an histogram of the size used for operator new
	//printf("operator new: %05d Bytes\n", size);
	// allocate the memory with the standard C allocator
	void*	ptr	= malloc(size);
	// if the allocation failed, throw the standard bad_alloc() exception
	if( ptr == NULL )	throw std::bad_alloc();
	// fill the memory with a pattern
	memset( ptr, 0x42, size );
	// return the pointer on the allocated memory
	return ptr;
}

/** \brief Global redefinition of the delete operator
 * 
 * - NOTE: here it would be nice to erase the memory with a pattern, but
 *   the size of the zone is not available
 */
void operator delete(void* ptr)
{
	// log to debug - dont use any c++ stuff as it may create an infinite loop
	// puts("operator delete");
	// free the allocated memory
	free(ptr);
}
#endif // !NO_NIPMEM_SMASHING




