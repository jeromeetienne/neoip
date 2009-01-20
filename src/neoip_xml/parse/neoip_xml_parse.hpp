/*! \file
    \brief Header of the xml_parse_t
    
*/


#ifndef __NEOIP_XML_PARSE_HPP__ 
#define __NEOIP_XML_PARSE_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_xml_except.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	xml_parse_doc_t;
class	file_path_t;

/** \brief class definition for xml_parse_t
 * 
 * - this allows to parse the xml file
 */
class xml_parse_t : NEOIP_COPY_CTOR_ALLOW {
private:
	xml_parse_doc_t*parse_doc;	//!< to store the xml tree of the whole document
	struct _xmlNode*root_node;	//!< the relative root_node within the xml tree
	struct _xmlNode*curr_node;	//!< the cursor node used while parseing the tree

	/*************** Internal function	*******************************/
	void		internal_ctor(xml_parse_doc_t *parse_doc, struct _xmlNode* root_node1
							, struct _xmlNode* root_node2)	throw();
	void		internal_dtor()							throw();
	struct _xmlNode*get_path_node(const file_path_t &node_path)			const throw();
	struct _xmlNode*get_firstsib_node(struct _xmlNode * start_node
							, const std::string &node_name)	const throw();
	struct _xmlNode*get_firstsib_neg_node(struct _xmlNode * start_node
							, const std::string &node_name)	const throw();
	std::string	get_content_node(struct _xmlNode * start_node)			const throw();
public:
	/*************** ctor/dtor	***************************************/
	xml_parse_t(xml_parse_doc_t *parse_doc)	throw();
	xml_parse_t()				throw();
	~xml_parse_t()				throw()	{ internal_dtor();	}
	xml_parse_t(const xml_parse_t &other)	throw();
	xml_parse_t &	operator = (const xml_parse_t &other)			throw();	

	/*************** query function	***************************************/
	xml_parse_doc_t*get_parse_doc()	const throw()	{ return parse_doc;	}
	bool		on_root()	const throw();
	bool		has_prevsib()	const throw();
	bool		has_nextsib()	const throw();
	bool		has_children()	const throw();
	bool		has_parent()	const throw();
	bool		has_path(const file_path_t &node_path)		const throw();
	bool		has_firstsib(const std::string &name)		const throw();
	bool		has_firstsib_neg(const std::string &name)	const throw();

	std::string	node_content()					const throw();
	std::string	node_name()					const throw();
	std::string	node_attr(const std::string &key)		const throw();
	std::string	path_content_opt(const file_path_t &node_path, const std::string &dfl_value = std::string())
									const throw();

	/*************** action function	*******************************/
	xml_parse_t &	goto_root()				throw();
	xml_parse_t &	goto_prevsib()				throw(xml_except_t);
	xml_parse_t &	goto_nextsib()				throw(xml_except_t);
	xml_parse_t &	goto_children()				throw(xml_except_t);
	xml_parse_t &	goto_parent()				throw(xml_except_t);
	xml_parse_t &	goto_firstsib(const std::string &name)	throw(xml_except_t);
	xml_parse_t &	goto_firstsib_neg(const std::string &name)throw(xml_except_t);
	xml_parse_t &	goto_path(const file_path_t &node_path)	throw(xml_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XML_PARSE_HPP__  */



