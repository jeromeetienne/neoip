/*! \file
    \brief Header of the \ref neoip_cnxstat.cpp

*/


#ifndef __NEOIP_CNXSTAT_HPP__ 
#define __NEOIP_CNXSTAT_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief count the packets and data sent and receive
 * 
 * - it use counters which are never reseted or decreased
 *   - the overflow is possible but unlikely as the counter are unsigned 64-bit
 */
class cnxstat_t : NEOIP_COPY_CTOR_ALLOW {
private:
	uint64_t	npkt_in;
	uint64_t	ndata_in;
	uint64_t	npkt_out;
	uint64_t	ndata_out;
public:
	// ctor/dtor
	cnxstat_t()					throw();
	~cnxstat_t()					throw();
	
	// counting function
	void	count_incoming(size_t pkt_len)		throw();
	void	count_outgoing(size_t pkt_len)		throw();

	// helper for counting function
	void	count_incoming(const pkt_t &pkt)	throw()	{ count_incoming(pkt.get_len());	}
	void	count_outgoing(const pkt_t &pkt)	throw()	{ count_outgoing(pkt.get_len());	}
	
	// display function
	std::string	to_string()	const throw();
	friend std::ostream& operator << ( std::ostream& os, const cnxstat_t &cnxstat)	throw()
		{ return os << cnxstat.to_string();	}
	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CNXSTAT_HPP__  */



