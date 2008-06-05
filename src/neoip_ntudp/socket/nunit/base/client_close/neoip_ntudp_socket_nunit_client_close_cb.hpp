/*! \file
    \brief Declaration of the ntudp_client_t
    
*/


#ifndef __NEOIP_NTUDP_SOCKET_NUNIT_CLIENT_CLOSE_CB_HPP__ 
#define __NEOIP_NTUDP_SOCKET_NUNIT_CLIENT_CLOSE_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_socket_nunit_client_close_t;

/** \brief the callback class for ntudp_client_t
 */
class ntudp_socket_nunit_client_close_cb_t {
public:
	/** \brief callback notified by \ref ntudp_socket_nunit_client_close_t to notify a result
	 * 
	 * @param userptr  		the userptr associated with this callback
	 * @param cb_client_close 	the ntudp_socket_nunit_client_close_t which notified this callback
	 * @param succeed		true if the client succeed, false otherwise
	 * @return false if the notifier has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_ntudp_socket_nunit_client_close_cb(void *cb_userptr
					, ntudp_socket_nunit_client_close_t &cb_client_close
					, bool succeed)	throw() = 0;
	virtual ~ntudp_socket_nunit_client_close_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_SOCKET_NUNIT_CLIENT_CLOSE_CB_HPP__  */



