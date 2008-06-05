/*! \file
    \brief Declaration of the bt_utmsg_vapi_t

\par Brief Description
a \ref bt_utmsg_vapi_t is a object which is 
- handle the recv/xmit of payload for the ut extension protocol
- notify the bt_swarm of the received command.
- possibly maintains an internal state for the extension.
- IMPORTANT: and nothing more than that
  - especially bt_swarm_t logic MUST NOT be done here, but in bt_swarm_t
    
*/


#ifndef __NEOIP_BT_UTMSG_VAPI_HPP__ 
#define __NEOIP_BT_UTMSG_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_utmsg_cnx_vapi_t;
class	bt_utmsgtype_t;
class	bt_swarm_full_utmsg_t;

/** \brief Virtual API for the utmsg plugins
 * 
 * - it MUST be constructed after the bt_swarm_t and deleted before
 */
class bt_utmsg_vapi_t {
public:
	//! return the bt_utmsgtype_t handled by this bt_utmsg_vapi_t
	virtual bt_utmsgtype_t	utmsgtype()						const throw() = 0;
	//! return the string of the handled bt_utmsgtype_t in the bt_utmsgtype_t::HANDSHAKE
	virtual std::string	utmsgstr()						const throw() = 0;
	//! to contruct a bt_usmg_cnx_vapi_t
	virtual	bt_utmsg_cnx_vapi_t *	cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)	throw() = 0;
	
	// virtual destructor
	virtual ~bt_utmsg_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_VAPI_HPP__  */



