/*! \file
    \brief Definition of the \ref mlink_subfile_arr_t
    
*/

/* system include */
/* local include */
#include "neoip_mlink_subfile_arr.hpp"
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
xml_build_t& operator << (xml_build_t & xml_build, const mlink_subfile_arr_t &subfile_arr)	throw()
{
	// build a xml document
	xml_build.element_beg("files");
	for(size_t i = 0; i < subfile_arr.size(); i++)		xml_build << subfile_arr[i];
	xml_build.element_end();	
	// return the object itself
	return xml_build;
}

/** \brief Parse a int32_t
 */
xml_parse_t&	operator >> (xml_parse_t & xml_parse_orig, mlink_subfile_arr_t &subfile_arr)	throw(xml_except_t)
{
	// log to debug
	KLOG_DBG("enter");
	// copy the xml_parse_t
	xml_parse_t	xml_parse	= xml_parse_orig;
	// check that the node_name is "files"
	if( xml_parse.node_name() != "files" )	nthrow_xml_plain("Can't find <files> root node");
	// goto children
	xml_parse.goto_children();
	// if it doesnt contains any <file>, throw an exception
	if( !xml_parse.has_firstsib("file") )	nthrow_xml_plain("Can't find a single <file> node");
	// loop until there is no more <file> in this <files>
	while( xml_parse.has_firstsib("file") ){
		// goto this <file>
		xml_parse.goto_firstsib("file");
		// parse the mlink_subfile_t
		mlink_subfile_t	mlink_subfile;
		xml_parse >> mlink_subfile;
		// add this mlink_subfile_t to the subfile_arr
		subfile_arr	+= mlink_subfile;
		// goto the next sibling
		xml_parse.goto_nextsib();
	}
	// return the object itself
	return xml_parse_orig;
}

NEOIP_NAMESPACE_END;






