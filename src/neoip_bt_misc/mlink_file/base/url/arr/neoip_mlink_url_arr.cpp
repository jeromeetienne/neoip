/*! \file
    \brief Definition of the \ref mlink_url_arr_t
    
*/

/* system include */
/* local include */
#include "neoip_mlink_url_arr.hpp"
#include "neoip_xml_build.hpp"
#include "neoip_xml_parse.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    xml build/parse
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator for xml_build_t 
 */
xml_build_t& operator << (xml_build_t & xml_build, const mlink_url_arr_t &url_arr)	throw()
{
	// build a xml document
	xml_build.element_beg("resources");
	for(size_t i = 0; i < url_arr.size(); i++)		xml_build << url_arr[i];
	xml_build.element_end();	
	// return the object itself
	return xml_build;
}

/** \brief Parse a int32_t
 */
xml_parse_t&	operator >> (xml_parse_t & xml_parse_orig, mlink_url_arr_t &url_arr)	throw(xml_except_t)
{
	// log to debug
	KLOG_DBG("enter");
	// copy the xml_parse_t
	xml_parse_t	xml_parse	= xml_parse_orig;
	// check that the node_name is <resources>
	if( xml_parse.node_name() != "resources" )	nthrow_xml_plain("Can't find <resources> root node");
	// goto children
	xml_parse.goto_children();
	// if it doesnt contains any <url>, throw an exception
	if( !xml_parse.has_firstsib("url") )	nthrow_xml_plain("Can't find a single <url> node");
	// loop until there is no more <url> in this <resources>
	while( xml_parse.has_firstsib("url") ){
		// goto this <url>
		xml_parse.goto_firstsib("url");
		// parse the mlink_url_t
		mlink_url_t	mlink_url;
		xml_parse >> mlink_url;
		// add this mlink_url_t to the url_arr
		url_arr	+= mlink_url;
		// goto the next sibling
		xml_parse.goto_nextsib();
	}
	// return the object itself
	return xml_parse_orig;
}

NEOIP_NAMESPACE_END;






