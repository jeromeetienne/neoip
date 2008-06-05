/*! \file
    \brief Declaration of the bt_utmsg_cnx_vapi_t

\par Brief Description
a \ref bt_utmsg_cnx_vapi_t is a object which is handle the reception of 
payload for a given connection of the ut extension protocol.
    
*/


#ifndef __NEOIP_BT_UTMSG_CNX_VAPI_HPP__ 
#define __NEOIP_BT_UTMSG_CNX_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_utmsg_vapi_t;
class	bt_swarm_full_utmsg_t;
class	bt_utmsgtype_t;
class	pkt_t;

/** \brief Virtual API for each cnx of utmsg plugins
 */
class bt_utmsg_cnx_vapi_t {
public:
	/** \brief call the parser of a utmsg packet for this connection
	 * 
	 * - the return bool tokeep to mimic the bt_swarm_full_t parsing function 
	 *   - return false *IIF* this function notify back bt_swarm_full_t and
	 *     got false returned.
	 *   - return true in all other cases
	 * 
	 * @return return bool tokeep for the bt_swarm_full_t
	 */
	virtual bool	parse_pkt(const bt_utmsgtype_t &utmsgtype, const pkt_t &pkt)	throw() = 0;
	//! return the bt_utmsg_vapi_t for this connection
	virtual	bt_utmsg_vapi_t	* utmsg_vapi()	throw()	= 0;

	// virtual destructor
	virtual ~bt_utmsg_cnx_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_CNX_VAPI_HPP__  */



