/*! \file
    \brief Header of the \ref rtmp_resp_t

*/


#ifndef __NEOIP_RTMP_RESP_HPP__
#define __NEOIP_RTMP_RESP_HPP__
/* system include */
#include <list>
/* local include */
#include "neoip_rtmp_resp_cb.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_socket_resp_cb.hpp"
#include "neoip_socket_resp_arg.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_resp_cnx_t;

/** \brief Accept the http connections and then spawn rtmp_resp_cnx_t to handle them
 */
class rtmp_resp_t : NEOIP_COPY_CTOR_DENY, private socket_resp_cb_t {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the size of the padding during the handshake
	static const size_t	HANDSHAKE_PADLEN;
private:
	/*************** socket_resp_t	***************************************/
	socket_resp_t *	socket_resp;
	bool		neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
					, const socket_event_t &socket_event)	throw();

	/*************** store the rtmp_resp_cnx_t	*******************************/
	std::list<rtmp_resp_cnx_t *>	cnx_db;
	void cnx_dolink(rtmp_resp_cnx_t *cnx) 	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(rtmp_resp_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}

	/*************** callback stuff	***************************************/
	rtmp_resp_cb_t *	callback;	//!< callback to notify event
	void *			userptr;	//!< userptr associated with the event callback
	bool			notify_callback(rtmp_full_t *rtmp_full)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	rtmp_resp_t()		throw();
	~rtmp_resp_t()		throw();

	/*************** Setup function	***************************************/
	rtmp_err_t		start(const socket_resp_arg_t &resp_arg
					, rtmp_resp_cb_t *callback, void *userptr)	throw();

	/*************** Query function	***************************************/
	const socket_addr_t  &	listen_addr()					const throw();

	/*************** list of friend class	*******************************/
	friend class	rtmp_resp_cnx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_RESP_HPP__  */










