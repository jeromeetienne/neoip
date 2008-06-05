/*! \file
    \brief Definition of the socket_addr_t

- TODO to refactor
  - the socket_addr_t is currently a mess
  - noway to convert to native type
    - e.g. a tcp socket addr to a ipport_addr
  - there is no is_fully_qualified()
  - the whole logic seems bogus to access peerid/portid
  - port the new way to get/set stuff (aka without the set_/get_ prefix)
  
*/


/* system include */
/* local include */
#include "neoip_socket_addr.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor - build a null socket_addr_t
 */
socket_addr_t::socket_addr_t()			throw()
{
	// zero some field
	m_peerid_vapi	= NULL;
	m_portid_vapi	= NULL;
}

/** \brief Destructor
 */
socket_addr_t::~socket_addr_t()			 throw()
{
	// destroy the socket_peerid/portid_vapi_t if needed
	if( !is_null() ){
		m_peerid_vapi->destroy();
		m_portid_vapi->destroy();
	}
}

/** \brief Constructor from socket_peerid_t and socket_portid_t
 */
socket_addr_t::socket_addr_t(const socket_peerid_t &p_peerid, const socket_portid_t &p_portid)	throw()
{
	// sanity check - peerid and portid MUST have the same domain
	DBG_ASSERT( p_peerid.get_domain() == p_portid.get_domain() );
	// if both domain are socket_domain_t, produce a null socket_addr_t
	if( p_peerid.get_domain() == socket_domain_t::NONE ){
		m_peerid_vapi	= NULL;
		m_portid_vapi	= NULL;
		return;
	}	
	// copy the peerid_vapi and portid_vapi
	m_peerid_vapi	= p_peerid.get_internal_api()->clone();
	m_portid_vapi	= p_portid.get_internal_api()->clone();
}

/** \brief Constructor from a string
 * 
 * - return a null socket_addr_t if the string is badly formed
 */
void socket_addr_t::ctor_from_str(const std::string &addr_str)	throw()
{
	std::vector<std::string>	addr_part	= string_t::split(addr_str, ":", 3);
	socket_err_t			socket_err;
	socket_domain_t			socket_domain;
	// log to debug	
	KLOG_DBG("trying to create socket_addr_t with " << addr_str );
	
	// zero some field
	m_peerid_vapi	= NULL;
	m_portid_vapi	= NULL;
	// if the string split doesnt result in 3 part, return now
	if( addr_part.size() != 3 ){
		KLOG_ERR("poorly formed socket_addr_t " << addr_str);
		return;
	}
	// if the second part doesnt start with a "//", return now
	if( addr_part[1].substr(0,2) != "//" ){
		KLOG_ERR("poorly formed socket_addr_t " << addr_str);
		return;
	}
	// remove "//" from the second part
	addr_part[1].erase(0,2);
	// log to debug
	KLOG_DBG("domain str=" << addr_part[0] );
	KLOG_DBG("the peerid string data are=" << addr_part[1] );
	KLOG_DBG("the portid string data are=" << addr_part[2] );

	// try to find socket_domain_t from the string
	socket_domain	= socket_domain_t::from_string_nocase(addr_part[0]);
	// if the socket_domain_t has not been found. return now
	if( socket_domain.is_null() ){
		KLOG_ERR("poorly formed socket_addr_t " << addr_str 
					<< ". unknown socket_domain_t " << addr_part[0]);
		return;
	}
	// if this socket_domain_t is NOT available in the peerid_factory, retrun now
	if( !socket_peerid_factory->is_key_present(socket_domain) ){
		KLOG_ERR("No way to product a socket_addr_t for " << addr_str);
		return;
	}
	// if this socket_domain_t is NOT available in the portid_factory, retrun now
	if( !socket_portid_factory->is_key_present(socket_domain) ){
		KLOG_ERR("No way to product a socket_addr_t for " << addr_str);
		return;
	}

	// create the peerid associated with this socket_domain_t
	m_peerid_vapi	= socket_peerid_factory->create(socket_domain);
	// set the object value
	socket_err 	= m_peerid_vapi->ctor_from_str(addr_part[1]);
	if( !socket_err.succeed() ){
		KLOG_ERR("poorly formed socket_addr_t " << addr_str);
		// destroy peerid_vapi and mark it unused
		m_peerid_vapi->destroy();
		m_peerid_vapi	 = NULL;
		return;
	}
	// create the portid associated with this socket_domain_t
	m_portid_vapi	= socket_portid_factory->create(socket_domain);
	// set the object value
	socket_err 	= m_portid_vapi->ctor_from_str(addr_part[2]);
	if( !socket_err.succeed() ){
		KLOG_ERR("poorly formed socket_addr_t " << addr_str);
		// destroy peerid_vapi and mark it unused
		m_peerid_vapi->destroy();
		m_peerid_vapi	 = NULL;
		// destroy portid_vapi and mark it unused
		m_portid_vapi->destroy();
		m_portid_vapi	 = NULL;
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
socket_addr_t::socket_addr_t(const socket_addr_t &other)			throw()
{
	// copy the peerid_vapi from the other socket_addr_t
	if( !other.is_null() ){
		m_peerid_vapi	= other.m_peerid_vapi->clone();
		m_portid_vapi	= other.m_portid_vapi->clone();
	}else{
		m_peerid_vapi	= NULL;	
		m_portid_vapi	= NULL;	
	}
}

/** \brief assignement operator
 */ 
socket_addr_t &socket_addr_t::operator = (const socket_addr_t & other)	throw()
{
	// if it is a self-assignement, do nothing
	if( this == &other )	return *this;
	// destroy the socket_peerid/portid_vapi_t if needed
	if( !is_null() ){
		m_peerid_vapi->destroy();
		m_portid_vapi->destroy();
	}
	// copy the peerid_vapi from the other socket_addr_t
	if( !other.is_null() ){
		m_peerid_vapi	= other.m_peerid_vapi->clone();
		m_portid_vapi	= other.m_portid_vapi->clone();
	}else{
		m_peerid_vapi	= NULL;	
		m_portid_vapi	= NULL;	
	}
	// return the object itself
	return *this;	
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       utility function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the domain of this peerid
 */
socket_domain_t socket_addr_t::get_domain()	const throw()
{
	// if the socket_addr_t is null, return socket_domain_t::NONE
	if( is_null() )	return socket_domain_t::NONE;
	// sanity check - the peerid and portid MUST have the same domain
	DBG_ASSERT( m_peerid_vapi->get_domain() == m_portid_vapi->get_domain() );
	// else return the peerid_vapi domain
	return m_peerid_vapi->get_domain();
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
int socket_addr_t::compare( const socket_addr_t & other )  const throw()
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
	int peerid_cmp = m_peerid_vapi->compare(*other.m_peerid_vapi);
	if( peerid_cmp )	return peerid_cmp;

	// compare the portid
	int portid_cmp = m_portid_vapi->compare(*other.m_portid_vapi);
	if( portid_cmp )	return portid_cmp;

	// note: here both are considered equal
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string socket_addr_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// if the object is null, return "null"
	if( is_null() )	return "null";
	// build the string
	oss	<< m_peerid_vapi->get_domain();
	oss	<< "://";
	oss	<< m_peerid_vapi->to_string();
	oss	<< ":";
	oss	<< m_portid_vapi->to_string();
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       Serialization function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Serialize a socket_addr_t
 * 
 * - null socket_addr_t are allowed
 */
serial_t& operator << (serial_t& serial, const socket_addr_t & socket_addr)	throw()
{
	// serialize the domain
	serial << socket_addr.get_domain();
	// serialize the domain specific part if socket_domain_t is not NONE
	if( socket_addr.get_domain() != socket_domain_t::NONE ){
		// sanity check - the socket_addr_t MUST NOT be null
		DBG_ASSERT( !socket_addr.is_null() );
		// serialize the part specific to the domain if domain is not NONE
		socket_addr.m_peerid_vapi->serialize(serial);
		socket_addr.m_portid_vapi->serialize(serial);
	}
	// return the serial object itself
	return serial;
}

/** \brief UNSerialize a socket_addr_t
 * 
 * - null socket_addr_t are allowed
 */
serial_t& operator >> (serial_t& serial, socket_addr_t &socket_addr)	throw(serial_except_t)
{
	socket_domain_t		socket_domain;
	socket_peerid_vapi_t *	m_peerid_vapi;
	socket_portid_vapi_t *	m_portid_vapi;

	// nullify the destination object
	socket_addr	= socket_addr_t();

	// get the domain
	serial >> socket_domain;

	// if the socket_domain is NONE, return now
	// - it will result in unserializing a NULL socket_addr_t
	if( socket_domain == socket_domain_t::NONE )	return serial;

	// create the peerid associated with this socket_domain_t
	m_peerid_vapi	= socket_peerid_factory->create(socket_domain);
	if( !m_peerid_vapi )	nthrow_serial_plain("socket_addr_t domain is unavailable.");

	// create the portid associated with this socket_domain_t
	m_portid_vapi	= socket_portid_factory->create(socket_domain);
	if( !m_portid_vapi ){
		m_peerid_vapi->destroy();
		nthrow_serial_plain("socket_addr_t domain is unavailable.");
	}
	
	// unserialize the peerid_vapi part
	try {
		m_peerid_vapi->unserialize(serial);
		m_portid_vapi->unserialize(serial);
	}catch(serial_except_t &e){
		m_peerid_vapi->destroy();
		m_portid_vapi->destroy();
		// forward the exception
		throw e;
	}
	
	// copy the just created socket_addr_api_t *
	socket_addr.m_peerid_vapi= m_peerid_vapi;
	socket_addr.m_portid_vapi= m_portid_vapi;
	// return the serial
	return serial;
}

NEOIP_NAMESPACE_END

