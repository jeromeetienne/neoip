/*! \file
    \brief Declaration of the upnp_disc_t

\par Brief Description
\ref upnp_disc_t does a single discovery of the local upnp router.
This handles any type of SOAP disc (well the upnp subset) but there are some
helper start function to help discing the functions hiding the upnp stuff.
- additionally it does a test on the upnp router if it is availble to determine
  it is contain the getport_endianbug. and set it accordingly in
  the upnp_disc_res_t

*/

/* system include */
/* local include */
#include "neoip_upnp_disc.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_upnp_getportendian_test.hpp"
#include "neoip_nudp.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_http_sclient_res.hpp"
//#ifdef _WIN32	// NOTE: only needed for the nudp_t local_addr and IP_PKTINFO kludge
#if 1
#	include "neoip_socket.hpp"
#endif
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_disc_t::upnp_disc_t()		throw()
{
	// zero some fields
	nudp 			= NULL;
	http_sclient		= NULL;
	getportendian_test	= NULL;
}

/** \brief Desstructor
 */
upnp_disc_t::~upnp_disc_t()		throw()
{	
	// destroy the nudp_t if needed
	nipmem_zdelete 	nudp;
	// destroy the http_sclient_t if needed
	nipmem_zdelete	http_sclient;
	// delete the upnp_getportendian_test_t if needed
	nipmem_zdelete	getportendian_test;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
upnp_disc_t &upnp_disc_t::set_profile(const upnp_disc_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == upnp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}
/** \brief start function - the generic one used by all the helper one
 */
upnp_err_t upnp_disc_t::start(const std::string &m_service_name, upnp_disc_cb_t *callback
							, void * userptr)	throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;

	// copy the service_name into the current_res
	current_res.service_name(m_service_name);

	// Start the nudp and bind any local port
	nudp		= nipmem_new nudp_t();
	inet_err	= nudp->start(ipport_addr_t::ANY_IP4, this, NULL );
	if( inet_err.failed() )	return upnp_err_from_inet(inet_err);

	// init the expire_timeout
	expire_timeout.start(profile.expire_delay(), this, NULL);
	
	// init the nudprxmit_timeout stuff
	nudprxmit_delaygen	= delaygen_t(profile.nudprxmit_delaygen_arg());
	nudprxmit_timeout.start(nudprxmit_delaygen.current(), this, NULL);
	
	// return no error
	return upnp_err_t::OK;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_disc_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	if( &cb_timeout == &nudprxmit_timeout )	return nudprxmit_timeout_cb(userptr, cb_timeout);
	if( &cb_timeout == &expire_timeout )	return expire_timeout_cb(userptr, cb_timeout);
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;
}


/** \brief callback called when the neoip_timeout expire
 */
bool upnp_disc_t::nudprxmit_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - nudp_t MUST be set
	DBG_ASSERT( nudp );

	// notify the expiration of the timeout to the delaygen
	nudprxmit_delaygen.notify_expiration();
	// sanity check - the nudprxmit_delaygen MUST NEVER expire, it is up the expire_delay to handle this
	DBG_ASSERT( !nudprxmit_delaygen.is_timedout() );

	// send the udp discovery packet
	inet_err_t	inet_err;
	inet_err	= nudp->send_to(build_upnp_disc_req(), ipport_addr_t("239.255.255.250:1900"));
	if( inet_err.failed() )	KLOG_ERR("Cant send the upnp_disc_t udp packet due to " << inet_err);

	// relaunch the disc_timeout for the next upnp_disc_t
	nudprxmit_timeout.change_period(nudprxmit_delaygen.pre_inc());
	// return a 'tokeep'
	return true;
}

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_disc_t::expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// notify the caller of the expiration
	std::string	reason	= "upnp_disc_t timedout after " + OSTREAMSTR(expire_timeout.get_period());
	return notify_callback(upnp_err_t(upnp_err_t::ERROR, reason), upnp_disc_res_t());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a upnp discovery request packet (for UDP)
 */
pkt_t	upnp_disc_t::build_upnp_disc_req()					const throw()
{
	std::ostringstream	oss;
	// sanity check - the service_name MUST be set
	DBG_ASSERT( !service_name().empty() );
	// build the upnp_req
	oss << "M-SEARCH * HTTP/1.1\r\n"; 
	oss << "Host:239.255.255.250:1900\r\n";
	oss << "ST: urn:schemas-upnp-org:service:" << service_name() << ":1\r\n";
	oss << "Man:\"ssdp:discover\"\r\n";
	oss << "MX:3\r\n";
	oss << "\r\n";
	oss << "\r\n";
	// return the pkt_t for it
	return pkt_t(oss.str().c_str(), oss.str().size());
}

/** \brief Parse a upnp discovery reply packet (for UDP)
 */
upnp_err_t	upnp_disc_t::parse_upnp_disc_rep(pkt_t &pkt)			throw()
{
	http_rephd_t	http_rephd;
	// parse the http_rephd_t 
	http_rephd	= http_rephd_t::from_http(pkt.to_stdstring());
	// if not able to parse the http_rephd_t, log and return a error
	if( http_rephd.is_null() ){
		KLOG_ERR("unable to parse http_rephd_t in " << pkt);
		return upnp_err_t(upnp_err_t::ERROR, "unable to parse http_rephd_t in udp disc reply");
	}
	// if the http_rephd_t::status_code() is NOT OK, log and return a error
	if( http_rephd.status_code() != 200 ){
		KLOG_ERR("Receive a non OK status code in http_rephd_t=" << http_rephd);
		return upnp_err_t(upnp_err_t::ERROR, "upnp router replied a non OK status code");
	}

	// build an alias on http_rephd.header_db
	const strvar_db_t & header_db	= http_rephd.header_db();

	// get upnp_disc_res_t::server_name()
	if( header_db.contain_key("SERVER") )
		current_res.server_name( header_db.get_first_value("SERVER") );

	// get upnp_disc_res_t::location_uri()
	if( header_db.contain_key("LOCATION") )
		current_res.location_uri( header_db.get_first_value("LOCATION") );

	// if no valid location http_uri_t has been found, log the event
	if( current_res.location_uri().is_null() ){
		KLOG_ERR("unable to get a location http_uri_t from http_rephd_t=" << http_rephd);
		return upnp_err_t(upnp_err_t::ERROR, "unable to get a location http_uri_t in udp disc reply");
	}

	// return no error
	return upnp_err_t::OK;
}

/** \brief Parse the http_sclient_t reply and fill the this->current_res with it
 */
upnp_err_t	upnp_disc_t::parse_upnp_http_rep(const http_sclient_res_t &sclient_res)	throw(xml_except_t)
{
	// get the reply_body which is in xml
	std::string	xml_str	= sclient_res.reply_body().to_stdstring();
	// some router get the idea it is good to put additionnal \r\n, so i strip them :)
	xml_str		= string_t::strip(xml_str, "\r\n \t");

	// set the document to parse
	xml_parse_doc_t	xml_parse_doc;
	xml_parse_doc.set_document(datum_t(xml_str));
	// check that the parsing suceed
	if( xml_parse_doc.is_null() )	return upnp_err_t(upnp_err_t::ERROR, "unable to parse the upnp_disc_t http reply as xml"); 

	// extract the urlbase from the description response
	http_uri_t	urlbase	= sclient_res.http_reqhd().uri();
	if( xml_parse_t(&xml_parse_doc).has_path("/root/URLBase") )
		urlbase	= xml_parse_t(&xml_parse_doc).path_content_opt("/root/URLBase");
	// log to debug
	KLOG_DBG("urlbase=" << urlbase);


	// init the xml_parse_t on this document
	xml_parse_t	xml_parse(&xml_parse_doc);
	xml_parse.goto_children();
	
	// go thru all the device to find the one acting as InternetGatewayDevice
	while( 1 ){
		// find the next sibling called "device"
		xml_parse.goto_firstsib("device");
		// extract the device type from it
		std::string device_type	= xml_parse_t(xml_parse).path_content_opt("device/deviceType");
		// if this device_type is the one of a InternetGatewayDevice, leave the loop
		if( device_type == "urn:schemas-upnp-org:device:InternetGatewayDevice:1" )	break;
		// goto the next sibling
		xml_parse.goto_nextsib();		
	}

	// go inside the device for InternetGatewayDevice section
	xml_parse.goto_children();
	// go inside the deviceList section
	xml_parse.goto_firstsib("deviceList");
	xml_parse.goto_children();

	// go thru all the device to find the one acting as WANDevice
	while( 1 ){
		// find the next sibling called "device"
		xml_parse.goto_firstsib("device");
		// extract the device type from it
		std::string device_type	= xml_parse_t(xml_parse).path_content_opt("device/deviceType");
		// if this device_type is the one of a WANDevice, leave the loop
		if( device_type == "urn:schemas-upnp-org:device:WANDevice:1" )	break;
		// goto the next sibling
		xml_parse.goto_nextsib();		
	}
	
	// go inside the device for WANDevice
	xml_parse.goto_children();
	// go inside the deviceList section
	xml_parse.goto_firstsib("deviceList");
	xml_parse.goto_children();

	// go thru all the device to find the one acting as WANConnectionDevice
	while( 1 ){
		// find the next sibling called "device"
		xml_parse.goto_firstsib("device");
		// extract the device type from it
		std::string device_type	= xml_parse_t(xml_parse).path_content_opt("device/deviceType");
		// if this device_type is the one of a WANConnectionDevice, leave the loop
		if( device_type == "urn:schemas-upnp-org:device:WANConnectionDevice:1" )	break;
		// goto the next sibling
		xml_parse.goto_nextsib();		
	}

	// go inside the device for WANConnectionDevice
	xml_parse.goto_children();
	// go inside the serviceList section
	xml_parse.goto_firstsib("serviceList");
	xml_parse.goto_children();

	// go thru all the device to find the one acting as the service_name variable
	// - service_name is [WANIPConnection|WANPPPConnection]
	while( 1 ){
		// find the next sibling called "service"
		xml_parse.goto_firstsib("service");
		// extract the device type from it
		std::string device_type	= xml_parse_t(xml_parse).path_content_opt("service/serviceType");
		// if this device_type is the one of the service_name, leave the loop
		if( device_type == std::string("urn:schemas-upnp-org:service:")+service_name()+":1") break;
		// goto the next sibling
		xml_parse.goto_nextsib();		
	}

	// go inside the device for this service_name
	xml_parse.goto_children();
	// extract the controlURL content
	std::string	constrolURL	= xml_parse.goto_firstsib("controlURL").node_content();
	
	// build the definitive control_uri base on the constrolURL and the baseurl
	http_uri_t	control_uri;
	if( !string_t::casecmp("http://", constrolURL.substr(0, strlen("http://"))) ){
		// if control url start with a http://. this is a full url
		control_uri	= constrolURL;
	}else if( constrolURL.substr(0, 1) == "/" ){
		// if control url start with /, this is the absolute path for the baseurl
		control_uri	= std::string("http://") + urlbase.hostport_str() + constrolURL + urlbase.query_str();
	}else{
		// if control url start with somehting else, this is a relative path for the baseurl
		control_uri	= std::string("http://") + urlbase.hostport_str() + urlbase.path_str() + "/" + constrolURL + urlbase.query_str();
	} 
	// copy the control_uri in the upnp_disc_res_t
	current_res.control_uri	(control_uri);

	// log to debug	
	KLOG_DBG("control_uri=" << control_uri);

	// return no error
	return upnp_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     nudp callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref nudp_t receive a packet
 */
bool	upnp_disc_t::neoip_inet_nudp_event_cb(void *cb_userptr, nudp_t &cb_nudp, pkt_t &pkt
					, const ipport_addr_t &local_addr
					, const ipport_addr_t &remote_addr)	throw()
{
	http_err_t	http_err;
	upnp_err_t	upnp_err;
	// log to debug
	KLOG_DBG("Received from remote_addr=" << remote_addr << " local_addr=" << local_addr << " pkt = " << pkt );
	KLOG_DBG("response=" << pkt.to_stdstring());

	// copy the local and server ip_addr_t in the upnp_disc_res_t
	current_res.local_ipaddr(local_addr.ipaddr());
	current_res.server_ipaddr(remote_addr.ipaddr());
	
	// parse the received packet
	upnp_err	= parse_upnp_disc_rep(pkt);
	if( upnp_err.failed() )	return notify_callback(upnp_err, upnp_disc_res_t());

	// delete the nudp_t
	nipmem_zdelete	nudp;
	// stop the nudprxmit_timeout
	nudprxmit_timeout.stop();
	
	// start the http_sclient_t
	http_sclient	= nipmem_new http_sclient_t();
	http_err	= http_sclient->start(current_res.location_uri(), this, NULL);
	if( upnp_err.failed() )	return notify_callback(upnp_err_from_http(http_err), upnp_disc_res_t());

	// return dontkeep	
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	upnp_disc_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &cb_sclient_res)	throw() 
{
	http_sclient_res_t	sclient_res	= cb_sclient_res;
	upnp_err_t		upnp_err;
	// log to debug
	KLOG_DBG("enter http_sclient_res=" << sclient_res);
	KLOG_DBG("reply_body=" << sclient_res.reply_body().to_stdstring());

	// if http_sclient_t failed, notify the caller
	if( !sclient_res.full_get_ok() ){
		std::string	reason	= "http_sclient_res_t not is_full_ok";
		return notify_callback(upnp_err_t(upnp_err_t::ERROR, reason), upnp_disc_res_t());
	}

#ifdef _WIN32
	// KLUDGE: WORKAROUND: lack of IP_PKTINFO in nudp_t 
	// - under _WIN32, nudp_t is unable to provide the local_addr of the received packet
	// - this is due to a lack of support of IP_PKTINFO
	// - BUT upnp_disc_t needs to know the local_ipaddr()
	// - so it is extracted from the http_sclient_t::socket_full local_addr
	// - btw as http_sclient_t got no http_client_pool_t, the socket_full 
	//   should still be there. 
	socket_full_t *	socket_full	= http_sclient->get_socket_full();
	if( socket_full && current_res.local_ipaddr().is_any() ){
		ip_addr_t	local_ipaddr;
		// get the local_ipaddr from the socket_full_t::local_addr()
		local_ipaddr	= socket_full->local_addr().get_peerid_vapi()->to_string();
		// set the local_ipaddr in the current_res
		current_res.local_ipaddr( local_ipaddr );
	}
#endif

	// delete the http_sclient_t
	nipmem_zdelete	http_sclient;

	// parse its reply
	try{
		upnp_err	= parse_upnp_http_rep(sclient_res);
	}catch(xml_except_t &e){
		std::string reason = "Cant parse the upnp_disc_t http_sclient_res_t due to " + e.what();
		return notify_callback(upnp_err_t(upnp_err_t::ERROR, reason), upnp_disc_res_t());
	}

	// if the parsing failed, notify the client
	if( upnp_err.failed() )	return notify_callback(upnp_err, upnp_disc_res_t());

	// if profile has NO getportendian_test_enabled, the discovery is completed
	if( !profile.getportendian_test_enabled() )
		return notify_callback(upnp_err_t::OK, current_res);

	// launch the getportendian_test - with the local upnp_disc_res_t
	getportendian_test	= nipmem_new upnp_getportendian_test_t();
	upnp_err		= getportendian_test->start(current_res, this, NULL);
	if( upnp_err.failed() )	return notify_callback(upnp_err, upnp_disc_res_t());
	
	// return dontkeep
	return false;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_getendian_test_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref upnp_getportendian_test_t when completed
 */
bool	upnp_disc_t::neoip_upnp_getportendian_test_cb(void *cb_userptr, upnp_getportendian_test_t &cb_getportendian_test
							, const upnp_err_t &upnp_err)	throw()
{
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err);

	// if upnp_getportendian_test_t failed, notify the caller
	if( upnp_err.failed() )		return notify_callback(upnp_err, upnp_disc_res_t());
	
	// copy the upnp_getportendian_test_t result into the current_res
	current_res.getport_endianbug	( getportendian_test->is_revendian() ); 
	
	// delete the upnp_getportendian_test_t
	nipmem_zdelete	getportendian_test;

	// notify the successfull completion to the caller
	return notify_callback(upnp_err_t::OK, current_res);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool upnp_disc_t::notify_callback(const upnp_err_t &upnp_err, const upnp_disc_res_t &disc_res)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the discer
	bool tokeep = callback->neoip_upnp_disc_cb(userptr, *this, upnp_err, disc_res);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


