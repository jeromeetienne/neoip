/*! \file
    \brief Definition of the \ref dnsgrab_request_t class

*/

/* system include */
/* local include */
#include "neoip_dnsgrab_request.hpp"
#include "neoip_dns_helper.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor with value
 * 
 * - if the parsing fails in the constructor, the returned object is null
 * 
 * - the request string looks like this:
 *   - "gethostbyname/jerome.neoip/AF_INET"
 *   - "gethostbyaddr/10.0.0.1/AF_INET"
 *   - all fields divided by '/'
 *   - MUST have 3 fields
 *   - Field 1: the command i.e. gethostbyname or gethostbyaddr
 *   - Field 2: the name to look for. it is a string for gethostbyname and a dotted number
 *     if gethostbyaddr
 *   - Field 3: the address space to be queried
 * 
 */
dnsgrab_request_t::dnsgrab_request_t(const slot_id_t &cnx_slot_id, const std::string &request_str)	throw()
{
	// copy the cnx_slot_id
	this->cnx_slot_id	= cnx_slot_id;
	// set the reply_present to false
	this->reply_present	= false;

	// log to debug
	KLOG_DBG("request_str=" << request_str);

// start the parsing	
	std::vector<std::string>	words;
	// parse the request string
	words	= string_t::split(request_str, "/");
	KLOG_DBG("words.size()="<< words.size());
	// it MUST have 3 fields
	if( words.size() != 3 )	return;

	// if the last parameter still contain a \r\n, remove it now
	if( words[2].rfind("\n") != std::string::npos ){
		words[2].erase(words[2].rfind("\n"));
	}
	
// syntax checking
	// syntax check - Field 1 is gethostbyname | gethostbyaddr
	if( string_t::casecmp(words[0], "gethostbyname") &&  string_t::casecmp(words[0], "gethostbyaddr"))
		return;
	// syntax check - Field 2 is non empty
	if( words[1].empty() )		return;
	// parse the Field 3 : AF_INET | AF_INET6
	if( string_t::casecmp(words[2], "AF_INET") && string_t::casecmp(words[2], "AF_INET6"))
		return;

// request init
	request_name	= words[1];
	// convert the request_name into ascii if it is in idna notation
	request_name	= dns_helper_t::idna_to_ascii(request_name);

	if( !string_t::casecmp(words[0], "gethostbyname") )	request_by_name	= true;
	else							request_by_name	= false;
	addr_family	= words[2];
}

/** \brief Return true if the object is null, false otherwise
 */
bool	dnsgrab_request_t::is_null()	const throw()
{
	// it test addr_family because this field is mandatoraly set after a good init
	if( !addr_family.empty() )	return false;
	return true;
}

/** \brief Return the reply string from the replied request
 * 
 * - Example of reply
 *   - "found/jerome.NfkfkeoIP/jerome.etienne@jeje/AF_INET/127.1.42.1@192.168.1.2"
 *   - "notfound"
 *     "found/jerome.NfkfkeoIP/jerome.etienne@jeje/AF_INET/127.1.42.1@192.168.1.2"
 * - all fields separated by a /
 * - Field is the command: [found|notfound]
 * - if the command is notfound, it has no parameter
 * - if the command is found, it return all the paremeter that will be used to fill
 *   the struct hostent of a gethostbyname() call
 *   - param1: h_name the official name
 *   - param2: h_aliases the list of aliases, all separated by @, it may be null
 *   - param3: h_addrtype the address family of the address AF_INET|AF_INET6
 *   - param4: h_addr_list the list of all addresses, all of the same addr_family
 *             all separated by @
 */
std::string	dnsgrab_request_t::get_reply_str()		const throw()
{
	std::ostringstream	oss;
	// sanity check - the reply MUST be present
	DBG_ASSERT( reply_present );
	// if no answer has been found, reply name is empty so answer a notfound
	if( reply_name.empty() ){
		oss << "notfound";
		return oss.str();
	}
	
	// put the found command
	oss << "found/";

	// put the h_name
	oss << reply_name << "/";

	// build the aliases list
	std::list<std::string>::const_iterator iter_alias;
	for( iter_alias = reply_aliases.begin(); iter_alias != reply_aliases.end(); iter_alias++ ){
		if( iter_alias != reply_aliases.begin() )	oss << "@";
		oss << *iter_alias;
	}
	oss << "/";

	// put the address family
	if( reply_addresses.front().is_v4() )	oss << "AF_INET/";
	else					oss << "AF_INET6/";

	// build the addresses list
	std::list<ip_addr_t>::const_iterator iter_addr;
	for( iter_addr = reply_addresses.begin(); iter_addr != reply_addresses.end(); iter_addr++ ){
		if( iter_addr != reply_addresses.begin() )	oss << "@";
		oss << *iter_addr;
	}
	
	// return the string
	return oss.str();
}


NEOIP_NAMESPACE_END



