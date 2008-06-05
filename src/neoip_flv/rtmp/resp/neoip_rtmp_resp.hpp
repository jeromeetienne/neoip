/*! \file
    \brief Header of the \ref rtmp_resp_t
    
*/


#ifndef __NEOIP_RTMP_RESP_HPP__ 
#define __NEOIP_RTMP_RESP_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_flv_err.hpp"
#include "neoip_socket_resp_cb.hpp"
#include "neoip_socket_resp_arg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration

/** \brief Accept the http connections and then spawn rtmp_resp_cnx_t to handle them
 */
class rtmp_resp_t : NEOIP_COPY_CTOR_DENY, private socket_resp_cb_t {
private:
	/*************** socket_resp_t	***************************************/
	socket_resp_t *	socket_resp;
	bool		neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
							, const socket_event_t &socket_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	rtmp_resp_t()		throw();
	~rtmp_resp_t()		throw();
	
	/*************** Setup function	***************************************/
	flv_err_t		start(const socket_resp_arg_t &resp_arg)	throw();
	
	/*************** Query function	***************************************/
	const socket_addr_t  &	listen_addr()					const throw();


	/*************** list of friend class	*******************************/
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_RESP_HPP__  */










