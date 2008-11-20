
/*! \file
    \brief Header of the \ref rtmp_cam_listener_t

*/


#ifndef __NEOIP_RTMP_CAM_LISTENER_HPP__
#define __NEOIP_RTMP_CAM_LISTENER_HPP__
/* system include */
#include <list>
/* local include */
#include "neoip_rtmp_cam_listener_wikidbg.hpp"
#include "neoip_rtmp_resp_cb.hpp"
#include "neoip_rtmp_cam_full_cb.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_socket_resp_arg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_cam_resp_t;
class	rtmp_cam_full_t;
class	http_uri_t;

/** \brief Accept the http connections and then spawn rtmp_cam_listener_cnx_t to handle them
 */
class rtmp_cam_listener_t : NEOIP_COPY_CTOR_DENY, private rtmp_resp_cb_t, private rtmp_cam_full_cb_t
			, private wikidbg_obj_t<rtmp_cam_listener_t, rtmp_cam_listener_wikidbg_init>
			{
private:
	/*************** rtmp_resp_t	***************************************/
	rtmp_resp_t *	rtmp_resp;
	bool		neoip_rtmp_resp_cb(void *cb_userptr, rtmp_resp_t &cb_rtmp_resp
					, rtmp_full_t *rtmp_full)		throw();

	/*************** store the cam_resp_t	*******************************/
	std::list<rtmp_cam_resp_t *>	cam_resp_db;
	void resp_dolink(rtmp_cam_resp_t *resp)	throw()	{ cam_resp_db.push_back(resp);	}
	void resp_unlink(rtmp_cam_resp_t *resp)	throw()	{ cam_resp_db.remove(resp);	}
	rtmp_cam_resp_t *	find_resp(const http_uri_t &connect_uri)	throw();

	/*************** store the cam_full_t	*******************************/
	std::list<rtmp_cam_full_t *>	cam_full_db;
	void full_dolink(rtmp_cam_full_t *full)	throw()	{ cam_full_db.push_back(full);	}
	void full_unlink(rtmp_cam_full_t *full)	throw()	{ cam_full_db.remove(full);	}
	bool		neoip_rtmp_cam_full_cb(void *cb_userptr, rtmp_cam_full_t &cb_cam_full
					, const rtmp_event_t &rtmp_event)	throw();
	bool		handle_event_connected(rtmp_cam_full_t *cam_full
					, const rtmp_event_t &rtmp_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	rtmp_cam_listener_t()		throw();
	~rtmp_cam_listener_t()		throw();

	/*************** Setup function	***************************************/
	rtmp_err_t	start(const socket_resp_arg_t &resp_arg)		throw();

	/*************** Query function	***************************************/


	/*************** list of friend class	*******************************/
	friend class	rtmp_cam_listener_wikidbg_t;
	friend class	rtmp_cam_resp_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_CAM_LISTENER_HPP__  */










