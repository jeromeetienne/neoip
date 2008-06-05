/*! \file
    \brief Header of the xmlrpc_parse_t

*/


#ifndef __NEOIP_XMLRPC_PARSE_HPP__ 
#define __NEOIP_XMLRPC_PARSE_HPP__ 
/* system include */
/* local include */
#include "neoip_xml_parse.hpp"
#include "neoip_xml_parse_doc.hpp"
#include "neoip_datum.hpp"
#include "neoip_except.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief class definition for xmlrpc_parse_t
 * 
 * - this allows to parse the xmlrpc call or response
 */
class xmlrpc_parse_t : NEOIP_COPY_CTOR_DENY {
public:	enum flag_t {
		//CALL_BEG,		// See special case below to get a parameter in the flag
		CALL_END,
		RESP_BEG,
		RESP_END,
		//FAULT,		// See special case below to get a parameter in the flag
		PARAM_BEG,
		PARAM_END,
		ARRAY_BEG,
		ARRAY_END,
		STRUCT_BEG,
		STRUCT_END,
		//MEMBER_BEG,		// See special case below to get a parameter in the flag
		MEMBER_END,
	};
	
	//! Special structure for CALL_BEG - provide nicer API to caller by holding parameter in the flag
	class CALL_BEG {
	public:	std::string *	str_ptr;
		CALL_BEG(std::string &str)		throw() : str_ptr(&str) {}
	};

	//! Special structure for FAULT - provide nicer API to caller by holding parameter in flag
	class FAULT {
	public:	int32_t	*	code_ptr;
		std::string *	str_ptr;
		FAULT(int32_t &code, std::string &str)	throw() : code_ptr(&code), str_ptr(&str) {}
	};
	
	//! Special structure for MEMBER_BEG - provide nicer API to caller by holding parameter in flag
	class MEMBER_BEG {
	public:	std::string	str;
		MEMBER_BEG(const std::string &str)	throw() : str(str) {}
	};

	//! Special structure for VALUE_TO_JSON - to convert a value into json format
	class VALUE_TO_JSON {
	public:	std::ostringstream &	oss_json;
		VALUE_TO_JSON(std::ostringstream &oss_json)	throw() : oss_json(oss_json) {}
	};
private:
	xml_parse_doc_t	xml_parse_doc;	//!< the xml document to parse
	xml_parse_t	xml_parse;	//!< the xml parser itself
	
	/*************** Internal function	*******************************/
	bool		try_find_member(const std::string &member_name)	throw(xml_except_t);
	std::list<std::string>	all_member_name()			throw(xml_except_t);
	static std::string	escape_json_string(const std::string &str)	throw();

	/*************** entoken_only handling	*******************************/
	bool		endtoken_only;	//!< true if only endtoken may be accepted
	void		header_for_endtoken()		throw();
	void		header_non_endtoken()		throw(xml_except_t);
	void		goto_nextsib_optional()		throw();
	void		goto_children_optional()	throw();
public:
	/*************** ctor/dtor	***************************************/
	xmlrpc_parse_t()	throw();
	~xmlrpc_parse_t()	throw();

	/*************** setup function	***************************************/
	xmlrpc_parse_t &	set_document(const datum_t &xmlenc_datum)	throw();

	/*************** query function	***************************************/
	bool		is_null()		const throw()	{ return xml_parse_doc.is_null();	}
	bool		has_more_sibling()	const throw()	{ return !endtoken_only;		}
	std::string	value_type()					throw(xml_except_t);
	bool		is_fault_resp()					throw(xml_except_t);
	bool		has_member(const std::string &member_name)	throw(xml_except_t);
	
	/*************** function to parse the internal flags	***************/
	friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::flag_t &flag)		throw(xml_except_t);
	friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::CALL_BEG &call_beg)	throw(xml_except_t);
	friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::FAULT &fault)		throw(xml_except_t);
	friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::MEMBER_BEG &member_beg)throw(xml_except_t);
	friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::VALUE_TO_JSON &stt)	throw(xml_except_t);

	/*************** function to parse basic types	***********************/
	friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, int32_t &val)	throw(xml_except_t);
	friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, bool &val)		throw(xml_except_t);
	friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, std::string &val)	throw(xml_except_t);
	friend	xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, double &val)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_PARSE_HPP__  */



