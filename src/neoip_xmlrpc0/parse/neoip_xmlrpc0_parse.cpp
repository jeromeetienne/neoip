/*! \file
    \brief Implementation of the xmlrpc_parse_t
*/

/* system include */
/* local include */
#include "neoip_xmlrpc_parse.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief consructor of the class
 */
xmlrpc_parse_t::xmlrpc_parse_t()			throw()
{
	// init the document
	doc_xml		= NULL;
	cur_node	= NULL;
}

/** \brief Destructor
 */
xmlrpc_parse_t::~xmlrpc_parse_t()			throw()
{
	// free the XML tree
	if( doc_xml )	xmlFreeDoc(doc_xml);	
}

/** \brief Set the xml document to parse
 * 
 * @return true if an error occur while parsing the XML, false otherwise
 */
bool	xmlrpc_parse_t::set_document(const void *doc_data, size_t doc_len)	throw()
{
	// parse the document and build a xml tree
	// - from libxml doc, "The document being in memory, it have no base per 
	//   RFC 2396, and the "noname.xml" argument will serve as its base."
	doc_xml = xmlReadMemory((char*)doc_data, doc_len, "noname.xml", NULL, 0);
	if( !doc_xml )	return true;
	// log to debug
	// xmlDocFormatDump(stdout, doc_xml, 1);

	// Get the method_node from the xml tree
	xmlNodePtr	node_root	= xmlDocGetRootElement(doc_xml);
	if( get_first_sibling_node(node_root, "methodCall") ){
		// Get the method name
		xmlNodePtr	node_call = get_first_sibling_node(node_root, "methodCall");
		if( !node_call )	return true;
		xmlNodePtr	node_name = get_first_sibling_node(node_call->children, "methodName");
		if( !node_name )	return true;
		xmlNodePtr	node_text = get_first_sibling_node(node_name->children, "text");
		if( !node_text )	return true;
		// copy the method name
		call_method	= (char *)node_text->content;

		KLOG_DBG("call_method=" << call_method);
		
		// set the cur_node to the first <param>
		xmlNodePtr	node_params = get_first_sibling_node(node_call->children, "params");
		KLOG_DBG("node_params=" << node_params);
		if( !node_params )	return true;
		cur_node	= node_params->children;
	}else if( get_first_sibling_node(node_root, "methodResponse") ){
		xmlNodePtr	node_resp = get_first_sibling_node(node_root, "methodResponse");
		if( !node_resp )	return true;	

		DBG_ASSERT( 0 );
	}else{
		// if it isent a methodCall or a methodResponse, report an error
		return true;
	};
	
	// return no error
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       UTILITY function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Go to the first sibling node named 'name'
 */
xmlNodePtr 	xmlrpc_parse_t::get_first_sibling_node(xmlNodePtr node, const std::string &name)
										const throw()
{
	xmlNodePtr 	result;
	// go thru all the next slibing
	for( result = node; result; result = result->next ){
		if( !strcasecmp((char*)result->name, name.c_str()) )
			break;
		// KLUDGE: if the name is 'int', try to look for 'i4' too
		// - <i4> and <int> are alias in the spec
		if( name == "int" && !strcasecmp((char*)result->name, "i4") )	break;	
	}
	return result;
}


/** \brief Go to the first sibling node named 'name'
 */
xmlrpc_parse_t &	xmlrpc_parse_t::goto_first_sibling(const std::string &name)throw(xml_except_t)
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( cur_node );
	// make the current node points to its parent
	cur_node	= get_first_sibling_node(cur_node, name);
	
	// if cur_node is NULL, throw an exception
	if( !cur_node )		nthrow_xmlrpc_plain("Can't find sibling named " + name);

	// return the object itself
	return *this;
}

/** \brief Make the current node to become its next
 */
xmlrpc_parse_t &	xmlrpc_parse_t::goto_next()				throw(xml_except_t)
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( cur_node );
	// make the current node points to its next
	cur_node	= cur_node->next;
	
	// NOTE: it is allowed to goto_next() with no next
	
	// return the object itself
	return *this;
}

/** \brief Make the current node to become its parent
 */
xmlrpc_parse_t &	xmlrpc_parse_t::goto_parent()				throw(xml_except_t)
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( cur_node );
	// make the current node points to its parent
	cur_node	= cur_node->parent;
	
	// if cur_node is NULL, throw an exception
	if( !cur_node )		nthrow_xmlrpc_plain("Can't find parent");

	// return the object itself
	return *this;
}

/** \brief Make the current node to become its children
 */
xmlrpc_parse_t &	xmlrpc_parse_t::goto_children()				throw(xml_except_t)
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( cur_node );
	// make the current node points to its parent
	cur_node	= cur_node->children;
	
	// if cur_node is NULL, throw an exception
	if( !cur_node )		nthrow_xmlrpc_plain("Can't find children");

	// return the object itself
	return *this;
}

/** \brief Return a string of the content of the current node
 * 
 * - it throw an exception if there is none
 */
std::string	xmlrpc_parse_t::get_current_content()				throw(xml_except_t)
{
	// if there is no content, throw an exception
	if( !cur_node->content )	nthrow_xmlrpc_plain("Cant Find a content when one was expected");
	// convert the content into a std::string
	return std::string((char *)cur_node->content);
}

/** \brief Return true if the array has more values available
 * 
 * - this function MUST NOT when not parsing a array
 */
bool	xmlrpc_parse_t::array_has_more_value() 				const throw()
{
	if( get_first_sibling_node(cur_node, "value") )	return true;
	return false;
}

/** \brief Return true if the <params> has more <param> available
 * 
 * - this function MUST NOT when not parsing a <params>
 */
bool	xmlrpc_parse_t::params_has_more_param() 				const throw()
{
	if( get_first_sibling_node(cur_node, "param") )	return true;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    for parsing xmlrpc_t::flag_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::flag_t &flag)
										throw(xml_except_t)
{
	// log to debug
	KLOG_DBG("try to put a flag = " << flag);
	
	switch( flag ){
	case xmlrpc_parse_t::PARAMS_BEGIN:	break;
	case xmlrpc_parse_t::PARAMS_END:	if( xmlrpc_parse.params_has_more_param() )
							nthrow_xmlrpc_plain("Too many <param> in the <params>");
						break;

	case xmlrpc_parse_t::PARAM_BEGIN:	xmlrpc_parse.goto_first_sibling("param");
						xmlrpc_parse.goto_children();
						break;
	case xmlrpc_parse_t::PARAM_END:		xmlrpc_parse.goto_parent();
						xmlrpc_parse.goto_next();
						break;

	case xmlrpc_parse_t::ARRAY_BEGIN:	xmlrpc_parse.goto_first_sibling("value");
						xmlrpc_parse.goto_children();
						xmlrpc_parse.goto_first_sibling("array");
						xmlrpc_parse.goto_children();		
						xmlrpc_parse.goto_first_sibling("data");
						xmlrpc_parse.goto_children();	
						break;
	case xmlrpc_parse_t::ARRAY_END:		if( xmlrpc_parse.array_has_more_value() )
							nthrow_xmlrpc_plain("Too many value in array");
						xmlrpc_parse.goto_parent();
						xmlrpc_parse.goto_parent();
						xmlrpc_parse.goto_parent();
						xmlrpc_parse.goto_next();
						break;


	default:	DBG_ASSERT( 0 );
	}
			
	// return the object itself
	return xmlrpc_parse;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    for parsing basic type
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Parse a int32_t
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, int32_t &value)	throw(xml_except_t)
{
	std::string	text_content;
	// log to debug
	KLOG_DBG("enter");
	// goto the <int> and get its content
	xmlrpc_parse.goto_first_sibling("value").goto_children();
	xmlrpc_parse.goto_first_sibling("int").goto_children();
	xmlrpc_parse.goto_first_sibling("text");
	
	// get the content
	text_content	= xmlrpc_parse.get_current_content();

	// go back
	xmlrpc_parse.goto_parent().goto_parent();
	if( xmlrpc_parse.cur_node->next )	// TODO this command is shit!!!
		xmlrpc_parse.cur_node	= xmlrpc_parse.cur_node->next;

	// convert the content to the value
	value	= atoi(text_content.c_str());
	// return the object itself
	return xmlrpc_parse;
}

/** \brief Parse a bool
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, bool &value)		throw(xml_except_t)
{
	std::string	text_content;
	// log to debug
	KLOG_DBG("enter");
	// goto the <int> and get its content
	xmlrpc_parse.goto_first_sibling("value").goto_children();
	xmlrpc_parse.goto_first_sibling("boolean").goto_children();
	xmlrpc_parse.goto_first_sibling("text");
	
	// get the content
	text_content	= xmlrpc_parse.get_current_content();

	// go back
	xmlrpc_parse.goto_parent().goto_parent();
	if( xmlrpc_parse.cur_node->next )	// TODO this command is shit!!!
		xmlrpc_parse.cur_node	= xmlrpc_parse.cur_node->next;

	// convert the content to the value
	value	= atoi(text_content.c_str());
	// return the object itself
	return xmlrpc_parse;
}

/** \brief Parse a string
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, std::string &value)	throw(xml_except_t)
{
	std::string	text_content;
	// log to debug
	KLOG_DBG("enter");
	// goto the <int> and get its content
	xmlrpc_parse.goto_first_sibling("value").goto_children();
	xmlrpc_parse.goto_first_sibling("string").goto_children();
	xmlrpc_parse.goto_first_sibling("text");

	// get the content
	text_content	= xmlrpc_parse.get_current_content();
	KLOG_DBG("std::string content = " << text_content );

	// go back
	xmlrpc_parse.goto_parent().goto_parent();
	if( xmlrpc_parse.cur_node->next )	// TODO this command is shit!!!
		xmlrpc_parse.cur_node	= xmlrpc_parse.cur_node->next;

	// convert the content to the value
	value	= text_content;
	// return the object itself
	return xmlrpc_parse;
}


/** \brief Parse a double
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, double &value)	throw(xml_except_t)
{
	std::string	text_content;
	// log to debug
	KLOG_DBG("enter");
	// goto the <int> and get its content
	xmlrpc_parse.goto_first_sibling("value").goto_children();
	xmlrpc_parse.goto_first_sibling("double").goto_children();
	xmlrpc_parse.goto_first_sibling("text");

	// get the content
	text_content	= xmlrpc_parse.get_current_content();
	KLOG_DBG("std::string content = " << text_content );

	// go back
	xmlrpc_parse.goto_parent().goto_parent();
	if( xmlrpc_parse.cur_node->next )	// TODO this command is shit!!!
		xmlrpc_parse.cur_node	= xmlrpc_parse.cur_node->next;

	// convert the content to the value
	value	= atof(text_content.c_str());
	// return the object itself
	return xmlrpc_parse;
}

NEOIP_NAMESPACE_END

