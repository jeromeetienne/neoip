/*! \file
    \brief Header of the xmlrpc_build_t

- see \ref neoip_xmlrpc_build.cpp

*/


#ifndef __NEOIP_XMLRPC_BUILD_HPP__ 
#define __NEOIP_XMLRPC_BUILD_HPP__ 
/* system include */
#include <libxml/parser.h>
#include <libxml/tree.h>
/* local include */
#include "neoip_datum.hpp"
#include "neoip_except.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief class definition for xmlrpc_build_t
 * 
 * - this allows to build the xmlrpc call or response
 * - it is up to the caller to ensure the produced xml match the spec.
 *   - i.e. this doesnt do anything about stuff like
 *     - 'a response MUST NOT contains more than one param'
 *     - 'Multiple values MUST be in a array'
 */
class xmlrpc_build_t {
public:	enum flag_t {
		PARAM_BEGIN,
		PARAM_END,
		ARRAY_BEGIN,
		ARRAY_END,
		STRUCT_BEGIN,
		STRUCT_END,
		//MEMBER_BEGIN,		// hacked below to get the name of the member
		MEMBER_END,
	};
	
	class MEMBER_BEGIN {	// TODO to comment this silly trick to get nicer API
	public:	std::string	member_name;
		MEMBER_BEGIN(const std::string &member_name)	throw()
			: member_name(member_name) {}
	};
	
private:
	xmlDocPtr	doc_xml;	//!< to store the xml tree of the whole document
	xmlNodePtr	cur_node;	//!< the cursor node used while building the tree
public:
	// ctor/dtor
	xmlrpc_build_t()	throw();
	~xmlrpc_build_t()	throw();

	// setup function
	void	start_call(const std::string &method_name)			throw();
	void	start_resp()							throw();
	void	put_fault(int32_t faultCode, const std::string &faultStr)	throw();

	// to walk the xml tree
	xmlrpc_build_t &create_child(const std::string &tag_name, const std::string &tag_content = "") throw();
	xmlrpc_build_t &goto_parent()			throw();

	// to get the result	
	datum_t		get_xml_output()	const throw();

	// function to build the internal flags
friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::flag_t &flag)
												throw();
friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build
					, const xmlrpc_build_t::MEMBER_BEGIN &member_beg)	throw();

	// function to build basic types
friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const int32_t &val)		throw();
friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const bool &val)		throw();
friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const std::string &val)	throw();
friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const double &val)		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_BUILD_HPP__  */



