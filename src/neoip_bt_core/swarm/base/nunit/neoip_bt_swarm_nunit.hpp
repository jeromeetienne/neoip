/*! \file
    \brief Header of the test of bt_swarm_t
*/


#ifndef __NEOIP_BT_SWARM_NUNIT_HPP__ 
#define __NEOIP_BT_SWARM_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_file_path.hpp"
#include "neoip_bt_swarm_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_session_t;
class	bt_io_vapi_t;
class	bt_http_ecnx_pool_t;
class	udp_vresp_t;
class	kad_listener_t;
class	kad_peer_t;
class	bt_peersrc_http_t;
class	bt_peersrc_kad_t;
class	bt_peersrc_utpex_t;

/** \brief Class which implement a nunit for the bt_swarm_t
 */
class bt_swarm_testclass_t : public nunit_testclass_api_t, private bt_swarm_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	file_path_t		mfile_path;
	bt_session_t *		bt_session;
	bt_io_vapi_t *		bt_io_vapi;	//!< the bt_io_vapi_t to use
	bt_peersrc_http_t *	peersrc_http;
	bt_peersrc_utpex_t *	peersrc_utpex;
	bt_http_ecnx_pool_t *	http_ecnx_pool;
	
	/*************** stuff for bt_peersrc_kad_t	***********************/
	udp_vresp_t *		udp_vresp;	//!< the core udp_vresp_t listening on a ipport_addr_t
	kad_listener_t *	kad_listener;	//!< the kad_listener for this ntudp_peer_t 
	kad_peer_t *		kad_peer;	//!< the kad_peer_t for the underlying NS
	bt_peersrc_kad_t *	peersrc_kad;	//!< the bt_peersrc_kad_t for this bt_swarm_t
		
	/*************** bt_swarm_t	***************************************/
	bt_swarm_t *		bt_swarm;
	bool 			neoip_bt_swarm_cb(void *cb_userptr, bt_swarm_t &cb_bt_swarm
					, const bt_swarm_event_t &swarm_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_testclass_t()		throw();
	~bt_swarm_testclass_t()		throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_NUNIT_HPP__  */



