/*! \file
    \brief Header of the test of ntudp_npos_t
*/


#ifndef __NEOIP_NTUDP_NPOS_CLI_NUNIT_HPP__ 
#define __NEOIP_NTUDP_NPOS_CLI_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_ntudp_npos_saddrecho_cb.hpp"
#include "neoip_ntudp_npos_inetreach_cb.hpp"
#include "neoip_ntudp_npos_natlback_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declarations
class	ntudp_peer_t;

/** \brief Class which implement a nunit for the ntudp_npos_t
 */
class ntudp_npos_cli_testclass_t : public nunit_testclass_api_t
					, private ntudp_npos_saddrecho_cb_t
					, private ntudp_npos_inetreach_cb_t
					, private ntudp_npos_natlback_cb_t
					{
private:
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
	bool			notify_nunit_res(const nunit_res_t &nunit_res)	throw();
	ntudp_peer_t *		ntudp_peer1;	//!< the ntudp_peer_t which act as clients
	ntudp_peer_t *		ntudp_peer2;	//!< the ntudp_peer_t which act as server
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	/*************** ntudp_npos_saddrecho_t	***********************/
	ntudp_npos_saddrecho_t *	ntudp_npos_saddrecho;
	bool neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
						, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();
	/*************** ntudp_npos_inetreach_t	***********************/
	ntudp_npos_inetreach_t *	ntudp_npos_inetreach;
	bool neoip_ntudp_npos_inetreach_event_cb(void *cb_userptr
						, ntudp_npos_inetreach_t &cb_ntudp_npos_inetreach
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();

	/*************** ntudp_npos_natlback_t	***********************/
	ntudp_npos_natlback_t *	ntudp_npos_natlback;
	bool neoip_ntudp_npos_natlback_event_cb(void *cb_userptr
						, ntudp_npos_natlback_t &cb_ntudp_npos_natlback
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_cli_testclass_t()	throw();
	~ntudp_npos_cli_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	saddrecho(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	inetreach(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	natlback(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_CLI_NUNIT_HPP__  */



