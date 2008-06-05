/*! \file
    \brief Header of the \ref httpd_handler_t class
    
*/


#ifndef __NEOIP_HTTPD_HANDLER_HPP__ 
#define __NEOIP_HTTPD_HANDLER_HPP__ 
/* system include */
/* local include */
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

typedef	uint32_t	httpd_handler_flag_t;

/** \brief store a handler for a given path in a \ref httpd_t
 */
class httpd_handler_t : NEOIP_COPY_CTOR_ALLOW {
public:	/*************** handler flag	***************************************/
	/** \brief if DEEPER_PATH_OK, the handler will accepts deeper path.
	 * - if handler_path == "/myhandler/path" and a request for "/myhandler/path/deeper/file"
	 *   occurs, the handler will be called
	 * - it is off by default
	 */
	static const httpd_handler_flag_t DEEPER_PATH_OK	= 1 << 0;
	static const httpd_handler_flag_t FLAG_DFL		= 0;
	
private:
	std::string		path;
	httpd_handler_flag_t	handler_flag;
	httpd_handler_cb_t *	callback;
	void *			userptr;
public:
	/*************** ctor/dtor	***************************************/
	httpd_handler_t()							throw();
	httpd_handler_t(std::string path, httpd_handler_cb_t *callback, void *userptr
				, httpd_handler_flag_t handler_flag = FLAG_DFL)	throw();
	
	/*************** query function	***************************************/
	const std::string &	get_path()	const throw() { return path;		}
	httpd_handler_cb_t *	get_callback()	const throw() { return callback;	}
	void *			get_userptr()	const throw() { return userptr;		}
	
	/*************** flag testing	***************************************/
	bool	is_deeper_path_ok()	const throw()	{ return handler_flag & DEEPER_PATH_OK;	}
	
	/*************** utility function	*******************************/
	httpd_err_t	notify(httpd_request_t &httpd_request)	throw()
			{ return callback->neoip_httpd_handler_cb(userptr, httpd_request);	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTPD_HANDLER_HPP__  */



