/*! \file
    \brief Header of the test of bt_ezswarm_t
*/


#ifndef __NEOIP_BT_EZSWARM_NUNIT_HPP__ 
#define __NEOIP_BT_EZSWARM_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_bt_ezswarm_cb.hpp"
#include "neoip_file_path.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_ezsession_t;
class	bt_err_t;

/** \brief Class which implement a nunit for the bt_ezswarm_t
 */
class bt_ezswarm_testclass_t : public nunit_testclass_api_t, private bt_ezswarm_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	file_path_t		mfile_path;

	/*************** bt_ezsession_t	***************************************/
	bt_ezsession_t *	bt_ezsession;
	bt_err_t		launch_ezsession()		throw();

	/*************** bt_ezswarm_t	***************************************/
	bt_ezswarm_t *		bt_ezswarm;
	bool 			neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_ezswarm_testclass_t()		throw();
	~bt_ezswarm_testclass_t()		throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_NUNIT_HPP__  */



