/*! \file
    \brief Header of the xml_build_t
    
*/


#ifndef __NEOIP_XML_BUILD_HPP__ 
#define __NEOIP_XML_BUILD_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	datum_t;

/** \brief class definition for xml_build_t
 * 
 * - this allows to build the xml file
 */
class xml_build_t : NEOIP_COPY_CTOR_DENY {
private:
	struct _xmlDoc *doc_xml;	//!< to store the xml tree of the whole document
	struct _xmlNode*cur_node;	//!< the cursor node used while building the tree
public:
	/*************** ctor/dtor	***************************************/
	xml_build_t()	throw();
	~xml_build_t()	throw();

	/*************** query function	***************************************/
	bool		is_null()		const throw()	{ return cur_node == NULL;	}
	datum_t		to_datum()		const throw();
	std::string	to_stdstring()		const throw();

	/*************** action function	*******************************/
	xml_build_t &	element_beg(const std::string &name)					throw();
	xml_build_t &	element_att(const std::string &value1, const std::string &value2)	throw();
	xml_build_t &	element_val(const std::string &value)					throw();
	xml_build_t &	element_end()								throw();
	
	xml_build_t &	element_full_simple(const std::string &name, const std::string &value)	throw();

	/*************** display function	*******************************/	
	std::string	to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const xml_build_t &xml_build)	throw()
						{ return os << xml_build.to_string();	}	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XML_BUILD_HPP__  */



