/*! \file
    \brief Header of the bt_http_ecnx_cnx_t
    
*/


#ifndef __NEOIP_BT_HTTP_ECNX_CNX_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_CNX_HPP__ 
/* system include */
#include <list>
#include <map>
/* local include */
#include "neoip_bt_http_ecnx_cnx_wikidbg.hpp"
#include "neoip_bt_http_ecnx_cnx_stats.hpp"
#include "neoip_bt_http_ecnx_cnx_profile.hpp"
#include "neoip_bt_http_ecnx_iov.hpp"
#include "neoip_bt_ecnx_vapi.hpp"
#include "neoip_bt_ecnx_vapi_cb.hpp"
#include "neoip_bt_swarm_sched_cnx_vapi.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_pieceavail.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_rate_estim.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_http_ecnx_pool_t;
class	bt_http_ecnx_req_t;
class	bt_swarm_sched_request_t;

/** \brief class definition for bt_ecnx
 */
class bt_http_ecnx_cnx_t : NEOIP_COPY_CTOR_DENY, private bt_http_ecnx_iov_cb_t, private zerotimer_cb_t
			, public bt_ecnx_vapi_t, public bt_swarm_sched_cnx_vapi_t
			, private wikidbg_obj_t<bt_http_ecnx_cnx_t, bt_http_ecnx_cnx_wikidbg_init, bt_http_ecnx_iov_cb_t, bt_ecnx_vapi_t, bt_swarm_sched_cnx_vapi_t>
			{
private:
	bt_http_ecnx_pool_t *		m_ecnx_pool;	//!< backpointer on the attached bt_http_ecnx_pool_t
	bt_http_ecnx_cnx_profile_t	m_profile;	//!< the profile to use for the bt_http_ecnx_cnx_t	
	std::string			m_hostport_str;	//!< the hostport for this connectoin
	bt_http_ecnx_cnx_stats_t	m_cnx_stats;	//!< the statistics for the bt_http_ecnx_cnx_t
	rate_estim_t<size_t>		m_recv_rate;	//!< estimate the recv_rate
	bool				m_is_cnx_close;	//!< true if the http cnx toward hostport_str
							//!< is expected to close connection after each request

	/*************** Internal function	*******************************/
	void			build_subfile_uri_arr()				throw();
	void			build_pieceavail()				throw();
	bt_iov_t		build_gathered_iov()				const throw();
	void			launch_next_iov()				throw();
	bool			is_sane()					const throw();

	/*************** pieceavail	***************************************/
	bt_pieceavail_t		pieceavail;	//!< the complete pieces available thru this connection
	
	/*************** subfile_uri_db	***************************************/
	typedef item_arr_t<http_uri_t> subfile_uri_arr_t;
	subfile_uri_arr_t	subfile_uri_arr;	//!< contain a http_uri_t for each subfile idx

	/*************** bt_http_ecnx_req_t	*******************************/
	std::list<bt_http_ecnx_req_t *>					ecnx_req_fifo;
	std::map<file_size_t, bt_http_ecnx_req_t *>			ecnx_req_offs;
	std::map<bt_swarm_sched_request_t *, bt_http_ecnx_req_t *>	ecnx_req_db;
	void			ecnx_req_dolink(bt_http_ecnx_req_t *ecnx_req)	throw();
	void			ecnx_req_unlink(bt_http_ecnx_req_t *ecnx_req)	throw();

	/*************** zerotimer_t main callback	***********************/
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** next_iov_zerotimer	*******************************/
	zerotimer_t		next_iov_zerotimer;
	bool			next_iov_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** kill_iov_zerotimer	*******************************/
	zerotimer_t		kill_iov_zerotimer;
	bool			kill_iov_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** bt_http_ecnx_iov_t	*******************************/
	bt_http_ecnx_iov_t *	m_ecnx_iov;
	bool	neoip_bt_http_ecnx_iov_cb(void *cb_userptr, bt_http_ecnx_iov_t &cb_ecnx_iov
				, const bt_err_t &bt_err, const datum_t &recved_data
				, bool cb_is_cnx_close)					throw();
	bool	neoip_bt_http_ecnx_iov_progress_chunk_cb(void *cb_userptr, bt_http_ecnx_iov_t &cb_ecnx_iov
				, const bt_iov_t &bt_iov, const datum_t &recved_data)	throw();	

	/*************** handle bt_http_ecnx_iov_t result	***************/
	bool	handle_iov_cnx_error(const bt_err_t &bt_err)				throw();
	bool	handle_iov_http_error(const bt_err_t &bt_err)				throw();
	bool	handle_iov_recved_data(const datum_t &recved_data)			throw();
	bool	process_iov_data(const bt_iov_t &bt_iov, const datum_t &recved_data)	throw();

	/*************** callback stuff	***************************************/
	bt_ecnx_vapi_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_ecnx_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_http_ecnx_cnx_t() 		throw();
	~bt_http_ecnx_cnx_t()		throw();
	
	/*************** setup function	***************************************/
	bt_http_ecnx_cnx_t&	set_profile(const bt_http_ecnx_cnx_profile_t &profile)	throw();	
	bt_err_t		start(bt_http_ecnx_pool_t *ecnx_pool, const std::string &hostport_str
					, bt_ecnx_vapi_cb_t *callback, void *userptr)	throw();

	/*************** Query function	***************************************/
	const std::string &	hostport_str()	const throw()	{ return m_hostport_str;}
	bt_http_ecnx_pool_t *	ecnx_pool()	const throw()	{ return m_ecnx_pool;	}
	bt_http_ecnx_cnx_stats_t&cnx_stats()	throw()		{ return m_cnx_stats;	}
	const bt_http_ecnx_cnx_profile_t &profile()const throw(){ return m_profile;	}

	/*************** bt_swarm_sched_cnx_vapi_t	***********************/
	void		sched_req_dolink_start(bt_swarm_sched_request_t *sched_req)	throw();
	void		sched_req_unlink(bt_swarm_sched_request_t *sched_req)		throw();
	void		sched_req_cancel(bt_swarm_sched_request_t *sched_req)		throw();

	/*************** bt_ecnx_vapi_t	***************************************/
	const bt_pieceavail_t &	remote_pavail()		const throw()	{ return pieceavail;		}
	size_t		nb_pending_request()		const throw()	{ return ecnx_req_db.size();	}
	bt_swarm_sched_cnx_vapi_t *sched_cnx_vapi()	throw()		{ return this;			}	
	double		recv_rate_avg()			const throw()	{ return m_recv_rate.average();	}
	size_t		req_queue_maxlen()		const throw();

	/*************** List of friend class	*******************************/
	friend class	bt_http_ecnx_cnx_wikidbg_t;
	friend class	bt_http_ecnx_req_t;
	friend class	bt_http_ecnx_iov_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_CNX_HPP__  */



