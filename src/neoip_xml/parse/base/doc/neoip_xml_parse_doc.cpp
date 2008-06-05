/*! \file
    \brief Implementation of the xml_parse_doc_t
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
#include "neoip_xml_parse_doc.hpp"
#include "neoip_datum.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief function to intercept the libxml2 error
 */
static void   local_xmlGenericErrorFunc(void * ctx, const char *fmt, ...)
{
	char	str[1024];
	va_list ap;
	// dump the var arg into the str
	va_start(ap, fmt);
	vsnprintf(str, sizeof(str), fmt, ap);
	va_end(ap);
	// display the error via the local logging system
	KLOG_DBG(str);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief consructor of the class
 */
xml_parse_doc_t::xml_parse_doc_t()			throw()
{
	// init the document
	doc_xml		= NULL;
}

/** \brief Destructor
 */
xml_parse_doc_t::~xml_parse_doc_t()			throw()
{
	// free the XML tree
	if( doc_xml )	xmlFreeDoc(doc_xml);	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the document to parse from a datum_t
 * 
 * @return the object itself, it is null if the parsing failed
 */
xml_parse_doc_t &	xml_parse_doc_t::set_document(const datum_t &xml_datum)	throw()
{
	// sanity check - doc_xml MUST NOT be init
	DBG_ASSERT( !doc_xml );

	// parse the document and build a xml tree
	// - from libxml doc, "The document being in memory, it have no base per 
	//   RFC 2396, and the "noname.xml" argument will serve as its base."
	doc_xml = xmlReadMemory((char*)xml_datum.get_data(), xml_datum.get_len(), "noname.xml", NULL, 0);
	// if the parsing failed, return now
	if( !doc_xml )	return *this;

	// this is a libxml2 call to set a custum error function
	// - this is required or the libxml2 dump its error direct on the console
	// - not a clean thing for a library
	xmlGenericErrorFunc	fct	= local_xmlGenericErrorFunc;
	initGenericErrorDefaultFunc(&fct);
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
std::string xml_parse_doc_t::to_string()			const throw()
{
	xmlChar*	doc_mem;
	int		doc_mem_size;
	// get the built tree in memory allocated by libxml
	xmlDocDumpFormatMemory(doc_xml, &doc_mem, &doc_mem_size, 0);
	// copy the xml into a std::string
	std::string	result((char*)doc_mem, doc_mem_size);
	// free the memory allocated by libxml
	xmlFree(doc_mem);	
	// return the just built string
	return result;
}

NEOIP_NAMESPACE_END



