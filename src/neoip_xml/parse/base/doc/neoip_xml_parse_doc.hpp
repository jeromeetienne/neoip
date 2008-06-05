/*! \file
    \brief Header of the xml_parse_doc_t
    
*/


#ifndef __NEOIP_XML_PARSE_DOC_HPP__ 
#define __NEOIP_XML_PARSE_DOC_HPP__ 
/* system include */
#include <string>
#include <list>
/* local include */
#include "neoip_xml_except.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	datum_t;
class	xml_parse_t;

/** \brief class definition for xml_parse_doc_t
 * 
 * - this allows to parse the xml file
 */
class xml_parse_doc_t : NEOIP_COPY_CTOR_DENY {
private:
	struct _xmlDoc *doc_xml;	//!< to store the xml tree of the whole document

	/*************** xml_parse_t	*******************************/
	std::list<xml_parse_t *>	xml_parse_db;
	void xml_parse_dolink(xml_parse_t *xml_parse)	throw()	{ xml_parse_db.push_back(xml_parse);	}
	void xml_parse_unlink(xml_parse_t *xml_parse)	throw()	{ xml_parse_db.remove(xml_parse);		}
public:
	/*************** ctor/dtor	***************************************/
	xml_parse_doc_t()	throw();
	~xml_parse_doc_t()	throw();

	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return doc_xml == NULL;	}

	/*************** action function	*******************************/
	xml_parse_doc_t &	set_document(const datum_t &xml_datum)	throw();

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const xml_parse_doc_t &xml_parse_doc)	throw()
						{ return os << xml_parse_doc.to_string();	}	

	/*************** List of friend function	***********************/
	friend	class	xml_parse_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XML_PARSE_DOC_HPP__  */
