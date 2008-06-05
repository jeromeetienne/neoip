/*! \file
    \brief Definition of the \ref profile_nunit_example_t

- TODO put all the function which may vary on top of the file
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_profile_nunit_example.hpp"
#include "neoip_kad_pkttype.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// include for the struct field
#include "neoip_kad_rpc_profile.hpp"


NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_rpc_profile_t constant
const pkttype_profile_t	profile_nunit_example_t::PKTTYPE_PROFILE	
					= pkttype_profile_t(0, 16, pkttype_profile_t::UINT8);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
profile_nunit_example_t::profile_nunit_example_t()	throw()
{
	// set the default pkttype_profile_t
	pkttype		(PKTTYPE_PROFILE);
	
	// ctor all the struct fields
	NUNIT_EXAMPLE_PROFILE_STRUCT_FIELD( NEOIP_PROFILE_CTOR_STRUCT_FIELD );
}

/** \brief Destructor
 */
profile_nunit_example_t::~profile_nunit_example_t()	throw()
{
	// dtor all the struct fields
	// - NOTE: no need to destroy the direct fields
	NUNIT_EXAMPLE_PROFILE_STRUCT_FIELD( NEOIP_PROFILE_DTOR_STRUCT_FIELD );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          object copy stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Copy constructor
 */ 
profile_nunit_example_t::profile_nunit_example_t(const profile_nunit_example_t &other)			throw()
{
	// copy all the direct/struct fields
	NUNIT_EXAMPLE_PROFILE_DIRECT_FIELD( NEOIP_PROFILE_COPY_DIRECT_FIELD );	
	NUNIT_EXAMPLE_PROFILE_STRUCT_FIELD( NEOIP_PROFILE_COPY_STRUCT_FIELD );	
}

/** \brief assignement operator
 */ 
profile_nunit_example_t &profile_nunit_example_t::operator = (const profile_nunit_example_t & other)	throw()
{
	// if it is a self-assignement, do nothing
	if( this == &other )	return *this;
	// dtor all the struct fields if needed
	// - NOTE: no need to destroy the direct fields
	NUNIT_EXAMPLE_PROFILE_STRUCT_FIELD( NEOIP_PROFILE_DTOR_STRUCT_FIELD );	
	// copy all the direct/struct fields
	NUNIT_EXAMPLE_PROFILE_DIRECT_FIELD( NEOIP_PROFILE_COPY_DIRECT_FIELD );	
	NUNIT_EXAMPLE_PROFILE_STRUCT_FIELD( NEOIP_PROFILE_COPY_STRUCT_FIELD );	
	// return the object itself
	return *this;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       struct field definition
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

NUNIT_EXAMPLE_PROFILE_STRUCT_FIELD( NEOIP_PROFILE_DEFINE_STRUCT_FIELD );

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main compare function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int profile_nunit_example_t::compare( const profile_nunit_example_t & other )  const throw()
{
	// compare all the direct/struct fields
	NUNIT_EXAMPLE_PROFILE_DIRECT_FIELD( NEOIP_PROFILE_COMPARE_DIRECT_FIELD );
	// TODO this is is currently disable as this subprofile doesnt support comparison
//	NUNIT_EXAMPLE_PROFILE_STRUCT_FIELD( NEOIP_PROFILE_COMPARE_STRUCT_FIELD );	
	// note: here both are considered equal
	return 0;
}


NEOIP_NAMESPACE_END

