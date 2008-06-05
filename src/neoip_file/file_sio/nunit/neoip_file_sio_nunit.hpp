/*! \file
    \brief Header of the test of file_sio_t
*/


#ifndef __NEOIP_FILE_SIO_NUNIT_HPP__ 
#define __NEOIP_FILE_SIO_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	file_sio_t;

/** \brief Class which implement a nunit for the file_sio_t
 */
class file_sio_testclass_t : public nunit_testclass_api_t {
private:
	file_sio_t *	file_sio;
	datum_t		data2write;
public:
	/*************** ctor/dtor	***************************************/
	file_sio_testclass_t()	throw();
	~file_sio_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	write(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	read(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_SIO_NUNIT_HPP__  */



