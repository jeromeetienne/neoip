/*! \file
    \brief Implementation of the xmlrpc_build_t
*/

/* system include */
/* local include */
#include "neoip_xmlrpc_build.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief consructor of the class
 */
xmlrpc_build_t::xmlrpc_build_t()			throw()
{
	// init the document
	doc_xml		= xmlNewDoc(BAD_CAST"1.0");
	cur_node	= NULL;
}

/** \brief Destructor
 */
xmlrpc_build_t::~xmlrpc_build_t()			throw()
{
	// free the XML tree
	xmlFreeDoc(doc_xml);	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set this xmlrpc_build_t for building a call to the <method>
 * 
 * - from the spec, "The <methodCall> must contain a <methodName> sub-item, 
 *   a string, containing the name of the method to be called. "
 * - from the spec, "If the procedure call has parameters, the <methodCall>
 *   must contain a <params> sub-item. The <params> sub-item can contain any
 *   number of <param>s, each of which has a <value>."
 */
void xmlrpc_build_t::start_call(const std::string &method)	throw()
{
	cur_node	= xmlNewNode(NULL, BAD_CAST"methodCall");
	xmlDocSetRootElement(doc_xml, cur_node);
	
	create_child("methodName", method);
	goto_parent();
	create_child("params");
}

/** \brief Set this xmlrpc_build_t for building a response with params
 * 
 * - from the spec "The body of the response is a single XML structure, a 
 *   <methodResponse>, which can contain a single <params> which contains
 *   a single <param> which contains a single <value>."
 */
void xmlrpc_build_t::start_resp()				throw()
{
	// set the root
	cur_node	= xmlNewNode(NULL, BAD_CAST"methodResponse");
	xmlDocSetRootElement(doc_xml, cur_node);

	create_child("params");
}

/** \brief Set this xmlrpc_build_t for building a response with fault
 */
void xmlrpc_build_t::put_fault(int32_t faultCode, const std::string &faultString)	throw()
{
	// set the root
	cur_node	= xmlNewNode(NULL, BAD_CAST"methodResponse");
	xmlDocSetRootElement(doc_xml, cur_node);

	this->create_child("fault");
		this->create_child("value");
			*this << xmlrpc_build_t::STRUCT_BEGIN;
				*this << xmlrpc_build_t::MEMBER_BEGIN("faultCode");
					*this << faultCode;
				*this << xmlrpc_build_t::MEMBER_END;
				*this << xmlrpc_build_t::MEMBER_BEGIN("faultString");
					*this << faultString;
				*this << xmlrpc_build_t::MEMBER_END;
			*this << xmlrpc_build_t::STRUCT_END;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     tree walking function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Create a child node below the current node
 */
xmlrpc_build_t &	xmlrpc_build_t::create_child(const std::string &tag_name
						, const std::string &tag_content)	throw()
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( cur_node );
	
	// determine the content of this child
	const char *	content	= NULL;
	if( !tag_content.empty() )	content	= tag_content.c_str();

	// create the child node just below the current node
	cur_node	= xmlNewChild(cur_node, NULL, BAD_CAST(tag_name.c_str()), BAD_CAST(content));

	// return the object itself
	return *this;
}

/** \brief Make the current node to become its parent
 */
xmlrpc_build_t &	xmlrpc_build_t::goto_parent()			throw()
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( cur_node );
	
	// make the current node points to its parent
	cur_node	= cur_node->parent;
	
	// sanity check - the current node MUST be not null
	DBG_ASSERT( cur_node );	
	// return the object itself
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    GET RESULT
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return the xml output of the buildonse
 */
datum_t	xmlrpc_build_t::get_xml_output()		const throw()
{
	xmlChar*doc_mem;
	int	doc_mem_size;
	// log to debug
	//xmlDocFormatDump(stdout, doc_xml, 1);	
	
	// get the built tree in memory allocated by libxml
	xmlDocDumpFormatMemory(doc_xml, &doc_mem, &doc_mem_size, 0);
	// copy the xml into a datum
	datum_t	xml_output((void*)doc_mem, doc_mem_size);
	// free the memory allocated by libxml
	xmlFree(doc_mem);
	// return the datum
	return xml_output;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    for building xmlrpc_t::flag_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::flag_t &flag)	throw()
{
	// log to debug
	KLOG_DBG("try to put a flag = " << flag);
	
	switch( flag ){
	///////////// PARAM /////////////
	case xmlrpc_build_t::PARAM_BEGIN:	xmlrpc_build.create_child("param");
						break;
	case xmlrpc_build_t::PARAM_END:		xmlrpc_build.goto_parent();
						break;
	///////////// ARRAY ////////////
	case xmlrpc_build_t::ARRAY_BEGIN:	xmlrpc_build.create_child("array");
						xmlrpc_build.create_child("data");
						break;
	case xmlrpc_build_t::ARRAY_END:		xmlrpc_build.goto_parent();
						xmlrpc_build.goto_parent();
						break;
	///////////// STRUCT ///////////
	case xmlrpc_build_t::STRUCT_BEGIN:	xmlrpc_build.create_child("struct");
						break;
	case xmlrpc_build_t::STRUCT_END:	xmlrpc_build.goto_parent();
						break;

	///////////// MEMBER ///////////
	case xmlrpc_build_t::MEMBER_END:	xmlrpc_build.goto_parent();
						break;

	default:	DBG_ASSERT( 0 );
	}
			
	// return the object itself
	return xmlrpc_build;
}

/** \brief overload the << operator
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build
			, const xmlrpc_build_t::MEMBER_BEGIN &member_beg)	throw()
{
	// log to debug
	KLOG_DBG("try to begin a member called = " << member_beg.member_name);
	
	xmlrpc_build.create_child("member");
		xmlrpc_build.create_child("name", member_beg.member_name);
		xmlrpc_build.goto_parent();
	// return the object itself
	return xmlrpc_build;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    for building basic type
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator for int32_t
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const int32_t &val)	throw()
{
	// log to debug
	KLOG_DBG("try to put a int=" << val);

	// put the int in the tree
	xmlrpc_build.create_child("value");
		xmlrpc_build.create_child("int", OSTREAMSTR(val));
		xmlrpc_build.goto_parent();
	xmlrpc_build.goto_parent();
	// return the object itself
	return xmlrpc_build;
}

/** \brief overload the << operator for bool
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const bool &val)	throw()
{
	// log to debug
	KLOG_DBG("try to put a bool=" << val);

	// put the int in the tree
	xmlrpc_build.create_child("value");
		xmlrpc_build.create_child("boolean", val ? "1" : "0");
		xmlrpc_build.goto_parent();
	xmlrpc_build.goto_parent();
	// return the object itself
	return xmlrpc_build;
}

/** \brief overload the << operator for std::string
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const std::string &val)	throw()
{
	// log to debug
	KLOG_DBG("try to put a std::string=" << val);

	// put the int in the tree
	xmlrpc_build.create_child("value");
		xmlrpc_build.create_child("string", OSTREAMSTR(val));
		xmlrpc_build.goto_parent();
	xmlrpc_build.goto_parent();
	// return the object itself
	return xmlrpc_build;
}

/** \brief overload the << operator for double
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const double &val)	throw()
{
	// log to debug
	KLOG_DBG("try to put a double=" << val);

	// put the int in the tree
	xmlrpc_build.create_child("value");
		xmlrpc_build.create_child("double", OSTREAMSTR(val));
		xmlrpc_build.goto_parent();
	xmlrpc_build.goto_parent();
	// return the object itself
	return xmlrpc_build;
}
NEOIP_NAMESPACE_END

