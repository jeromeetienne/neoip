/*! \file
    \brief Definition of the \ref httpd_t class

\par Possible Improvement
- to port on top of socket_t once it works well
- to handle the maysend_on/off in transmit

*/

/* system include */
#include <algorithm>
/* local include */
#include "neoip_httpd_cnx.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_string.hpp"
#include "neoip_pkt.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_compress.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    httpd_t::cnx_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
httpd_t::cnx_t::cnx_t(httpd_t *httpd, tcp_full_t *tcp_full)			throw()
{
	inet_err_t	inet_err;
	// copy parameter
	this->httpd	= httpd;
	this->tcp_full	= tcp_full;
	// allocate a slot_id for this connection
	slot_id		= httpd->cnx_slotpool.allocate(this);
	// start the tcp_full
	inet_err	= tcp_full->start(this, NULL);
	DBG_ASSERT( inet_err.succeed() );		// TODO poor error management
	// log to debug
	KLOG_DBG("create a httpd_t::cnx_t slot_id=" << slot_id
						<< " local=" << tcp_full->get_local_addr()
						<< " remote=" << tcp_full->get_remote_addr() );	
	// link the connection to the cnx_db
	httpd->cnx_dolink( this );
	
	// zero the sendfile_fd
	sendfile_fd	= NULL;
}

/** \brief Desstructor
 */
httpd_t::cnx_t::~cnx_t()			throw()
{
	// log to debug
	KLOG_DBG("Destroy a httpd_t::cnx_t slot_id=" << slot_id);
	// close a open sendfile if needed
	if( sendfile_inprogress() )	sendfile_close();
	
	// release the slot_id of this connection
	httpd->cnx_slotpool.release(slot_id);
	// close the full connection
	nipmem_delete	tcp_full;
	// unlink the connection to the cnx_db
	httpd->cnx_unlink( this );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   tcp full callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_full_t when to notify an event
 */
bool	httpd_t::cnx_t::neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( tcp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_CLOSED:	// autodelete
					nipmem_delete	this;
					return false;
	case tcp_event_t::RECVED_DATA:{	pkt_t *	pkt = tcp_event.get_recved_data();
					return handle_recved_data(*pkt);}
	case tcp_event_t::MAYSEND_ON:{	// if sendfile is NOT in progress, delete the socket
					if( !sendfile_inprogress() ){
						nipmem_delete	this;
						return false;
					}
					bool failed = sendfile_fill_sendbuf();
					if( failed ){
						nipmem_delete	this;
						return false;
					}
					return true;}
	default:	DBG_ASSERT(0);
	}	
	return true;
}

/** \brief Handler RECVED_DATA from the tcp_full_t
 */
bool	httpd_t::cnx_t::handle_recved_data(const pkt_t &pkt)	throw()
{
	httpd_err_t		httpd_err	= httpd_err_t::OK;
	httpd_request_t		request;
	// queue the received data to the one already received
	pending_queue << string_t::from_datum(pkt.to_datum(datum_t::NOCOPY));
	// if the pending_data are too long, close the connection
	if( pending_queue.str().length() > 5*1024 ){
		nipmem_delete	this;
		return false;
	}
	// some logging
	KLOG_DBG("recved data " << pending_queue.str().length() << "-byte");
	// if all the received data forms a http request, process it
	// - it may be incomplete in case of POST
	if( contain_http_request(pending_queue) == false )	return true;

	// parse the pending queue and try to build a request
	httpd_err = parse_request(pending_queue.str(), request);
	// special case if the request is a POST and some/all posted data didn't yet arrived
	if( request.is_null() && httpd_err == httpd_err_t::OK )	return true;

	// if there is no error, handle the request
	if( !httpd_err.is_error() )	httpd_err = process_request(request);

	// if the replied is delayed, return now but keep the connection open
 	if( httpd_err.get_value() == httpd_err_t::DELAYED_REPLY )	return true;

	// if the reply is available immediatly, do it immediatly
	handle_send_reply(request, httpd_err);
#if 1
	// TODO the comment below is likely due to a misunderstanding of the 'tokeep'
	// mechanism and a confusion with a congestion system.
	// - here it is clearly a tokeep that MUST be returned
	// - maybe there are some reason to stop the delivery system of the 
	//   socket tho...
	return true;
#else
 	// return false as no more data will be read from this connection
 	// - handle_send_reply() may even delete the connection
 	//   - TODO is this true ?!?!?! 
 	// - NOTE: in anycase all this closing connection is poorly coded at best
 	//   - currently it is up to the client to close the connection
 	return false;
#endif
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            Utility functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief read a line from a http header aka a normal line but ended with \r\n
 * 
 * - TODO may be replace by some std::string.find()
 * - the whole stringstream should be replaced by a std::string as in http_client_t
 */
std::string httpd_t::cnx_t::read_httpline(std::stringstream &ss)	throw()
{
	std::string	line = "";
	if( std::getline(ss, line,'\n') ){
		// check if the last charaterer is a \r
		if( line.substr(line.length()-1, 1) != "\r" )	return "";
		// if the last charaterer is a \r, remove it
		line.erase( line.length()-1, line.length() );
	}
	return line;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     client HTTP request handling
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return true if a request has been received (aka bunch of lines followed by CRLF CRLF)
 * 
 * - see rfc2616.4.1
 */
bool httpd_t::cnx_t::contain_http_request(const std::stringstream &ss) throw()
{
	// if the string contains the trailler, return true
	if( ss.str().rfind("\r\n\r\n") != std::string::npos )	return true;
	// else return false
	return false;
}

/** \brief parse a input stream into a httpd_request_t
 * 
 * @return a non null httpd_request_t if no error occured, otherwise a null one
 */
httpd_err_t httpd_t::cnx_t::parse_request(const std::string &str, httpd_request_t &request_out)	throw()
{
	std::stringstream		ss(str);
	property_t			uri_variable;
	property_t			req_header;
	datum_t				posted_data;
	std::string			line;
	std::vector<std::string>	words, words2;

	// zero the request_out
	request_out	= httpd_request_t();
	// read the first line
	line = read_httpline(ss);
	if( line.empty() )		return httpd_err_t::BOGUS_REQUEST;

	// parse the first line
	words	= string_t::split( line, " ");
	if( words.size() < 3 )		return httpd_err_t::BOGUS_REQUEST;
	
	// get the url_method from the first line
	std::string req_method	= words[0];
	// get the request_uri from the first line
	words2	= string_t::split(words[1], "?", 2);
	// get the path from the request_uri
	std::string	uri_path	= words2[0];
	// if there is some variable in the uri, parse them
	if( words2.size() > 1 ){
		// parse the variables
		std::vector<std::string>	var = string_t::split( words2[1], "&" );
		// populate the url_var
		for( size_t i = 0; i < var.size(); i++ ){
			std::vector<std::string> name_value = string_t::split( var[i], "=", 2 );
			if(name_value.size() == 2){
				uri_variable.insert_string( name_value[0], name_value[1] );
			}else{
				uri_variable.insert_string( name_value[0], "true" );
			}
		}
	}
	
	// parse all the request header lines
	while( (line = read_httpline(ss)).empty() == false ){
		words = string_t::split( line, ":", 2);
		req_header.insert_string( words[0], string_t::strip(words[1]) );
	}
	
	// in cae of POST method, read the posted data
	if( req_method == "POST" ){
		// sanity check - the "content-length" field MUST exists
		DBG_ASSERT( req_header.exist("Content-Length") );
		// read the content length from the header
		size_t	content_len	= atoi( req_header.find_string("Content-Length").c_str() );
		std::ostringstream	content_oss;
		// TODO it would be nice to put some size limitation  :)
		// - else there is a DOS which allow anybody to hog the memory of host running httpd_t
		size_t	nb_read_byte;
		// read byte from the stringstream until either it end or reach content_len
		for( nb_read_byte = 0; ss.good() && nb_read_byte < content_len; nb_read_byte++ ){
			content_oss << (char)ss.get();
		}
		// if the number of read_byte isnt enougth, considere this request as incomplete
		if( nb_read_byte != content_len )	return httpd_err_t::OK;
		// copy the read data as datum to the kad_request_t::posted_data;
		posted_data	= datum_t(content_oss.str().c_str(), content_oss.str().size());
		// log to debug
		KLOG_DBG("posted_data=" << posted_data);
	}
	
	// build the request_out
	request_out = httpd_request_t(req_method, uri_path, uri_variable, req_header
						, posted_data, slot_id, tcp_full);

	// return no error;
	return httpd_err_t::OK;
}

/** \brief handle a request
 */
httpd_err_t httpd_t::cnx_t::process_request(httpd_request_t &request) throw()
{
	// log to debug
	KLOG_DBG("path=" << request.get_path() << " remote_addr=" << tcp_full->get_remote_addr());
	// find the proper handler
	httpd_handler_t *	handler	= httpd->handler_find(request.get_path());
	// if no handler has been found, return not found
	if( handler == NULL )		return httpd_err_t::NOT_FOUND;
	// log to debug
	KLOG_DBG("handler for path=" << request.get_path() );
	// call the found handler
	return handler->notify(request);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             reply handling
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Handle sending a reply to the request received on this connection
 * 
 * - It may happen just after the request reception or later by using httpd_err_t::DELAYED_REPLY
 */
void	httpd_t::cnx_t::handle_send_reply(const httpd_request_t &request, const httpd_err_t &httpd_err)
										throw()
{
	// log to debug
	KLOG_DBG("request for " << request.get_path() << " reported " << httpd_err );

	// send the reply
	if( httpd_err == httpd_err_t::SEND_FILE ){
		bool	failed = sendfile_start(request, httpd_err);
		if( failed )	nipmem_delete this;
		return;
	}else if( httpd_err.is_error() ){
		reply_error(request, httpd_err);
	}else{
		reply_data(request, httpd_err);
	}

#if 1
	// ask to be notified when the sendbuf is empty
	// - lame way to do a linger
	inet_err_t	inet_err;
	inet_err = tcp_full->sendbuf_set_max_len(tcp_full->sendbuf_get_used_len());
	DBG_ASSERT( inet_err.succeed() );
	inet_err = tcp_full->maysend_set_threshold(tcp_full->sendbuf_get_max_len());
	DBG_ASSERT( inet_err.succeed() );
#else
	// delete the connection
	nipmem_delete	this;
	// TODO here the connection MUST be closed
	// - but the delete will not let the time to actually transmit the data
	//   - it comes directly from the tcp_full to have its own window
	// - here the choise is to handle a linger in the neoip_socket_full
#endif
}

/** \brief send a data reply to this client
 */
void httpd_t::cnx_t::reply_data(const httpd_request_t &request, const httpd_err_t &httpd_err)	throw()
{
	const std::ostringstream &	reply		= request.get_reply();
	datum_t				content_body	= datum_t(reply.str().c_str(), reply.str().size());
	std::ostringstream		oss;
	std::string			content_encoding;

	// test if the gzip encoding is accepted by the client
	std::string	accept_encoding	= request.get_header("Accept-Encoding");
	std::vector<std::string> words	= string_t::split(accept_encoding, ",");
	for( size_t i = 0; i < words.size(); i++ )	words[i] = string_t::strip(words[i]);
	// NOTE: here http deflate is zlib, they missed the name in the spec
	//       - gzip is really gzip
	std::vector<std::string>::iterator	iter = std::find(words.begin(), words.end(), "gzip");
	// try to encode in zlib IFF the client support it and the reply is "text/html"
	if( iter != words.end() && request.get_reply_mimetype() == "text/html" ){
		compress_t	compress("gzip");
		datum_t		compressed	= compress.compress(content_body, content_body.get_len());
		if( !compressed.is_null() ){
			KLOG_DBG("size orig=" << content_body.get_len() << "  compressed=" << compressed.get_len());
			content_body		= compressed;			
			content_encoding	= "gzip";
		}
	}

	// put the http header
	oss << build_http_header(request, httpd_err, content_body.get_len(), content_encoding);
	
	// put the replied data
	oss << string_t::from_datum(content_body);
	// send the built reply
	tcp_full->send( oss.str().c_str(), oss.str().size() );
	// TODO if the write fails, close the socket
}

/** \brief send a error reply to this client
 */
void httpd_t::cnx_t::reply_error(const httpd_request_t &request, const httpd_err_t &httpd_err)	throw()
{
	std::ostringstream	oss;
	// put the http header
	oss << build_http_header(request, httpd_err, oss.str().size());
	// put some text
	// - TODO: why reporting an http error should write a text ?
	oss << "Request for path " << request.get_path() << " reported the error " << httpd_err;
	// send the built reply
	tcp_full->send( oss.str().c_str(), oss.str().size() );
	// TODO if the write fails, close the socket
}

/** \brief return a string containing a http header
 */
std::string	httpd_t::cnx_t::build_http_header(const httpd_request_t &request
						, const httpd_err_t &httpd_err
						, const size_t content_length
						, const std::string &content_encoding)	const throw()
{
	std::ostringstream	oss;
	// build the http header
	if( httpd_err == httpd_err_t::OK ){
		oss << "HTTP/1.1 200 OK\r\n";
	}else if( httpd_err == httpd_err_t::MOVED_TEMP ){
		oss << "HTTP/1.1 302 temporarily moved\r\n";
		// if the httpd_err is MOVED_TEMP, its reason contains the url where it has been moved
		oss << "Location: " << httpd_err.get_reason() << "\r\n";
	}else if( httpd_err == httpd_err_t::UNAUTHORIZED ){
		oss << "HTTP/1.1 401 Authentication Required\r\n";
		// if the httpd_err is UNAUTHORIZED, its reason contains the www-authenticate reply header
		oss << "WWW-Authenticate: " << httpd_err.get_reason() << "\r\n";
	}else{	// else report a http error
		oss << "HTTP/1.1 404 error\r\n";
	}
	// write the rest of the http header
	oss << "Content-Length: "	<< content_length << "\r\n";
	oss << "Connection: close\r\n";
	oss << "Content-type: "		<< request.get_reply_mimetype() << "\r\n";
	if( !content_encoding.empty() )	oss << "Content-encoding: " << content_encoding << "\r\n";
//	oss << "Cache-Control: no-store, no-cache, must-revalidate\r\n";
//	oss << "Cache-Control: post-check=0, pre-check=0\r\n";
//	oss << "Pragma: no-cache\r\n";
	oss << "\r\n";
	return oss.str();	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         SEND_FILE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief send a sendfile reply to this client
 * 
 * @return true on error, false otherwise
 */
bool httpd_t::cnx_t::sendfile_start(const httpd_request_t &request, const httpd_err_t &httpd_err)	throw()
{
	file_path_t	file_path	= httpd_err.get_reason();
	file_stat_t	file_stat(file_path);
	std::ostringstream		oss;

	KLOG_ERR("reply_sendfile file_path=" << file_path);

	// if the file can't be stat() return an error now
	if( file_stat.is_null() )	return true;

	DBG_ASSERT( httpd_err == httpd_err_t::SEND_FILE );

	// set the tcp sendbuf length	
	inet_err_t	inet_err = tcp_full->sendbuf_set_max_len( 10*1024 );
	DBG_ASSERT( inet_err.succeed() );

	// build the http header
	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Content-Type: "		<< request.get_reply_mimetype()	<< "\r\n";
	oss << "Content-Length: "	<< file_stat.get_size()		<< "\r\n";
	oss << "Connection: close\r\n";
	oss << "Cache-Control: max-age=300\r\n";
	oss << "\r\n";
	KLOG_ERR("the whole httpd header=" << oss.str() );
	// send the header thru the connection
	tcp_full->send( oss.str().c_str(), oss.str().size() );


	// open the sendfile_fd
	sendfile_fd	= fopen( file_path.to_string().c_str(), "rb" );
	if( sendfile_fd == NULL ){
		KLOG_ERR("Cant open httpd_t sendfile " << file_path << " due to " << neoip_strerror(errno));
		return true;
	}

	// fill connection sendbuf with the file
	return sendfile_fill_sendbuf();
}

/** \brief return true if a sendfile is in progress, false otherwise
 */
bool	httpd_t::cnx_t::sendfile_inprogress()					const throw()
{
	if( !sendfile_fd )	return false;
	return true;
}

/** \brief return true if a sendfile is in progress, false otherwise
 */
void	httpd_t::cnx_t::sendfile_close()					throw()
{
	// sanity check - a sendfile MUST be in progress
	DBG_ASSERT( sendfile_inprogress() );
	// close the file and mark it unused
	fclose( sendfile_fd );
	sendfile_fd	= NULL;
}


/** \brief fill the connection sendbuf with content from sendfile_fd
 * 
 * @return true on error, false otherwise
 */
bool	httpd_t::cnx_t::sendfile_fill_sendbuf()		throw()
{
	size_t		data_len	= tcp_full->sendbuf_get_free_len();
	void *		data_ptr	= nipmem_alloca(data_len);
	inet_err_t	inet_err;
	size_t		read_len;

	// log to debug
	KLOG_DBG("http sendfile will try to fill the sendbuf with a freelen=" << data_len);

	// sanity check - the sendfile_fd MUST be open
	DBG_ASSERT( sendfile_fd );
	DBG_ASSERT( sendfile_inprogress() );

	// read data from the file
	read_len = fread( data_ptr, 1, data_len, sendfile_fd );

	// send the data to the connection
	ssize_t	written_len = tcp_full->send( data_ptr, read_len );
	DBG_ASSERT( written_len == (ssize_t)read_len );

	// log to debug
	KLOG_DBG("http sendfile has sent " << read_len << " more data");
	
	
	// setup the maysend limit
	// - if the sendbuf_free_len is not empty at this point, the sendfile is completed
	// - else the sendfile is going on
	if( tcp_full->sendbuf_get_free_len() ){
		// log to debug
		KLOG_DBG("http sendfile is completed");
		// close the file and mark it unused
		sendfile_close();
		// set the maysend threshold to be notified when the sendbuf is empty
		inet_err = tcp_full->maysend_set_threshold( tcp_full->sendbuf_get_max_len() );
		DBG_ASSERT( inet_err.succeed() );
	}else{
		// log to debug
		KLOG_DBG("http sendfile is going on");
		// set the maysend threshold to be notified when the sendbuf is half empty
		inet_err = tcp_full->maysend_set_threshold( tcp_full->sendbuf_get_max_len()/2 );
		DBG_ASSERT( inet_err.succeed() );
	}

	// return no error
	return false;
}


NEOIP_NAMESPACE_END









