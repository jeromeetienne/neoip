/*! \file
    \brief Header of the bt_httpo_full_t
    
*/


#ifndef __NEOIP_BT_HTTPO_FULL_HPP__ 
#define __NEOIP_BT_HTTPO_FULL_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_httpo_full_wikidbg.hpp"
#include "neoip_bt_httpo_full_cb.hpp"
#include "neoip_bt_httpo_full_arg.hpp"
#include "neoip_bt_httpo_full_profile.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_bt_io_read_cb.hpp"
#include "neoip_file_range.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_httpo_resp_t;
class	bt_swarm_t;
class	bt_io_pfile_t;
class	bt_pselect_slide_curs_t;
class	rate_sched_t;
class	http_reqhd_t;
class	http_rephd_t;
class	http_status_t;


/** \brief class definition for bt_httpo_full
 */
class bt_httpo_full_t : NEOIP_COPY_CTOR_DENY, public socket_full_cb_t, private bt_io_read_cb_t
			, private zerotimer_cb_t
			, private wikidbg_obj_t<bt_httpo_full_t, bt_httpo_full_wikidbg_init>
			{
private:
	bt_httpo_full_profile_t m_profile;//!< the profile for this object
	bt_httpo_resp_t*httpo_resp;	//!< backpointer on the bt_httpo_resp_t
	bt_swarm_t *	bt_swarm;	//!< the bt_swarm_t which provide data for this bt_httpo_full_t
	rate_sched_t *	m_xmit_rsched;	//!< rate_sched_t for xmit
	
	http_reqhd_t	m_http_reqhd;	//!< the http_reqhd_t which caused this socket_full_t
	
	file_range_t	m_range_tosend;	//!< the file_range_t to forward thru this bt_httpo_full_t
	file_size_t	m_current_pos;	//!< the current position within the bt_mfile_t::totfile
	file_size_t	m_sent_length;	//!< the amount of data already sent

	/*************** piece deletion policy	*******************************/
	bool		piecedel_in_dtor;	//!< if true, in dtor attempt to delete piece
	bool		piecedel_as_delivered;	//!< if true, attempt to delete piece when delivered
	bool		piecedel_pre_newlyavail;//!< if true, in notify_newly_avail_piece() attempt 
						//!< to delete piece which are m_slide_curs in past
	
	/*************** bt_pselect_slide_curs_t	***********************/
	bt_pselect_slide_curs_t*m_slide_curs;
	bt_err_t		slide_curs_ctor(const bt_pselect_slide_curs_arg_t &arg)	throw();

	/*************** Internal function	*******************************/
	void		try_fill_xmitbuf()		throw();
	file_range_t	range_to_read()			const throw();
	void		try_del_piece(size_t pieceidx)	throw();
	bool		has_circularidx()		const throw();

	/*************** socket_full_t	***************************************/
	socket_full_t *	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw();

	/*************** head_autodel_zerotimer	*******************************/
	zerotimer_t	head_autodel_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer
								, void *userptr)	throw();

	/*************** bt_io_read_t	***************************************/
	bt_io_read_t *	bt_io_read;
	bool 		neoip_bt_io_read_cb(void *cb_userptr, bt_io_read_t &cb_bt_io_read
				, const bt_err_t &bt_err, const datum_t &read_data)	throw();

	/*************** callback stuff	***************************************/
	bt_httpo_full_cb_t *callback;	//!< callback used to notify result
	void *		m_userptr;	//!< userptr associated with the callback
	bool		notify_callback(const bt_httpo_event_t &cnx_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_httpo_full_t(bt_httpo_resp_t *httpo_resp, socket_full_t *socket_full
					, const http_reqhd_t &http_reqhd)	throw();
	~bt_httpo_full_t()		throw();

	/*************** setup	***********************************************/
	bt_httpo_full_t&profile(const bt_httpo_full_profile_t &profile)		throw();
	bt_err_t	start(const bt_httpo_full_arg_t &arg, bt_httpo_full_cb_t *callback
							, void *userptr)	throw();
	bt_err_t	start_reply_error(const	http_rephd_t &http_rephd)	throw();
	bt_err_t	start_reply_error(const http_status_t &status_code
					, const std::string &reason_phrase ="")	throw();

	/*************** Query function	***************************************/
	bool			is_started()	const throw()	{ return callback;		}
	const http_reqhd_t &	http_reqhd()	const throw()	{ return m_http_reqhd;		}
	bt_pselect_slide_curs_t*slide_curs()	const throw()	{ return m_slide_curs;		}
	const file_range_t &	range_tosend()	const throw()	{ return m_range_tosend;	}
	const file_size_t &	current_pos()	const throw()	{ return m_current_pos;		}
	const file_size_t &	sent_length()	const throw()	{ return m_sent_length;		}
	void *			userptr()	const throw()	{ return m_userptr;		}

	/*************** Action function	*******************************/
	void			notify_newly_avail_piece(size_t pieceidx)		throw();
	void			reinit_range_tosend(const file_range_t &new_range)	throw();
	bt_httpo_full_t &	http_reqhd(const http_reqhd_t &new_reqhd)		throw();
	bt_httpo_full_t &	userptr(void *new_val)					throw();
	bt_httpo_full_t &	xmit_maxrate(double maxrate)				throw();

	/*************** List of friend class	*******************************/
	friend class	bt_httpo_full_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPO_FULL_HPP__  */



