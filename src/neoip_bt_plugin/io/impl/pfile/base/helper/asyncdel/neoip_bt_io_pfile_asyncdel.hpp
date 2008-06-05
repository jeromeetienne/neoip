/*! \file
    \brief Header of the bt_io_pfile_asyncdel_t
    
*/


#ifndef __NEOIP_BT_IO_PFILE_ASYNCDEL_HPP__ 
#define __NEOIP_BT_IO_PFILE_ASYNCDEL_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_io_pfile_asyncdel_profile.hpp"
#include "neoip_bt_io_pfile_asyncdel_policy.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;

/** \brief Handle the deletion of piece which are no more required
 * 
 * - when bt_swarm_t caches some of the pieces
 *   - e.g. with http offload to be able to share the backqueue
 */
class bt_io_pfile_asyncdel_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
					, private zerotimer_cb_t {
private:
	bt_swarm_t *			bt_swarm;	//!< pointer to the bt_swarm_t
	bt_io_pfile_asyncdel_profile_t	profile;	//!< the bt_io_pfile_asyncdel_profile_t for this bt_io_pfile_asyncdel_t
	bt_io_pfile_asyncdel_policy_t	asyncdel_policy;
	std::list<size_t>		del_pieceidx_db;//!< list of pieceidx to delete

	/*************** zerotimer_t	***************************************/
	zerotimer_t	del_pieceidx_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** Internal function	*******************************/
	void		delete_all_notrequired()	throw();
	
	/*************** timer stuff	***************************************/
	delaygen_t	deletor_delaygen;	//!< the delay_t generator for the deletor
	timeout_t	deletor_timeout;	//!< to trigger the next deletor
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_io_pfile_asyncdel_t() 		throw();
	~bt_io_pfile_asyncdel_t()		throw();
	
	/*************** setup function	***************************************/
	bt_io_pfile_asyncdel_t &	set_profile(const bt_io_pfile_asyncdel_profile_t &profile)	throw();	
	bt_err_t		start(const bt_io_pfile_asyncdel_policy_t &asyncdel_policy
							, bt_swarm_t *bt_swarm)		throw();

};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_PFILE_ASYNCDEL_HPP__  */



