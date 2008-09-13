/*! \file
    \brief Declaration of the neoip_nipmem

- lots of #define + c++ template magic to intercept the new/delete
  and especially log their locations

*/


#ifndef __NEOIP_NIPMEM_ALLOC_HPP__ 
#define __NEOIP_NIPMEM_ALLOC_HPP__ 
/* system include */
#include <stdlib.h>
#ifdef _WIN32
#	include <malloc.h>
#endif
/* local include */
#include "neoip_nipmem_tracker.hpp"
#include "neoip_cpp_demangle.hpp"
#include "neoip_assert.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// function declaration
void *	nipmem_malloc_(size_t len, bool secure_f, const char * filename, int lineno
				, const char *fct_name)		throw();
void *	nipmem_zalloc_(size_t len, bool secure_f, const char * filename, int lineno
				, const char *fct_name)		throw();
void 	nipmem_free_(void *buffer_ptr, const char * filename, int lineno
				, const char *fct_name)		throw();
void 	nipmem_free_sec_(void *buffer_ptr, size_t len, const char * filename, int lineno
				, const char *fct_name)		throw();

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     nipmem_new
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief store the location where a new occured
 */
class nipmem_new_param_t {
public:
	nipmem_new_param_t(const char * filename, int lineno, const char *fct_name) throw()
				: filename(filename), lineno(lineno), fct_name(fct_name) {}
	const char *	filename;
	int		lineno;
	const char *	fct_name;
};

/** \brief template function to intercept the new parameters
 */
template <class T> T* operator<<(const nipmem_new_param_t &param, T * ptr)	throw()
{
#if !NO_NIPMEM_TRACKING	
	// insert the zone in the global nipmem_tracker_t
	get_global_nipmem_tracker()->zone_insert((void *)ptr, sizeof(*(ptr)), false, param.filename 
				, param.lineno, param.fct_name, neoip_cpp_typename(*ptr));
#endif
	return ptr;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     nipmem_delete
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief store the location where a new occured
 */
class nipmem_del_param_t {
public:
	nipmem_del_param_t(const char * filename, int lineno, const char *fct_name) throw()
				: filename(filename), lineno(lineno), fct_name(fct_name) {}

	const char *	filename;
	int		lineno;
	const char *	fct_name;
};


template <class T> T* operator<<(const nipmem_del_param_t &param, T * ptr)		throw()
{
#if !NO_NIPMEM_TRACKING
	// remove the zone in the global nipmem_tracker_t
	get_global_nipmem_tracker()->zone_remove((void *)ptr, false, param.filename, param.lineno 
					, param.fct_name, neoip_cpp_typename(*ptr));
#endif
	// actually delete the object
#if NO_NIPMEM_SMASHING
	// delete it with the standard delete call
	delete ptr;
#else
	// delete it but filling the memory with 0x43 
	// - assume the global operator delete is overloaded by neoip
	// destruct the object
	ptr->~T();
	// fill the memory with a pattern
	memset( (void *)ptr, 0x43, sizeof(*ptr) );
	// actually free the memory
	free( (void*)ptr );
#endif
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        nipmem_zdelete
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief store the location where a new nipmem_zdelete occured
 */
class nipmem_zdel_param_t {
public:
	nipmem_zdel_param_t(const char * filename, int lineno, const char *fct_name) throw()
				: filename(filename), lineno(lineno), fct_name(fct_name) {}

	const char *	filename;
	int		lineno;
	const char *	fct_name;
};

/**
 */
template <class T> void operator<<(const nipmem_zdel_param_t &param, T * & ptr)		throw()
{
	// if the input pointer is null, do nothing
	if( ptr == NULL )	return;
#if !NO_NIPMEM_TRACKING
	// remove the zone in the global nipmem_tracker_t
	get_global_nipmem_tracker()->zone_remove( (void *)ptr, false, param.filename, param.lineno 
					, param.fct_name, neoip_cpp_typename(*ptr));
#endif
	// actually delete the object
#if NO_NIPMEM_SMASHING
	// delete it with the standard delete call
	delete ptr;
#else
	// delete it but filling the memory with 0x43 
	// - assume the global operator delete is overloaded by neoip
	// destruct the object
	ptr->~T();
	// fill the memory with a pattern
	memset( (void *)ptr, 0x43, sizeof(*ptr) );
	// actually free the memory
	free( (void *)ptr );
#endif
	// set the ptr to NULL
	ptr = NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//	inlined malloc/calloc/free which assert in case of null pointer
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to do a malloc and assert if the allocation fails
 */
static inline void * malloc_nullptr_check(size_t size)
{
	void *	ptr = malloc(size);
	DBG_ASSERT(ptr);
	return ptr;
}

/** \brief function to do a calloc and assert if the allocation fails
 */
static inline void * calloc_nullptr_check(size_t nmemb, size_t size)
{
	void *	ptr = calloc(nmemb, size);
	DBG_ASSERT(ptr);
	return ptr;
}

/** \brief function to do a free and assert if the pointer is null
 */
static inline void free_nullptr_check(void *ptr)
{
	DBG_ASSERT(ptr);
	free(ptr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// macro to make it easy to use
#if NO_NIPMEM_SMASHING && NO_NIPMEM_TRACKING
#	define nipmem_new	new
#	define nipmem_delete	delete
#else
#	define nipmem_new	nipmem_new_param_t(__FILE__, __LINE__, __func__) << new
#	define nipmem_delete	nipmem_del_param_t(__FILE__, __LINE__, __func__) <<
#endif
#define nipmem_zdelete	nipmem_zdel_param_t(__FILE__, __LINE__, __func__) <<


#if NO_NIPMEM_SMASHING && NO_NIPMEM_TRACKING
#	define nipmem_malloc(len)	malloc_nullptr_check(len)
#	define nipmem_zalloc(len)	calloc_nullptr_check(1,len)
#	define nipmem_free(ptr)		free_nullptr_check(ptr)
#else
#	define nipmem_malloc(len)	nipmem_malloc_(len, false, __FILE__, __LINE__, __func__)
#	define nipmem_zalloc(len)	nipmem_zalloc_(len, false, __FILE__, __LINE__, __func__)
#	define nipmem_free(ptr)		nipmem_free_  (ptr, __FILE__, __LINE__, __func__)
#endif

#define nipmem_alloca(len)		alloca(len)

// as the secure malloc/free doesnt exist by default, always use local functions
#define nipmem_malloc_sec(len)		nipmem_malloc_(len, true, __FILE__, __LINE__, __func__)
#define nipmem_zalloc_sec(len)		nipmem_zalloc_(len, true, __FILE__, __LINE__, __func__)
#define nipmem_free_sec(ptr, len)	nipmem_free_sec_(ptr,  len, __FILE__, __LINE__, __func__)

NEOIP_NAMESPACE_END

#endif // __NEOIP_NIPMEM_ALLOC_HPP__ 


