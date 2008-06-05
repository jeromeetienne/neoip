/*! \file
    \brief Header of the test of flv_parse_t
*/


#ifndef __NEOIP_FLV_PARSE_NUNIT_HPP__ 
#define __NEOIP_FLV_PARSE_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_flv_parse_cb.hpp"
#include "neoip_file_size.hpp"
#include "neoip_file_aread_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	flv_err_t;
class	file_aio_t;

/** \brief Class which implement a nunit for the flv_parse_t
 */
class flv_parse_testclass_t : public nunit_testclass_api_t, private file_aread_cb_t
						, private flv_parse_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	file_aio_t *	file_aio;
	file_size_t	file_size;
	file_size_t	read_size;

	/*************** flv_parse_t	***************************************/
	flv_parse_t *	flv_parse;	
	bool		neoip_flv_parse_cb(void *cb_userptr, flv_parse_t &cb_flv_parse
					, const flv_parse_event_t &parse_event)	throw();

	/*************** file_aread_t	***************************************/
	file_aread_t *	file_aread;
	bool		neoip_file_aread_cb(void *cb_userptr, file_aread_t &cb_file_aread
							, const file_err_t &file_err
							, const datum_t &data)	throw();
	flv_err_t	launch_next_read()	throw();
public:
	/*************** ctor/dtor	***************************************/
	flv_parse_testclass_t()		throw();
	~flv_parse_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_PARSE_NUNIT_HPP__  */



