/*! \file
    \brief Implementation of the delaygen_t

- TODO use the nipmem_new/delete
  - for unknown reason this code use directly new/delete

*/

/* system include */
/* local include */
#include "neoip_delaygen.hpp"
#include "neoip_delaygen_regular.hpp"
#include "neoip_delaygen_expboff.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

delaygen_t::delaygen_t()	throw()
{
	regular		= NULL;
	expboff		= NULL;
	delaygen_vapi	= NULL;
}

delaygen_t::~delaygen_t()	throw()
{
	nullify();
}

/** \brief Constructor from the argument
 */
delaygen_t::delaygen_t(const delaygen_arg_t &delaygen_arg)	throw()
{
	// TODO why no nipmem_new here ?
	if( delaygen_arg.is_regular() ){
		regular 	= new delaygen_regular_t(delaygen_arg.get_regular());
		expboff		= NULL;
		delaygen_vapi	= regular;
	}else if( delaygen_arg.is_expboff() ){
		regular		= NULL;
		expboff		= new delaygen_expboff_t(delaygen_arg.get_expboff());
		delaygen_vapi	= expboff;
	}else{
		DBG_ASSERT( 0 );
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief copy operator
 */
void delaygen_t::copy(const delaygen_t &other)		throw()
{
	// do copy the object depending on its type
	if( other.is_regular() ){
		regular		= new delaygen_regular_t(*other.regular);
		expboff		= NULL;
		delaygen_vapi	= regular;
	}else if( other.is_expboff() ){
		regular		= NULL;
		expboff		= new delaygen_expboff_t(*other.expboff);
		delaygen_vapi	= expboff;
	}else{
		DBG_ASSERT( other.is_null() );
		regular	= NULL;
		expboff	= NULL;
		
	}
}

/** \brief nullify function
 */
void delaygen_t::nullify()			throw()
{
	if( regular ){
		delete regular;
		regular	= NULL;
	}
	if( expboff ){
		delete expboff;
		expboff	= NULL;
	}
	delaygen_vapi	= NULL;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           copy stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief copy operator
 */
delaygen_t::delaygen_t(const delaygen_t &other)			throw()
{
	copy(other);
}

/** \brief assignement operator
 */
delaygen_t & delaygen_t::operator = (const delaygen_t & other)	throw()
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



