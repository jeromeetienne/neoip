/*! \file
    \brief Definition of the \ref upnp_portdesc_t

*/


/* system include */
/* local include */
#include "neoip_upnp_portdesc.hpp"
#include "neoip_upnp_portcleaner.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_endianswap.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


#define COMMAND_FOR_EACH_FIELD( COMMAND )		\
	COMMAND( ipport_addr_t	, ipport_pview)		\
	COMMAND( upnp_sockfam_t	, sockfam)		\
	COMMAND( ipport_addr_t	, ipport_lview)		\
	COMMAND( bool		, map_enabled)		\
	COMMAND( std::string	, desc_str)		\
	COMMAND( delay_t	, lease_delay)		\
	COMMAND( delay_t	, portcleaner_lease)	\
	COMMAND( std::string	, portcleaner_nonce)
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			convertion
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a upnp_portdesc_t from a strvar_db_t 
 *
 * - this is the strvar_db_t replied from a upnp_call_t
 */
upnp_portdesc_t	upnp_portdesc_t::from_strvar_db(const strvar_db_t &strvar_db, bool revendian)	throw()
{	
	upnp_portdesc_t	portdesc;
	// log to debug
	KLOG_DBG("enter revendian=" << revendian << " strvar_db=" << strvar_db);

	/*************** parse the NewExternalPort port	***********************/
	if( strvar_db.contain_key("NewExternalPort") ){
		// try to convert the content into a port
		uint16_t port_pview	= atoi(strvar_db.get_first_value("NewExternalPort").c_str());
		if( revendian )		port_pview = endianswap_t::swap16(port_pview);

		/*************** parse the NewExternalHost ip_addr_t	***************/
		if( !strvar_db.contain_key("NewRemoteHost") )	return upnp_portdesc_t();
		// try to convert the content into a port
		std::string	ipaddr_pview_str	= strvar_db.get_first_value("NewRemoteHost");
		ip_addr_t	ipaddr_pview		= ipaddr_pview_str.size() ? ipaddr_pview_str : "0.0.0.0";
		if( revendian && ipaddr_pview.is_v4() )
			ipaddr_pview	= ip_addr_t(endianswap_t::swap32(ipaddr_pview.get_v4_addr()));
	
		// copy the result into the internal variable
		portdesc.ipport_pview	( ipport_addr_t(ipaddr_pview, port_pview) );
	}

	/*************** parse the NewProtocol upnp_sockfam_t	***************/
	if( strvar_db.contain_key("NewProtocol") ){
		std::string str	= strvar_db.get_first_value("NewProtocol");
		portdesc.sockfam	( upnp_sockfam_t::from_string_nocase(str) );
	}
	
	/*************** parse the NewInternalPort port	***********************/
	if( !strvar_db.contain_key("NewInternalPort") )		return upnp_portdesc_t();
	// try to convert the content into a port
	uint16_t port_lview	= atoi(strvar_db.get_first_value("NewInternalPort").c_str());
	if( revendian )		port_lview = endianswap_t::swap16(port_lview);

	/*************** parse the NewInternalClient ip_addr_t	***************/
	if( !strvar_db.contain_key("NewInternalClient") )	return upnp_portdesc_t();
	// try to convert the content into a ip_addr_t
	ip_addr_t ipaddr_lview	= strvar_db.get_first_value("NewInternalClient");
	if( revendian && ipaddr_lview.is_v4() )
		ipaddr_lview = ip_addr_t(endianswap_t::swap32(ipaddr_lview.get_v4_addr()));
	
	// if the content can't be parse as an ip_addr_t, notify an error
	if( !ipaddr_lview.is_fully_qualified() )		return upnp_portdesc_t();

	// copy the result into the internal variable
	portdesc.ipport_lview	( ipport_addr_t(ipaddr_lview, port_lview) );
	DBG_ASSERT( portdesc.ipport_lview().is_fully_qualified() );

	/*************** parse the NewEnabled	*******************************/
	if( !strvar_db.contain_key("NewEnabled") )		return upnp_portdesc_t();
	// try to convert the content into a std::string
	portdesc.map_enabled	( atoi(strvar_db.get_first_value("NewEnabled").c_str()) ? true : false ); 

	/*************** parse the NewPortMappingDescription	***************/
	if( !strvar_db.contain_key("NewPortMappingDescription"))return upnp_portdesc_t();	
	portdesc.desc_str	( strvar_db.get_first_value("NewPortMappingDescription") );

	/*************** parse the portcleaner_tag if needed	***************/
	delay_t		portcleaner_lease;
	std::string	portcleaner_nonce;
	std::string	real_desc	= upnp_portcleaner_t::parse_tag_desc(portdesc.desc_str()
						, portcleaner_lease, portcleaner_nonce);
	portdesc.desc_str		( real_desc		);
	portdesc.portcleaner_lease	( portcleaner_lease	);
	portdesc.portcleaner_nonce	( portcleaner_nonce	);

	/*************** parse the NewLeaseDuration	***********************/
	if( !strvar_db.contain_key("NewLeaseDuration") )	return upnp_portdesc_t();
	std::string	lease_delay_str	= strvar_db.get_first_value("NewLeaseDuration");
	delay_t		lease_delay	= delay_t::from_sec(atoi(lease_delay_str.c_str()));
	// handle the special case of 0sec means INFINITE
	if( lease_delay == delay_t::from_sec(0) )	lease_delay = delay_t::INFINITE;
	portdesc.lease_delay	( lease_delay );
	
	// sanity check - here portdesc MUST NOT be null
	DBG_ASSERT( !portdesc.is_null() );
	// return the just built upnp_portdesc_t
	return portdesc;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 upnp_portdesc_t and return value ala strcmp/memcmp
 */
int	upnp_portdesc_t::compare(const upnp_portdesc_t &other)	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;

	// define the macro
	#define CMD(var_type, var_name)					\
		if( var_name() < other.var_name() )	return -1;	\
		if( var_name() > other.var_name() )	return +1;
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD

	// at this point, both are considered equal, so return 0
	return 0;
}
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	upnp_portdesc_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	// define the macro
	#define CMD(var_type, var_name)				\
		if( !oss.str().empty() )	oss << " ";	\
		oss << #var_name << "=" << var_name();
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	// return the just built string
	return oss.str();
}

/** \brief convert the object to a string more 'human readable'
 * 
 * - NOTE: it may contains \n or other 'beautifuller'
 */
std::string	upnp_portdesc_t::to_human_str()	const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << sockfam() << " " << ipport_pview() << " -> " << ipport_lview();
	
	oss << "\t";
	oss << "desc='" << desc_str() << "'";
	oss << "\n";
	oss << "\tlease_delay=" << lease_delay();
	oss << "\tmap_enabled=" << (map_enabled() ? "yes" : "no");
	
	// display the portcleaner part
	oss << "\tPortcleaner_tag(";
	if( has_portcleaner_tag() ){
		oss << "lease of " << portcleaner_lease() << " with nonce " << portcleaner_nonce();
	}else{
		oss << "not present";
	}
	oss << ")";
	// return the just built string
	return oss.str();
}


NEOIP_NAMESPACE_END

