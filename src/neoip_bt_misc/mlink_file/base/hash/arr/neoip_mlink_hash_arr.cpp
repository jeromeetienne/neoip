/*! \file
    \brief Definition of the \ref mlink_hash_arr_t
    
*/

/* system include */
/* local include */
#include "neoip_mlink_hash_arr.hpp"
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
xml_build_t& operator << (xml_build_t & xml_build, const mlink_hash_arr_t &hash_arr)	throw()
{
	// build a xml document
	xml_build.element_beg("verification");
	for(size_t i = 0; i < hash_arr.size(); i++)		xml_build << hash_arr[i];
	xml_build.element_end();	
	// return the object itself
	return xml_build;
}

/** \brief Parse a int32_t
 */
xml_parse_t&	operator >> (xml_parse_t & xml_parse_orig, mlink_hash_arr_t &hash_arr)	throw(xml_except_t)
{
	// log to debug
	KLOG_WARN("enter");
	// copy the xml_parse_t
	xml_parse_t	xml_parse	= xml_parse_orig;
	// check that the node_name is <verification>
	if( xml_parse.node_name() != "verification" )	nthrow_xml_plain("Can't find <verification> root node");
	// goto children
	xml_parse.goto_children();
	// if it doesnt contains any <hash>, throw an exception
	if( !xml_parse.has_firstsib("hash") )	nthrow_xml_plain("Can't find a single <hash> node");
	// loop until there is no more <hash> in this <resources>
	while( xml_parse.has_firstsib("hash") ){
		// goto this <hash>
		xml_parse.goto_firstsib("hash");
		// parse the mlink_hash_t
		mlink_hash_t	mlink_hash;
		xml_parse >> mlink_hash;
		// add this mlink_hash_t to the hash_arr
		hash_arr	+= mlink_hash;
		// goto the next sibling
		xml_parse.goto_nextsib();
	}
	// return the object itself
	return xml_parse_orig;
}

NEOIP_NAMESPACE_END;






