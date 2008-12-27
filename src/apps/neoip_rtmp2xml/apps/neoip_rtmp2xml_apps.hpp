/*! \file
    \brief Header of the \ref rtmp2xml_apps_t class

*/


#ifndef __NEOIP_RTMP2XML_APPS_HPP__
#define __NEOIP_RTMP2XML_APPS_HPP__
/* system include */
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_rtmp_parse_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	rtmp_err_t;
class	rtmp_pkthd_t;
class	datum_t;
class	bytearray_t;
class	clineopt_arr_t;

/** \brief The object to run the neoip_get apps
 */
class rtmp2xml_apps_t : NEOIP_COPY_CTOR_DENY, private fdwatch_cb_t, private rtmp_parse_cb_t
					, private zerotimer_cb_t {
private:
	bool		output_pkthd;		//!< true if the pkthd should be output
	bool		output_pkt_invoke;	//!< true if the pkt invoke should be output
	bool		output_pkt_ping;	//!< true if the pkt ping should be output

	/*************** Internal function	*******************************/
	bool		exit_asap(const rtmp_err_t &rtmp_err)			throw();
	bool		exit_asap(const std::string &reason)			throw();
	bool		parsing_completed(const rtmp_event_t &rtmp_event)	throw();
	rtmp_err_t	rtmp_event_packet(const rtmp_event_t &rtmp_event)	throw();

	/*************** parse per flv_type_t	*******************************/
	rtmp_err_t	parse_pkttype_invoke(const rtmp_pkthd_t &rtmp_pkthd
						, const datum_t &pkt_data)		throw();
	rtmp_err_t	parse_pkttype_invoke_connect(bytearray_t &amf0_body)		throw();
	rtmp_err_t	parse_pkttype_invoke_createStream(bytearray_t &amf0_body)	throw();
	rtmp_err_t	parse_pkttype_invoke_publish(bytearray_t &amf0_body)		throw();
	rtmp_err_t	parse_pkttype_invoke_play(bytearray_t &amf0_body)		throw();
	rtmp_err_t	parse_pkttype_ping(const rtmp_pkthd_t &rtmp_pkthd
						, const datum_t &pkt_data)		throw();

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	m_fdwatch;
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
				, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** rtmp_event_t	***************************************/
	rtmp_parse_t *	m_rtmp_parse;
	bool		neoip_rtmp_parse_cb(void *cb_userptr, rtmp_parse_t &cb_rtmp_parse
					, const rtmp_event_t &rtmp_event)	throw();

	/*************** head_autodel_zerotimer	*******************************/
	zerotimer_t	post_fdwatch_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer
							, void *userptr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	rtmp2xml_apps_t()		throw();
	~rtmp2xml_apps_t()		throw();

	/*************** setup function	***************************************/
	rtmp_err_t	start()	throw();

	/*************** query function	***************************************/
	static clineopt_arr_t		clineopt_arr()		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP2XML_APPS_HPP__  */



