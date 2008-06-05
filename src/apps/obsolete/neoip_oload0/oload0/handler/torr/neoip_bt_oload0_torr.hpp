/*! \file
    \brief Header of the \ref bt_oload0_torr_t
    
*/


#ifndef __NEOIP_BT_OLOAD0_TORR_HPP__ 
#define __NEOIP_BT_OLOAD0_TORR_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_oload0_torr_profile.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_httpo_resp_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_oload0_t;
class	bt_oload0_torr_itor_t;
class	bt_oload0_swarm_t;
class	http_uri_t;

/** \brief Handle the bt_httpo_resp_t for the "torrent" handler
 */
class bt_oload0_torr_t : NEOIP_COPY_CTOR_DENY, private bt_httpo_resp_cb_t {
private:
	bt_oload0_t *		bt_oload0;	//!< backpointer to the attached bt_oload0_t
	bt_oload0_torr_profile_t profile;	//!< the profile to use for the bt_oload0_torr_t
	/*************** bt_httpo_resp_cb_t	*******************************/
	bt_httpo_resp_t*	httpo_resp;
	bool			neoip_bt_httpo_resp_cb(void *cb_userptr, bt_httpo_resp_t &cb_bt_httpo_resp
						, const bt_httpo_event_t &httpo_event)	throw();	

	/*************** bt_oload0_torr_itor_t	*******************************/
	std::list<bt_oload0_torr_itor_t *>	torr_itor_db;
	void stat_itor_dolink(bt_oload0_torr_itor_t *itor) 	throw()	{ torr_itor_db.push_back(itor);	}
	void stat_itor_unlink(bt_oload0_torr_itor_t *itor)	throw()	{ torr_itor_db.remove(itor);	}
public:
	/*************** ctor/dtor	***************************************/
	bt_oload0_torr_t()		throw();
	~bt_oload0_torr_t()		throw();

	/*************** Setup function	***************************************/
	bt_oload0_torr_t &	set_profile(const bt_oload0_torr_profile_t &profile)	throw();
	bt_err_t		start(bt_oload0_t *bt_load)				throw();

	/*************** Query function	***************************************/
	bt_oload0_t *		get_bt_oload0()		const throw()	{ return bt_oload0;	}
	const bt_oload0_torr_profile_t &get_profile()	const throw()	{ return profile;	}
	bt_oload0_torr_itor_t *	itor_by_nested_uri(const http_uri_t &nested_uri)	const throw();

	/*************** List of friend class	*******************************/
	friend class	bt_oload0_torr_itor_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_OLOAD0_TORR_HPP__ */










