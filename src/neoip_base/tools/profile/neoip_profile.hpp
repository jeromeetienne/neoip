/*! \file
    \brief List of macro to ease the declaration and definition of profile

*/


#ifndef __NEOIP_PROFILE_HPP__ 
#define __NEOIP_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_ptr_counted.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         field declaration
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//! declare a direct field
#define NEOIP_PROFILE_DECLARE_DIRECT_FIELD(obj_type, var_type, var_name)						\
	private:var_type	var_name ## _val;							\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
		obj_type &	var_name(const var_type &value)		throw()				\
						{ var_name ## _val = value; return *this;	}

//! declare a struct field
#define NEOIP_PROFILE_DECLARE_STRUCT_FIELD(obj_type, var_type, var_name)				\
	private:ptr_counted_t<var_type>	var_name ## _ptr;						\
	public:	ptr_counted_t<var_type> &	var_name()	throw() { return var_name ## _ptr;}	\
		ptr_counted_t<const var_type>	var_name()	const throw();				\
		obj_type &			var_name(const var_type &value)	throw();


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         struct field definition
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//! define a struct field
#define NEOIP_PROFILE_DEFINE_STRUCT_FIELD(obj_type, var_type, var_name)				\
	ptr_counted_t<const var_type>	obj_type::var_name()		const throw()		\
	{											\
		return ptr_counted_t<const var_type>(var_name ## _ptr);				\
	}											\
	obj_type &	obj_type::var_name(const var_type &value)	throw()			\
	{											\
		var_name ## _ptr	= ptr_counted_t<var_type>(nipmem_new var_type(value));	\
		return *this;									\
	}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//! Constructor for a struct field
#define NEOIP_PROFILE_CTOR_STRUCT_FIELD(obj_type, var_type, var_name)		\
	var_name ## _ptr	= ptr_counted_t<var_type>(nipmem_new var_type);
	
//! Destructor for a struct field
#define NEOIP_PROFILE_DTOR_STRUCT_FIELD(obj_type, var_type, var_name)
	

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             field copy
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//! copy a direct field
#define NEOIP_PROFILE_COPY_DIRECT_FIELD(obj_type, var_type, var_name)		\
	var_name ## _val	= other.var_name ## _val;

//! copy a struct field
#define NEOIP_PROFILE_COPY_STRUCT_FIELD(obj_type, var_type, var_name)		\
	var_name ## _ptr	= ptr_counted_t<var_type>(nipmem_new var_type(*other.var_name ## _ptr));


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             field comparison
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//! compare a direct field
#define NEOIP_PROFILE_COMPARE_DIRECT_FIELD(obj_type, var_type, var_name)	\
	if( var_name ## _val < other.var_name ## _val )		return -1;	\
	if( var_name ## _val > other.var_name ## _val )		return +1;

//! compare a struct field
#define NEOIP_PROFILE_COMPARE_STRUCT_FIELD(obj_type, var_type, var_name)	\
	if( *var_name ## _ptr < *other.var_name ## _ptr )	return -1;	\
	if( *var_name ## _ptr > *other.var_name ## _ptr )	return +1;


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PROFILE_HPP__  */



