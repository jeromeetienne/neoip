/*! \file
    \brief Implementation of the mlink_url_t
*/

/* system include */
/* local include */
#include "neoip_mlink_url.hpp"
#include "neoip_xml_build.hpp"
#include "neoip_xml_parse.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief consructor of the class
 */
mlink_url_t::mlink_url_t()			throw()
{
}

/** \brief Destructor
 */
mlink_url_t::~mlink_url_t()			throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string mlink_url_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "[";
	oss << "[url_content=\""	<< url_content()	<< "\"]";
	if( !type().empty() )		oss << "[type=\""	<< type()	<< "\"]";
	if( !location().empty() )	oss << "[location=\""	<< location()	<< "\"]";
	if( !preference().empty() )	oss << "[preference=\""	<< preference()	<< "\"]";
	oss << "]";
	// return the just built string
	return oss.str();
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    xml build/parse
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator for xml_build_t 
 */
xml_build_t& operator << (xml_build_t & xml_build, const mlink_url_t &mlink_url)	throw()
{
	// build a xml document
	xml_build.element_beg("url");
	if( !mlink_url.type().empty() )		xml_build.element_att("type"		, mlink_url.type());
	if( !mlink_url.location().empty() )	xml_build.element_att("location"	, mlink_url.location());
	if( !mlink_url.preference().empty() )	xml_build.element_att("preference"	, mlink_url.preference());
	xml_build.element_val(mlink_url.url_content());
	xml_build.element_end();
	// return the object itself
	return xml_build;
}

/** \brief Parse a int32_t
 */
xml_parse_t&	operator >> (xml_parse_t & xml_parse_orig, mlink_url_t &mlink_url)	throw(xml_except_t)
{
	// log to debug
	KLOG_DBG("enter");
	// copy the xml_parse_t
	xml_parse_t	xml_parse	= xml_parse_orig;
	// check that the node_name is <url>
	if( xml_parse.node_name() != "url" )	nthrow_xml_plain("Can't find <url> root node");
	// parse the name attribute
	if( !xml_parse.node_attr("type").empty() )	mlink_url.type		( xml_parse.node_attr("type")		);
	if( !xml_parse.node_attr("location").empty() )	mlink_url.location	( xml_parse.node_attr("location")	);
	if( !xml_parse.node_attr("preference").empty())	mlink_url.preference	( xml_parse.node_attr("preference")	);
	// parse some field
	mlink_url.url_content	( string_t::strip(xml_parse.node_content(), " \r\n")	);
	
	// return the object itself
	return xml_parse_orig;
}

NEOIP_NAMESPACE_END

