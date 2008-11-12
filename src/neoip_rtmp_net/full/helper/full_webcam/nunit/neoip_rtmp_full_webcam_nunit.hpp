/*! \file
    \brief Header of the test of rtmp_resp_t and all
*/


#ifndef __NEOIP_RTMP_FULL_WEBCAM_NUNIT_HPP__
#define __NEOIP_RTMP_FULL_WEBCAM_NUNIT_HPP__
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_rtmp_resp_cb.hpp"
#include "neoip_rtmp_full_webcam_cb.hpp"
#include "neoip_delay.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration

/** \brief Class which implement a nunit for the rtmp_resp_t
 */
class rtmp_full_webcam_testclass_t : public nunit_testclass_api_t, private rtmp_resp_cb_t
					, private rtmp_full_webcam_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously
	uint32_t		prev_tagsize;	//!< the flv_taghd_t::prev_tagsize (for flv file writing)
	delay_t			prev_timestamp;	//!< the flv_taghd_t::prev_timestamp (for flv file writing)

	/*************** rtmp_resp_t	***************************************/
	rtmp_resp_t *		rtmp_resp;
	bool			neoip_rtmp_resp_cb(void *cb_userptr, rtmp_resp_t &cb_rtmp_resp
						, rtmp_full_t *rtmp_full)		throw();
	/*************** rtmp_full_webcam_t	*******************************/
	rtmp_full_webcam_t *	full_webcam;
	bool			neoip_rtmp_full_webcam_cb(void *cb_userptr, rtmp_full_webcam_t &cb_full_webcam
						, const rtmp_event_t &rtmp_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	rtmp_full_webcam_testclass_t()		throw();
	~rtmp_full_webcam_testclass_t()		throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_FULL_WEBCAM_NUNIT_HPP__  */



