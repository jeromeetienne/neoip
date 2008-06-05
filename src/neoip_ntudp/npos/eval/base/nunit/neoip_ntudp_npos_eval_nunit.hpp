/*! \file
    \brief Header of the test of ntudp_npos_eval_t
*/


#ifndef __NEOIP_NTUDP_NPOS_EVAL_NUNIT_HPP__ 
#define __NEOIP_NTUDP_NPOS_EVAL_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_ntudp_npos_eval_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declarations
class	ntudp_peer_t;
class	ntudp_npos_eval_t;

/** \brief Class which implement a nunit for the ntudp_npos_eval_t
 */
class ntudp_npos_eval_testclass_t : public nunit_testclass_api_t, private ntudp_npos_eval_cb_t {
private:
	ntudp_peer_t *		ntudp_peer1;	//!< the ntudp_peer_t which act as server for clients
	ntudp_peer_t *		ntudp_peer2;	//!< the ntudp_peer_t which act as server for clients
	ntudp_peer_t *		ntudp_peer3;	//!< the ntudp_peer_t which act as server for clients
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	ntudp_npos_eval_t *	ntudp_npos_eval;
	bool neoip_ntudp_npos_eval_cb(void *cb_userptr, ntudp_npos_eval_t &cb_ntudp_npos_eval
						, const ntudp_npos_res_t &ntudp_npos_res)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_eval_testclass_t()	throw();
	~ntudp_npos_eval_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_EVAL_NUNIT_HPP__  */



