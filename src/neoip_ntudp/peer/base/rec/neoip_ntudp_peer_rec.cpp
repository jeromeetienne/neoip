/*! \file
    \brief Implementation of the ntudp_peer_rec_t
    
*/

/* system include */
/* local include */
#include "neoip_ntudp_peer_rec.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the ntudp_peer_rec_t class
 */
ntudp_peer_rec_t::ntudp_peer_rec_t(const ntudp_peerid_t &m_peerid, const ntudp_rdvpt_arr_t &m_rdvpt_arr
						, const ntudp_npos_res_t &m_npos_res)	throw()
{
	// copy the parameter
	this->m_peerid		= m_peerid;
	this->m_rdvpt_arr	= m_rdvpt_arr;
	this->m_npos_res	= m_npos_res;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  	from/to_string
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the address to a string
 */
std::string	ntudp_peer_rec_t::to_string()				const throw()
{
	// handle the null case
	if( is_null() )			return "null";
	// build the string
	std::ostringstream oss;
	oss << "peerid=" << peerid();
	oss << " ";
	oss << "rdvpt_arr=" << rdvpt_arr();
	oss << " ";
	oss << "npos_res=" << npos_res();
	// return a string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		 COMPARISON OPERTATOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 ntudp_peer_rec_t and return value ala strcmp/memcmp
 */
int	ntudp_peer_rec_t::compare( const ntudp_peer_rec_t &other )	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// handle the peerid
	if( peerid() < other.peerid() )			return -1;
	if( peerid() > other.peerid() )			return +1;
	// handle the rdvpt_arr
	if( rdvpt_arr() < other.rdvpt_arr() )		return -1;
	if( rdvpt_arr() > other.rdvpt_arr() )		return +1;
	// handle the npos_res
	if( npos_res() < other.npos_res() )		return -1;
	if( npos_res() > other.npos_res() )		return +1;
	// here both are considered equal
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ntudp_peer_rec_t
 */
serial_t& operator << (serial_t& serial, const ntudp_peer_rec_t &ntudp_peer_rec)		throw()
{
	// serialize the object
	serial << ntudp_peer_rec.peerid();
	serial << ntudp_peer_rec.rdvpt_arr();
	// NOTE: here when serializing ntudp_npos_res_t the local_ipaddr_lview/pview are useless
	//       but are kept to avoid a special case
	serial << ntudp_peer_rec.npos_res();
	// return serial
	return serial;
}

/** \brief unserialze a ntudp_peer_rec_t
 */
serial_t& operator >> (serial_t & serial, ntudp_peer_rec_t &ntudp_peer_rec)		throw(serial_except_t)
{
	ntudp_peerid_t		m_peerid;
	ntudp_rdvpt_arr_t	m_rdvpt_arr;
	ntudp_npos_res_t	m_npos_res;
	// unserialize the object
	serial >> m_peerid;
	serial >> m_rdvpt_arr;
	serial >> m_npos_res;
	// set the returned variable
	ntudp_peer_rec	= ntudp_peer_rec_t(m_peerid, m_rdvpt_arr, m_npos_res);
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END





