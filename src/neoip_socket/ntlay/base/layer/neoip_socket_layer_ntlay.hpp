/*! \file
    \brief Header of the \ref socket_layer_ntlay_t
*/


#ifndef __NEOIP_SOCKET_LAYER_NTLAY_HPP__
#define __NEOIP_SOCKET_LAYER_NTLAY_HPP__
/* system include */
#include <list>
/* local include */
#include "neoip_event_hook_cb.hpp"
#include "neoip_socket_layer_ntlay_init.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class socket_resp_ntlay_t;
class socket_itor_ntlay_t;
class socket_full_ntlay_t;
class socket_full_close_ntlay_t;

/** \brief This class handles global stuff in the \ref socket_full_t and co
 */
class socket_layer_ntlay_t {
private:
	/**************** socket_resp_ntlay_t	*******************************/
	std::list<socket_resp_ntlay_t *>	socket_resp_db;
	void socket_resp_dolink(socket_resp_ntlay_t *socket_resp)	throw()	{ socket_resp_db.push_back(socket_resp);}
	void socket_resp_unlink(socket_resp_ntlay_t *socket_resp)	throw()	{ socket_resp_db.remove(socket_resp);	}

	/**************** socket_itor_ntlay_t	*******************************/
	std::list<socket_itor_ntlay_t *>	socket_itor_db;
	void socket_itor_dolink(socket_itor_ntlay_t *socket_itor)	throw()	{ socket_itor_db.push_back(socket_itor);}
	void socket_itor_unlink(socket_itor_ntlay_t *socket_itor)	throw()	{ socket_itor_db.remove(socket_itor);	}

	/**************** socket_full_ntlay_t	*******************************/
	std::list<socket_full_ntlay_t *>	socket_full_db;
	void socket_full_dolink(socket_full_ntlay_t *socket_full)	throw()	{ socket_full_db.push_back(socket_full);}
	void socket_full_unlink(socket_full_ntlay_t *socket_full)	throw()	{ socket_full_db.remove(socket_full);	}

	/**************** socket_full_close_ntlay_t	***********************/
	std::list<socket_full_close_ntlay_t *>	full_close_db;
	void full_close_dolink(socket_full_close_ntlay_t *full_close) throw() { full_close_db.push_back(full_close);	}
	void full_close_unlink(socket_full_close_ntlay_t *full_close) throw() { full_close_db.remove(full_close);	}

public:
	/**************** ctor/dtor	***************************************/
	socket_layer_ntlay_t()		throw();
	~socket_layer_ntlay_t()		throw();

	/**************** List of friend class	*******************************/
	friend	class socket_resp_ntlay_t;
	friend	class socket_itor_ntlay_t;
	friend	class socket_full_ntlay_t;
	friend	class socket_full_close_ntlay_t;
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_SOCKET_LAYER_NTLAY_HPP__


