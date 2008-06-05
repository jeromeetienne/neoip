/*! \file
    \brief Header of the \ref slay_profile_t

*/

/* system include */
/* local include */
#include "neoip_slay_profile.hpp"
#include "neoip_slay_profile_vapi.hpp"

NEOIP_NAMESPACE_BEGIN;

// define the factory plant for slay_profile_t
FACTORY_PLANT_DEFINITION (slay_profile_factory	, slay_domain_t, slay_profile_vapi_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
slay_profile_t::slay_profile_t()	throw()
{
	// zero some field
	m_profile_vapi	= NULL;	
	
	// sanity check - the produced object MUST be null
	DBG_ASSERT( is_null() );
}

/** \brief Destructor
 */
slay_profile_t::~slay_profile_t()	throw()
{
	// destroy the slay_profile_vapi_t if needed
	if( m_profile_vapi )	m_profile_vapi->destroy();	
}

/** \brief Constructor based on profile
 */
slay_profile_t::slay_profile_t(const slay_domain_t &slay_domain)	throw()
{
	// create the product associated with this slay_domain_t
	m_profile_vapi	= slay_profile_factory->create(slay_domain);
	// if the creation failed, return a null object
	if( !m_profile_vapi )	return;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          object copy stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Copy constructor
 */ 
slay_profile_t::slay_profile_t(const slay_profile_t &other)			throw()
{
	// copy the profile_api from the other slay_profile_t
	if( !other.is_null() )	m_profile_vapi	= other.m_profile_vapi->clone();
	else			m_profile_vapi	= NULL;
}

/** \brief assignement operator
 */ 
slay_profile_t &slay_profile_t::operator = (const slay_profile_t & other)	throw()
{
	// if it is a self-assignement, do nothing
	if( this == &other )	return *this;
	// destroy the slay_profile_vapi_t if needed
	if( m_profile_vapi )	m_profile_vapi->destroy();
	// copy the pm_rofile_api from the other slay_profile_t
	if( !other.is_null() )	m_profile_vapi	= other.m_profile_vapi->clone();
	else			m_profile_vapi	= NULL;
	// return the object itself
	return *this;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the domain of this profile
 */
slay_domain_t slay_profile_t::domain()	const throw()
{
	// if the slay_profile_t is null, return slay_domain_t::NONE
	if( is_null() )	return slay_domain_t::NONE;
	// else return the profile_api domain
	return	m_profile_vapi->domain();
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
slay_err_t	slay_profile_t::check()	const throw()
{
	slay_err_t	slay_err;
	// sanity check - the profile MUST NOT be null
	DBG_ASSERT( !is_null() );
	// check the domain specific profile_api
	slay_err	= m_profile_vapi->check();
	if( !slay_err.succeed() )	return slay_err;
	
	// return no error
	return slay_err_t::OK;
}



NEOIP_NAMESPACE_END







