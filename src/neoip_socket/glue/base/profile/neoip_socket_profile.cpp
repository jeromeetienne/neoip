/*! \file
    \brief Header of the \ref socket_profile_t

- TODO to check the copy of it
- TODO this is currently quite unclean
  - the linger should be in the full
  - the use of this socket_profile_t is unclear

*/

/* system include */
/* local include */
#include "neoip_socket_profile.hpp"
#include "neoip_socket_profile_vapi.hpp"

// include to access domain specific api
#include "neoip_socket_profile_udp.hpp"
#include "neoip_socket_profile_ntudp.hpp"
#include "neoip_socket_profile_ntlay.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref socket_profile_t constant
const delay_t	socket_profile_t::ITOR_TIMEOUT		= delay_t::from_sec(3*60);
// end of constants definition

// definition of the factory plant for socket_profile_vapi_t 
FACTORY_PLANT_DEFINITION (socket_profile_factory, socket_domain_t, socket_profile_vapi_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
socket_profile_t::socket_profile_t()	throw()
{
	// zero some field
	m_profile_vapi	= NULL;	
	
	// sanity check - the produced object MUST be null
	DBG_ASSERT( is_null() );
}

/** \brief Destructor
 */
socket_profile_t::~socket_profile_t()	throw()
{
	// destroy the socket_profile_vapi_t if needed
	if( m_profile_vapi )	m_profile_vapi->destroy();	
}

/** \brief Constructor based on profile
 */
socket_profile_t::socket_profile_t(const socket_domain_t &socket_domain)	throw()
{
	// create the product associated with this socket_domain_t
	m_profile_vapi	= socket_profile_factory->create(socket_domain);
	// if the creation failed, return a null object
	if( !m_profile_vapi )	return;

	// put the default value of the direct fields	
	itor_timeout	(ITOR_TIMEOUT);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          object copy stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Copy constructor
 */ 
socket_profile_t::socket_profile_t(const socket_profile_t &other)			throw()
{
	// copy the m_profile_vapi from the other socket_profile_t
	if( !other.is_null() )	m_profile_vapi	= other.m_profile_vapi->clone();
	else			m_profile_vapi	= NULL;
	// copy all the direct field and subprofile one by one
	itor_timeout	( other.itor_timeout()	);
	linger		( other.linger()	);
	full		( other.full()		);
}

/** \brief assignement operator
 */ 
socket_profile_t &socket_profile_t::operator = (const socket_profile_t & other)	throw()
{
	// if it is a self-assignement, do nothing
	if( this == &other )	return *this;
	// destroy the socket_profile_vapi_t if needed
	if( m_profile_vapi )	m_profile_vapi->destroy();
	// copy the m_profile_vapi from the other socket_profile_t
	if( !other.is_null() )	m_profile_vapi	= other.m_profile_vapi->clone();
	else			m_profile_vapi	= NULL;
	// copy all the direct field and subprofile one by one
	itor_timeout	( other.itor_timeout()	);
	linger		( other.linger()	);
	full		( other.full()		);
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
socket_domain_t socket_profile_t::get_domain()	const throw()
{
	// if the socket_profile_t is null, return socket_domain_t::NONE
	if( is_null() )	return socket_domain_t::NONE;
	// else return the m_profile_vapi domain
	return m_profile_vapi->get_domain();
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
socket_err_t	socket_profile_t::check()	const throw()
{
	socket_err_t	socket_err;
	// sanity check - the profile MUST NOT be null
	DBG_ASSERT( !is_null() );
	// check the domain specific m_profile_vapi
	socket_err	= m_profile_vapi->check();
	if( !socket_err.succeed() )	return socket_err;
	// check the socket_linger_profile
	socket_err	 = linger().check();
	if( !socket_err.succeed() )	return socket_err;
	
	// TODO check the idle_timeout
	
	// return no error
	return socket_err_t::OK;
}

NEOIP_NAMESPACE_END







