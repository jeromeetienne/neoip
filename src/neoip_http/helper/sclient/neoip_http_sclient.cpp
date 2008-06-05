/*! \file
    \brief Class to handle the http_sclient_t

\par TODO to refactor the error handling and message length
- so it is about permanent or transient error
  - put some method in http_sclient_res_t to test the permanent/transient status
  - has_err()/has_permanent_err()/has_transient_err() ?
- ok what are the various cases occuring in the field and how to handle them
- how to test the various error in the http header ?
  - e.g. a given file is requested and a NotFound is returned ?
  - currently there are stuff to handle this case as method in the http_sclient_res_t
  - i think the location is the good one
- how to organize the refactoring ?
  - do i copy the source, modify and then backport ?
  - do i refactor in place ?

\par TODO include a cnxerr_retry
- if the http server close the connection immediatly, as many do in case of 
  overload, retry connecting with a randomized exponantial backoff timer
  and with a maximum amount of retry
- NOTE: it has been done in upnp_call_t because the case happened for upnp router


\par About the http_sclient_mod_vapi_t
- this is used to alter the default behaviour of the http_sclient_t
- http_sclient_mod_raw_t is the default behaviour one might expect from a http client.
- http_sclient_mod_flv_t is a trick to be able to perform simulate http range-request 
  but using only flv range-request.
  - see neoip_oload_mod_flv.cpp for details about what is a 'flv range-request' 
  - http://www.flashcomguru.com/index.cfm/2005/11/2/Streaming-flv-video-via-PHP-take-two
    for the original description of the technic
  - it allows to transparently do http request on server which only support flv range-request 
- the http_sclient_mod_vapi_t to use is specifies in the uri via 'metavar'
  - called the metavar_db "all the one which start with "neoip_metavar_" prefix
  - http_mod_type=flv|raw with raw by default
  - http_mod_flv_varname="whatever" with "pos" by default 
    - this is valid IIF mod_type="flv"
- the uri_metavar_db is extracted on the start() uri
  - aka put in the uri_metavar_db and removed from the uri before using it to http_client_t
  

\par About progress_chunk notification
http_sclient_t include a progress indicator. It is disable by default. It is 
enabled by setting http_sclient_profile_t::progress_chunk_len() to the length
of the chunk to notify (0 meaning no notification)
- NOTE: it may be produce unexpected result if the body is "transfer-encoding" = "chunked"
  because it is notified before the data got 'unchunked'

\par About the error reporting
http_sclient_t does not interpret the http_rephd_t (except to get the content-length)
so if the http_rephd_t report an error at the http level, http_sclient_t wont
know it, it is up to the caller to test it via the proper method in the 
http_sclient_res_t object.
- http_sclient_res_t::cnx_err() is not OK IIF there is a trouble at the connection level
- http_sclient_res_t::http_rephd_t() is equal to the value which have been reported
  by the server. it may be null if an error occured before it has been received
- http_sclient_res_t::reply_body() is equal to the value which add when it has
  been notified. 
  - it doesnt implies it is completed. it is up to the http_sclient_res_t to ensure 
    that.

\par About reply message body length
- see rfc2616.4.4 "Message Length", there are 3 cases:
  1. if the http_rephd_t is "transfer-encoding" = "chunked" - rfc2616.14.41
     - if so, the reply length is determined by chunked format (until the last chunk is read)
     - the name of this one is 'msglen_chunkedenc'
  2. if the http_rephd_t has a "Content-Length" - rfc2616.14.13
     - if the header contains the exact length of content and the data MUST be read
       as many byte as indicated in the header
     - the name of this one is 'msglen_contentlen'
  3. none of the above
     - if so, the message body is read until the connection close
     - the name of this one is 'msglen_lenonclose'
     
\par About http_sclient_profile_t::reply_maxlen()
- it is possible to bound the possible reply length in order to avoid DOS
  - aka the http_sclient_t taking huge amount of memory
- about http_sclient_profile_t::reply_maxlen() and chunked transfer-encoding
  - if the maxlen = decoded data it is more predictable and easier to handle for the caller
  - but the chunked encoded data may be much larger than the decoded version depending
    on the various chunk size...
  - as it is more a DOS prevention to avoid large amount of ram for no reason, it is
    tested predecoding

\par About the error handling
- well this is shitty at best currently
- what is needed:
  1. determine if the request succeed
  2. if it fails, what kind of error ? a network error or a http error
     - in http, it is quite common to have the server not answering socket because it is overloaded
     - so this happen in normal case (so one may retry shortly after)
     - an http error is more permanent (aka quite unlikely to be solved in a near future)
     - in fact the stuff is more, is this a permanent error or a transient one
- how to handle it ?
  - with the content-length
  - with the chunked transfer-encoding
  - when there is nothing
    - e.g. some server doesnt set any reply header
- the http_client_pool_t may be used IIF content-length or chunked transfer-encoding
- else the data are read until the socket close

- ok 3 case to report a successfull request
  - chunked transfert encoding is set, and is fully_recved()
  - when content-length is set, and the recved_data.size() is equal to that
  - when the socket close
  
- if the http_client failed during the connection too ?

*/

/* system include */
/* local include */
#include "neoip_http_sclient.hpp"
#include "neoip_http_sclient_mod_vapi.hpp"
#include "neoip_http_sclient_mod_type.hpp"
#include "neoip_http_sclient_mod_raw.hpp"
#include "neoip_http_sclient_mod_flv.hpp"
#include "neoip_http_client.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_http_client_pool_cnx.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_base_oswarp.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


// definition of \ref http_sclient_t constant
// TODO find a better string for that
const std::string	http_sclient_t::METAVAR_PREFIX	= "neoip_metavar_";
// end of constants definition


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
http_sclient_t::http_sclient_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	http_client	= nipmem_new http_client_t();
	socket_full	= NULL;
	client_pool	= NULL;
	// set some default variable - later only one of the 3 may be set
	msglen_onclosecnx	= false;
	msglen_contentlen	= false;
	msglen_chunkedenc	= false;
}

/** \brief Destructor
 */
http_sclient_t::~http_sclient_t()		throw()
{
	// log to debug
	KLOG_DBG("enter socket_full=" << socket_full);
	// delete hte http_sclient_mod_vapi_t if needed
	nipmem_zdelete	m_mod_vapi;
	// delete the http_client_t if needed
	nipmem_zdelete	http_client;
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
http_sclient_t &	http_sclient_t::set_profile(const http_sclient_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == http_err_t::OK );
	// copy the profile
	this->profile	= profile;
	// set the http_client_profile_t in the http_client_t
	http_client->set_profile(profile.http_client());
	// return the object iself
	return *this;
}

/** \brief Set the socket_itor_arg_t for this object
 */
http_sclient_t &	http_sclient_t::set_itor_arg(const socket_itor_arg_t &itor_arg)	throw()
{
	// set the socket_itor_arg_t in the http_client_t
	http_client->set_itor_arg(itor_arg);
	// return the object iself
	return *this;
}

/** \brief Set the http_client_pool_t for this object
 */
http_sclient_t &	http_sclient_t::set_client_pool(http_client_pool_t *client_pool)	throw()
{
	// copy the parameter
	this->client_pool	= client_pool;
	// set the http_client_pool_t in the http_client_t
	http_client->set_client_pool(client_pool);
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
http_err_t	http_sclient_t::start(const http_reqhd_t &p_http_reqhd, http_sclient_cb_t *callback
					, void *userptr, const datum_t &data2post)	throw()
{
	http_err_t	http_err;
	// log to debug
	KLOG_DBG("enter http_reqhd=" << p_http_reqhd);
	KLOG_DBG("enter data2post=" << data2post.to_stdstring() );
	// sanity check - if data2post IS NOT null, the method MUST be POST 
	if( !data2post.is_null() ) DBG_ASSERT( p_http_reqhd.method() == http_method_t::POST );

	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	this->data2post	= data2post;

	// *copy* http_reqhd - important to copy in order to allow mod_vapi to modify it 
	http_reqhd_t	http_reqhd	= p_http_reqhd;

	// extract the metavar_db from the http_reqhd - NOTE: this modify http_reqhd_t 
	metavar_db_extract(&http_reqhd);

	// handle "http_useragent_mimic" in m_metavar_db - useragent in http rfc2616.14.43
	// - if m_metavar_db has the variable, and the useragent is not specified in http_reqhd
	std::string	useragent_mimic	= m_metavar_db.get_first_value("http_useragent_mimic");
	if( !http_reqhd.header_db().contain_key("User-Agent") && !useragent_mimic.empty() ){
		// log to debug
		KLOG_DBG("useragent_mimic=" << useragent_mimic);
		// handle all the alias
		// - TODO to code currently this is a stub.. with a silly example :)
		// - see how ruby mechanize handle it
		if( useragent_mimic == "firefox" ){
			useragent_mimic	= "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.6) Gecko/20061201 Firefox/2.0.0.6 (Ubuntu-feisty)";
		}
		// put the useragent_mimic as "User-Agent" fields
		http_reqhd.header_db().append("User-Agent", useragent_mimic);
	}

	
	// copy the original http_reqhd in the http_sclient_res_t
	// - this MUST be done before the metavar 
	m_current_res.http_reqhd(http_reqhd);
	
	// init the http_sclient_mod_vapi_t depending on the m_metavar_db
	http_sclient_mod_type_t	mod_type= m_metavar_db.get_first_value("http_mod_type", "raw");
	if( mod_type == http_sclient_mod_type_t::RAW ){
		http_sclient_mod_raw_t * mod_raw;
		mod_raw		= nipmem_new http_sclient_mod_raw_t();
		m_mod_vapi	= mod_raw;
		http_err	= mod_raw->start(this);
		if( http_err.failed() )	return http_err;
	}else if( mod_type == http_sclient_mod_type_t::FLV ){
		http_sclient_mod_flv_t * mod_flv;
		mod_flv		= nipmem_new http_sclient_mod_flv_t();
		m_mod_vapi	= mod_flv;
		http_err	= mod_flv->start(this);
		if( http_err.failed() )	return http_err;
	}else{
		return http_err_t(http_err_t::ERROR, "Invalid " + METAVAR_PREFIX + " mod_type");
	}

	// call the reqhd_caller2server in mod_vapi
	http_err	= m_mod_vapi->reqhd_caller2server(&http_reqhd);
	if( http_err.failed() )	return http_err;

	// start the http_client_t
	http_err	= http_client->start(http_reqhd, this, NULL);
	if( http_err.failed() )	return http_err;

	// return no error
	return http_err_t::OK;
}

/** \brief Start the operation (just an helper with only a http_uri_t as http_reqhd_t)
 */
http_err_t	http_sclient_t::start(const http_uri_t &http_uri, http_sclient_cb_t *callback
					, void *userptr, const datum_t &data2post)	throw()
{
	// build a standard http_rephd_t for this http_uri_t
	http_reqhd_t	http_reqhd	= http_reqhd_t().uri(http_uri);
	// do the real start()
	return start(http_reqhd, callback, userptr, data2post);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Extract the metavar_db from the http_reqhd_t::uri() and remove them from it
 */ 
void	http_sclient_t::metavar_db_extract(http_reqhd_t *http_reqhd)	throw()
{
	const strvar_db_t &	var_uri	= http_reqhd->uri().var();
	// sanity check - m_metavar_db MUST be empty
	DBG_ASSERT( m_metavar_db.empty() );
	
	// go thru all the variables of this http_uri
	strvar_db_t	kept_var;
	for(size_t i = 0; i < var_uri.size(); i++){
		const strvar_item_t & var_item	= http_reqhd->uri().var()[i]; 
		// if this var_item DOES NOT start with METAVAR_PREFIX, goto the next
		if( var_item.key().find(METAVAR_PREFIX) != 0 ){
			kept_var	+= var_item;
			continue;
		}
		// put this var_item into m_metavar_db - after removing prefix from key string
		std::string	new_key	= var_item.key().substr(METAVAR_PREFIX.size());
		m_metavar_db.append(new_key, var_item.val()); 
	}
	
	// copy back the kept_var into the http_reqhd_t
	http_reqhd->uri().var()	= kept_var;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   forward_cnx_to_pool
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Release back this connection to the destination http_client_pool_t
 * 
 * - If the client_pool is set *within* the https_client_t, the connection is passed
 *   to pool *before* notifying the result.
 * - else the notified caller may use this function to pass it to the pool of his
 *   choice.
 * - in any case after a forward_cnx_to_pool(), the https_client_t MUST be delete as
 *   it is no more usable.
 * - NOTE: this function doesnt use the local http_client_pool_t by default
 * 
 * @return a tokeep for the socket_full_t
 */
bool	http_sclient_t::forward_cnx_to_pool(http_client_pool_t *dest_pool)	throw()
{
	http_client_pool_cnx_t *pool_cnx;
	http_err_t		http_err;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the socket_full MUST be set
	DBG_ASSERT( socket_full );
	// sanity check - the msglen_contentlen or msglen_chunkedenc MUST be true
	DBG_ASSERT( msglen_contentlen || msglen_chunkedenc );
	DBG_ASSERT( !msglen_onclosecnx );

	// backup the object_slotid of the socket_full_t - to be able to return its tokeep value
	slot_id_t	socket_full_slotid	= socket_full->get_object_slotid();
	socket_full_t *	stolen_socket_full;

	// steal the socket_full
	stolen_socket_full	= socket_full;
	// set the callback to NULL
	// - protective measure to ensure it wont call this object without noticing
	stolen_socket_full->set_callback(NULL, NULL);
	// mark socket_full as unused
	socket_full	= NULL;

	// create the http_client_pool_cnx_t
	pool_cnx	= nipmem_new http_client_pool_cnx_t();
	http_err	= pool_cnx->start(dest_pool, stolen_socket_full, pool_current_uri
								, pool_handled_uri_db);
	// handle the error case
	if( http_err.failed() )	nipmem_delete	pool_cnx;

	// if the socket_full_t has no been deleted, so 'tokeep' else return 'dontkeep'
	// - it may happen when the socket_full_t is deleted by the caller
	return object_slotid_tokeep(socket_full_slotid);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    http_client_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t when to notify an event
 */
bool	http_sclient_t::neoip_http_client_cb(void *cb_userptr, http_client_t &cb_http_client
			, const http_err_t &cb_http_err, const http_rephd_t &server_rephd
			, socket_full_t *socket_full, const bytearray_t &recved_data) throw()
{
	// log to debug
	KLOG_DBG("enter http_err=" << cb_http_err	<< " server_rephd=" << server_rephd
					<< "recved_data=" << recved_data.length());


	// build the current_res from the replied parameter
	this->socket_full	= socket_full;
#if 0
	// TODO about mod_vapi
	// TODO why do i copy the http_reqhd back ??!?!?
	// - is it in relation with redirection ?
	// - in anycase this breaks the assumption that the http_reqhd is the same before 
	//   after the request...
	// - if i want to support this rewrite... i have to write a reqhd_server2caller
	// - but do i want to ? the http_reqhd of current_res should be the one passed
	//   by the caller... 
	//   - i like it this way. this is the 'natural' instinctive way
	//   - to get it modified due to some http internal (redirection) is 'exporting
	//     internal' to the caller
	//   - doesnt seems needed and confusing... 
	// - ok set it to NO and see what happen
	m_current_res.http_reqhd	( http_client->get_http_reqhd() );
#endif

	// convert the server_rephd into a caller_rephd thru mod_vapi
	http_rephd_t	caller_rephd	= server_rephd;
	http_err_t	http_err;
	http_err	= m_mod_vapi->rephd_server2caller(&caller_rephd, m_current_res.http_reqhd() );
	if( http_err.failed() )		return notify_callback_short(http_err);
	// copy the caller_rephd in m_current_res - m_current_res contains only caller hd
	m_current_res.http_rephd	( caller_rephd );
	// NOTE: - the m_current_res.reply_body is assigned later to allow bytearray_t preallocation

	// if the connection failed, notify the caller
	if( cb_http_err.failed() )	return notify_callback_short(cb_http_err);
	
	// if the http_reqhd_t::method() is http_method_t::HEAD, return success now
	// - no need to continue in order to get the reply body
	if( m_current_res.http_reqhd().method() == http_method_t::HEAD )
		return notify_callback_short(http_err_t::OK);

	// set all msglen_* state to false - just to be sure
	msglen_onclosecnx	= false;
	msglen_contentlen	= false;
	msglen_chunkedenc	= false;
	// Set the msglen_* state to determine how the message length is known
	if( server_rephd.is_chunked_encoded() ){
		msglen_chunkedenc	= true;		
	}else if( !server_rephd.content_length().is_null() ){
		msglen_contentlen	= true;
		// cache the "Content-Length" for later use - get it thru mod_vapi
		content_length		= m_mod_vapi->contentlen_server2internal(server_rephd
							, m_current_res.http_reqhd() );
		// if the "Content-Length" field is invalid, notify an error
		if( content_length.is_null() || !content_length.is_size_t_ok() ){
			std::string	reason = "Unable to parse Content-Length http reply header";
			return notify_callback_short(http_err_t(http_err_t::ERROR, reason));
		}
		// if "Content-Length" is > than profile.reply_maxlen, notify an error
		if( profile.reply_maxlen() && content_length.to_size_t() > profile.reply_maxlen() ){
			std::string	reason = "replied content_length "+ content_length.to_string()
						+ " larger than profile value("
						+ OSTREAMSTR(profile.reply_maxlen()) +")";
			return notify_callback_short(http_err_t(http_err_t::ERROR, reason));
		}
	}else{
		msglen_onclosecnx	= true;
	}

	// preallocate m_current_res.reply_body - to avoid many reallocation while receiving data
	// - TODO this is a very dirty way to do it 
	//   - but currently my brain is off and this seems to work so i leave it
	//   - would require a bytearray_t modification to do it cleaner i guess
	m_current_res.reply_body()	= bytearray_t();
	if( msglen_contentlen ){
		// preallocate the reply_body in the http_rephd_t has a "Content-Length"
		m_current_res.reply_body().tail_alloc(content_length.to_size_t());
		m_current_res.reply_body().tail_free(content_length.to_size_t());
	}else{	// as the length is not known at this point, preallocate with the default len from profile
		m_current_res.reply_body().tail_alloc(profile.dfl_msglen_prealloc());
		m_current_res.reply_body().tail_free(profile.dfl_msglen_prealloc());
	}

	// copy the already recved_data in the reply_body
	m_current_res.reply_body().append(recved_data.void_ptr(), recved_data.length());

	// backup the parameters of this http_client_t parameter in case of forward to http_client_pool_t
	pool_handled_uri_db	= http_client->get_handled_uri_db();
	pool_current_uri	= http_client->get_current_uri();
	
	// delete the http_client_t
	nipmem_zdelete	http_client;

	// setup the socket_full_t - it is already started by http_client_t
	socket_full->set_callback(this, NULL);

	// start the parsing of the answer
	// - tokeep is ignored on purpose as it return false immediatly after
	pkt_t	tmp;		// TODO this tmp is shit - i dont understand why it is needed
	handle_recved_data(tmp);

	// return dontkeep
	return false;
}

/** \brief Callback used by http_client_t to get the data to post
 */ 
datum_t		http_sclient_t::neoip_http_client_data2post_cb(void *cb_userptr, http_client_t &cb_http_client
						, size_t offset, size_t maxlen)	throw()
{
	const http_reqhd_t &	http_reqhd	= http_client->get_http_reqhd();
	// sanity check - this callback MUST NOT be used if http_reqhd().method() is NOT "POST"
	DBG_ASSERT( http_reqhd.method() == "POST" );	
	// sanity check - the offset MUST be <= the data2post length
	DBG_ASSERT( offset <= data2post.length() );
	// compute the chunk length
	size_t	chunk_len	= data2post.length() - offset;
	chunk_len		= std::min(chunk_len, maxlen);
	// return the chunk itself
	return datum_t(data2post.char_ptr() + offset, chunk_len, datum_t::NOCOPY);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t to provide event
 */
bool	http_sclient_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );
	// sanity check - the socket_full MUST be set
	DBG_ASSERT( socket_full );

	// handle the fatal events
	if( socket_event.is_fatal() ){
		// if the msglen is known on close connection, notify a success
		// - it is the normal end of the message body - see rfc2616.4.4.5
		if( msglen_onclosecnx ){
			// convert the reply_body to a caller view thru mod_vapi
			http_err_t	http_err;
			http_err	= m_mod_vapi->reply_body_server2caller(m_current_res.reply_body()
							, m_current_res.http_reqhd());
			if( http_err.failed() )	return notify_callback_short(http_err);
			// return no error
			return notify_callback_short(http_err_t::OK);
		}
		// else it is an error, notify
		std::string	reason = "socket reported fatal event " + socket_event.to_string();
		return notify_callback_short(http_err_t(http_err_t::ERROR, reason));
	}

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:
			return handle_recved_data(*socket_event.get_recved_data());
	default:	DBG_ASSERT( 0 );	
	}

	// return tokeep
	return true;
}

/** \brief Handle the socket_event_t::RECVED_DATA event
 * 
 * @return a tokeep for the socket_full_t
 */
bool	http_sclient_t::handle_recved_data(pkt_t &pkt)	throw()
{
	bytearray_t &	recved_data		= m_current_res.reply_body();
	bool		progress_chunk_on	= profile.progress_chunk_len() != 0;
	size_t		progress_chunk_len	= profile.progress_chunk_len();
	size_t		progress_chunk_old	= 0x4242;	// NOTE: just to remove a compiler warning
	size_t		progress_chunk_new;
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);
	
	// if progress_chunk is enable, set progress_chunk_old
	if( progress_chunk_on )	progress_chunk_old = recved_data.size() / progress_chunk_len;
	
	// queue the received data to the one already received
	recved_data.append(pkt.void_ptr(), pkt.length());

	// convert the reply_body to a caller view thru mod_vapi
	http_err_t	http_err;
	http_err	= m_mod_vapi->reply_body_server2caller(recved_data, m_current_res.http_reqhd());
	if( http_err.failed() )	return notify_callback_short(http_err);
	
	// if progress_chunk is enable, set progress_chunk_new
	if( progress_chunk_on )	progress_chunk_new = recved_data.size() / progress_chunk_len;

	// if progress_chunk is enable, and this is a new chunk, do the notification
	if( progress_chunk_on && progress_chunk_old != progress_chunk_new ){
		bool tokeep	= notify_callback_progress_chunk();
		if( !tokeep )	return false;
	}

	// check if the profile.reply_maxlen() is exceeded
	if( profile.reply_maxlen() && recved_data.size() > profile.reply_maxlen() ){
		std::string	reason = "reply larger than the maximum(" + OSTREAMSTR(profile.reply_maxlen()) + "-byte)";
		return notify_callback_short(http_err_t(http_err_t::ERROR, reason));
	}
	
	// test for the various case of completion depending on the msglen_* state
	if( msglen_chunkedenc ){
		// if the chunked encoded reply is not yet fully received, return true
		if( !chunkedenc_fully_recved() )	return true;
		// if it is fully received, do a decoding in place
		chunkedenc_decoding();
	}else if( msglen_contentlen ){
		// if the reply_body is not yet fully received, return true
		if( recved_data.size() < content_length.to_size_t() )	return true;
	}else{
		// if the message length is known onclosecnx, always return true
		// - it is up to the socket_full_cb_t to notify the completion on connection close
		DBG_ASSERT( msglen_onclosecnx );
		return true;
	}
	
	// NOTE: here the request is considered completed

	// if the client_pool is enabled, put this connection in it
	// - NOTE: it MUST NOT be done for msglen_onclosecnx but it's ok as this case dont reach this point
	if( client_pool ){
		bool socket_full_tokeep	= forward_cnx_to_pool(client_pool);
		// notify the caller - with tokeep ignored on purpose to use the socket_full_tokeep
		notify_callback_short(http_err_t::OK);
		// return the socket_full_tokeep
		return socket_full_tokeep;
	}

	// notify the caller
	return 	notify_callback_short(http_err_t::OK);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			chunked encoding handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the reply is fully_recved assuming the reply is chunked encoded
 * 
 * - it attempts to parse the whole chunked message body, it is fully received
 */
bool	http_sclient_t::chunkedenc_fully_recved()				const throw()
{
	char *	buf_ptr	= m_current_res.reply_body().char_ptr();
	size_t	buf_len	= m_current_res.reply_body().length();
	// sanity check - the http_rephd_t MUST be chunked_encoded
	DBG_ASSERT( m_current_res.http_rephd().is_chunked_encoded() );
	DBG_ASSERT( msglen_chunkedenc );
	// loop to go thru every chunk
 	while(true){
 		// get the chunk header length
 		size_t	headlen	= chunk_headlen(buf_ptr, buf_len);
 		// if no chunk header has been found, it is no fully received, return false
 		if( headlen == 0 )	return false;
 		// get the chunk data length
 		size_t	datalen	= chunk_datalen(buf_ptr, buf_len);
 		// if the datalen == 0, this is this last chunk, leave the loop
 		if( datalen == 0 )	break;
 		// update the pointer/length
 		buf_ptr	+= headlen + datalen + strlen("\r\n");
 		buf_len	-= headlen + datalen + strlen("\r\n");
 	}
 	
 	// NOTE: try to consume the entity header
 	
 	// goes back of 2 char "\r\n" - this allow to search for "\r\n\r\n" to consume the entity-header
 	// - this is ok to go back of 2 char as the buffer contain at least 1 chunk header which is 
 	//   always more than two charaters long.
	buf_ptr	-= strlen("\r\n");
	buf_len	+= strlen("\r\n");

	// if the pattern "\r\n\r\n" is not found in the remaining buffer, it is not fully received
	if( base_oswarp_t::memmem(buf_ptr, buf_len, "\r\n\r\n", strlen("\r\n\r\n")) == NULL )
		return false;

	// if all previous tests passed, it is fully received, return true
	return true;
}

/** \brief Decode the recved_data once a chunked encoded reply has been fully received
 */
void	http_sclient_t::chunkedenc_decoding()					throw()
{
	bytearray_t & msgbody	= m_current_res.reply_body();
	char *	src_ptr	= m_current_res.reply_body().char_ptr();
	size_t	src_len	= m_current_res.reply_body().length();
	char *	dst_ptr	= m_current_res.reply_body().char_ptr();
	// sanity check - the reply MUST be fully received
	DBG_ASSERT( chunkedenc_fully_recved() );
	
	// loop to go thru every chunk
	while(true){
 		// get the chunk header length
 		size_t	headlen	= chunk_headlen(src_ptr, src_len);
 		DBG_ASSERT( headlen > 0 );
 		// get the chunk data length
 		size_t	datalen	= chunk_datalen(src_ptr, src_len);
 		// log to debug
 		KLOG_DBG("headlen=" << headlen << " header datum=" << datum_t(src_ptr, headlen) << " datalen=" << datalen);
 		// if the datalen == 0, this is this last chunk, discard the remaining of the buffer
 		if( datalen == 0 ){
 			msgbody.tail_free(msgbody.length() - (dst_ptr - msgbody.char_ptr()));
 			return;
 		}
		// update the src_ptr/src_len with the headlen
		src_ptr	+= headlen;
		src_len -= headlen;
		// move the data to the destination 
		memmove(dst_ptr, src_ptr, datalen);
		// update the dst_ptr with the datalen
		dst_ptr	+= datalen;
		// update the src_ptr/src_len with the datalen
		src_ptr	+= datalen + strlen("\r\n");
		src_len -= datalen + strlen("\r\n");
	}
}

/** \brief Return the length of the chunk header at the begining of the buffer
 * 
 * - if no chunk header is found, return 0
 * - the returned length include the "\r\n"
 * - from rfc2516.3.6.1 a chunk header is "1*HEX[;extension]\r\n", so to get
 *   the chunk header means to go up to the first "\r\n"
 * - this function is in http_sclient_t as it is the only place doing http chunked
 *   encoding but it is generic and not specific to http_sclient_
 */
size_t	http_sclient_t::chunk_headlen(const char *buf_ptr, size_t buf_len)		const throw()
{
	// try to find "\r\n", at the begining of the buffer
	char *p	= (char *)base_oswarp_t::memmem(buf_ptr, buf_len, "\r\n", strlen("\r\n"));
	// if "\r\n" is not found, return 0
	if( p == NULL )	return 0;
	// else return its length
	return p - buf_ptr + strlen("\r\n"); 
}

/** \brief Return the length of the chunk data at the begining of the buffer
 * 
 * - from rfc2516.3.6.1 a chunk header is "1*HEX[;extension]\r\n", so to get
 *   the chunk datalen means to go convert the HEX part
 * - the buf_ptr MUST points to a valid chunk header
 */
size_t	http_sclient_t::chunk_datalen(const char *buf_ptr, size_t buf_len)		const throw()
{
	// sanity check - buf_ptr/buf_len MUST point to a valid chunk header
	DBG_ASSERT( chunk_headlen(buf_ptr, buf_len) != 0 );
	// return the length
	return strtoul(buf_ptr, NULL, 16);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Notify the caller with specifying only the error connection, the rest
 *         being taken from the object
 * 
 * - NOTE: this does a copy of the reply_body...
 */
bool	http_sclient_t::notify_callback_short(const http_err_t &cnx_err)	throw()
{
	// set the m_current_res.cnx_err()
	m_current_res.cnx_err(cnx_err);
	// notify the caller
	return notify_callback(m_current_res);
}

/** \brief notify the callback
 */
bool http_sclient_t::notify_callback(const http_sclient_res_t &sclient_res)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_http_sclient_cb(userptr, *this, sclient_res);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

/** \brief notify the callback but only for progress_chunk
 */
bool http_sclient_t::notify_callback_progress_chunk()				throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_http_sclient_progress_chunk_cb(userptr, *this);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





