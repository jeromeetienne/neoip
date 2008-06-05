/*! \file
    \brief Implementation of the delaygen_regular_t
*/

/* system include */
/* local include */
#include "neoip_delaygen_arg.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default contructor
 */
delaygen_arg_t::delaygen_arg_t()	throw()
{
	// zero some parameter
	regular	= NULL;
	expboff	= NULL;
}

/** \brief Destructor
 */
delaygen_arg_t::~delaygen_arg_t()	throw()
{
	nullify();
}

/** \brief Constructor from a delaygen_regular_arg_t
 */
delaygen_arg_t::delaygen_arg_t(const delaygen_regular_arg_t &regular_arg)	throw()
{
	// sanity check - the regular_arg MUST be valid
	DBG_ASSERT( regular_arg.is_valid() );
	// set the parameter
	regular	= new delaygen_regular_arg_t(regular_arg);
	expboff	= NULL;
}

/** \brief Constructor from a delaygen_expboff_arg_t
 */
delaygen_arg_t::delaygen_arg_t(const delaygen_expboff_arg_t &expboff_arg)	throw()
{
	// sanity check - the expboff_arg MUST be valid
	DBG_ASSERT( expboff_arg.is_valid() );
	// set the parameter
	regular	= NULL;
	expboff	= new delaygen_expboff_arg_t(expboff_arg);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief copy operator
 */
void delaygen_arg_t::copy(const delaygen_arg_t &other)			throw()
{
	// do copy the object depending on its type
	if( other.is_regular() ){
		regular	= new delaygen_regular_arg_t(*other.regular);
		expboff = NULL;
	}else if( other.is_expboff() ){
		regular	= NULL;
		expboff	= new delaygen_expboff_arg_t(*other.expboff);
	}else{
		DBG_ASSERT( other.is_null() );
		regular	= NULL;
		expboff	= NULL;
	}
}

/** \brief nullify function
 */
void delaygen_arg_t::nullify()			throw()
{
	if( regular ){
		delete regular;
		regular	= NULL;
	}
	if( expboff ){
		delete expboff;
		expboff	= NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           copy stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief copy operator
 */
delaygen_arg_t::delaygen_arg_t(const delaygen_arg_t &other)			throw()
{
	copy(other);
}

/** \brief assignement operator
 */
delaygen_arg_t & delaygen_arg_t::operator = (const delaygen_arg_t & other)	throw()
{
	// if it is a self assignement, do nothing
	if( this == &other )	return *this;

	// nullify the current object
	nullify();
	// copy it
	copy(other);
	
	// return the object itself
	return *this;
}

NEOIP_NAMESPACE_END



