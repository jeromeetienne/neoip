/*! \file
    \brief Header of the rtmp_full_t

*/


#ifndef __NEOIP_RTMP_FULL_HPP__
#define __NEOIP_RTMP_FULL_HPP__
/* system include */
/* local include */
#include "neoip_rtmp_full_cb.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_rtmp_parse_cb.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_pkt.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_event_t;

/** \brief class definition for rtmp_full_t
 */
class rtmp_full_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t, private rtmp_parse_cb_t {
private:
	/************** socket_full_t	***************************************/
	socket_full_t *	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
				, const socket_event_t &socket_event)		throw();

	/************** rtmp_parse_t	***************************************/
	rtmp_parse_t *	rtmp_parse;
	bool		neoip_rtmp_parse_cb(void *cb_userptr, rtmp_parse_t &cb_rtmp_parse
					, const rtmp_event_t &parse_event)	throw();

	/*************** callback stuff	***************************************/
	rtmp_full_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const rtmp_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	rtmp_full_t(socket_full_t *socket_full, const bytearray_t &recved_data)		throw();
	~rtmp_full_t()									throw();

	/*************** setup function	***************************************/
	rtmp_err_t	start(rtmp_full_cb_t *callback, void *userptr)			throw();

	/*************** Action function	*******************************/
	rtmp_err_t	send(const void *data_ptr, size_t data_len) 	throw();
	rtmp_err_t	send(const pkt_t &pkt) 		throw()		{ return send(pkt.void_ptr(), pkt.length());	}
	rtmp_err_t	send(const datum_t &datum) 	throw()		{ return send(datum.void_ptr(), datum.length());}


	/*************** List of friend class	*******************************/
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_FULL_HPP__  */



