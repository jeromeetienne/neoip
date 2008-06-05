/*! \file
    \brief Implementation of the mlink_file_t aka the metalink format

- NOTE: this is halfbacked
  - the 'specification' are quite vague so which field are supported and where is 
    still unknown
  - additionnaly metalink is for now only a student project, if it takeoff
    the work for a more rigurous suport may be done.    

- the email of the author: Anthony Bryan <albryan@comcast.net>
  on irc freenode his nick is twanj, and supposed to hang on #suse
  - anthonybryan@gmail.com and is nice
  - group talking about it
    http://groups.google.com/group/metalink-discussion?hl=en

*/

/* system include */
/* local include */
#include "neoip_mlink_file.hpp"
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
mlink_file_t::mlink_file_t()			throw()
{
}

/** \brief Destructor
 */
mlink_file_t::~mlink_file_t()			throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string mlink_file_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "[";
	if( !head_version().empty() )	oss << "[head_version=\""	<< head_version()	<< "\"]";
	if( !head_xmlns().empty() )	oss << "[head_xmlns=\""		<< head_xmlns()		<< "\"]";
	if( !head_origin().empty() )	oss << "[head_origin=\""	<< head_origin()	<< "\"]";
	if( !head_type().empty() )	oss << "[head_type=\""		<< head_type()		<< "\"]";
	if( !head_pubdate().empty() )	oss << "[head_pubdate=\""	<< head_pubdate()	<< "\"]";
	if( !head_refreshdate().empty())oss << "[head_refreshdate=\""	<< head_refreshdate()	<< "\"]";
	if( !head_generator().empty() )	oss << "[head_generator=\""	<< head_generator()	<< "\"]";
	if( !publisher_name().empty() )	oss << "[publisher_name=\""	<< publisher_name()	<< "\"]";
	if( !publisher_url().empty() )	oss << "[publisher_url=\""	<< publisher_url()	<< "\"]";
	if( !license_name().empty() )	oss << "[license_name=\""	<< license_name()	<< "\"]";
	if( !license_url().empty() )	oss << "[license_url=\""	<< license_url()	<< "\"]";
	if( !description().empty() )	oss << "[description=\""	<< description()	<< "\"]";
	if( !tags().empty() )		oss << "[tags=\""		<< tags()		<< "\"]";
	if( !identity().empty() )	oss << "[identity=\""		<< identity()		<< "\"]";
	if( !version().empty() )	oss << "[version=\""		<< version()		<< "\"]";
	if( !subfile_arr().empty())	oss << "[subfile_arr="		<< subfile_arr()	<< "]";
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
xml_build_t& operator << (xml_build_t & xml_build, const mlink_file_t &mlink_file)	throw()
{
	// build a xml document
	xml_build.element_beg("metalink");
	if( !mlink_file.head_version().empty() )	xml_build.element_att("version"	, mlink_file.head_version());
	if( !mlink_file.head_xmlns().empty() )		xml_build.element_att("xmlns"	, mlink_file.head_xmlns());
	if( !mlink_file.head_origin().empty() )		xml_build.element_att("origin"	, mlink_file.head_origin());
	if( !mlink_file.head_type().empty() )		xml_build.element_att("type"	, mlink_file.head_type());
	if( !mlink_file.head_pubdate().empty() )	xml_build.element_att("pubdate"	, mlink_file.head_pubdate());
	if( !mlink_file.head_refreshdate().empty() )	xml_build.element_att("refreshdate", mlink_file.head_refreshdate());
	if( !mlink_file.head_generator().empty() )	xml_build.element_att("generator", mlink_file.head_generator());
	
	// encode the publisher element
	if( !mlink_file.publisher_name().empty() || !mlink_file.publisher_url().empty() ){
		xml_build.element_beg("publisher");
		if( !mlink_file.publisher_name().empty() )
			xml_build.element_full_simple("name", mlink_file.publisher_name());
		if( !mlink_file.publisher_url().empty() )
			xml_build.element_full_simple("url", mlink_file.publisher_url());
		xml_build.element_end();
	}

	// encode the license element
	if( !mlink_file.license_name().empty() || !mlink_file.license_url().empty() ){
		xml_build.element_beg("license");
		if( !mlink_file.license_name().empty() )
			xml_build.element_full_simple("name", mlink_file.license_name());
		if( !mlink_file.license_url().empty() )
			xml_build.element_full_simple("url", mlink_file.license_url());
		xml_build.element_end();
	}

	if( !mlink_file.description().empty() )		xml_build.element_full_simple("description"	, mlink_file.description());
	if( !mlink_file.tags().empty() )		xml_build.element_full_simple("tags"		, mlink_file.tags());
	if( !mlink_file.identity().empty() )		xml_build.element_full_simple("identity"	, mlink_file.identity());
	if( !mlink_file.version().empty() )		xml_build.element_full_simple("version"		, mlink_file.version());

	// encode the subfile_arr
	xml_build	<< mlink_file.subfile_arr();

	xml_build.element_end();
	
	// return the object itself
	return xml_build;
}

/** \brief Parse a int32_t
 */
xml_parse_t&	operator >> (xml_parse_t & xml_parse, mlink_file_t &mlink_file)	throw(xml_except_t)
{
	if( xml_parse.node_name() != "metalink" )	nthrow_xml_plain("Can't find metalink root node");
	// parse the header
	mlink_file.head_version		( xml_parse.node_attr("version")	);
	mlink_file.head_xmlns		( xml_parse.node_attr("xmlns")		);
	mlink_file.head_origin		( xml_parse.node_attr("origin")		);
	mlink_file.head_type		( xml_parse.node_attr("type")		);
	mlink_file.head_pubdate		( xml_parse.node_attr("pubdate")	);
	mlink_file.head_refreshdate	( xml_parse.node_attr("refreshdate")	);
	mlink_file.head_generator	( xml_parse.node_attr("generator")	);
	
	// parse various optionnal body element
	mlink_file.publisher_url	( xml_parse.path_content_opt("/metalink/publisher/url")	);
	mlink_file.publisher_name	( xml_parse.path_content_opt("/metalink/publisher/name"));
	mlink_file.license_url		( xml_parse.path_content_opt("/metalink/license/url")	);
	mlink_file.license_name		( xml_parse.path_content_opt("/metalink/license/name")	);
	mlink_file.description		( xml_parse.path_content_opt("/metalink/description")	);
	mlink_file.tags			( xml_parse.path_content_opt("/metalink/tags")		);
	mlink_file.identity		( xml_parse.path_content_opt("/metalink/identity")	);
	mlink_file.version		( xml_parse.path_content_opt("/metalink/vdersion")	);

	// goto the "/metalink/files" children
	xml_parse.goto_path("/metalink/files");
	xml_parse >> mlink_file.subfile_arr();

	// return the object itself
	return xml_parse;
}

NEOIP_NAMESPACE_END

