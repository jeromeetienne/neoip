/*! \file
    \brief Definition of the \ref netif_stat_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_netif_stat.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
netif_stat_t::netif_stat_t()	throw()
{
	// zero some fields
	m_rx_byte	= 0;
	m_tx_byte	= 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        arithmetic operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief the += operator
 */
netif_stat_t &	netif_stat_t::operator +=(const netif_stat_t &other)	throw()
{
	m_name		= std::string("(") + m_name + " + " + other.m_name + ")";
	m_capture_date	= date_t();
	m_rx_byte	+= other.m_rx_byte;
	m_tx_byte	+= other.m_tx_byte;
	// return the object itself
	return *this;
}


/** \brief the -= operator
 */
netif_stat_t &	netif_stat_t::operator -=(const netif_stat_t &other)	throw()
{
	m_name		= std::string("(") + m_name + " - " + other.m_name + ")";
	m_capture_date	= date_t();
	m_rx_byte	-= other.m_rx_byte;
	m_tx_byte	-= other.m_tx_byte;		
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int netif_stat_t::compare(const netif_stat_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// NOTE: here both are NOT null

	// compare the name
	if( name() < other.name() )			return -1;
	if( name() > other.name() )			return +1;
	// NOTE: here both name are equal

	// compare the capture_date
	if( capture_date() < other.capture_date() )	return -1;
	if( capture_date() > other.capture_date() )	return +1;
	// NOTE: here both capture_date are equal

	// compare the rx_byte
	if( rx_byte() < other.rx_byte() )		return -1;
	if( rx_byte() > other.rx_byte() )		return +1;
	// NOTE: here both rx_byte are equal

	// compare the tx_byte
	if( tx_byte() < other.tx_byte() )		return -1;
	if( tx_byte() > other.tx_byte() )		return +1;
	// NOTE: here both tx_byte are equal

	// here both are considered equal
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	netif_stat_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss        << "name="		<< name();
	oss << " " << "capture_date="	<< capture_date();
	oss << " " << "rx_byte="	<< rx_byte();
	oss << " " << "tx_byte="	<< tx_byte();
	
	// return the just built string
	return oss.str();
}


NEOIP_NAMESPACE_END

