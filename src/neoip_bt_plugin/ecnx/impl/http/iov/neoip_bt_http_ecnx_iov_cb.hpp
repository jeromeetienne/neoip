/*! \file
    \brief Declaration of the bt_http_ecnx_iov_t
    
*/


#ifndef __NEOIP_BT_HTTP_ECNX_IOV_CB_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_IOV_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_http_ecnx_iov_t;
class	bt_err_t;
class	bt_iov_t;
class	datum_t;

/** \brief the callback class for bt_http_ecnx_iov_t
 */
class bt_http_ecnx_iov_cb_t {
public:
	/** \brief callback notified by \ref bt_http_ecnx_iov_t when to notify an event
	 * 
	 * - recved_data and is_cnx_close are valid IIF bt_err.succeed()
	 */
	virtual bool neoip_bt_http_ecnx_iov_cb(void *cb_userptr, bt_http_ecnx_iov_t &cb_ecnx_iov
				, const bt_err_t &bt_err, const datum_t &recved_data
				, bool cb_is_cnx_close)	throw() = 0;

	/** \brief callback notified by \ref bt_http_ecnx_iov_t has a chunk to notify
	 */
	virtual bool neoip_bt_http_ecnx_iov_progress_chunk_cb(void *cb_userptr
				, bt_http_ecnx_iov_t &cb_ecnx_iov
				, const bt_iov_t &bt_iov, const datum_t &recved_data)	throw() = 0;
	// virtual destructor
	virtual ~bt_http_ecnx_iov_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_IOV_CB_HPP__  */



