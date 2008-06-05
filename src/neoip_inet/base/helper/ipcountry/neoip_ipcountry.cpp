/*! \file
    \brief Definition of the \ref ipcountry_t class

\par Brief description
\ref ipcountry_t try to figure out the country in which a ip_addr_t is located.
This is really adversory and not to rely on, so more a nice toy to provide 
nice display to the user that a feature to rely on. 

This is based on a service available from a site http://countries.nerd.dk
It provides this in order to be used for email blacklisting in order to 
avoid spamming.
- the fact to rely on external service makes this function failling if the 
  service is temporarily or permanantly unavailable.
- about the accuracy of the result, this is adversory only
  - the few tests i made were exact
  
\par possible Improvement - rely on a local database
- this would avoid the unreliability associated with external service
- there are some library available with some free database 
  - http://www.maxmind.com/app/geoip_country is a wellknown one

*/

/* system include */
/* local include */
#include "neoip_ipcountry.hpp"
#include "neoip_endian.hpp"
#include "neoip_host2ip.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ipcountry_t::ipcountry_t()	throw()
{
	// zero some field
	host2ip	= NULL;
}

/** \brief Destructor
 */
ipcountry_t::~ipcountry_t()	throw()
{
	// delete host2ip_t if needed
	nipmem_zdelete	host2ip;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
inet_err_t	ipcountry_t::start(const ip_addr_t &m_ipaddr, ipcountry_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->m_ipaddr	= m_ipaddr;
	this->callback	= callback;
	this->userptr	= userptr;

	// build the hostname to resolve - ip_addr_t in reverse order appended with .zz.countries.nerd.dk
	std::string	hostname;
	hostname	= ip_addr_t(NEOIP_HTONL(ipaddr().get_v4_addr())).to_string();
	hostname	+= ".zz.countries.nerd.dk";

	// log to debug
	KLOG_DBG("try to resolve hostname=" << hostname);

	// launch the host2ip_t on the just built hostname
	inet_err_t	inet_err;
	host2ip		= nipmem_new host2ip_t();
	inet_err	= host2ip->start(hostname, this, NULL);
	if( inet_err.failed() )		return inet_err;

	// return no error
	return inet_err_t::OK;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			host2ip_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by host2ip_t when the result is known
 */
bool	ipcountry_t::neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
						, const inet_err_t &inet_err
						, const std::vector<ip_addr_t> &result_arr)	throw()
{
	// display the result
	KLOG_DBG("enter host2ip returned err=" << inet_err << " with " << result_arr.size() << " ip_addr_t"
					<< " for hostname=" << host2ip->hostname());
	// if the host2ip failed to return an ip address, notify the caller
	if( inet_err.failed() || result_arr.empty() ){
		std::string	reason("unable to resolve the ip_addr_t " + ipaddr().to_string());
		return notify_callback(inet_err_t(inet_err_t::ERROR, reason), std::string());
	}
	
	// create an alias on the result_ipaddr
	const ip_addr_t	result_ipaddr	= result_arr[0]; 
	// log to debug
	KLOG_DBG("result=" << result_ipaddr);
	
	// convert the result_ipaddr into a country code
	const char * country_ptr	= ipaddr2countrycode(result_ipaddr);
	if( !country_ptr ){
		std::string	reason("Internal error. the return ip_addr_t is NOT a ISO3166 code. ip_addr=" + result_ipaddr.to_string());
		return notify_callback(inet_err_t(inet_err_t::ERROR, reason), std::string());
	}
	// notify the caller of the successfull result
	std::string	country_code	= country_ptr;
	return notify_callback(inet_err_t::OK, country_code);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a ip_addr_t with the 2 last digit being ISO 3166 number code into the country_code
 * 
 * - for a description of ISO3166, see http://en.wikipedia.org/wiki/ISO_3166-1
 * - this list has been derived from http://countries.nerd.dk/isolist.txt
 * - NOTE: if the ISO3166 code is unknown, it return NULL
 */
const char *ipcountry_t::ipaddr2countrycode(const ip_addr_t &ip_addr)	const throw()
{
	const char *str	= ip_addr.to_string().c_str();
	
#	define IPCOUNTRY_CST(country_str, ipaddr_str)			\
	if(strcmp(str, ipaddr_str) == 0)	return country_str;

IPCOUNTRY_CST("ad", "127.0.0.20");
IPCOUNTRY_CST("ae", "127.0.3.16");
IPCOUNTRY_CST("af", "127.0.0.4");
IPCOUNTRY_CST("ag", "127.0.0.28");
IPCOUNTRY_CST("ai", "127.0.2.148");
IPCOUNTRY_CST("al", "127.0.0.8");
IPCOUNTRY_CST("am", "127.0.0.51");
IPCOUNTRY_CST("an", "127.0.2.18");
IPCOUNTRY_CST("ao", "127.0.0.24");
IPCOUNTRY_CST("aq", "127.0.0.10");
IPCOUNTRY_CST("ar", "127.0.0.32");
IPCOUNTRY_CST("as", "127.0.0.16");
IPCOUNTRY_CST("at", "127.0.0.40");
IPCOUNTRY_CST("au", "127.0.0.36");
IPCOUNTRY_CST("aw", "127.0.2.21");
IPCOUNTRY_CST("az", "127.0.0.31");
IPCOUNTRY_CST("ba", "127.0.0.70");
IPCOUNTRY_CST("bb", "127.0.0.52");
IPCOUNTRY_CST("bd", "127.0.0.50");
IPCOUNTRY_CST("be", "127.0.0.56");
IPCOUNTRY_CST("bf", "127.0.3.86");
IPCOUNTRY_CST("bg", "127.0.0.100");
IPCOUNTRY_CST("bh", "127.0.0.48");
IPCOUNTRY_CST("bi", "127.0.0.108");
IPCOUNTRY_CST("bj", "127.0.0.204");
IPCOUNTRY_CST("bm", "127.0.0.60");
IPCOUNTRY_CST("bn", "127.0.0.96");
IPCOUNTRY_CST("bo", "127.0.0.68");
IPCOUNTRY_CST("br", "127.0.0.76");
IPCOUNTRY_CST("bs", "127.0.0.44");
IPCOUNTRY_CST("bt", "127.0.0.64");
IPCOUNTRY_CST("bv", "127.0.0.74");
IPCOUNTRY_CST("bw", "127.0.0.72");
IPCOUNTRY_CST("by", "127.0.0.112");
IPCOUNTRY_CST("bz", "127.0.0.84");
IPCOUNTRY_CST("ca", "127.0.0.124");
IPCOUNTRY_CST("cc", "127.0.0.166");
IPCOUNTRY_CST("cf", "127.0.0.140");
IPCOUNTRY_CST("cg", "127.0.0.178");
IPCOUNTRY_CST("ch", "127.0.2.244");
IPCOUNTRY_CST("ci", "127.0.1.128");
IPCOUNTRY_CST("ck", "127.0.0.184");
IPCOUNTRY_CST("cl", "127.0.0.152");
IPCOUNTRY_CST("cm", "127.0.0.120");
IPCOUNTRY_CST("cn", "127.0.0.156");
IPCOUNTRY_CST("co", "127.0.0.170");
IPCOUNTRY_CST("cr", "127.0.0.188");
IPCOUNTRY_CST("cu", "127.0.0.192");
IPCOUNTRY_CST("cv", "127.0.0.132");
IPCOUNTRY_CST("cx", "127.0.0.162");
IPCOUNTRY_CST("cy", "127.0.0.196");
IPCOUNTRY_CST("cz", "127.0.0.203");
IPCOUNTRY_CST("de", "127.0.1.20");
IPCOUNTRY_CST("dj", "127.0.1.6");
IPCOUNTRY_CST("dk", "127.0.0.208");
IPCOUNTRY_CST("dm", "127.0.0.212");
IPCOUNTRY_CST("do", "127.0.0.214");
IPCOUNTRY_CST("dz", "127.0.0.12");
IPCOUNTRY_CST("ec", "127.0.0.218");
IPCOUNTRY_CST("ee", "127.0.0.233");
IPCOUNTRY_CST("eg", "127.0.3.50");
IPCOUNTRY_CST("eh", "127.0.2.220");
IPCOUNTRY_CST("er", "127.0.0.232");
IPCOUNTRY_CST("es", "127.0.2.212");
IPCOUNTRY_CST("et", "127.0.0.231");
IPCOUNTRY_CST("fi", "127.0.0.246");
IPCOUNTRY_CST("fj", "127.0.0.242");
IPCOUNTRY_CST("fk", "127.0.0.238");
IPCOUNTRY_CST("fm", "127.0.2.71");
IPCOUNTRY_CST("fo", "127.0.0.234");
IPCOUNTRY_CST("fr", "127.0.0.250");
IPCOUNTRY_CST("fx", "127.0.0.249");
IPCOUNTRY_CST("ga", "127.0.1.10");
IPCOUNTRY_CST("gb", "127.0.3.58");
IPCOUNTRY_CST("gd", "127.0.1.52");
IPCOUNTRY_CST("ge", "127.0.1.12");
IPCOUNTRY_CST("gf", "127.0.0.254");
IPCOUNTRY_CST("gh", "127.0.1.32");
IPCOUNTRY_CST("gi", "127.0.1.36");
IPCOUNTRY_CST("gl", "127.0.1.48");
IPCOUNTRY_CST("gm", "127.0.1.14");
IPCOUNTRY_CST("gn", "127.0.1.68");
IPCOUNTRY_CST("gp", "127.0.1.56");
IPCOUNTRY_CST("gq", "127.0.0.226");
IPCOUNTRY_CST("gr", "127.0.1.44");
IPCOUNTRY_CST("gs", "127.0.0.239");
IPCOUNTRY_CST("gt", "127.0.1.64");
IPCOUNTRY_CST("gu", "127.0.1.60");
IPCOUNTRY_CST("gw", "127.0.2.112");
IPCOUNTRY_CST("gy", "127.0.1.72");
IPCOUNTRY_CST("hk", "127.0.1.88");
IPCOUNTRY_CST("hm", "127.0.1.78");
IPCOUNTRY_CST("hn", "127.0.1.84");
IPCOUNTRY_CST("hr", "127.0.0.191");
IPCOUNTRY_CST("ht", "127.0.1.76");
IPCOUNTRY_CST("hu", "127.0.1.92");
IPCOUNTRY_CST("id", "127.0.1.104");
IPCOUNTRY_CST("ie", "127.0.1.116");
IPCOUNTRY_CST("il", "127.0.1.120");
IPCOUNTRY_CST("in", "127.0.1.100");
IPCOUNTRY_CST("io", "127.0.0.86");
IPCOUNTRY_CST("iq", "127.0.1.112");
IPCOUNTRY_CST("ir", "127.0.1.108");
IPCOUNTRY_CST("is", "127.0.1.96");
IPCOUNTRY_CST("it", "127.0.1.124");
IPCOUNTRY_CST("jm", "127.0.1.132");
IPCOUNTRY_CST("jo", "127.0.1.144");
IPCOUNTRY_CST("jp", "127.0.1.136");
IPCOUNTRY_CST("ke", "127.0.1.148");
IPCOUNTRY_CST("kg", "127.0.1.161");
IPCOUNTRY_CST("kh", "127.0.0.116");
IPCOUNTRY_CST("ki", "127.0.1.40");
IPCOUNTRY_CST("km", "127.0.0.174");
IPCOUNTRY_CST("kn", "127.0.2.147");
IPCOUNTRY_CST("kp", "127.0.1.152");
IPCOUNTRY_CST("kr", "127.0.1.154");
IPCOUNTRY_CST("kw", "127.0.1.158");
IPCOUNTRY_CST("ky", "127.0.0.136");
IPCOUNTRY_CST("kz", "127.0.1.142");
IPCOUNTRY_CST("la", "127.0.1.162");
IPCOUNTRY_CST("lb", "127.0.1.166");
IPCOUNTRY_CST("lc", "127.0.2.150");
IPCOUNTRY_CST("li", "127.0.1.182");
IPCOUNTRY_CST("lk", "127.0.0.144");
IPCOUNTRY_CST("lr", "127.0.1.174");
IPCOUNTRY_CST("ls", "127.0.1.170");
IPCOUNTRY_CST("lt", "127.0.1.184");
IPCOUNTRY_CST("lu", "127.0.1.186");
IPCOUNTRY_CST("lv", "127.0.1.172");
IPCOUNTRY_CST("ly", "127.0.1.178");
IPCOUNTRY_CST("ma", "127.0.1.248");
IPCOUNTRY_CST("mc", "127.0.1.236");
IPCOUNTRY_CST("md", "127.0.1.242");
IPCOUNTRY_CST("mg", "127.0.1.194");
IPCOUNTRY_CST("mh", "127.0.2.72");
IPCOUNTRY_CST("mk", "127.0.3.39");
IPCOUNTRY_CST("ml", "127.0.1.210");
IPCOUNTRY_CST("mm", "127.0.0.104");
IPCOUNTRY_CST("mn", "127.0.1.240");
IPCOUNTRY_CST("mo", "127.0.1.190");
IPCOUNTRY_CST("mp", "127.0.2.68");
IPCOUNTRY_CST("mq", "127.0.1.218");
IPCOUNTRY_CST("mr", "127.0.1.222");
IPCOUNTRY_CST("ms", "127.0.1.244");
IPCOUNTRY_CST("mt", "127.0.1.214");
IPCOUNTRY_CST("mu", "127.0.1.224");
IPCOUNTRY_CST("mv", "127.0.1.206");
IPCOUNTRY_CST("mw", "127.0.1.198");
IPCOUNTRY_CST("mx", "127.0.1.228");
IPCOUNTRY_CST("my", "127.0.1.202");
IPCOUNTRY_CST("mz", "127.0.1.252");
IPCOUNTRY_CST("na", "127.0.2.4");
IPCOUNTRY_CST("nc", "127.0.2.28");
IPCOUNTRY_CST("ne", "127.0.2.50");
IPCOUNTRY_CST("nf", "127.0.2.62");
IPCOUNTRY_CST("ng", "127.0.2.54");
IPCOUNTRY_CST("ni", "127.0.2.46");
IPCOUNTRY_CST("nl", "127.0.2.16");
IPCOUNTRY_CST("no", "127.0.2.66");
IPCOUNTRY_CST("np", "127.0.2.12");
IPCOUNTRY_CST("nr", "127.0.2.8");
IPCOUNTRY_CST("nu", "127.0.2.58");
IPCOUNTRY_CST("nz", "127.0.2.42");
IPCOUNTRY_CST("om", "127.0.2.0");
IPCOUNTRY_CST("pa", "127.0.2.79");
IPCOUNTRY_CST("pe", "127.0.2.92");
IPCOUNTRY_CST("pf", "127.0.1.2");
IPCOUNTRY_CST("pg", "127.0.2.86");
IPCOUNTRY_CST("ph", "127.0.2.96");
IPCOUNTRY_CST("pk", "127.0.2.74");
IPCOUNTRY_CST("pl", "127.0.2.104");
IPCOUNTRY_CST("pm", "127.0.2.154");
IPCOUNTRY_CST("pn", "127.0.2.100");
IPCOUNTRY_CST("pr", "127.0.2.118");
IPCOUNTRY_CST("pt", "127.0.2.108");
IPCOUNTRY_CST("pw", "127.0.2.73");
IPCOUNTRY_CST("py", "127.0.2.88");
IPCOUNTRY_CST("qa", "127.0.2.122");
IPCOUNTRY_CST("re", "127.0.2.126");
IPCOUNTRY_CST("ro", "127.0.2.130");
IPCOUNTRY_CST("ru", "127.0.2.131");
IPCOUNTRY_CST("rw", "127.0.2.134");
IPCOUNTRY_CST("sa", "127.0.2.170");
IPCOUNTRY_CST("sb", "127.0.0.90");
IPCOUNTRY_CST("sc", "127.0.2.178");
IPCOUNTRY_CST("sd", "127.0.2.224");
IPCOUNTRY_CST("se", "127.0.2.240");
IPCOUNTRY_CST("sg", "127.0.2.190");
IPCOUNTRY_CST("sh", "127.0.2.142");
IPCOUNTRY_CST("si", "127.0.2.193");
IPCOUNTRY_CST("sj", "127.0.2.232");
IPCOUNTRY_CST("sk", "127.0.2.191");
IPCOUNTRY_CST("sl", "127.0.2.182");
IPCOUNTRY_CST("sm", "127.0.2.162");
IPCOUNTRY_CST("sn", "127.0.2.174");
IPCOUNTRY_CST("so", "127.0.2.194");
IPCOUNTRY_CST("sr", "127.0.2.228");
IPCOUNTRY_CST("st", "127.0.2.166");
IPCOUNTRY_CST("sv", "127.0.0.222");
IPCOUNTRY_CST("sy", "127.0.2.248");
IPCOUNTRY_CST("sz", "127.0.2.236");
IPCOUNTRY_CST("tc", "127.0.3.28");
IPCOUNTRY_CST("td", "127.0.0.148");
IPCOUNTRY_CST("tf", "127.0.1.4");
IPCOUNTRY_CST("tg", "127.0.3.0");
IPCOUNTRY_CST("th", "127.0.2.252");
IPCOUNTRY_CST("tj", "127.0.2.250");
IPCOUNTRY_CST("tk", "127.0.3.4");
IPCOUNTRY_CST("tm", "127.0.3.27");
IPCOUNTRY_CST("tn", "127.0.3.20");
IPCOUNTRY_CST("to", "127.0.3.8");
IPCOUNTRY_CST("tp", "127.0.2.114");
IPCOUNTRY_CST("tr", "127.0.3.24");
IPCOUNTRY_CST("tt", "127.0.3.12");
IPCOUNTRY_CST("tv", "127.0.3.30");
IPCOUNTRY_CST("tw", "127.0.0.158");
IPCOUNTRY_CST("tz", "127.0.3.66");
IPCOUNTRY_CST("ua", "127.0.3.36");
IPCOUNTRY_CST("ug", "127.0.3.32");
IPCOUNTRY_CST("uk", "127.0.3.58");
IPCOUNTRY_CST("um", "127.0.2.69");
IPCOUNTRY_CST("us", "127.0.3.72");
IPCOUNTRY_CST("uy", "127.0.3.90");
IPCOUNTRY_CST("uz", "127.0.3.92");
IPCOUNTRY_CST("va", "127.0.1.80");
IPCOUNTRY_CST("vc", "127.0.2.158");
IPCOUNTRY_CST("ve", "127.0.3.94");
IPCOUNTRY_CST("vg", "127.0.0.92");
IPCOUNTRY_CST("vi", "127.0.3.82");
IPCOUNTRY_CST("vn", "127.0.2.192");
IPCOUNTRY_CST("vu", "127.0.2.36");
IPCOUNTRY_CST("wf", "127.0.3.108");
IPCOUNTRY_CST("ws", "127.0.3.114");
IPCOUNTRY_CST("ye", "127.0.3.119");
IPCOUNTRY_CST("yt", "127.0.0.175");
IPCOUNTRY_CST("yu", "127.0.3.123");
IPCOUNTRY_CST("za", "127.0.2.198");
IPCOUNTRY_CST("zm", "127.0.3.126");
IPCOUNTRY_CST("zr", "127.0.0.180");
IPCOUNTRY_CST("zw", "127.0.2.204");

	// if this point is reached, the ip_addr_t have no match, return an error
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool ipcountry_t::notify_callback(const inet_err_t &inet_err, const std::string &country_code)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ipcountry_cb(userptr, *this, inet_err, country_code);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END

