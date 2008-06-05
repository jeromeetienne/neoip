/*! \file
    \brief Definition of the socket_peerid_t
*/


/* system include */
/* local include */
#include "neoip_socket_peerid.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of the factory plant for socket_peerid_vapi_t 
FACTORY_PLANT_DEFINITION (socket_peerid_factory, socket_domain_t, socket_peerid_vapi_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor - build a null socket_peerid_t
 */
socket_peerid_t::socket_peerid_t()			throw()
{
	// zero some field
	peerid_vapi	= NULL;
}

/** \brief Destructor
 */
socket_peerid_t::~socket_peerid_t()			 throw()
{
	// destroy the socket_peerid_vapi_t if needed
	if( peerid_vapi )	peerid_vapi->destroy();
}

/** \Constructor from a socket_peerid_vapi_t
 */
socket_peerid_t::socket_peerid_t(const socket_peerid_vapi_t *other_peerid_vapi)	throw()
{
	// copy the peerid_vapi from the other socket_peerid_t
	if( other_peerid_vapi )	peerid_vapi	= other_peerid_vapi->clone();
	else			peerid_vapi	= NULL;
}
/** \brief Constructor from a string
 * 
 * - return a null socket_peerid_t if the string is badly formed
 */
void socket_peerid_t::ctor_from_str(const std::string &peerid_str)	throw()
{
	std::vector<std::string>	peerid_part	= string_t::split(peerid_str, ":", 2);
	socket_err_t			socket_err;
	socket_domain_t			socket_domain;
	// log to debug	
	KLOG_DBG("trying to create socket_peerid_t with " << peerid_str );
	
	// zero some field
	peerid_vapi	= NULL;
	// if the string split doesnt result in 2 part, return now
	if( peerid_part.size() != 2 ){
		KLOG_ERR("poorly formed socket_peerid_t " << peerid_str);
		return;
	}
	// if the second part doesnt start with a "//", return now
	if( peerid_part[1].substr(0,2) != "//" ){
		KLOG_ERR("poorly formed socket_peerid_t " << peerid_str);
		return;
	}
	// remove "//" from the second part
	peerid_part[1].erase(0,2);
	// log to debug
	KLOG_DBG("domain str=" << peerid_part[0] );
	KLOG_DBG("the peerid string data are=" << peerid_part[1] );

	// try to find socket_domain_t from the string
	socket_domain	= socket_domain_t::from_string_nocase(peerid_part[0]);
	// if the socket_domain_t has not been found. return now
	if( socket_domain.is_null() ){
		KLOG_ERR("poorly formed socket_peerid_t " << peerid_str 
					<< ". unknown socket_domain_t " << peerid_part[0]);
		return;
	}
	// if this socket_domain_t is NOT available in the peerid_factory, retrun now
	if( !socket_peerid_factory->is_key_present(socket_domain) ){
		KLOG_ERR("No way to product a socket_peerid_t for " << peerid_part[0]);
		return;
	}

	// create the product associated with this socket_domain_t
	peerid_vapi	= socket_peerid_factory->create(socket_domain);
	// set the object value
	socket_err 	= peerid_vapi->ctor_from_str(peerid_part[1]);
	if( !socket_err.succeed() ){
		KLOG_ERR("poorly formed socket_peerid_t " << peerid_str);
		// destroy peerid_vapi and mark it unused
		peerid_vapi->destroy();
		peerid_vapi	 = NULL;
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          object copy stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Copy constructor
 */ 
socket_peerid_t::socket_peerid_t(const socket_peerid_t &other)			throw()
{
	// copy the peerid_vapi from the other socket_peerid_t
	if( !other.is_null() )	peerid_vapi	= other.peerid_vapi->clone();
	else			peerid_vapi	= NULL;
}

/** \brief assignement operator
 */ 
socket_peerid_t &socket_peerid_t::operator = (const socket_peerid_t & other)	throw()
{
	// if it is a self-assignement, do nothing
	if( this == &other )	return *this;
	// destroy the socket_peerid_vapi_t if needed
	if( peerid_vapi )	peerid_vapi->destroy();
	// copy the peerid_vapi from the other socket_peerid_t
	if( !other.is_null() )	peerid_vapi	= other.peerid_vapi->clone();
	else			peerid_vapi	= NULL;
	// return the object itself
	return *this;	
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the domain of this peerid
 */
socket_domain_t socket_peerid_t::get_domain()	const throw()
{
	// if the socket_peerid_t is null, return socket_domain_t::NONE
	if( is_null() )	return socket_domain_t::NONE;
	// else return the peerid_vapi domain
	return peerid_vapi->get_domain();
}

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
int socket_peerid_t::compare( const socket_peerid_t & other )  const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// Handle the case where they are not both of the same domain
	if( get_domain() < other.get_domain())	return -1;
	if( get_domain() > other.get_domain())	return +1;
	// NOTE: here both of the same domain

	// compare the peerid
	return peerid_vapi->compare(*other.peerid_vapi);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string socket_peerid_t::to_string()	const throw()
{
	// if the object is null, return "null"
	if( is_null() )	return "null";
	// else return a string
	return peerid_vapi->get_domain().to_string() + "://" + peerid_vapi->to_string();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       Serialization function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Serialize a socket_peerid_t
 * 
 * - null socket_peerid_t are allowed
 */
serial_t& operator << (serial_t& serial, const socket_peerid_t & socket_peerid)	throw()
{
	// serialize the domain
	serial << socket_peerid.get_domain();
	// serialize the domain specific part if socket_domain_t is not NONE
	if( socket_peerid.get_domain() != socket_domain_t::NONE ){
		// sanity check - the socket_peerid_t MUST NOT be null
		DBG_ASSERT( !socket_peerid.is_null() );
		// serialize the part specific to the domain if domain is not NONE
		socket_peerid.peerid_vapi->serialize(serial);
	}
	// return the serial object itself
	return serial;
}

/** \brief UNSerialize a socket_peerid_t
 * 
 * - null socket_peerid_t are allowed
 */
serial_t& operator >> (serial_t& serial, socket_peerid_t &socket_peerid)	throw(serial_except_t)
{
	socket_domain_t			socket_domain;
	socket_peerid_vapi_t *		peerid_vapi;

	// nullify the destination object
	socket_peerid	= socket_peerid_t();

	// get the domain
	serial >> socket_domain;

	// if the socket_domain is NONE, return now
	// - it will result in unserializing a NULL socket_peerid_t
	if( socket_domain == socket_domain_t::NONE )	return serial;

	// create the product associated with this socket_domain_t
	peerid_vapi	= socket_peerid_factory->create(socket_domain);
	if( !peerid_vapi )	nthrow_serial_plain("socket_peerid_t domain is unavailable.");
	
	// unserialize the peerid_vapi part
	try {
		peerid_vapi->unserialize(serial);
	}catch(serial_except_t &e){
		// destroy the socket_peerid_vapi_t *
		peerid_vapi->destroy();
		// forward the exception
		throw e;
	}
	
	// copy the just created socket_peerid_vapi_t *
	socket_peerid.peerid_vapi	= peerid_vapi;
	// return the serial
	return serial;
}

NEOIP_NAMESPACE_END

