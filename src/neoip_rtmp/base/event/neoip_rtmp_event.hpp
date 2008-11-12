/*! \file
    \brief Header of the \ref rtmp_event_t
*/


#ifndef __NEOIP_RTMP_EVENT_HPP__
#define __NEOIP_RTMP_EVENT_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_rtmp_err.hpp"
#include "neoip_rtmp_pkthd.hpp"
#include "neoip_datum.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the event from flv_parse_t
 */
class rtmp_event_t: NEOIP_COPY_CTOR_ALLOW {
public:
	enum type {
		NONE,
		ERROR,
		PACKET,
		CONNECTED,
		MAX
	};
private:
	rtmp_event_t::type	type_val;

	// all event parameters
	rtmp_err_t	rtmp_err;	//!< only for rtmp_event_t::ERROR
	rtmp_pkthd_t	pkthd;		//!< only for rtmp_event_t::PACKET
	datum_t		pktdata;	//!< only for rtmp_event_t::PACKET
	http_uri_t	connect_uri;	//!< only for rtmp_event_t::CONNECTED
public:
	/*************** ctor/dtor	***************************************/
	rtmp_event_t()	throw();
	~rtmp_event_t()	throw();

	/*************** query function	***************************************/
	rtmp_event_t::type	value()		const throw()	{ return type_val;	}
	bool			is_null()	const throw()	{ return type_val==NONE;}
	bool			is_fatal()	const throw()	{ return is_error();	}

	/*************** specific event ctor/dtor/query	***********************/
	bool			is_error() 				const throw();
	static rtmp_event_t	build_error(const rtmp_err_t &rtmp_err)	throw();
	const rtmp_err_t &	get_error()				const throw();

	bool			is_packet() 				const throw();
	static rtmp_event_t	build_packet(const rtmp_pkthd_t &pkthd
					, const datum_t &pktdata)	throw();
	const rtmp_pkthd_t &	get_packet(datum_t *pktdata_out)	const throw();

	bool			is_connected() 				const throw();
	static rtmp_event_t	build_connected(const http_uri_t &connect_uri)throw();
	const http_uri_t &	get_connected_uri()			const throw();

	//! return true is the event is allowed to be returned by a rtmp_parse_t
	bool is_parse_ok() const throw() {
		return is_error() || is_packet();
	}
	//! return true is the event is allowed to be returned by a rtmp_full_t
	bool is_full_ok() const throw() {
		return is_error() || is_packet();
	}
	//! return true is the event is allowed to be returned by a rtmp_cam_full_t
	bool is_cam_full_ok() const throw() {
		return is_error() || is_packet() || is_connected();
	}

	/*************** display function	*******************************/
	std::string		to_string()				const throw();
	friend	std::ostream &	operator << (std::ostream & oss, const rtmp_event_t &event) throw()
						{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_EVENT_HPP__  */



