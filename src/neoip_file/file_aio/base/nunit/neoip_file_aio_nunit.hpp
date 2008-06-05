/*! \file
    \brief Header of the test of file_aio_t
*/


#ifndef __NEOIP_FILE_AIO_NUNIT_HPP__ 
#define __NEOIP_FILE_AIO_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_file_awrite_cb.hpp"
#include "neoip_file_aread_cb.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	file_aio_t;

/** \brief Class which implement a nunit for the file_aio_t
 */
class file_aio_testclass_t : public nunit_testclass_api_t, private file_awrite_cb_t
							, private file_aread_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	file_aio_t *	file_aio;
	datum_t		data2write;

	/*************** file_awrite_t	***************************************/
	file_awrite_t *	file_awrite;
	bool		neoip_file_awrite_cb(void *cb_userptr, file_awrite_t &cb_file_awrite
							, const file_err_t &file_err)	throw();
							
	/*************** file_aread_t	***************************************/
	file_aread_t *	file_aread;
	bool		neoip_file_aread_cb(void *cb_userptr, file_aread_t &cb_file_aread
							, const file_err_t &file_err
							, const datum_t &data)	throw();
public:
	/*************** ctor/dtor	***************************************/
	file_aio_testclass_t()	throw();
	~file_aio_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	awrite(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	aread(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_AIO_NUNIT_HPP__  */



