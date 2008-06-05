/*! \file
    \brief Definition of the socket_portid_t
*/


/* system include */
/* local include */
#include "neoip_socket_portid.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of the factory plant for socket_portid_vapi_t 
FACTORY_PLANT_DEFINITION (socket_portid_factory, socket_domain_t, socket_portid_vapi_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor - build a null socket_portid_t
 */
socket_portid_t::socket_portid_t()			throw()
{
	// zero some field
	portid_vapi = NULL;
}

/** \brief Destructor
 */
socket_portid_t::~socket_portid_t()			 throw()
{
	// destroy the socket_portid_vapi_t if needed
	if( portid_vapi )	portid_vapi->destroy();
}

/** \Constructor from a socket_portid_vapi_t
 */
socket_portid_t::socket_portid_t(const socket_portid_vapi_t *other_portid_vapi)	throw()
{
	// copy the portid_vapi from the other socket_portid_t
	if( other_portid_vapi )	portid_vapi	= other_portid_vapi->clone();
	else			portid_vapi	= NULL;
}

/** \brief Constructor from a string
 * 
 * - return a null socket_portid_t if the string is badly formed
 */
void socket_portid_t::ctor_from_str(const std::string &portid_str)	throw()
{
	std::vector<std::string>	portid_part	= string_t::split(portid_str, ":", 2);
	socket_err_t			socket_err;
	socket_domain_t			socket_domain;
	// log to debug	
	KLOG_DBG("trying to create socket_portid_t with " << portid_str );
	
	// zero some field
	portid_vapi	= NULL;
	// if the string split doesnt result in 2 part, return now
	if( portid_part.size() != 2 ){
		KLOG_ERR("poorly formed socket_portid_t " << portid_str);
		return;
	}
	// if the second part doesnt start with a "//", return now
	if( portid_part[1].substr(0,2) != "//" ){
		KLOG_ERR("poorly formed socket_portid_t " << portid_str);
		return;
	}
	// remove "//" from the second part
	portid_part[1].erase(0,2);
	// log to debug
	KLOG_DBG("domain str=" << portid_part[0] );
	KLOG_DBG("the portid string data are=" << portid_part[1] );

	// try to find socket_domain_t from the string
	socket_domain	= socket_domain_t::from_string_nocase(portid_part[0]);
	// if the socket_domain_t has not been found. return now
	if( socket_domain.is_null() ){
		KLOG_ERR("poorly formed socket_portid_t " << portid_str 
					<< ". unknown socket_domain_t " << portid_part[0]);
		return;
	}
	// if this socket_domain_t is NOT available in the portid_factory, retrun now
	if( !socket_portid_factory->is_key_present(socket_domain) ){
		KLOG_ERR("No way to product a socket_portid_t for " << portid_part[0]);
		return;
	}

	// create the product associated with this socket_domain_t
	portid_vapi	= socket_portid_factory->create(socket_domain);
	// set the object value
	socket_err 	= portid_vapi->ctor_from_str(portid_part[1]);
	if( !socket_err.succeed() ){
		KLOG_ERR("poorly formed socket_portid_t " << portid_str);
		// destroy portid_vapi and mark it unused
		portid_vapi->destroy();
		portid_vapi	 = NULL;
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
socket_portid_t::socket_portid_t(const socket_portid_t &other)			throw()
{
	// copy the portid_vapi from the other socket_portid_t
	if( !other.is_null() )	portid_vapi	= other.portid_vapi->clone();
	else			portid_vapi	= NULL;
}

/** \brief assignement operator
 */ 
socket_portid_t &socket_portid_t::operator = (const socket_portid_t & other)	throw()
{
	// if it is a self-assignement, do nothing
	if( this == &other )	return *this;
	// destroy the socket_portid_vapi_t if needed
	if( portid_vapi )	portid_vapi->destroy();
	// copy the portid_vapi from the other socket_portid_t
	if( !other.is_null() )	portid_vapi	= other.portid_vapi->clone();
	else			portid_vapi	= NULL;
	// return the object itself
	return *this;	
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       utility function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the domain of this portid
 */
socket_domain_t socket_portid_t::get_domain()	const throw()
{
	// if the socket_portid_t is null, return socket_domain_t::NONE
	if( is_null() )	return socket_domain_t::NONE;
	// else return the portid_vapi domain
	return portid_vapi->get_domain();
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
int socket_portid_t::compare( const socket_portid_t & other )  const throw()
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

	// compare the portid
	return portid_vapi->compare(*other.portid_vapi);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string socket_portid_t::to_string()	const throw()
{
	// if the object is null, return "null"
	if( is_null() )	return "null";
	// else return a string
	return portid_vapi->get_domain().to_string() + "://" + portid_vapi->to_string();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       Serialization function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Serialize a socket_portid_t
 * 
 * - null socket_portid_t are allowed
 */
serial_t& operator << (serial_t& serial, const socket_portid_t & socket_portid)	throw()
{
	// serialize the domain
	serial << socket_portid.get_domain();
	// serialize the domain specific part if socket_domain_t is not NONE
	if( socket_portid.get_domain() != socket_domain_t::NONE ){
		// sanity check - the socket_portid_t MUST NOT be null
		DBG_ASSERT( !socket_portid.is_null() );
		// serialize the part specific to the domain if domain is not NONE
		socket_portid.portid_vapi->serialize(serial);
	}
	// return the serial object itself
	return serial;
}

/** \brief UNSerialize a socket_portid_t
 * 
 * - null socket_portid_t are allowed
 */
serial_t& operator >> (serial_t& serial, socket_portid_t &socket_portid)	throw(serial_except_t)
{
	socket_domain_t			socket_domain;
	socket_portid_vapi_t *		portid_vapi;

	// nullify the destination object
	socket_portid	= socket_portid_t();

	// get the domain
	serial >> socket_domain;

	// if the socket_domain is NONE, return now
	// - it will result in unserializing a NULL socket_portid_t
	if( socket_domain == socket_domain_t::NONE )	return serial;

	// create the product associated with this socket_domain_t
	portid_vapi	= socket_portid_factory->create(socket_domain);
	if( !portid_vapi )	nthrow_serial_plain("socket_portid_t domain is unavailable.");
	
	// unserialize the portid_vapi part
	try {
		portid_vapi->unserialize(serial);
	}catch(serial_except_t &e){
		// destroy the socket_portid_vapi_t *
		portid_vapi->destroy();
		// forward the exception
		throw e;
	}
	
	// copy the just created socket_portid_vapi_t *
	socket_portid.portid_vapi	= portid_vapi;
	// return the serial
	return serial;
}

NEOIP_NAMESPACE_END

