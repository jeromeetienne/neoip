/*! \file
    \brief Definition of the \ref nlay_regpkt_t

*/


/* system include */
/* local include */
#include "neoip_nlay_regpkt.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_pkt.hpp"

NEOIP_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                                 ctor/dtor
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_regpkt_t::nlay_regpkt_t()		throw()
{
	dfl_handler	= NULL;
}

/** \brief Destructor
 */
nlay_regpkt_t::~nlay_regpkt_t()	throw()
{
}

/** \brief return the number of registered packet handler (including the default
 */
size_t	nlay_regpkt_t::get_nb_handler()				const throw()
{
	size_t	nb = handler_db.size();
	if( dfl_handler )	nb++;
	return nb;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//                        default nlay_full_api_t function
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/** \brief set the default packet handler
 */
void	nlay_regpkt_t::set_dfl_handler(nlay_full_api_t *callback)	throw()
{
	dfl_handler	= callback;
}

/** \brief get the default packet handler
 */
nlay_full_api_t *nlay_regpkt_t::get_dfl_handler()			throw()
{
	return dfl_handler;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/** \brief Return the packet handler associated with a given packet type
 */
nlay_full_api_t *	nlay_regpkt_t::find(nlay_pkttype_t pkttype)			throw()
{
	std::map<nlay_pkttype_t, nlay_full_api_t *>::iterator	iter;
	iter = handler_db.find(pkttype);
	// if no handler is found, return an error
	if( iter == handler_db.end() )	return dfl_handler;
	// call the handler
	return (*iter).second;
}

/** \brief register packet handler for a given packet type
 */
void nlay_regpkt_t::register_handler(nlay_pkttype_t pkttype, nlay_full_api_t *callback) throw()
{
	bool	succeed = handler_db.insert(std::make_pair(pkttype, callback)).second;
	DBG_ASSERT( succeed );
}

NEOIP_NAMESPACE_END

