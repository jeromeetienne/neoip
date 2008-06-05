/*! \file
    \brief Header of the xmlrpc_build_t

*/


#ifndef __NEOIP_XMLRPC_BUILD_HPP__ 
#define __NEOIP_XMLRPC_BUILD_HPP__ 
/* system include */
/* local include */
#include "neoip_xml_build.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief class definition for xmlrpc_build_t
 * 
 * - this allows to build the xmlrpc call or response
 * - it is up to the caller to ensure the produced xml match the spec.
 *   - it may be found at http://www.xmlrpc.com/spec
 *   - i.e. this doesnt do anything about stuff like
 *     - 'a response MUST NOT contains more than one param'
 *     - 'Multiple values MUST be in a array'
 */
class xmlrpc_build_t : NEOIP_COPY_CTOR_DENY {
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
	public:	std::string	str;
		CALL_BEG(const std::string &str)		throw() : str(str) {}
	};

	//! Special structure for FAULT - provide nicer API to caller by holding parameter in flag
	class FAULT {
	public:	int32_t		code;
		std::string	str;
		FAULT(int32_t code, const std::string &str)	throw() : code(code), str(str) {}
	};
	
	//! Special structure for MEMBER_BEG - provide nicer API to caller by holding parameter in flag
	class MEMBER_BEG {
	public:	std::string	str;
		MEMBER_BEG(const std::string &str)		throw() : str(str) {}
	};
private:
	xml_build_t	xml_build;	//!< object to build the underlying xml
public:
	/*************** ctor/dtor	***************************************/
	xmlrpc_build_t()	throw();
	~xmlrpc_build_t()	throw();

	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return xml_build.is_null();		}
	datum_t		to_datum()	const throw()	{ return xml_build.to_datum();		}
	std::string	to_stdstring()	const throw()	{ return xml_build.to_stdstring();	}

	/*************** function to build the internal flags	***************/
	friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::flag_t &flag)		throw();
	friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::CALL_BEG &call_beg)	throw();
	friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::FAULT &fault)		throw();
	friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::MEMBER_BEG &member_beg)throw();

	/*************** function to build basic types	***********************/
	friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const int32_t &val)	throw();
	friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const bool &val)	throw();
	friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const std::string &val)throw();
	friend	xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const double &val)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_BUILD_HPP__  */



