/*! \file
    \brief Header of the \ref bt_oload0_mlink_itor_t
    
*/


#ifndef __NEOIP_BT_OLOAD0_MLINK_ITOR_HPP__ 
#define __NEOIP_BT_OLOAD0_MLINK_ITOR_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_mlink_file.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_oload0_mlink_t;
class	bt_httpo_full_t;
class	bt_mfile_t;
class	file_size_t;

/** \brief Handle the itor part for the bt_oload0_mlink_t
 */
class bt_oload0_mlink_itor_t : NEOIP_COPY_CTOR_DENY, private http_sclient_cb_t {
private:
	bt_oload0_mlink_t*oload0_mlink;	//!< backpointer to the attached bt_oload0_mlink_t
	http_uri_t	nested_uri;	//!< the nested_uri for this bt_oload0_mlink_itor_t
	mlink_file_t	mlink_file;	//!< the mlink_file_t containing the subfile to download
	size_t		subfile_idx;	//!< the subfile_idx to download within the mlink_file

	/*************** Internal function	*******************************/
	bt_mfile_t	build_bt_mfile()					const throw();
	bt_id_t		build_infohash()					const throw();
	bool		launch_oload0_swarm()					throw();
	bool		launch_getlen_sclient()					throw();
	
	/*************** bt_httpo_full_t	*******************************/
	std::list<bt_httpo_full_t *>	httpo_full_db;	

	/*************** http_client_t callback	*******************************/
	bool 		neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
						, const http_sclient_res_t &sclient_res)	throw();

	/*************** http_client_t to get the mlink_file_t	***************/
	http_sclient_t *mlink_sclient;
	bool 		mlink_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
						, const http_sclient_res_t &sclient_res)	throw();

	/*************** http_client_t to get the length of subfile	*******/
	std::list<http_sclient_t *>	getlen_sclient_db;
	bool 		getlen_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
						, const http_sclient_res_t &sclient_res)	throw();

	/*************** Handle the error	*******************************/
	bool		handle_itor_error(const std::string &reason)				throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_oload0_mlink_itor_t()		throw();
	~bt_oload0_mlink_itor_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t		start(bt_oload0_mlink_t *oload0_mlink, const http_uri_t &nested_uri
						, bt_httpo_full_t *httpo_full)	throw();
	
	/*************** Query function	***************************************/
	const http_uri_t &	get_nested_uri()	const throw()	{ return nested_uri;	}

	/*************** Action function	*******************************/
	void			add_httpo_full(bt_httpo_full_t *httpo_full)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_OLOAD0_MLINK_ITOR_HPP__ */










