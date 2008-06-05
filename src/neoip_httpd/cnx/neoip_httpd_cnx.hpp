/*! \file
    \brief Header of the \ref httpd_t class
    
*/


#ifndef __NEOIP_HTTPD_CNX_HPP__ 
#define __NEOIP_HTTPD_CNX_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_httpd.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	pkt_t;

/** \brief handle a given connection received by the socket_utest_resp_t
 */
class httpd_t::cnx_t : private tcp_full_cb_t {
private:
	httpd_t *	httpd;		//!< backpointer
	
	slot_id_t	slot_id;	//!< the slot_id to identify this connection
	
	// the network connection over which this httpd_t::cnx_t happen
	tcp_full_t *	tcp_full;
	bool		neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
					, const tcp_event_t &tcp_event)		throw();

	std::stringstream	pending_queue;	//!< contains the data read from the connection
						//!< until the http request if fully received
	bool		contain_http_request(const std::stringstream &ss) 	throw();
	bool		handle_recved_data(const pkt_t &pkt)			throw();

	std::string	read_httpline(std::stringstream &ss)			throw();
	
	httpd_err_t	parse_request(const std::string &str, httpd_request_t &request_out)	throw();
	httpd_err_t	process_request(httpd_request_t &request)		throw();

	std::string	build_http_header(const httpd_request_t &request, const httpd_err_t &httpd_err
						, const size_t content_length
						, const std::string &content_encoding = "" )	const throw();

	void		reply_data(const httpd_request_t &request, const httpd_err_t &http_err)
										throw();
	void		reply_error(const httpd_request_t &request, const httpd_err_t &httpd_err)
										throw();

	/*************** sendfile function	*******************************/
	FILE *		sendfile_fd;
	bool		sendfile_start(const httpd_request_t &request, const httpd_err_t &http_err)
										throw();
	bool		sendfile_inprogress()					const throw();
	void		sendfile_close()					throw();
	bool		sendfile_fill_sendbuf()					throw();
	
public:
	/*************** ctor/dtor	***************************************/
	// ctor/dtor
	cnx_t(httpd_t *httpd, tcp_full_t *tcp_full)	throw();
	~cnx_t()					throw();
	
	/*************** query function	***************************************/
	slot_id_t	get_slot_id()		const throw()	{ return slot_id;	}

	/*************** to notify delayed reply	***********************/
	void		handle_send_reply(const httpd_request_t &request, const httpd_err_t &httpd_err)
										throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTPD_CNX_HPP__  */



