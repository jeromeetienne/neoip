/*! \file
    \brief Header of the \ref flv2xml_apps_t class

*/


#ifndef __NEOIP_FLV2XML_APPS_HPP__ 
#define __NEOIP_FLV2XML_APPS_HPP__ 
/* system include */
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_flv_parse_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	flv_err_t;
class	flv_taghd_t;
class	datum_t;
class	clineopt_arr_t;

/** \brief The object to run the neoip_get apps
 */
class flv2xml_apps_t : NEOIP_COPY_CTOR_DENY, private fdwatch_cb_t, private flv_parse_cb_t
					, private zerotimer_cb_t {
private:
	bool		output_tophd;		//!< true if the tophd should be output
	bool		output_tag_audio;	//!< true if the tag audio should be output
	bool		output_tag_video;	//!< true if the tag video should be output
	bool		output_tag_meta;	//!< true if the tag meta  should be output
	bool		output_custom_kframe;	//!< true if the tag meta  should be output

	/*************** Internal function	*******************************/
	bool		exit_asap(const flv_err_t &flv_err)				throw();
	bool		exit_asap(const std::string &reason)				throw();
	bool		parsing_completed(const flv_parse_event_t &parse_event)		throw();
	flv_err_t	parse_event_tophd(const flv_parse_event_t &parse_event)		throw();
	flv_err_t	parse_event_tag(const flv_parse_event_t &parse_event)		throw();
	flv_err_t	display_kframe_ifavail(const flv_parse_event_t &parse_event)	throw();
	static void	display_taghd_beg(const flv_taghd_t &flv_taghd)			throw();
	static void	display_taghd_end(const flv_taghd_t &flv_taghd)			throw();

	/*************** parser per flv_tagtype_t	***********************/
	flv_err_t	parse_tagtype_audio(const flv_taghd_t &flv_taghd
						, const datum_t &tag_data)	throw();
	flv_err_t	parse_tagtype_video(const flv_taghd_t &flv_taghd
						, const datum_t &tag_data)	throw();
	flv_err_t	parse_tagtype_meta(const flv_taghd_t &flv_taghd
						, const datum_t &tag_data)	throw();

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	m_fdwatch;
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
				, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** flv_parse_t	***************************************/
	flv_parse_t *	m_flv_parse;	
	bool		neoip_flv_parse_cb(void *cb_userptr, flv_parse_t &cb_flv_parse
					, const flv_parse_event_t &parse_event)	throw();

	/*************** head_autodel_zerotimer	*******************************/
	zerotimer_t	post_fdwatch_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer
							, void *userptr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	flv2xml_apps_t()		throw();
	~flv2xml_apps_t()		throw();
	
	/*************** setup function	***************************************/
	flv_err_t	start()	throw();

	/*************** query function	***************************************/
	static clineopt_arr_t		clineopt_arr()		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV2XML_APPS_HPP__  */


 
