/*! \file
    \brief Header of the unit test of the kad_bstrap_src_t layer
*/


#ifndef __NEOIP_KAD_BSTRAP_SRC_NUNIT_HPP__ 
#define __NEOIP_KAD_BSTRAP_SRC_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_kad_bstrap_src_cb.hpp"
#include "neoip_file_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	nslan_listener_t;
class	nslan_peer_t;
class	nslan_publish_t;

/** \brief Class which implement a nunit for the gen_id_t
 */
class kad_bstrap_src_testclass_t : public nunit_testclass_api_t, private kad_bstrap_src_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	
	/*************** nslan stuff	***************************************/
	nslan_listener_t *	nslan_listener;	//!< the nslan_listener_t for this nunit
	nslan_peer_t *		nslan_peer;	//!< the nslan_peer_t for this nunit
	nslan_publish_t *	nslan_publish;

	/*************** kad_bstrap_src_t callback	***********************/
	kad_bstrap_src_t *	kad_bstrap_src;	//!< the kad_bstrap_src_t which is tested
	bool 			neoip_kad_bstrap_src_cb(void *cb_userptr
						, kad_bstrap_src_t &cb_kad_bstrap_src
						, const kad_addr_t &kad_addr)	throw();

	
	file_path_t	dynafile_path;		//!< the file_path_t of the dynafile source
	file_path_t	statfile_path;		//!< the file_path_t of the dynafile source

	/*************** file create/delete during the nunit	***************/
	file_err_t	create_dynastat_file()	throw();
	file_err_t	delete_dynastat_file()	throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_bstrap_src_testclass_t()	throw();
	~kad_bstrap_src_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)			throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAP_SRC_NUNIT_HPP__  */



