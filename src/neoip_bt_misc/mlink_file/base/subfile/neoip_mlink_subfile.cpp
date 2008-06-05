/*! \file
    \brief Implementation of the mlink_subfile_t
*/

/* system include */
/* local include */
#include "neoip_mlink_subfile.hpp"
#include "neoip_xml_build.hpp"
#include "neoip_xml_parse.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief consructor of the class
 */
mlink_subfile_t::mlink_subfile_t()			throw()
{
}

/** \brief Destructor
 */
mlink_subfile_t::~mlink_subfile_t()			throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string mlink_subfile_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "[";
	oss << "[name=\""	<< name()	<< "\"]";
	if( !version().empty() )	oss << "[version=\""	<< version()	<< "\"]";
	if( !os().empty() )		oss << "[os=\""		<< os()		<< "\"]";
	if( !language().empty() )	oss << "[language=\""	<< language()	<< "\"]";
	if( !file_size().is_null() )	oss << "[file_size=\""	<< file_size()	<< "\"]";
	oss << url_arr();
	oss << hash_arr();
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
xml_build_t& operator << (xml_build_t & xml_build, const mlink_subfile_t &mlink_subfile)	throw()
{
	// build a xml document
	xml_build.element_beg("file");
	if( !mlink_subfile.name().is_null() )	xml_build.element_att("name"		, mlink_subfile.name().to_string());
	if( !mlink_subfile.version().empty() )	xml_build.element_full_simple("version"	, mlink_subfile.version());
	if( !mlink_subfile.os().empty() )	xml_build.element_full_simple("os"	, mlink_subfile.os());
	if( !mlink_subfile.language().empty() )	xml_build.element_full_simple("language", mlink_subfile.language());
	if(!mlink_subfile.file_size().is_null())xml_build.element_full_simple("size"	, mlink_subfile.file_size().to_string());
	xml_build	<< mlink_subfile.url_arr();
	xml_build	<< mlink_subfile.hash_arr();
	xml_build.element_end();
	// return the object itself
	return xml_build;
}

/** \brief Parse a int32_t
 */
xml_parse_t&	operator >> (xml_parse_t & xml_parse_orig, mlink_subfile_t &mlink_subfile)	throw(xml_except_t)
{
	// log to debug
	KLOG_DBG("enter");
	// copy the xml_parse_t
	xml_parse_t	xml_parse	= xml_parse_orig;
	// check that the node_name is "file"
	if( xml_parse.node_name() != "file" )	nthrow_xml_plain("Can't find <file> root node");
	// check that the name attribute exists
	if(xml_parse.node_attr("name").empty())	nthrow_xml_plain("Can't find <name> attribute in subfile");
	// parse the name attribute
	mlink_subfile.name	( xml_parse.node_attr("name")	);	
	// goto children
	xml_parse.goto_children();

	// parse some field
	mlink_subfile.os	( xml_parse.path_content_opt("os")	);
	mlink_subfile.version	( xml_parse.path_content_opt("version")	);
	mlink_subfile.language	( xml_parse.path_content_opt("language"));
	// parse the file_size
	std::string size_str = xml_parse.path_content_opt("size");
	if( !size_str.empty() )	mlink_subfile.file_size	( atoi(size_str.c_str()) );

	// goto the <rescources> children
	xml_parse.goto_root().goto_children().goto_path("resources");
	xml_parse >> mlink_subfile.url_arr();

	// parse the <verification> if present
	if( xml_parse.goto_root().goto_children().has_path("verification") ){
		xml_parse.goto_root().goto_children().goto_path("verification");
		xml_parse >> mlink_subfile.hash_arr();
	}

	// return the object itself
	return xml_parse_orig;
}

NEOIP_NAMESPACE_END

