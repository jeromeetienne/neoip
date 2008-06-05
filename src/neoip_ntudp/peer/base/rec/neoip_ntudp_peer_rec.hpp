/*! \file
    \brief Header of the ntudp_peer_rec_t
    
*/


#ifndef __NEOIP_NTUDP_PEER_REC_HPP__ 
#define __NEOIP_NTUDP_PEER_REC_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ntudp_rdvpt_arr.hpp"
#include "neoip_ntudp_npos_res.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for ntudp_peer_rec_t
 * 
 * - currently it is a stub
 * - more field will be added later
 */
class ntudp_peer_rec_t : NEOIP_COPY_CTOR_ALLOW {
private:
	ntudp_peerid_t		m_peerid;	//!< the peerid of the peer
	ntudp_rdvpt_arr_t	m_rdvpt_arr;	//!< the list of rendez vous point for this peers
	ntudp_npos_res_t	m_npos_res;	//!< the network position of the peer
public:
	/*************** ctor/dtor	***************************************/
	ntudp_peer_rec_t()						throw()	{}
	ntudp_peer_rec_t(const ntudp_peerid_t &m_peerid, const ntudp_rdvpt_arr_t &m_rdvpt_arr
					, const ntudp_npos_res_t &m_npos_res)	throw();

	/*************** Query function	***************************************/
	bool				is_null()	const throw()	{ return m_peerid.is_null();	}
	const	ntudp_rdvpt_arr_t &	rdvpt_arr()	const throw()	{ return m_rdvpt_arr;		}
	const	ntudp_peerid_t &	peerid()	const throw()	{ return m_peerid;		}
	const	ntudp_npos_res_t &	npos_res()	const throw()	{ return m_npos_res;		}

	/*************** Comparison Operator	*******************************/
	int	compare(const ntudp_peer_rec_t &other)	    const throw();
	bool	operator == (const ntudp_peer_rec_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const ntudp_peer_rec_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const ntudp_peer_rec_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const ntudp_peer_rec_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const ntudp_peer_rec_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const ntudp_peer_rec_t &other) const throw() { return compare(other) >= 0; }
	
	/*************** display function	*******************************/
	std::string	to_string()			const throw();
	friend	std::ostream & operator << (std::ostream & os, const ntudp_peer_rec_t &ntudp_peer_rec ) throw()
				{ return os << ntudp_peer_rec.to_string();	}

	/*************** serial_t function	*******************************/
	friend serial_t& operator << (serial_t& serial, const ntudp_peer_rec_t &ntudp_peer_rec)	throw();
	friend serial_t& operator >> (serial_t & serial, ntudp_peer_rec_t &ntudp_peer_rec)	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PEER_REC_HPP__  */



