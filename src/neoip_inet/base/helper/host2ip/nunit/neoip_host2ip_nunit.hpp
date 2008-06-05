/*! \file
    \brief Header of the unit test of the host2ip_t layer
*/


#ifndef __NEOIP_HOST2IP_NUNIT_HPP__
#define __NEOIP_HOST2IP_NUNIT_HPP__
/* system include */
#include <list>
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_host2ip_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the gen_id_t
 */
class host2ip_testclass_t : public nunit_testclass_api_t, private host2ip_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	std::list<std::string>	hostname_db;
	size_t			nb_concurrent;

	/*************** internal function	*******************************/
	void			launch_host2ip()	throw();

	/*************** host2ip_db_t	***************************************/
	std::list<host2ip_t *>	host2ip_db;
	bool			neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
						, const inet_err_t &inet_err
						, const std::vector<ip_addr_t> &ipaddr_arr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	host2ip_testclass_t()	throw();
	~host2ip_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HOST2IP_NUNIT_HPP__  */



