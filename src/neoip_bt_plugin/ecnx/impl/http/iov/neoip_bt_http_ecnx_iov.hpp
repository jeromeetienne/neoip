/*! \file
    \brief Header of the bt_http_ecnx_iov_t
    
*/


#ifndef __NEOIP_BT_HTTP_ECNX_IOV_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_IOV_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_http_ecnx_iov_cb.hpp"
#include "neoip_bt_http_ecnx_iov_wikidbg.hpp"
#include "neoip_bt_iov.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_http_ecnx_cnx_t;

/** \brief class definition for bt_ecnx
 */
class bt_http_ecnx_iov_t : NEOIP_COPY_CTOR_DENY, private http_sclient_cb_t
			, private wikidbg_obj_t<bt_http_ecnx_iov_t, bt_http_ecnx_iov_wikidbg_init>
			{
private:
	bt_http_ecnx_cnx_t *	m_ecnx_cnx;	//!< backpointer on the attached bt_http_ecnx_cnx_t
	bt_iov_t		m_bt_iov;	//!< the bt_iov_t to handle 

	/*************** http_client_t	*******************************/
	http_sclient_t *	m_http_sclient;
	bool 			neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
						, const http_sclient_res_t &sclient_res)throw();
	bool			neoip_http_sclient_progress_chunk_cb(void *cb_userptr
						, http_sclient_t &cb_sclient)		throw();

	/*************** callback stuff	***************************************/
	bt_http_ecnx_iov_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback_progress_chunk(const bt_iov_t &bt_iov
							, const datum_t &recved_data)	throw();
	bool			notify_callback_err(const bt_err_t &bt_err)	throw();
	bool			notify_callback(const bt_err_t &bt_err, const datum_t &recved_data
							, bool is_cnx_close)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_http_ecnx_iov_t() 		throw();
	~bt_http_ecnx_iov_t()		throw();

	/*************** setup function	***************************************/
	bt_err_t	start(bt_http_ecnx_cnx_t *ecnx_cnx, bt_iov_t &bt_iov
					, bt_http_ecnx_iov_cb_t *callback, void *userptr)	throw();

	/*************** query function	***************************************/
	const bt_iov_t &	bt_iov()	const throw() { return m_bt_iov;		}
	const file_range_t &	subfile_range()	const throw() { return m_bt_iov.subfile_range();}
	const file_size_t &	subfile_beg()	const throw() { return subfile_range().beg();	}
	const file_size_t &	subfile_end()	const throw() { return subfile_range().end();	}	

	/*************** List of friend class	*******************************/
	friend class	bt_http_ecnx_iov_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_IOV_HPP__  */



