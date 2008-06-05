/*! \file
    \brief Declaration of the nslan_publish_t

*/

/* system include */
/* local include */
#include "neoip_nslan_publish.hpp"
#include "neoip_nslan_peer.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nslan_publish_t::nslan_publish_t(nslan_peer_t *nslan_peer, const nslan_rec_t &nslan_rec)
										throw()
{
	// copy the field
	this->nslan_peer	= nslan_peer;
	this->nslan_rec		= nslan_rec;
	// link this object to the nslan_peer
	nslan_peer->publish_link(this);
}

/** \brief Desstructor
 */
nslan_publish_t::~nslan_publish_t()		throw()
{
	// unlink this object from the nslan_peer
	nslan_peer->publish_unlink(this);
}

NEOIP_NAMESPACE_END


