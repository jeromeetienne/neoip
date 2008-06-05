/*! \file
    \brief Definition of the \ref kad_stat_t
    
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_kad_stat.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor by default
 */
kad_stat_t::kad_stat_t()	throw()
{
	reset();
}

/** \brief Reset all the counters
 */
void kad_stat_t::reset()	throw()
{
	nb_ping_sent		= nb_ping_recv		= 0;
	nb_store_sent		= nb_store_recv		= 0;
	nb_findnode_sent	= nb_findnode_recv	= 0;
	nb_findsomeval_sent	= nb_findsomeval_recv	= 0;
	nb_findallval_sent	= nb_findallval_recv	= 0;
	nb_delete_sent		= nb_delete_recv	= 0;
	
	nb_byte_sent = nb_byte_recv = 0;
}

NEOIP_NAMESPACE_END;






