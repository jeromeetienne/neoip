/*! \file
    \brief Header of the \ref bt_cast_mdata_dopublish_t
    
*/


#ifndef __NEOIP_BT_CAST_MDATA_DOPUBLISH_HPP__ 
#define __NEOIP_BT_CAST_MDATA_DOPUBLISH_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_cast_mdata_dopublish_wikidbg.hpp"
#include "neoip_bt_cast_mdata_dopublish_profile.hpp"
#include "neoip_bt_cast_mdata_dopublish_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_xmlrpc_client_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casti_swarm_t;

/** \brief Handle the bt_cast_publish/mdata_t for the casti_swarm_t
 */
class bt_cast_mdata_dopublish_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
			, private xmlrpc_client_cb_t
			, private wikidbg_obj_t<bt_cast_mdata_dopublish_t, bt_cast_mdata_dopublish_wikidbg_init> {
private:
	bt_cast_mdata_dopublish_profile_t m_profile;//!< the profile for this object
	std::string	m_publish_type;		//!< the publish_type "push" or "pull"
	http_uri_t	m_mdata_srv_uri;	//!< the xmlrpc server http_uri_t
	std::string	m_cast_privtext;		//!< the cast_privtext for this publish
	std::string	m_cast_privhash;		//!< the cast_privhash for this publish
	bool		m_published;		//!< true if at least on bt_cast_mdata_t is published

	/*************** Internal function	*******************************/
	bool		may_set_pull()						throw();
	const delay_t &	periodic_delay()					const throw();
	datum_t		build_call_set_mdata(const std::string &type)		throw();
	datum_t		build_call_get_mdata(const std::string &cast_privhash)	throw();

	/*************** main xmlrpc_client_t callback	***********************/
	bool		neoip_xmlrpc_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw();

	/*************** m_periodic_timeout	*******************************/
	timeout_t	m_periodic_timeout;	//!< to periodically send a packet
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** m_probe_set	*******************************/
	xmlrpc_client_t *m_probe_set;
	bool		probe_set_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw();

	/*************** m_probe_get	***************************************/
	xmlrpc_client_t *m_probe_get;
	bool		probe_get_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw();
	
	/*************** m_periodic_set	***************************************/
	xmlrpc_client_t *m_periodic_set;
	bool		periodic_set_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw();

	/*************** callback stuff	***************************************/
	bt_cast_mdata_dopublish_cb_t *callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(bt_cast_mdata_t *cast_mdata_out
					, uint16_t *port_lview_out, uint16_t *post_pview_out
					, std::string *casti_uri_pathquery_out)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_mdata_dopublish_t()	throw();
	~bt_cast_mdata_dopublish_t()	throw();

	/*************** Setup function	***************************************/
	bt_cast_mdata_dopublish_t& profile(const bt_cast_mdata_dopublish_profile_t &profile)		throw();
	bt_err_t	start(const http_uri_t &m_mdata_srv_uri, const std::string &m_cast_privtext
					, bt_cast_mdata_dopublish_cb_t *callback
					, void *userptr)	throw();

	/*************** Query function	***************************************/
	bool			published()	const throw()	{ return m_published;	}
	const std::string &	cast_privhash()	const throw()	{ return m_cast_privhash;	}

	/*************** List of friend class	*******************************/
	friend class	bt_cast_mdata_dopublish_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_MDATA_DOPUBLISH_HPP__ */










