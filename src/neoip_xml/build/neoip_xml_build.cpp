/*! \file
    \brief Implementation of the xml_build_t

- TODO how come that xml_parse_t is copiable but NOT xml_build_t ??
  - look a lot like an oversight
  - to check and if it is confirmed make xml_build_t copiable too

*/

/* system include */
#if 1	// NOTE: to enable if libxml2 will be linked statically, disable otherwise
	// - libxml has a bug/feature. if this code is planned to be linked with a statically
	//   linked libxml, this code MUST #define LIBXML_STATIC
	// - this cause a layer violation because the .o of this source needs to be recompiled
	//   depending on how they are linked.
#	define LIBXML_STATIC
#endif
#include <libxml/parser.h>
#include <libxml/tree.h>
#ifdef _WIN32
#	undef ERROR
#	undef INFINITE
#	undef OPTIONAL
#endif
/* local include */
#include "neoip_xml_build.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief consructor of the class
 */
xml_build_t::xml_build_t()			throw()
{
	// init the document
	doc_xml		= xmlNewDoc(BAD_CAST"1.0");
	cur_node	= NULL;
}

/** \brief Destructor
 */
xml_build_t::~xml_build_t()			throw()
{
	// free the XML tree
	xmlFreeDoc(doc_xml);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return the xml datum_t resulting of the xml_built_t
 */
datum_t	xml_build_t::to_datum()		const throw()
{
	xmlChar*doc_mem;
	int	doc_mem_size;
	
	// get the built tree in memory allocated by libxml
	xmlDocDumpFormatMemory(doc_xml, &doc_mem, &doc_mem_size, 0);

	// copy the xml into a datum
	datum_t	xml_output((void*)doc_mem, doc_mem_size);

	// free the memory allocated by libxml
	xmlFree(doc_mem);

	// return the datum
	return xml_output;	
}

/** \brief Return the xml std::string resulting of the xml_built_t
 */
std::string	xml_build_t::to_stdstring()		const throw()
{
	return to_datum().to_stdstring();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start a new element 
 */
xml_build_t &	xml_build_t::element_beg(const std::string &name)		throw()
{
	xmlNodePtr	tmp_node;
	// create the new node
	tmp_node	= xmlNewNode(NULL, BAD_CAST name.c_str());
	// add this tmp_node as child of the cur_node
	// - if no cur_node, set this tmp_node as root_node
	if( cur_node )	xmlAddChild(cur_node, tmp_node);
	else		xmlDocSetRootElement(doc_xml, tmp_node);
	// set the tmp_node as the cur_node
	cur_node	= tmp_node;
	// return the object itself
	return *this;
}

/** \brief Start a new element 
 */
xml_build_t &	xml_build_t::element_att(const std::string &key, const std::string &val)	throw()
{
	// sanity check - the cur_node MUST NOT be NULL
	DBG_ASSERT( cur_node );
	// add this attributes to the cur_node
	xmlNewProp(cur_node, BAD_CAST key.c_str(), BAD_CAST val.c_str());

	// return the object itself
	return *this;
}

/** \brief Set the value of the current element
 */
xml_build_t &	xml_build_t::element_val(const std::string &value)				throw()
{
	xmlNodePtr	tmp_node;
	// sanity check - the cur_node MUST NOT be NULL
	DBG_ASSERT( cur_node );
	// create the text node
	tmp_node	= xmlNewText(BAD_CAST value.c_str());
	// add this text node to the current one
	xmlAddChild(cur_node, tmp_node);
	// return the object itself
	return *this;
}

/** \brief End the current element
 */
xml_build_t &	xml_build_t::element_end()							throw()
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( cur_node );	
	// make the current node points to its parent
	cur_node	= cur_node->parent;

	// return the object itself
	return *this;
}

/** \brief Add a full element with a simple value
 */
xml_build_t &	xml_build_t::element_full_simple(const std::string &name, const std::string &value)	throw()
{
	element_beg(name);
		element_val(value);
	element_end();
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string xml_build_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << to_stdstring();
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END

