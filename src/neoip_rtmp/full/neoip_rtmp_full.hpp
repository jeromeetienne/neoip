/*! \file
    \brief Header of the rtmp_full_t

*/


#ifndef __NEOIP_RTMP_FULL_HPP__
#define __NEOIP_RTMP_FULL_HPP__
/* system include */
/* local include */
#include "neoip_rtmp_full_cb.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_event_t;

/** \brief class definition for rtmp_full_t
 */
class rtmp_full_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t, private zerotimer_cb_t {
private:
	bytearray_t	m_recved_data;	//!< the receved data not yet parsed
	bool		autodelete()		throw();

	/*************** first_parse_zerotimer	*******************************/
	zerotimer_t	first_parse_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/************** socket_full_t	***************************************/
	socket_full_t*	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
				, const socket_event_t &socket_event)		throw();

	/************** socket_event_t handling	*******************************/
	bool		parse_recved_data()					throw();

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

	/*************** List of friend class	*******************************/
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_FULL_HPP__  */



