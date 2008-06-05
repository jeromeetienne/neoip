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
}

/** \brief Destructor
 */
xmlrpc_build_t::~xmlrpc_build_t()			throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			function to build flag WITHOUT parameter
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a given flag (those without parameters)
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::flag_t &flag) throw()
{
	xml_build_t &	xml_build	= xmlrpc_build.xml_build;
	// log to debug
	KLOG_DBG("enter");
	switch( flag ){
	/*************** CALL stuff	***************************************/
	// xmlrpc_build_t::CALL_BEG is handled separatly as it requires parameter
	case xmlrpc_build_t::CALL_END:		xml_build.element_end();
						break;
	/*************** RESP stuff	***************************************/
	case xmlrpc_build_t::RESP_BEG:		xml_build.element_beg("methodResponse");
						xml_build	.element_beg("params");
						xml_build		.element_beg("param");
						break;
	case xmlrpc_build_t::RESP_END:		xml_build		.element_end();
						xml_build	.element_end();
						xml_build.element_end();
						break;
	/*************** FAULT stuff	***************************************/
	// xmlrpc_build_t::FAULT is handled separatly as it requires parameter

	/*************** PARAM stuff	***************************************/
	case xmlrpc_build_t::PARAM_BEG:	xml_build.element_beg("param");
						break;
	case xmlrpc_build_t::PARAM_END:	xml_build.element_end();
						break;
	/*************** ARRAY stuff	***************************************/
	case xmlrpc_build_t::ARRAY_BEG:	xml_build.element_beg("value");
						xml_build	.element_beg("array");
						xml_build		.element_beg("data");
						break;
	case xmlrpc_build_t::ARRAY_END:	xml_build		.element_end();
						xml_build	.element_end();
						xml_build.element_end();
						break;
	/*************** STRUCT stuff	***************************************/
	case xmlrpc_build_t::STRUCT_BEG:	xml_build.element_beg("value");
						xml_build	.element_beg("struct");
						break;
	case xmlrpc_build_t::STRUCT_END:	xml_build	.element_end();
						xml_build.element_end();
						break;
	/*************** MEMBER stuff	***************************************/
	// xmlrpc_build_t::MEMBER_BEG is handled separatly as it requires parameter
	case xmlrpc_build_t::MEMBER_END:	xml_build.element_end();
						break;
	default:	DBG_ASSERT( 0 );
	}
			
	// return the object itself
	return xmlrpc_build;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			function to build flag WITH parameter
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a CALL_BEG
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::CALL_BEG &call_beg) throw()
{
	xml_build_t &	xml_build	= xmlrpc_build.xml_build;
	// log to debug
	KLOG_DBG("enter");
	// build the xml
	xml_build.element_beg("methodCall");
	xml_build	.element_beg("methodName");
	xml_build		.element_val(call_beg.str);
	xml_build	.element_end();
	xml_build	.element_beg("params");
	// return the object itself
	return xmlrpc_build;
}

/** \brief Build a FAULT
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::FAULT &fault) throw()
{
	xml_build_t &	xml_build	= xmlrpc_build.xml_build;
	// log to debug
	KLOG_DBG("enter");
	// build the xml
	xml_build.element_beg("methodResponse");
	xml_build	.element_beg("fault");
	xmlrpc_build		<< xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_BEG("faultCode");
	xmlrpc_build				<< fault.code;
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_BEG("faultString");
	xmlrpc_build				<< fault.str;
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build		<< xmlrpc_build_t::STRUCT_END;
	xml_build	.element_end();
	xml_build.element_end();
	// return the object itself
	return xmlrpc_build;
}

/** \brief Build a MEMBER_BEG
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const xmlrpc_build_t::MEMBER_BEG &member_beg) throw()
{
	xml_build_t &	xml_build	= xmlrpc_build.xml_build;
	// log to debug
	KLOG_DBG("enter");
	// build the xml
	xml_build.element_beg("member");
	xml_build	.element_beg("name");
	xml_build		.element_val(member_beg.str);
	xml_build	.element_end();
	// return the object itself
	return xmlrpc_build;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    building for building basic type
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator for int32_t
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const int32_t &val)	throw()
{
	xml_build_t &	xml_build	= xmlrpc_build.xml_build;
	// log to debug
	KLOG_DBG("enter");
	// build the value
	xml_build.element_beg("value");
	xml_build	.element_beg("int");
	xml_build		.element_val(OSTREAMSTR(val));
	xml_build	.element_end();
	xml_build.element_end();
	// return the object itself
	return xmlrpc_build;
}

/** \brief overload the << operator for bool
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const bool &val)	throw()
{
	xml_build_t &	xml_build	= xmlrpc_build.xml_build;
	// log to debug
	KLOG_DBG("enter");
	// build the value
	xml_build.element_beg("value");
	xml_build	.element_beg("boolean");
	xml_build		.element_val(val ? "1" : "0");
	xml_build	.element_end();
	xml_build.element_end();
	// return the object itself
	return xmlrpc_build;
}

/** \brief overload the << operator for string
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const std::string &val)	throw()
{
	xml_build_t &	xml_build	= xmlrpc_build.xml_build;
	// log to debug
	KLOG_DBG("enter");
	// build the value
	xml_build.element_beg("value");
	xml_build	.element_beg("string");
	xml_build		.element_val(val);
	xml_build	.element_end();
	xml_build.element_end();
	// return the object itself
	return xmlrpc_build;
}

/** \brief overload the << operator for double
 */
xmlrpc_build_t& operator << (xmlrpc_build_t& xmlrpc_build, const double &val)	throw()
{
	xml_build_t &		xml_build	= xmlrpc_build.xml_build;
	// build the string containing the double
	// - from the spec:
	//   Q. What is the legal syntax (and range) for floating point values (doubles)? How is
	//      the exponent represented? How to deal with whitespace? Can infinity and "not a 
	//	number" be represented?
	//   A. There is no representation for infinity or negative infinity or "not a number". 
	//      At this time, only decimal point notation is allowed, a plus or a minus, followed
	//      by any number of numeric characters, followed by a period and any number of numeric
	//      characters. Whitespace is not allowed. The range of allowable values is
	//      implementation-dependent, is not specified.
	// - this is 'solved' by setting an 'infinit' precision for the std::ostringstream
	std::ostringstream	oss;
	oss.precision(20);
	oss << val;
	// log to debug
	KLOG_DBG("enter");
	// build the value
	xml_build.element_beg("value");
	xml_build	.element_beg("double");
	xml_build		.element_val(oss.str());
	xml_build	.element_end();
	xml_build.element_end();
	// return the object itself
	return xmlrpc_build;
}

NEOIP_NAMESPACE_END

