/*! \file
    \brief Definition of the \ref bt_tracker_request_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_tracker_request.hpp"
#include "neoip_httpd_request.hpp"
#include "neoip_tcp_full.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                      ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_tracker_request_t::bt_tracker_request_t()	throw()
{
	// setup the default values of bt_tracker_request_t
	port		( 0 );
	uploaded	( 0 );
	downloaded	( 0 );
	left		( 0 );
	compact		( false );
	nopeerid	( false );
	event		( "started" );
	nb_peer_wanted	( 0 );
	jamstd_support	(false);
	jamstd_require	(false);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a full request http_uri_t for this bt_tracker_request_t
 */
http_uri_t	bt_tracker_request_t::build_full_uri()	const throw()
{
	http_uri_t	full_uri	= announce_uri();
	// add all the variables from the bt_tracker_request_t
	full_uri.var().append("info_hash"	, string_t::from_datum( infohash().to_datum() ) );
	full_uri.var().append("peer_id"		, string_t::from_datum( peerid().to_datum() ) );
	full_uri.var().append("port"		, OSTREAMSTR(port()) );
	full_uri.var().append("uploaded"	, OSTREAMSTR(uploaded()) );
	full_uri.var().append("downloaded"	, OSTREAMSTR(downloaded()) );
	full_uri.var().append("left"		, OSTREAMSTR(left()) );
	if( !event().empty() )	full_uri.var().append("event"		, event() );
	if( compact() )		full_uri.var().append("compact"		, "1");
	if( nopeerid() )	full_uri.var().append("no_peer_id"	, "1");
	if(!ipaddr().is_null())	full_uri.var().append("ip"		, ipaddr().to_string());
	if( nb_peer_wanted() )	full_uri.var().append("numwant"		, OSTREAMSTR(nb_peer_wanted()));
	if(!key().empty())	full_uri.var().append("key"		, key());
	if( jamstd_support() )	full_uri.var().append("supportcrypto"	, "1");
	if( jamstd_require() )	full_uri.var().append("requirecrypto"	, "1");
	// return the result
	return full_uri;
}


/** \brief Build a bt_tracker_request_t
 */
bt_tracker_request_t bt_tracker_request_t::from_httpd_request(const httpd_request_t &httpd_request)	throw()
{
	bt_tracker_request_t	request;

	// if the minimal field are not present, dont even parse it
	if( httpd_request.get_variable("info_hash").empty() 
			|| httpd_request.get_variable("peer_id").empty()
			|| httpd_request.get_variable("port").empty() ){
		return bt_tracker_request_t();
	}
/*
 * NOTE: this is ugly as the httpd_request_t is old and not well coded
 */

	// build the bt_tracker_request_t from the httpd_request_t variable
	std::string	infohash_str		= httpd_request.get_variable("info_hash");
	std::string	peerid_str		= httpd_request.get_variable("peer_id");
	std::string	port_str		= httpd_request.get_variable("port");
	std::string	uploaded_str		= httpd_request.get_variable("uploaded");
	std::string	downloaded_str		= httpd_request.get_variable("downloaded");
	std::string	left_str		= httpd_request.get_variable("left");
	std::string	compact_str		= httpd_request.get_variable("compact");
	std::string	nopeerid_str		= httpd_request.get_variable("no_peer_id");
	std::string	event_str		= httpd_request.get_variable("event");
	std::string	ip_str			= httpd_request.get_variable("ip");
	std::string	numwant_str		= httpd_request.get_variable("numwant");
	std::string	key_str			= httpd_request.get_variable("key");
	std::string	jamstd_support_str	= httpd_request.get_variable("supportcrypto");
	std::string	jamstd_require_str	= httpd_request.get_variable("requirecrypto");

	// populate the bt_tracker_request_t fields
	request.infohash( bt_id_t(datum_t(string_t::unescape(infohash_str))) );
	request.peerid	( bt_id_t(datum_t(string_t::unescape(peerid_str))) );
	request.port	( atoi(port_str.c_str()) );
	if( !uploaded_str.empty() )	request.uploaded	( atoi(uploaded_str.c_str()) );
	if( !downloaded_str.empty() )	request.downloaded	( atoi(downloaded_str.c_str()) );
	if( !left_str.empty() )		request.left		( atoi(left_str.c_str()) );
	if( !compact_str.empty() )	request.compact		( atoi(compact_str.c_str()) );
	if( !nopeerid_str.empty() )	request.nopeerid	( atoi(nopeerid_str.c_str()) );
	if( !event_str.empty() )	request.event		( event_str );
	if( !ip_str.empty() )		request.ipaddr		( ip_str );
	else				request.ipaddr		( httpd_request.get_tcp_full()->get_local_addr().get_ipaddr());
	if( !numwant_str.empty() )	request.nb_peer_wanted	( atoi(numwant_str.c_str()) );
	if( !key_str.empty() )		request.key		( key_str );
	if(!jamstd_support_str.empty())	request.jamstd_support	( atoi(jamstd_support_str.c_str()) );
	if(!jamstd_require_str.empty())	request.jamstd_require	( atoi(jamstd_require_str.c_str()) );

	// extract the announce_uri from the httpd_request_t
	std::ostringstream	oss;
	oss << "http://";
	oss << httpd_request.get_header("Host");
	oss << ":" << httpd_request.get_tcp_full()->get_local_addr().get_port();
	oss << httpd_request.get_path();
	// put the announce_uri in this object
	request.announce_uri	( oss.str() );

	// return the result
	return request;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	bt_tracker_request_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( announce_uri().is_null() )	return true;
	if( infohash().is_null() )	return true;
	if( peerid().is_null() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}
	

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_tracker_request_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss        << "announce_uri="	<< announce_uri();
	oss << " " << "infohash="	<< infohash();
	oss << " " << "peerid="		<< peerid();
	oss << " " << "port="		<< port();
	oss << " " << "uploaded="	<< uploaded();
	oss << " " << "downloaded="	<< downloaded();
	oss << " " << "left="		<< left();
	oss << " " << "compact="	<< std::boolalpha << compact();
	oss << " " << "nopeerid="	<< std::boolalpha << nopeerid();
	oss << " " << "event="		<< event();
	oss << " " << "ipaddr="		<< ipaddr();
	oss << " " << "nb_peer_wanted="	<< nb_peer_wanted();
	oss << " " << "key="		<< key();
	oss << " " << "jamstd_support="	<< jamstd_support();
	oss << " " << "jamstd_require="	<< jamstd_require();
	
	// return the just built string
	return oss.str();
}
NEOIP_NAMESPACE_END

