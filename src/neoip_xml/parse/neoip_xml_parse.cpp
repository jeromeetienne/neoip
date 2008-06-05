/*! \file
    \brief Implementation of the xml_parse_t

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
#include "neoip_xml_parse.hpp"
#include "neoip_xml_parse_doc.hpp"
#include "neoip_file_path.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Internal contructor
 */
void	xml_parse_t::internal_ctor(xml_parse_doc_t *parse_doc, xmlNodePtr root_node, xmlNodePtr curr_node)	throw()
{
	// sanity check - the root_node MUST be non-null
	DBG_ASSERT(root_node);
	// init the document
	this->parse_doc	= parse_doc;
	this->root_node	= root_node;
	this->curr_node	= curr_node;
	// link this object to the xml_parse_doc_t
	parse_doc->xml_parse_dolink(this);
}

/** \brief Internal destructor
 */
void	xml_parse_t::internal_dtor()		throw()
{
	// unlink this object from the xml_parse_doc_t
	if( parse_doc )	parse_doc->xml_parse_unlink(this);
}

/** \brief default Constructor from a xml_parse_doc_t
 */
xml_parse_t::xml_parse_t()			throw()
{
	parse_doc	= NULL;
}


/** \brief Constructor from a xml_parse_doc_t
 */
xml_parse_t::xml_parse_t(xml_parse_doc_t *parse_doc)	throw()
{
	xmlDocPtr	doc_xml	= parse_doc->doc_xml;
	internal_ctor(parse_doc, xmlDocGetRootElement(doc_xml), xmlDocGetRootElement(doc_xml));
}

/** \brief Copy Operator
 * 
 * - NOTE: the curr_node of the other become the root node of this one
 */
xml_parse_t::xml_parse_t(const xml_parse_t &other)	throw()
{
	internal_ctor(other.parse_doc, other.curr_node, other.curr_node);
}

/** \brief assignement operator
 * 
 * - NOTE: the curr_node of the other become the root node of this one
 */
xml_parse_t &xml_parse_t::operator = (const xml_parse_t &other)			throw()
{
	// if the assignment over itself, do nothing
	if( this == &other )	return *this;
	// destruct the old value
	internal_dtor();
	// initiator with the new values
	internal_ctor(other.parse_doc, other.curr_node, other.curr_node);
	// return a reference over itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a pointer on the first sibling of this name, NULL if none matches
 */
xmlNodePtr	xml_parse_t::get_firstsib_node(xmlNodePtr start_node	
					, const std::string &node_name)	const throw()
{
	xmlNodePtr	tmp_node	= start_node;
	// sanity check - the start_node MUST NOT be null
	DBG_ASSERT( start_node );
	do{
		// if this tmp_node has node_name as name
		if( xmlStrcmp(tmp_node->name, BAD_CAST node_name.c_str()) == 0 )
			return tmp_node;
		// goto the next sibling
		tmp_node	= tmp_node->next;
	}while( tmp_node );
	// if this point is reached, the start_node HAS NO sibling of this name, return NULL
	return NULL;
}

/** \brief return a pointer on the first sibling of which is not this name, NULL if none matches
 */
xmlNodePtr	xml_parse_t::get_firstsib_neg_node(xmlNodePtr start_node
					, const std::string &node_name)		const throw()
{
	xmlNodePtr	tmp_node	= start_node;
	// sanity check - the start_node MUST NOT be null
	DBG_ASSERT( start_node );
	do{
		// if this tmp_node HAS NOT node_name as name
		if( xmlStrcmp(tmp_node->name, BAD_CAST node_name.c_str()) )
			return tmp_node;
		// goto the next sibling
		tmp_node	= tmp_node->next;
	}while( tmp_node );
	// if this point is reached, the start_node HAS NO sibling of this name, return NULL
	return NULL;
}
/** \brief return a pointer on the node which matches this path, NULL if none matches
 * 
 * - if the file_path_t is absolute, start from the root node
 *   if the file_path_t is relative, start from the curr_node
 */
xmlNodePtr	xml_parse_t::get_path_node(const file_path_t &node_path)	const throw()
{
	xmlNodePtr	tmp_node;
	// set the tmp_node to root_node if node_path is absolute, to curr_node otherwise
	if( node_path.is_absolute() )	tmp_node = root_node;
	else				tmp_node = curr_node;
	// sanity check - the file_path_t MUST contain at least one name
	DBG_ASSERT( node_path.size() > 1 );
	// go thru the whole file_path_t
	for(size_t i = 1; i < node_path.size(); i++){
		// log to debug
		KLOG_DBG("path=" << node_path << " level=" << i << " name=" << node_path[i]);
		// goto the children node if it is not the first name
		if( i > 1 )	tmp_node	= tmp_node->children;
		if( !tmp_node )	return NULL;
		// get the first sibling with this name
		tmp_node	= get_firstsib_node(tmp_node, node_path[i].to_string());
		if( !tmp_node )	return NULL;
	}
	// if this point is reached, the file_path_t has been found, return its pointer
	return tmp_node;
}


/** \brief Return the content of the node pointed at this node_path, or an empty string
 * 
 * - NOTE: it doesnt change the curr_node
 */
std::string	xml_parse_t::get_content_node(xmlNodePtr tmp_node)	const throw()
{
	xmlDocPtr	doc_xml	= parse_doc->doc_xml;	
	xmlChar *	content	= xmlNodeListGetString(doc_xml, tmp_node->xmlChildrenNode, 1);
	// if content is NULL, return an empty string
	if( !content )	return std::string();
	// copy the content into a std::string
	std::string	result((char *)content);
	// free the content
	xmlFree(content);
	// return the result
	return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the xml_parse_t is on the root node, false otherwise
 */
bool	xml_parse_t::on_root()	const throw()
{
	return curr_node == root_node;
}

/** \brief return true if the curr_node has previous sibling, false otherwise
 */
bool	xml_parse_t::has_prevsib()						const throw()
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );
	// return true if the curr_node has previous sibling, false otherwise
	return curr_node->prev != NULL;		
}

/** \brief return true if the curr_node has next sibling, false otherwise
 */
bool	xml_parse_t::has_nextsib()						const throw()
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );
	// return true if the curr_node has next sibling, false otherwise
	return curr_node->next != NULL;		
}

/** \brief return true if the curr_node has children, false otherwise
 */
bool	xml_parse_t::has_children()						const throw()
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );
	// return true if the curr_node has children, false otherwise
	return curr_node->children != NULL;		
}

/** \brief return true if the curr_node has parent, false otherwise
 */
bool	xml_parse_t::has_parent()						const throw()
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );
	// return true if the curr_node has parent, false otherwise
	return curr_node->parent != NULL;		
}

/** \brief return true if the current node or any of its sibling HAS node_name as name
 *         false otherwise
 */
bool	xml_parse_t::has_firstsib(const std::string &node_name)		const throw()
{
	return get_firstsib_node(curr_node, node_name) != NULL;	
}

/** \brief return true if the current node or any of its sibling HAS NOT node_name as name
 *         false otherwise
 */
bool	xml_parse_t::has_firstsib_neg(const std::string &node_name)	const throw()
{
	return get_firstsib_neg_node(curr_node, node_name) != NULL;	
}

/** \brief return true if the current node has this path
 * 
 * - if the file_path_t is absolute, start from the root node
 *   if the file_path_t is relative, start from the curr_node
 */
bool	xml_parse_t::has_path(const file_path_t &node_path)	const throw()
{
	return get_path_node(node_path) != NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a string of the content of the current node
 * 
 * - return an empty string if there is none
 */
std::string	xml_parse_t::node_content()					const throw()
{
	return get_content_node(curr_node);
}

/** \brief Return a string of the name of the current node
 * 
 * - return an empty string if there is none
 */
std::string	xml_parse_t::node_name()					const throw()
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );	
	// if there is no name, return an empty string
	if( !curr_node->name )	return std::string();
	// convert the content into a std::string
	return std::string((char *)curr_node->name);
}

/** \brief Return a string of the value of this attribute for the current node
 * 
 * - return an empty string if there is none
 */
std::string	xml_parse_t::node_attr(const std::string &key)			const throw()
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );
	// get the attribute value
	xmlChar*	attr_val	= xmlGetProp(curr_node, BAD_CAST key.c_str());
	// if no attribute is found, return an empty string
	if( attr_val == NULL )	return std::string();
	// convert the attr_val to a cpp std::string
	std::string	val		= (char *)attr_val;
	// free the memory allocated by attribute
	xmlFree(attr_val);		
	// convert the content into a std::string
	return val;
}

/** \brief Return the content of the node pointed at this node_path, or an empty string
 * 
 * - NOTE: it doesnt change the curr_node
 */
std::string	xml_parse_t::path_content_opt(const file_path_t &node_path, const std::string &dfl_value)	const throw()
{
	// if the node_path is not present, return the default value
	if( !has_path(node_path) )	return dfl_value;
	// get the xmlNodePtr for this node_path
	xmlNodePtr	tmp_node	= get_path_node(node_path);
	DBG_ASSERT( tmp_node );
	// return the content
	return get_content_node(tmp_node);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the curr_node to the root node
 */
xml_parse_t &	xml_parse_t::goto_root()	throw()
{
	// Set the curr_node to the root element
	curr_node	= root_node;
	// return the object itself
	return *this;	
}

/** \brief Set the curr_node to the previous sibling node
 */
xml_parse_t &	xml_parse_t::goto_prevsib()	throw(xml_except_t)
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );
	// make the current node points to its previous sibling
	curr_node	= curr_node->prev;
	// if curr_node is NULL, throw an exception
	if( !curr_node )		nthrow_xml_plain("Can't find prevsib");	
	// return the object itself
	return *this;	
}

/** \brief Set the curr_node to the next sibling node
 */
xml_parse_t &	xml_parse_t::goto_nextsib()	throw(xml_except_t)
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );
	// make the current node points to its next sibling
	curr_node	= curr_node->next;
	// if curr_node is NULL, throw an exception
	if( !curr_node )		nthrow_xml_plain("Can't find nextsib");	
	// return the object itself
	return *this;	
}

/** \brief Set the curr_node to the children node
 */
xml_parse_t &	xml_parse_t::goto_children()	throw(xml_except_t)
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );
	// make the current node points to its children
	curr_node	= curr_node->children;	
	// if curr_node is NULL, throw an exception
	if( !curr_node )		nthrow_xml_plain("Can't find children");	
	// return the object itself
	return *this;	
}

/** \brief Set the curr_node to the parent node
 */
xml_parse_t &	xml_parse_t::goto_parent()	throw(xml_except_t)
{
	// sanity check - the current node MUST be not null
	DBG_ASSERT( curr_node );
	// if curr_node is already the relatative root_node, throw an exception
	if( curr_node == root_node )	nthrow_xml_plain("Can't goto relative root_node parent");
	// make the current node points to its children
	curr_node	= curr_node->parent;	
	// if curr_node is NULL, throw an exception
	if( !curr_node )		nthrow_xml_plain("Can't find parent");
	// return the object itself
	return *this;	
}

/** \brief Goto the first sibling of this name
 */
xml_parse_t &	xml_parse_t::goto_firstsib(const std::string &name)	throw(xml_except_t)
{
	// get the path
	curr_node	= get_firstsib_node(curr_node, name);
	// if curr_node is NULL, throw an exception
	if( !curr_node )	nthrow_xml_plain("Can't find firstsib named '" + name + "'");	
	// return the object itself
	return *this;	
}

/** \brief Goto the first sibling which has not this name
 */
xml_parse_t &	xml_parse_t::goto_firstsib_neg(const std::string &name)	throw(xml_except_t)
{
	// get the path
	curr_node	= get_firstsib_neg_node(curr_node, name);
	// if curr_node is NULL, throw an exception
	if( !curr_node )	nthrow_xml_plain("Can't find firstsib not named '" + name + "'");	
	// return the object itself
	return *this;	
}


/** \brief return true if the current node has this path
 * 
 * - if the file_path_t is absolute, start from the root node
 *   if the file_path_t is relative, start from the curr_node
 */
xml_parse_t &	xml_parse_t::goto_path(const file_path_t &node_path)		throw(xml_except_t)
{
	// get the path
	curr_node	= get_path_node(node_path);
	// if curr_node is NULL, throw an exception
	if( !curr_node )	nthrow_xml_plain("Can't find path");	
	// return the object itself
	return *this;	
}

NEOIP_NAMESPACE_END

