/*! \file
    \brief Implementation of the mlink_hash_t
*/

/* system include */
/* local include */
#include "neoip_mlink_hash.hpp"
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
mlink_hash_t::mlink_hash_t()			throw()
{
}

/** \brief Destructor
 */
mlink_hash_t::~mlink_hash_t()			throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string mlink_hash_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "[";
	oss << "[content=\""	<< content()	<< "\"]";
	if( !type().empty() )		oss << "[type=\""	<< type()	<< "\"]";
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
xml_build_t& operator << (xml_build_t & xml_build, const mlink_hash_t &mlink_hash)	throw()
{
	// build a xml document
	xml_build.element_beg("hash");
	if( !mlink_hash.type().empty() )	xml_build.element_att("type"		, mlink_hash.type());
	xml_build.element_val(mlink_hash.content());
	xml_build.element_end();
	// return the object itself
	return xml_build;
}

/** \brief Parse a int32_t
 */
xml_parse_t&	operator >> (xml_parse_t & xml_parse_orig, mlink_hash_t &mlink_hash)	throw(xml_except_t)
{
	// log to debug
	KLOG_WARN("enter");
	// copy the xml_parse_t
	xml_parse_t	xml_parse	= xml_parse_orig;
	// check that the node_name is <hash>
	if( xml_parse.node_name() != "hash" )	nthrow_xml_plain("Can't find <hash> root node");
	// parse the type attribute
	if( !xml_parse.node_attr("type").empty() )	mlink_hash.type	( xml_parse.node_attr("type")	);
	// parse some field
	mlink_hash.content	( xml_parse.node_content()	);

	// return the object itself
	return xml_parse_orig;
}


NEOIP_NAMESPACE_END

