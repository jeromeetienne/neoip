/*! \file
    \brief Implementation of \ref cnxstat_t

*/

/* system include */
/* local include */
#include "neoip_cnxstat.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor for the class
 */
cnxstat_t::cnxstat_t()	throw()
{
	npkt_in		= 0;
	ndata_in	= 0;
	npkt_out	= 0;
	ndata_out	= 0;
}

/** \brief Destructor for the class
 */
cnxstat_t::~cnxstat_t()	throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      counting function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief count incoming data
 */
void	cnxstat_t::count_incoming(size_t pkt_len)	throw()
{
	npkt_in++;
	ndata_in	+= pkt_len;
}

/** \brief count outgoing data
 */
void	cnxstat_t::count_outgoing(size_t pkt_len)	throw()
{
	npkt_out++;
	ndata_out	+= pkt_len;	
}

/** \brief convert the object into a string
 */
std::string cnxstat_t::to_string()	const throw()
{
	std::ostringstream	oss;
	oss << "incoming=" << ndata_in << "-byte in " << npkt_in << "-packet";
	oss << " ";
	oss << "outgoing=" << ndata_out << "-byte in " << npkt_out << "-packet";
	return oss.str();
}


NEOIP_NAMESPACE_END



