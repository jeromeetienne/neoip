/*! \file
    \brief Header of the test of rtmp_parse_t
*/


#ifndef __NEOIP_RTMP_PARSE_NUNIT_HPP__
#define __NEOIP_RTMP_PARSE_NUNIT_HPP__
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_rtmp_parse_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	rtmp_err_t;

/** \brief Class which implement a nunit for the rtmp_parse_t
 */
class rtmp_parse_testclass_t : public nunit_testclass_api_t, private rtmp_parse_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously

	/*************** rtmp_parse_t	***************************************/
	rtmp_parse_t *	rtmp_parse;
	bool		neoip_rtmp_parse_cb(void *cb_userptr, rtmp_parse_t &cb_rtmp_parse
					, const rtmp_event_t &parse_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	rtmp_parse_testclass_t()		throw();
	~rtmp_parse_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_PARSE_NUNIT_HPP__  */



