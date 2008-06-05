/*! \file
    \brief Header of the \ref bt_oload0_stat_itor_t
    
*/


#ifndef __NEOIP_BT_OLOAD0_STAT_ITOR_HPP__ 
#define __NEOIP_BT_OLOAD0_STAT_ITOR_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_oload0_stat_t;
class	bt_oload0_swarm_t;
class	bt_httpo_full_t;
class	bt_mfile_t;
class	file_size_t;

/** \brief Handle the itor part for the bt_oload0_stat_t
 */
class bt_oload0_stat_itor_t : NEOIP_COPY_CTOR_DENY, private http_sclient_cb_t, private zerotimer_cb_t {
private:
	bt_oload0_stat_t*oload0_stat;	//!< backpointer to the attached bt_oload0_stat_t
	http_uri_t	nested_uri;	//!< the nested_uri for this bt_oload0_stat_itor_t
	
	/*************** Internal function	*******************************/
	bt_oload0_swarm_t *	launch_oload0_swarm(const file_size_t &totfile_len)	throw();
	bt_mfile_t		build_bt_mfile(const file_size_t &totfile_len)		const throw();

	/*************** bt_httpo_full_t	*******************************/
	std::list<bt_httpo_full_t *>	httpo_full_db;	
	
	/*************** zerotimer_t	***************************************/
	zerotimer_t	filelen_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** http_client_t	***************************************/
	http_sclient_t *getlen_sclient;
	bool 		neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
						, const http_sclient_res_t &sclient_res)	throw();
	bool		handle_itor_error(const std::string &reason)				throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_oload0_stat_itor_t()		throw();
	~bt_oload0_stat_itor_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t		start(bt_oload0_stat_t *oload0_stat, const http_uri_t &nested_uri
						, bt_httpo_full_t *httpo_full)	throw();
	
	/*************** Query function	***************************************/
	const http_uri_t &	get_nested_uri()	const throw()	{ return nested_uri;	}

	/*************** Action function	*******************************/
	void			add_httpo_full(bt_httpo_full_t *httpo_full)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_OLOAD0_STAT_ITOR_HPP__ */










