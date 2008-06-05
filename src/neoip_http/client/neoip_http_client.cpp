/*! \file
    \brief Class to handle the http_client_t

\par Brief Description
\ref http_client_t establish a http connection from a http_reqhd_t and provide
a socket_full_t of this connection. It is up to the caller to receive the 
message body. 

\par About redirection
It automatically handles http redirection if the http_client_profile_t 
instructs it. It requires to have a automatic DNS resolution in order to 
resolve the target hostname of the redirection.
- NOTE: this is possible IFF socket_itor_arg_t is TCP and remote_addr() is 
  not present.

\par About the http_client_pool_t
It may uses the http_client_pool_t in order to reuse the same socket_full_t for
several http_client_t. It provides the 'persistent connection' feature from 
http/1.1. NOTE that several http_reqhd_t are not queued at the same time in 
the connection, increasing the latency..
- NOTE: there is some incompatibility with the socket_itor_arg_t mechanism
  - if the connection from the pool doesnt have the same socket_profile_t as
    the socket_itor_arg_t, it WILL NOT be detected and it will result in a silent BUG!
  - the same for local_addr()
  - the normal algo should be : ask the pool for a connection with this http_uri_t
    *AND* compatible with this socket_itor_arg_t
    - it is quite doable exect that profile cant be tested for equality... couch couch..
  - it is lame but not a actual problem now because http_client_t is used in practice
    only with always the same socket_profile_t 
- the fact to get a socket_full_t from the pool, force here not to use the 
  socket_client_t helper, but the socket_itor_t and socket_full_t.

\par About the socket_itor_arg_t
- http_client_t allow the caller to setup a socket_itor_arg_t
- if it is not provided, the socket_itor_arg_t default to a default from socket_domain_t::TCP
  with no remote_addr.
- if the socket_itor_arg_t doesnt contains a remote_addr() and if its domain is
  socket_domain_t::TCP, http_client_t will attempts to fill it up with this algo:
  - if http_uri_t::host() is an ip address, use it directly as remote_addr()
  - else try to do a host2ip_t on it, and use the result as remote_addr()
- it may creates a silent BUG with http_client_pool_t if the socket_itor_arg_t
  and the connection from the pool are not compatible. see details in http_client_pool_t
  stuff

*/

/* system include */
/* local include */
#include "neoip_http_client.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_http_client_pool.hpp"
#include "neoip_http_client_pool_cnx.hpp"
#include "neoip_host2ip.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_socket_itor.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"

// special include to handle socket for https
#include "neoip_socket_profile_stcp.hpp"
#include "neoip_slay_profile.hpp"
#include "neoip_slay_profile_tls.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
http_client_t::http_client_t()		throw()
{
	// zero some field
	host2ip		= NULL;
	socket_full	= NULL;
	socket_itor	= NULL;
	client_pool	= NULL;

	// set the default socket_itor_arg_t
	m_orig_itor_arg	= nipmem_new socket_itor_arg_t();
}

/** \brief Destructor
 */
http_client_t::~http_client_t()		throw()
{
	// delete the socket_itor_arg_t if needed
	nipmem_zdelete	m_orig_itor_arg;
	// delete the host2ip_t if needed
	nipmem_zdelete	host2ip;
	// delete the socket_itor_t if needed
	nipmem_zdelete	socket_itor;
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
http_client_t &	http_client_t::set_profile(const http_client_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == http_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Set the socket_itor_arg_t for this object
 * 
 * - WARNING: here the socket_itor_arg_t may not contains a remote_addr()
 *   - if there are no remote_addr, some attemps will be made to fill it up, see comment on top
 *   - so the socket_itor_arg_t may not be .is_valid()
 */
http_client_t &	http_client_t::set_itor_arg(const socket_itor_arg_t &p_itor_arg)	throw()
{
	// copy the itor_arg
	*m_orig_itor_arg	= p_itor_arg;	
	// return the object iself
	return *this;
}

/** \brief Set the http_client_pool_t for this object
 */
http_client_t &	http_client_t::set_client_pool(http_client_pool_t *client_pool)	throw()
{
	// copy the parameter
	this->client_pool	= client_pool;		
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
http_err_t	http_client_t::start(const http_reqhd_t &http_reqhd, http_client_cb_t *callback
							, void *userptr)	throw()
{
	// sanity check - the profile MUST be check() succeed
	DBG_ASSERT( profile.check().succeed() );
	// sanity check - if the client_pool is set, the http_method_t MUST be a GET
	// - current both are imcompatible
	if( client_pool )	DBG_ASSERT(http_reqhd.method().is_get());
	

	
	// copy the parameter
	this->http_reqhd	= http_reqhd;
	this->callback		= callback;
	this->userptr		= userptr;

	// if no socket_itor_arg_t has been set, set it now
	if( !m_orig_itor_arg->domain_present() ){
		switch(http_reqhd.uri().scheme().value()){
		case http_scheme_t::HTTP:
			m_orig_itor_arg->domain(socket_domain_t::TCP)
					.type(socket_type_t::STREAM)
					.profile(socket_profile_t("tcp"));
			break;
		case http_scheme_t::HTTPS:{
			socket_profile_t	socket_profile(socket_domain_t::STCP);
			// build a slay_profile_t for slay_domain_t::TLS
			slay_profile_t	slay_profile(slay_domain_t::TLS);
			slay_profile_tls_t &	profile_tls	= slay_profile_tls_t::from_slay(slay_profile);
			profile_tls.tls().authtype(tls_authtype_t::CERT);
			// set the slay_profile_t into the socket_profile_t
			socket_profile_stcp_t &	profile_dom	= socket_profile_stcp_t::from_socket(socket_profile);
			profile_dom.slay(slay_profile);
			// set the m_orig_itor_arg
			m_orig_itor_arg->profile(socket_profile).domain(socket_domain_t::STCP)
						.type(socket_type_t::STREAM);	

			break;}
		default:	DBG_ASSERT(0);
		}
	}

	// start the expire_timeout
	expire_timeout.start(profile.timeout_delay(), this, NULL);

	// launch the http_uri_t
	http_err_t	http_err;
	http_err	= launch_http_uri();
	if( http_err.failed() )	return http_err;

	// return no error
	return http_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch a new http_uri_t
 * 
 * - used in start() and for the http redirection
 */
http_err_t	http_client_t::launch_http_uri()	throw()
{
	const http_uri_t &	http_uri	= http_reqhd.uri();
	// log to debug
	KLOG_DBG("enter http_uri=" << http_uri);
	// insert the http_uri in the handled_uri_db - to detect infinit redirect loop latter
	bool	succeed = handled_uri_db.insert(http_uri).second;
	DBG_ASSERT( succeed );
	
	// test if this http_uri_t is in the http_client_pool_t
	if( client_pool ){
		http_client_pool_cnx_t * pool_cnx;
		pool_cnx	= client_pool->get_cnx_by_uri(http_uri);
		// if the http_uri_t is in the http_client_pool_t, use it
		if( pool_cnx )	return launch_by_pool_cnx(pool_cnx);
	}

	// if it is not allowed to do host2ip_t, try to launch the socket_client_t now
	if( !may_host2ip() )	return launch_socket_client(ip_addr_t());
	
	// if the http_uri_t host is not a ip address, launch a host2ip to resolve the hostname
	if( ip_addr_t(http_uri.host()).is_null() ){
		// launch a host2ip on the http_uri_t host
		inet_err_t	inet_err;
		host2ip		= nipmem_new host2ip_t();
		inet_err	= host2ip->start(http_uri.host(), this, NULL);
		if( inet_err.failed() )	return http_err_from_inet(inet_err);
		return http_err_t::OK;
	}
	
	// else launch the socket_client_t with a ipport_addr_t directly from the http_uri_t
	return launch_socket_client( ip_addr_t(http_uri.host()) );
}

/** \brief Launch the socket_client_t with the socket_itor_arg_t
 * 
 * @remote_ipaddr	ip_addr_t to put in socket_itor_arg_t IIF non null
 */
http_err_t	http_client_t::launch_socket_client(const ip_addr_t &remote_ipaddr)	throw()
{
	socket_itor_arg_t itor_arg	= *m_orig_itor_arg;
	// log to debug
	KLOG_DBG("remote_ipaddr=" << remote_ipaddr);
	// sanity check - the socket_itor_t and socket_full_t MUST be NULL
	DBG_ASSERT( !socket_itor && !socket_full );
	// if there is a remote_ipaddr to replace, set it in itor_arg
	// - TODO real real ugly
	if( !remote_ipaddr.is_null() ){
		DBG_ASSERT( may_host2ip() );
		const http_uri_t & http_uri	= http_reqhd.uri();
		std::string	domain_str;
		if( http_uri.scheme().is_http() )	domain_str = "tcp://";
		else if( http_uri.scheme().is_https() )	domain_str = "stcp://";
		else					DBG_ASSERT( 0 );	
		itor_arg.remote_addr(domain_str + remote_ipaddr.to_string() + ":" + OSTREAMSTR(http_uri.port()));
	}
	
	// NOTE: now the itor_arg MUST be valid
	DBG_ASSERT( itor_arg.is_valid() );
	// init and start socket_client_t
	socket_err_t	socket_err;
	socket_itor	= nipmem_new socket_itor_t();
	socket_err	= socket_itor->start(itor_arg, this, NULL);
	if( socket_err.failed() )	return http_err_from_socket(socket_err);

	// return no error
	return http_err_t::OK;
}

/** \brief Launch from a http_client_pool_cnx_t
 * 
 * - TODO here BIG BUG as it is imcompatible with the socket_itor_arg_t
 *   - what if the socket_profile_t in the local socket_itor_arg_t is not compatible
 *     with the connection from the pool ?//
 *   - well the socket_profile_t is not honored, so this is a BUG!!!!!!
 *   - currently it is not a real trouble because i only use this http_client_t
 *     with always the same socket_itor_arg_t but IT IS A CLEAR BUG!!!!
 */
http_err_t	http_client_t::launch_by_pool_cnx(http_client_pool_cnx_t *pool_cnx)	throw()
{
	const std::set<http_uri_t> &		pool_handled_uri_db = pool_cnx->get_handled_uri_db();
	std::set<http_uri_t>::const_iterator	iter;
	socket_full_t *				stolen_socket_full;
	socket_err_t				socket_err;
	// sanity check - the socket_itor_t and socket_full_t MUST be NULL
	DBG_ASSERT( !socket_itor && !socket_full );
	// merge the handled_uri_db from the pool_cnx to the local one
	for(iter = pool_handled_uri_db.begin(); iter != pool_handled_uri_db.end(); iter++)
		handled_uri_db.insert(*iter);
	// rewrite the http_reqhd_t http_uri_t with the current one from the pool_cnx
	http_reqhd.uri(pool_cnx->get_current_uri());
	// steal the pool_cnx socket_full
	stolen_socket_full	= pool_cnx->steal_full_and_dtor();
	// make the stolen_socket_full as the current one
	socket_err		= handle_cnx_established(stolen_socket_full);
	DBG_ASSERT( socket_err.succeed() );
	// return no error
	return http_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       expire_timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	http_client_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// notify the caller
	return notify_callback_failed(http_err_t(http_err_t::ERROR, "http_client_t timedout"));
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     host2ip_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if it is allowed to do host2ip_t on the socket_itor_arg_t, false otherwise
 */
bool	http_client_t::may_host2ip()						const throw()
{
	// if the socket_itor_arg_t already got a remote_addr() present, return false
	if( m_orig_itor_arg->remote_addr_present() )		return false;
	// if the socket_itor_arg_t domain() IS NOT socket_domain_t::TCP or STCP, return false
	if(!m_orig_itor_arg->domain_present() )			return false;
	if( !m_orig_itor_arg->domain().is_tcp() && !m_orig_itor_arg->domain().is_stcp())
		return false;
	// if all previous tests passed, return true
	return true;
}

/** \brief callback notified by host2ip_t when the result is known
 */
bool	http_client_t::neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
						, const inet_err_t &inet_err
						, const std::vector<ip_addr_t> &result_arr)	throw()
{
	// sanity check - may_host2ip() MUST be true
	DBG_ASSERT( may_host2ip() );
	// display the result
	KLOG_DBG("enter host2ip returned err=" << inet_err << " with " << result_arr.size() << " ip_addr_t"
					<< " for hostname=" << host2ip->hostname());
	// if the host2ip failed to return an ip address, notify the caller
	// - NOTE: the address to use MUST be fully_qualified
	//   - i have seen in the field some dns record pointing to 0.0.0.0... 
	if( inet_err.failed() || !result_arr[0].is_fully_qualified() ){
		std::string	reason("unable to resolve the host " + http_reqhd.uri().host());
		bool tokeep	= notify_callback_failed(http_err_t(http_err_t::ERROR, reason));
		if( !tokeep )	return false;
	}else{
		// launch the socket_client on the first of replied ip_addr_t
		launch_socket_client( result_arr[0] );
	}
	
	// delete host2ip
	nipmem_zdelete	host2ip;
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     socket_itor_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_itor_t to provide event
 */
bool	http_client_t::neoip_socket_itor_event_cb(void *userptr, socket_itor_t &cb_socket_itor
							, const socket_event_t &socket_event)	throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( socket_event.is_itor_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() ){
		std::string	reason	= "socket reported fatal event " + socket_event.to_string();
		http_err_t	http_err= http_err_t(http_err_t::ERROR, reason);
		return notify_callback_failed(http_err);
	}
	
	// delete the socket_itor
	nipmem_zdelete	socket_itor;

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:
			// start the just established socket_full_t
			// - NOTE: this is started here, because socket from the pool are already started
			socket_err 	= socket_event.get_cnx_established()->start(this, NULL);
			DBG_ASSERT( socket_err.succeed() );
			// handle it now
			socket_err	= handle_cnx_established( socket_event.get_cnx_established() );
			DBG_ASSERT( socket_err.succeed() );
			return false;
	default:	DBG_ASSERT( 0 );
	}	

	// return dontkeep
	return false;
}

/** \brief Handle the socket_event_t::CNX_ESTABLISHED event
 * 
 * @new_socket_full a socket_full_t already start()ed
 * 
 * @return a tokeep for the socket_client_t
 */
socket_err_t	http_client_t::handle_cnx_established(socket_full_t *new_socket_full)	throw()
{
	socket_err_t	socket_err;
	// sanity check - the socket_itor_t MUST be NULL
	DBG_ASSERT( !socket_itor );
	// log to debug
	KLOG_DBG("enter for http_reqhd=" << http_reqhd);

	// reinit the Host field to always match the uri one - even after indirection
	http_reqhd.header_db().update("Host", http_reqhd.uri().host());
	
	// get the socket_full pointer from the socket_event_t
	socket_full	= new_socket_full;
	// set the callback on the socket_full
	socket_full->set_callback(this, NULL);

	// configure the socket_full_t
	// - TODO hardcoded value change this
	//   - what are those very large values. seems really bad
	socket_full->rcvdata_maxlen( 512*1024 );
	socket_full->xmitbuf_maxlen( 50*1024 );
	socket_full->maysend_tshold( socket_full_t::UNLIMITED );
	
	// zero the data2post_off
	// - as it is a new connection the data2post MUST be sent or resent if it is a redirected cnx
	data2post_off	= 0;

	// sanity check the http_reqhd_t MUST be <= than the xmitbuf_maxlen
	DBG_ASSERT( http_reqhd.to_http().size() <= socket_full->xmitbuf_maxlen() );
	// write the http_reqhd_t thru the socket_full_t
	socket_err	= socket_full->send( datum_t(http_reqhd.to_http()) );
	DBG_ASSERT( socket_err.succeed() );	// NOTE: this is only temporary this wont reutrn error in the future

	// if the http_reqhd.method() is POST, start sending data2post
	if( http_reqhd.method() == http_method_t::POST )	send_data2post();

	// return no error
	return socket_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_client_t to provide event
 */
bool	http_client_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() ){
		std::string	reason	= "socket reported fatal event " + socket_event.to_string();
		http_err_t	http_err= http_err_t(http_err_t::ERROR, reason);
		return notify_callback_failed(http_err);
	}

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:	return handle_recved_data(*socket_event.get_recved_data());
	case socket_event_t::MAYSEND_ON:	return handle_maysend_on();			
	default:	DBG_ASSERT( 0 );
	}	

	// return tokeep
	return true;
}

/** \brief Handle the socket_event_t::RECVED_DATA event
 * 
 * @return a tokeep for the socket_full_t
 */
bool	http_client_t::handle_recved_data(pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);
	// queue the received data to the one already received
	recved_data.append(pkt.to_datum(datum_t::NOCOPY));

	// log to debug
	KLOG_DBG("recved_data=" << recved_data.to_datum(datum_t::NOCOPY).to_stdstring());
	
	// if the reply header is not yet fully received, return true
	std::string	recved_str	= recved_data.to_datum(datum_t::NOCOPY).to_stdstring();
	size_t		end_of_header	= recved_str.find("\r\n\r\n");
	if( end_of_header == std::string::npos )	return true;
	// update the end_of_header to include the end marker "\r\n\r\n"
	end_of_header	+= std::string("\r\n\r\n").size();

	// extract the header part
	std::string	header_str	= recved_str.substr(0, end_of_header);
	// convert it to its internal format
	http_rephd_t	http_rephd	= http_rephd_t::from_http(header_str);
	// if the parsing of the http_rephd_t fails, autodelete the connection
	if( http_rephd.is_null() )	return notify_callback_failed(http_err_t(http_err_t::ERROR, "Bogus http_rephd_t"));
	// consume the header in the recved_data
	recved_data.head_consume(end_of_header);

	// log to debug
	KLOG_DBG("http_rephd=" << http_rephd);	

	// test if a redirect MUST be followed. the conditions are:
	// - if the status_code is a redirect - as in rfc2616.10.3
	// - may_host2ip() MUST be true
	// - the "Location" variable MUST be contained in the http_client_t reply
	// - the profile.follow_redirect() MUST be set
	if( http_rephd.status_code() >= 300 && http_rephd.status_code() < 400 && may_host2ip()
			&& http_rephd.header_db().contain_key("Location") && profile.follow_redirect() ){
		return handle_http_redirect(http_rephd);
	}


	// steal the socket_full_t from the socket_client_t - after this the socket_client_t MUST be deleted
	slot_id_t	socket_full_slotid	= socket_full->get_object_slotid();	
	socket_full_t *	stolen_socket_full	= socket_full;
	socket_full	= NULL;

	// notify the caller of the received connection
	// - NOTE: tokeep is purposely ignored 
	notify_callback_succeed(http_rephd, stolen_socket_full, recved_data);
	
	// if the socket_full_t has no been deleted, so 'tokeep' else return 'dontkeep'
	// - it may happen when the socket_full_t is deleted by the caller
	return object_slotid_tokeep(socket_full_slotid);
}

/** \brief Handle the socket_event_t::MAYSEND_ON event
 * 
 * @return a tokeep for the socket_full_t
 */
bool	http_client_t::handle_maysend_on()	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// sanity check - this function is used IIF the http_reqhd.method() is POST
	DBG_ASSERT( http_reqhd.method() == http_method_t::POST );
	// send more data2post
	send_data2post();	
	// return tokeep
	return true;
}

/** \brief Send more data in case of http_method_t::POST
 */
void	http_client_t::send_data2post()						throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - this function is used IIF the http_reqhd.method() is POST
	DBG_ASSERT( http_reqhd.method() == http_method_t::POST );
	// sanity check - this function is used IIF the http_reqhd.method() is POST
	DBG_ASSERT( socket_full->xmitbuf_freelen() != socket_full_t::UNLIMITED );
	// request data2post to the caller 
	size_t	maxlen	= socket_full->xmitbuf_freelen();
	datum_t data2post=callback->neoip_http_client_data2post_cb(userptr, *this, data2post_off, maxlen);
	// if the data2post is null, no more data are to be posted, return now
	if( data2post.is_null() ){
		// set maysend_tshold to UNLIMITED
		socket_full->maysend_tshold( socket_full_t::UNLIMITED );
		return;
	}
	// write the data2post thru the socket_full_t
	socket_err	= socket_full->send( data2post );
	DBG_ASSERT( socket_err.succeed() );	// NOTE: this is only temporary this wont reutrn error in the future
	// update the data2post_off
	data2post_off	+= data2post.length();
	// set the socket_full->maysend_tshold to half the xmitbuf_usedlen
	socket_full->maysend_tshold( socket_full->xmitbuf_usedlen() / 2 );	
}

/** \brief Handle the http redirect - as in rfc2616.10.3
 * 
 * - the client_res variable and status_code MUST be initialized
 */
bool	http_client_t::handle_http_redirect(const http_rephd_t &http_rephd)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// TODO this handle temporary redirect as permanent one
	// - no good. but currently it wasnt an issue
	
	// sanity check - the status code MUST be a redirect one
	DBG_ASSERT( http_rephd.status_code() >= 300 && http_rephd.status_code() < 400 );
	// sanity check - the http_rephd_t MUST contain the Location
	DBG_ASSERT( http_rephd.header_db().contain_key("Location") );
	// copy the new http_uri in the http_reqhd_t
	http_reqhd.uri( http_rephd.header_db().get_first_value("Location") );
	// if the http_uri parsing fails, notify the error
	if( http_reqhd.uri().is_null() )
		return notify_callback_failed(http_err_t(http_err_t::ERROR, "Unable to parse the Location field while redirecting http request"));	
	// if the http_uri is already in the handled_uri_db, notify the error
	// - to test if the handled_uri_db already contains the new http_uri_t prevent infinit loop
	if( handled_uri_db.find(http_reqhd.uri()) != handled_uri_db.end() )
		return notify_callback_failed(http_err_t(http_err_t::ERROR, "Detected an infinit loop of http redirection"));

	// delete the socket_full_t
	nipmem_zdelete	socket_full;
	// zero the state variable to parse the previous reply
	recved_data	= bytearray_t();
	// launch the http_client on the new http_uri
	launch_http_uri();
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Notify the callback in case of faillure
 */
bool	http_client_t::notify_callback_failed(const http_err_t &http_err)	throw()
{
	return notify_callback(http_err, http_rephd_t(), NULL, bytearray_t() );
}

/** \brief Notify the callback in case of success
 */
bool	http_client_t::notify_callback_succeed(const http_rephd_t &http_rephd
			, socket_full_t *socket_full, const bytearray_t &recved_data)	throw()
{
	return notify_callback(http_err_t::OK, http_rephd, socket_full, recved_data);
}

/** \brief notify the callback with the tcp_event
 */
bool http_client_t::notify_callback(const http_err_t &http_err,const http_rephd_t &http_rephd
			, socket_full_t *socket_full, const bytearray_t &recved_data)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_http_client_cb(userptr, *this, http_err, http_rephd, socket_full, recved_data);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





