/*! \file
    \brief Header of the unit test of the kad_rec_src_t layer
*/


#ifndef __NEOIP_KAD_REC_SRC_NUNIT_HPP__ 
#define __NEOIP_KAD_REC_SRC_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_kad_rec_src_cb.hpp"
#include "neoip_kad_store_cb.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	udp_vresp_t;
class	kad_listener_t;
class	kad_peer_t;
class	kad_rec_src_t;

/** \brief Class which implement a nunit for the gen_id_t
 */
class kad_rec_src_testclass_t : public nunit_testclass_api_t, private kad_rec_src_cb_t
						, private kad_store_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
		
	udp_vresp_t *	udp_vresp;	//!< the udp_vresp for the kad_listener_t
	kad_listener_t*	kad_listener;	//!< the kad_listener_t on which the test is done
	kad_peer_t *	kad_peer;	//!< the kad_peer_t attached to kad_listener_t
	kad_keyid_t	keyid;		//!< the realm_id on which the unit test occurs
	
	/*************** kad_rec_src	***************************************/
	kad_rec_src_t *	kad_rec_src;	//!< the kad_rec_src_t which is tested
	bool 		neoip_kad_rec_src_cb(void *cb_userptr, kad_rec_src_t &cb_kad_rec_src
						, const kad_rec_t &record)	throw();
	/*************** kad_store_t	***************************************/			
	kad_store_t *	kad_store;
	bool		neoip_kad_store_cb(void *cb_userptr, kad_store_t &kad_store
						, const kad_event_t &kad_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_rec_src_testclass_t()	throw();
	~kad_rec_src_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	store_record(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)			throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_REC_SRC_NUNIT_HPP__  */



