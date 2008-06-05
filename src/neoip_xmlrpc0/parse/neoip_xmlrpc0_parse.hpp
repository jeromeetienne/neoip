/*! \file
    \brief Header of the xmlrpc_parse_t

- see \ref neoip_xmlrpc_parse.cpp

*/


#ifndef __NEOIP_XMLRPC_PARSE_HPP__ 
#define __NEOIP_XMLRPC_PARSE_HPP__ 
/* system include */
#include <libxml/parser.h>
#include <libxml/tree.h>
/* local include */
#include "neoip_datum.hpp"
#include "neoip_xmlrpc_except.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief class definition for xmlrpc_parse_t
 */
class xmlrpc_parse_t {
public:	enum flag_t {
		PARAM_BEGIN,
		PARAM_END,
		PARAMS_BEGIN,
		PARAMS_END,
		ARRAY_BEGIN,
		ARRAY_END,
		STRUCT_BEGIN,
		STRUCT_END,
		//MEMBER_BEGIN,		// hacked below to get the name of the member
		MEMBER_END,
	};	
private:
	xmlDocPtr	doc_xml;	//!< to store the xml tree of the whole document
	xmlNodePtr	cur_node;	//!< the cursor node used while parseing the tree
	
	std::string	call_method;	//!< the method specified in the call (empty if it is a response)

	// utility function	
	xmlNodePtr 	get_first_sibling_node(xmlNodePtr node, const std::string &name) const throw();
public:
	// ctor/dtor
	xmlrpc_parse_t()	throw();
	~xmlrpc_parse_t()	throw();
	
	
	// setup function
	bool	set_document(const void *doc_data, size_t doc_len)	throw();
	bool	set_document(const datum_t &doc_datum)			throw()
		{ return set_document(doc_datum.get_data(), doc_datum.get_len());	}

	xmlrpc_parse_t &goto_first_sibling(const std::string &name) 		throw(xml_except_t);
	xmlrpc_parse_t &goto_next()				 		throw(xml_except_t);
	xmlrpc_parse_t &goto_parent()				 		throw(xml_except_t);
	xmlrpc_parse_t &goto_children()				 		throw(xml_except_t);
	std::string	get_current_content()					throw(xml_except_t);

	// query function
	bool	is_call()	const throw()	{ return call_method.empty() ? false : true;	}
	bool	is_response()	const throw()	{ return call_method.empty() ? true : false;	}
	const std::string &	get_call_method()	const throw()	{ return call_method;	}

	bool	array_has_more_value() 				const throw();
	bool	params_has_more_param() 			const throw();

friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::flag_t &flag)
										throw(xml_except_t);
	// function to parse basic types
friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, int32_t &val)	throw(xml_except_t);
friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, bool &val)		throw(xml_except_t);
friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, std::string &val)	throw(xml_except_t);
friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, double &val)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_PARSE_HPP__  */



