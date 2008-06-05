/*! \file
    \brief Header of the \ref udp_layer_t
*/


#ifndef __NEOIP_UDP_LAYER_UDP_HPP__
#define __NEOIP_UDP_LAYER_UDP_HPP__
/* system include */
#include <list>
/* local include */
#include "neoip_event_hook_cb.hpp"
#include "neoip_udp_layer_init.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class udp_resp_t;
class udp_itor_t;
class udp_full_t;
class udp_layer_http_t;

/** \brief This class handles global stuff in the \ref udp_full_t and co
 */
class udp_layer_t : private event_hook_cb_t {
private:
	udp_layer_http_t *	udp_layer_http;
	bool	neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
							, int hook_level)	throw();
	// to store the pending udp_resp_t
	std::list<udp_resp_t *>	udp_resp_db;
	void udp_resp_link(udp_resp_t *udp_resp) 	throw()	{ udp_resp_db.push_back(udp_resp);	}
	void udp_resp_unlink(udp_resp_t *udp_resp)	throw()	{ udp_resp_db.remove(udp_resp);		}

	// to store the pending udp_itor_t
	std::list<udp_itor_t *>	udp_itor_db;
	void udp_itor_link(udp_itor_t *udp_itor) 	throw()	{ udp_itor_db.push_back(udp_itor);	}
	void udp_itor_unlink(udp_itor_t *udp_itor)	throw()	{ udp_itor_db.remove(udp_itor);		}

	// to store the pending udp_full_t
	std::list<udp_full_t *>	udp_full_db;
	void udp_full_link(udp_full_t *udp_full) 	throw()	{ udp_full_db.push_back(udp_full);	}
	void udp_full_unlink(udp_full_t *udp_full)	throw()	{ udp_full_db.remove(udp_full);		}
public:
	/*************** ctor/dtor	***************************************/
	udp_layer_t()		throw();
	~udp_layer_t()		throw();

	/*************** List of friend classes	*******************************/
	friend	class udp_resp_t;
	friend	class udp_itor_t;
	friend	class udp_full_t;
	friend	class udp_layer_http_t;
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_UDP_LAYER_UDP_HPP__


