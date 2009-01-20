/*! \file
    \brief Implementation of the xmlrpc_parse_t

- TODO fix the double case and the only dotted decimal stuff
  - find the c++ format for this case
  - and add it in the nunit consistency

\par Note about parsing and how it is done
- xmlrpc_parse_t init a xml_parse_t on the document and use this one for the whole parsing.
- xml_parse_t always 'points' on the node which is the next to parse by xmlrpc_parse_t.
  - this cause trouble to determine if there are tokens when they are optional
- NOTE: the function value_type() helps parsing the value which have variable
  type. quite frequent on new language.

\par Note about the implementation of the STRUCT
- from the spec
  "Q. Does the "struct" element keep the order of keys. Or in other words, is the 
      struct "foo=1, bar=2" equivalent to "bar=2, foo=1" or not?
   A. The struct element does not preserve the order of the keys. The two structs
      are equivalent."
- to implement this, each ::has_member(str) or >> MEMBER_BEG(str) require to 
  go thru all the members of the struct to find if one is interesting
- while parsing a STRUCT, between member, the xmlrpc_parse_t always point to the 
  main "struct" node.
- ::has_member(member_name) allow to test the presence of a given member
  - this helps parsing the structure with optionnal member

\par Note about detecting fault or param in response
- RESP_BEG/RESP_END are reserved for successfull response
  - they consume the <params><param> token too
  - similarly the building  of RESP_BEG/RESP_END add the <params><param> too
- to determine if xmlrpc_parse_t contains a fault, the caller may test it
  via the xmlrpc_parse.is_fault_resp()
  - it MUST be done *before* doing any >> RESP_BEG, as it is reserved for 
    positive answer
- note that is_fault_resp() may trigger xml_except_t if the document is not
  a valid response.
- if it is a fault
  if( xmlrpc_parse.is_fault_resp() ){
  	xmlrpc_parse >> xmlrpc_parse_t::FAULT(code, str);
  }else{
  	xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
  	xmlrpc_parse	>> myinteger
  	xmlrpc_parse >> xmlrpc_parse_t::RESP_END;
  }

\par Note about detecting the next token and endtoken_only flag
- as xmlrpc_parse_t is only on the next node, what to do when there are no next node ?
  - e.g. <params></params> or <array></array> are legal
  - or at the end of a filled array <array><value><int>42</int></value><value><int>99</int></value></array>
    the caller need to know when all the element of the array have been read
  - similarly for <param>
- this is handled by a internal state called endtoken_only flag
  - if endtoken_only is set, only endtoken will be accepted by xmlrpc_parse_t
    - non endtoken will throw exception
  - the endtoken are : CALL_END, RESP_END, PARAM_END, ARRAY_END, STRUCT_END, MEMBER_END
  - the non endtoken are all the rest.
- The set of rules is:
  - after a <value></value> is read, one should goto the next sibling
    - if it doesnt exist, leave the xml_parse_t untouched and set endtoken_only to true
    - coded in goto_nextsib_optional()
  - after a <params> or <arrays> or any other which may or may not contains children, do the same
    - if it doesnt exist, leave the xml_parse_t untouched and set endtoken_only to true
    - coded in goto_children_optional()
  - at the begining of each 'non endtoken' parsing, check that endtoken_only IS NOT set
    - coded in header_non_endtoken()
  - at the begining of each 'endtoken' parsing, set endtoken_only to false
    - coded in header_for_endtoken();
- to read an array up to the end, the caller do something like: 
  - similarly for call with variable number of param
  - example:
    xmlrpc_parse >> xmlrpc_parse_t::ARRAY_BEG;
    while( xmlrpc_parse.has_more_sibling() ){
    	xmlrpc_parse >> ... here parse the element of the array ...
    }
    xmlrpc_parse >> xmlrpc_parse_t::ARRAY_BEG;
  

*/

/* system include */
#include <cstdlib>
/* local include */
#include "neoip_xmlrpc_parse.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief consructor of the class
 */
xmlrpc_parse_t::xmlrpc_parse_t()			throw()
{
	// set endtoken_only to false by default
	endtoken_only	= false;
}

/** \brief Destructor
 */
xmlrpc_parse_t::~xmlrpc_parse_t()			throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the xml document to parse
 * 
 * - it will result in a is_null() xmlrpc_parse_t if the xml document cant be parsed
 */
xmlrpc_parse_t &	xmlrpc_parse_t::set_document(const datum_t &xmlenc_datum)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// set the xml_parse_doc_t
	xml_parse_doc.set_document(xmlenc_datum);
	// if the parsing failed, return now
	if( xml_parse_doc.is_null() )	return *this;
	// init the xml_parse_t on this document
	xml_parse	= xml_parse_t(&xml_parse_doc);
	// return the object itself
	return *this;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			endtoken_only handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief handle the endtoken_only flag for endtoken
 * 
 * - Code to put at the head of any token which IS an 'end'
 */
void	xmlrpc_parse_t::header_for_endtoken()		throw()
{
	// set the endtoken_only to false
	endtoken_only	= false;
}

/** \brief handle the endtoken_only flag for non endtoken
 * 
 * - Code to put at the head of any token which IS NOT an 'end'
 */
void	xmlrpc_parse_t::header_non_endtoken()		throw(xml_except_t)
{
	// if the endtoken_only is set, throw an exception
	if( endtoken_only )	nthrow_xml_plain("Impossible to find this element. entoken_only is set.");
}

/** \brief goto nextsib, with the endtoken_only flag handle
 * 
 * - Code to put at the end of any token parsing whatever it is a 'end' or not
 */
void	xmlrpc_parse_t::goto_nextsib_optional()		throw()
{
	// if there are no sibling, set the entoken_only to true and leave
	// - NOTE: this leave the xml_parse current_node in place, making it false for 
	//   further parsing except for endtoken
	// - hence the need to test for endtoken_only at EVERY token
	if( !xml_parse.has_nextsib() ){
		endtoken_only	= true;
		return;
	}
	// as there is a nextsib, goto the nextsib
	xml_parse.goto_nextsib();

	/*************** skip all 'text' sibling	***********************/
	// - NOTE: this 'text' exception is to be able to skip '\n' text between
	//   the xml node. some implementations uses this in order to make it
	//   more human-readable
	if( !xml_parse.has_firstsib_neg("text") ){
		endtoken_only	= true;
		return;
	}
	// goto the first sibling which is not 'text'
	xml_parse.goto_firstsib_neg("text");
}

/** \brief goto children, with the endtoken_only flag handle
 * 
 * - Code to put at the end of any token parsing whatever it is a 'end' or not
 */
void	xmlrpc_parse_t::goto_children_optional()		throw()
{
	// if there are no children, set the entoken_only to true and leave
	// - NOTE: this leave the xml_parse current_node in place, making it false for 
	//   further parsing except for endtoken
	// - hence the need to test for endtoken_only at EVERY token
	if( !xml_parse.has_children() ){
		endtoken_only	= true;
		return;
	}
	// as there is a children, goto the children
	xml_parse.goto_children();	

	/*************** skip all 'text' sibling	***********************/
	// - NOTE: this 'text' exception is to be able to skip '\n' text between
	//   the xml node. some implementations uses this in order to make it
	//   more human-readable
	if( !xml_parse.has_firstsib_neg("text") ){
		endtoken_only	= true;
		return;
	}
	// goto the first sibling which is not 'text'
	xml_parse.goto_firstsib_neg("text");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return XMLRPCtype of the next value
 * 
 * - NOTE: it MUST ONLY be used when the next node is a value
 */
std::string	xmlrpc_parse_t::value_type()		throw(xml_except_t)
{
	std::string	result;
	// init the endtoken flag for non endtoken
	header_non_endtoken();
	// parse the xml
	xml_parse.goto_firstsib("value");
	xml_parse.goto_children();
	result	= xml_parse.node_name();
	xml_parse.goto_parent();
	// handle the special case of the <int> alias
	if( result == "i4" )	result	= "int";
	// return the result
	return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			function to parse flag WITHOUT parameter
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a given flag (those without parameters)
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::flag_t &flag) throw(xml_except_t)
{
	xml_parse_t &	xml_parse	= xmlrpc_parse.xml_parse;
	// log to debug
	KLOG_DBG("enter");
	switch( flag ){
	/*************** CALL stuff	***************************************/
	// xmlrpc_parse_t::CALL_BEG is handled separatly as it requires parameter
	case xmlrpc_parse_t::CALL_END:		xmlrpc_parse.header_for_endtoken();
						xml_parse.goto_parent();
						xmlrpc_parse.goto_nextsib_optional();
						break;
	/*************** RESP stuff	***************************************/
	case xmlrpc_parse_t::RESP_BEG:		xmlrpc_parse.header_non_endtoken();
						xml_parse.goto_firstsib("methodResponse");
						xml_parse.goto_children().goto_firstsib("params");
						xml_parse.goto_children().goto_firstsib("param");
						xml_parse.goto_children();
						break;
	case xmlrpc_parse_t::RESP_END:		xmlrpc_parse.header_for_endtoken();
						xml_parse.goto_parent().goto_parent().goto_parent();
						xmlrpc_parse.goto_nextsib_optional();
						break;
	/*************** FAULT stuff	***************************************/
	// xmlrpc_parse_t::FAULT is handled separatly as it requires parameter

	/*************** PARAM stuff	***************************************/
	case xmlrpc_parse_t::PARAM_BEG:	xmlrpc_parse.header_non_endtoken();
						xml_parse.goto_firstsib("param");
						xmlrpc_parse.goto_children_optional();
						break;
	case xmlrpc_parse_t::PARAM_END:	xmlrpc_parse.header_for_endtoken();
						xml_parse.goto_parent();
						DBG_ASSERT( xml_parse.node_name() == "param" );
						xmlrpc_parse.goto_nextsib_optional();
						break;
	/*************** ARRAY stuff	***************************************/
	case xmlrpc_parse_t::ARRAY_BEG:	xmlrpc_parse.header_non_endtoken();
						xml_parse.goto_firstsib("value");
						xml_parse.goto_children().goto_firstsib("array");
						xml_parse.goto_children().goto_firstsib("data");
						xmlrpc_parse.goto_children_optional();
						break;
	case xmlrpc_parse_t::ARRAY_END:		xmlrpc_parse.header_for_endtoken();
						xml_parse.goto_parent();
						// NOTE: this allow to do this goto_parent IIF goto_children_optional did go 
						if( xml_parse.node_name() == "data" )	xml_parse.goto_parent();
						xml_parse.goto_parent();
						DBG_ASSERT( xml_parse.node_name() == "value" );
						xmlrpc_parse.goto_nextsib_optional();
						break;
	/*************** STRUCT stuff	***************************************/
	case xmlrpc_parse_t::STRUCT_BEG:	xmlrpc_parse.header_non_endtoken();
						xml_parse.goto_firstsib("value");
						xml_parse.goto_children().goto_firstsib("struct");
						break;
	case xmlrpc_parse_t::STRUCT_END:	xmlrpc_parse.header_for_endtoken();
						xml_parse.goto_firstsib("struct");
						xml_parse.goto_parent();
						DBG_ASSERT( xml_parse.node_name() == "value" );
						xmlrpc_parse.goto_nextsib_optional();
						break;
	/*************** MEMBER stuff	***************************************/
	// xmlrpc_parse_t::MEMBER_BEG is handled separatly as it requires parameter
	case xmlrpc_parse_t::MEMBER_END:	xmlrpc_parse.header_for_endtoken();
						xml_parse.goto_parent().goto_parent();
						DBG_ASSERT( xml_parse.node_name() == "struct" );
						break;
	default:	DBG_ASSERT( 0 );
	}
			
	// return the object itself
	return xmlrpc_parse;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			function to parse flag WITH parameter
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse a CALL_BEG
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::CALL_BEG &call_beg) throw(xml_except_t)
{
	xml_parse_t &	xml_parse	= xmlrpc_parse.xml_parse;
	// log to debug
	KLOG_DBG("enter");
	// init the endtoken flag for non endtoken
	xmlrpc_parse.header_non_endtoken();
	// parse the xml
	if( xml_parse.node_name() != "methodCall" )	nthrow_xml_plain("Can't find a 'methodCall' element");
	xml_parse.goto_children().goto_firstsib("methodName");
	// copy the method_name in the call_beg parameter
	*(call_beg.str_ptr)	= xml_parse.node_content();
	// goon the xml parse
	xml_parse.goto_firstsib("params");
	// goto the children with endtoken flag handle
	xmlrpc_parse.goto_children_optional();
	// return the object itself
	return xmlrpc_parse;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			struct member handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Internal function to return the list of all member_name of a struct
 * 
 * - NOTE: it MUST point on a "member" node
 * - it is up to the caller to go 'inside' the <struct> before calling this function
 *   and go back to <struct> after it
 *   - this helps the handling of what to do in case the <struct> doesnt have this 
 *     member.
 * - after the function, local xml_parse_t always points to a "member"
 */
std::list<std::string>	xmlrpc_parse_t::all_member_name()		throw(xml_except_t)
{
	std::list<std::string>	member_name_arr;
	// init the endtoken flag for non endtoken
	header_non_endtoken();
	// sanity check - the current nodename MUST be "struct"
	DBG_ASSERT( xml_parse.node_name() == "struct" );
	// if the struct has no member, return now
	if( !xml_parse.has_children() )	return member_name_arr;
	// goto the children to go at the "member" level
	xml_parse.goto_children();

	// loop thru all the "member"
	while( true ){
		// goto the first sibling 'member'
		xml_parse.goto_firstsib("member");
		// goto at the "name" level
		xml_parse.goto_children().goto_firstsib("name");
		// put this name into the member_name_arr
		member_name_arr.push_back( xml_parse.node_content() );
		// goto back at the "member" level
		xml_parse.goto_parent();
		// if there are no more sibling, leave the loop now
		if( !xml_parse.has_nextsib() )	break;
		// goto the next sibling
		xml_parse.goto_nextsib();
	}
	// come back at the "struct" level	
	xml_parse.goto_parent();
	// sanity check - the current nodename MUST be "struct"
	DBG_ASSERT( xml_parse.node_name() == "struct" );
	// return the result
	return member_name_arr;
}
/** \brief Internal function to find a given member name
 * 
 * - NOTE: it MUST point on a "member" node
 * - it is up to the caller to go 'inside' the <struct> before calling this function
 *   and go back to <struct> after it
 *   - this helps the handling of what to do in case the <struct> doesnt have this 
 *     member.
 * - after the function, local xml_parse_t always points to a "member"
 * - if the function returned true, this "member" has a name matching member_name
 * - if the function returned false, the "member" is last member of the <struct>
 */
bool	xmlrpc_parse_t::try_find_member(const std::string &member_name)	throw(xml_except_t)
{
	bool	member_found	= false;
	// loop thru all the "member"
	while( true ){
		// goto the first sibling 'member'
		xml_parse.goto_firstsib("member");
		// goto at the "name" level
		xml_parse.goto_children().goto_firstsib("name");
		// if the node_content equal to member_name, set member_found to true
		if( xml_parse.node_content() == member_name )	member_found	= true;
		// goto back at the "member" level
		xml_parse.goto_parent();
		// if the member_name has been found, leave the loop now
		if( member_found )		break;
		// if there are no more sibling, leave the loop now
		if( !xml_parse.has_nextsib() )	break;
		// goto the next sibling
		xml_parse.goto_nextsib();
	}
	// sanity check - the current nodename MUST be "member"
	DBG_ASSERT( xml_parse.node_name() == "member" );
	// log to debug
	KLOG_DBG("member_found=" << member_found << " member_name=" << member_name);
	// return the result
	return member_found;
}


/** \brief return true if the current STRUCT has a member called as member_name
 * 
 * - NOTE: it MUST ONLY when parsing a struct, so after a >> xmlrpc_parse_t::STRUCT_BEG
 */
bool	xmlrpc_parse_t::has_member(const std::string &member_name)	throw(xml_except_t)
{
	bool	member_found	= false;
	// init the endtoken flag for non endtoken
	header_non_endtoken();
	// sanity check - the current nodename MUST be "struct"
	DBG_ASSERT( xml_parse.node_name() == "struct" );
	// if the struct has no member, return false
	if( !xml_parse.has_children() )	return false;
	// goto the children to go at the "member" level
	xml_parse.goto_children();
	// try to find the <member> of this name
	member_found	= try_find_member(member_name);
	// go back to the "struct" level
	xml_parse.goto_parent();
	// sanity check - the current nodename MUST be "struct"
	DBG_ASSERT( xml_parse.node_name() == "struct" );
	// return the result
	return member_found;
}

/** \brief Parse a MEMBER_BEG
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::MEMBER_BEG &member_beg) throw(xml_except_t)
{
	xml_parse_t &		xml_parse	= xmlrpc_parse.xml_parse;
	const std::string &	member_name	= member_beg.str;
	// log to debug
	KLOG_DBG("enter");
	// init the endtoken flag for non endtoken
	xmlrpc_parse.header_non_endtoken();
	// sanity check - the current nodename MUST be "struct"
	DBG_ASSERT( xml_parse.node_name() == "struct" );
	// goto the children to go at the "member" level
	xml_parse.goto_children();
	// try to find the <member> of this name, if not found, throw an exception
	if( !xmlrpc_parse.try_find_member(member_name) )
		nthrow_xml_plain("Can't find a member named '" + OSTREAMSTR(member_name) + "'");
	// goto the children
	xml_parse.goto_children();
	// goto the "value" node
	xml_parse.goto_firstsib("value");
	// return the object itself
	return xmlrpc_parse;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			json handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a json escaped version of the string
 */
std::string	xmlrpc_parse_t::escape_json_string(const std::string &orig_str)	throw()
{
	std::string	str	= orig_str;
	// from http://www.json.org/
	str = string_t::replace(str, "\\"	, "\\\\");
	str = string_t::replace(str, "\""	, "\\\"");
	str = string_t::replace(str, "/"	, "\\/");
	str = string_t::replace(str, "\b"	, "\\b");
	str = string_t::replace(str, "\f"	, "\\f");
	str = string_t::replace(str, "\n"	, "\\n");
	str = string_t::replace(str, "\r"	, "\\r");
	str = string_t::replace(str, "\t"	, "\\t");
	str = string_t::replace(str, "\b"	, "\\b");
	// NOTE: here the space talks about a \u that i dont understand

	// log to debug
	//KLOG_ERR("orig_str=" << datum_t(orig_str));
	//KLOG_ERR("result str=" << datum_t(str));
	// return the just built result
	return str;
}


/** \brief Parse a VALUE_TO_JSON
 * 
 * - this is used to parse a xmlrpc value and output it in json format
 * - see http://www.json.org/ for details about json format 
 * - This is used by the xmlrpc_resp_t for jsrest which return json version of xmlrpc
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse
			, const xmlrpc_parse_t::VALUE_TO_JSON &value_to_json)	throw(xml_except_t)
{
	std::ostringstream &	oss_json	= value_to_json.oss_json;
	// log to debug
	KLOG_DBG("enter");
	// parse this value according to its value_type
	if( xmlrpc_parse.value_type() == "int" ){
		int32_t	val;
		xmlrpc_parse	>> val;
		oss_json	<< val;
	}else if( xmlrpc_parse.value_type() == "boolean" ){
		bool	val;
		xmlrpc_parse	>> val;
		oss_json	<< (val ? "true" : "false");
	}else if( xmlrpc_parse.value_type() == "string" ){
		std::string	val;
		xmlrpc_parse	>> val;	
		oss_json	<< "\"" << xmlrpc_parse_t::escape_json_string(val) << "\"";
	}else if( xmlrpc_parse.value_type() == "double" ){
		double		val;
		xmlrpc_parse	>> val;	 
		oss_json	<< val;
	}else if( xmlrpc_parse.value_type() == "array" ){
		// start the array
		xmlrpc_parse	>> xmlrpc_parse_t::ARRAY_BEG;
		oss_json	<< "[";
		// go thru all the element of this array
		while( xmlrpc_parse.has_more_sibling() ){
			// get this value reccursivly
			xmlrpc_parse >> xmlrpc_parse_t::VALUE_TO_JSON(oss_json);
			// if there are more sibling, put a "," as separator			
			if( xmlrpc_parse.has_more_sibling() )	oss_json << ", ";
		}
		// end the array
		xmlrpc_parse 	>> xmlrpc_parse_t::ARRAY_END;
		oss_json	<< "] ";
	}else if( xmlrpc_parse.value_type() == "struct" ){
		// start the struct
		xmlrpc_parse	>> xmlrpc_parse_t::STRUCT_BEG;
		oss_json	<< "{";
		// get the member_name for each member of this struct 
		std::list<std::string>	name_arr;
		name_arr	= xmlrpc_parse.all_member_name();
		
		// go thru all the member_name;
		while( !name_arr.empty() ){
			// get the member_name
			std::string	member_name	= name_arr.front();
			name_arr.pop_front();
			// start the member - TODO do i need any excaping here ?
			xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG(member_name);
			oss_json	<< "\"" << xmlrpc_parse_t::escape_json_string(member_name) 
					<< "\"" << ": ";
			// get this value reccursivly
			xmlrpc_parse	>> xmlrpc_parse_t::VALUE_TO_JSON(oss_json);
			// end the member
			xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_END;
			// if there are more member, put a "," as separator			
			if( !name_arr.empty() )		oss_json << ", ";
		}
		// end the struct
		xmlrpc_parse	>> xmlrpc_parse_t::STRUCT_END;
		oss_json	<< "} ";
	}else{
		nthrow_xml_plain("unknown value_type " + xmlrpc_parse.value_type());	
	}

	// return the object itself
	return xmlrpc_parse;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			fault handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the xmlrpc_parse_t document is a <fault> response
 * 
 * - NOTE: this function MUST be used before parsing anything
 */
bool	xmlrpc_parse_t::is_fault_resp()		throw(xml_except_t)
{
	bool	result	= false;
	if( xml_parse.node_name() != "methodResponse" )	nthrow_xml_plain("Can't find a 'methodResponse' element");
	xml_parse.goto_children();
	if( xml_parse.has_firstsib("fault") )		result	= true;
	xml_parse.goto_parent();
	// return the result
	return 	result;
}

/** \brief Parse a FAULT
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, const xmlrpc_parse_t::FAULT &fault) throw(xml_except_t)
{
	xml_parse_t &	xml_parse	= xmlrpc_parse.xml_parse;
	// log to debug
	KLOG_DBG("enter");
	// init the endtoken flag for non endtoken
	xmlrpc_parse.header_non_endtoken();
	// parse the xml
	if( xml_parse.node_name() != "methodResponse" )	nthrow_xml_plain("Can't find a 'methodResponse' element");
	xml_parse.goto_children().goto_firstsib("fault");
	xml_parse.goto_children();
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG("faultCode");
	xmlrpc_parse		>> *(fault.code_ptr);
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG("faultString");
	xmlrpc_parse		>> *(fault.str_ptr);
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;
	// go back at the "fault" level
	xml_parse.goto_parent();
	// go back at the "methodResponse" level
	xml_parse.goto_parent();
	// goto the nextsib with endtoken flag handle
	xmlrpc_parse.goto_nextsib_optional();
	// return the object itself
	return xmlrpc_parse;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parsing for basic types
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse a int32_t
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, int32_t &val)	throw(xml_except_t)
{
	xml_parse_t &	xml_parse	= xmlrpc_parse.xml_parse;
	// log to debug
	KLOG_DBG("enter");
	// init the endtoken flag for non endtoken
	xmlrpc_parse.header_non_endtoken();
	// goto the "value" node
	xml_parse.goto_firstsib("value");
	// goto the children node
	xml_parse.goto_children();
	// if the node name is not a "int" or "i4", throw an exception
	if( !xml_parse.has_firstsib("int") && !xml_parse.has_firstsib("i4") )	nthrow_xml_plain("Can't find type int");
	// goto the int node
	// - NOTE: some weirdness because int may be named 'int' or 'i4'
	if( xml_parse.has_firstsib("int") )	xml_parse.goto_firstsib("int");
	if( xml_parse.has_firstsib("i4") )	xml_parse.goto_firstsib("i4");
	// convert the node_content() into a int32_t
	val	= atoi(xml_parse.node_content().c_str());
	// goto the parent node
	xml_parse.goto_parent();
	// goto the nextsib with endtoken flag handle
	xmlrpc_parse.goto_nextsib_optional();
	// return the object itself
	return xmlrpc_parse;
}

/** \brief Parse a bool
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, bool &val)	throw(xml_except_t)
{
	xml_parse_t &	xml_parse	= xmlrpc_parse.xml_parse;
	// log to debug
	KLOG_DBG("enter");
	// init the endtoken flag for non endtoken
	xmlrpc_parse.header_non_endtoken();
	// goto the "value" node
	xml_parse.goto_firstsib("value");
	// goto the children node
	xml_parse.goto_children().goto_firstsib("boolean");
	// convert the node_content() into a bool
	val	= xml_parse.node_content() == "1" ? true : false;
	// goto the parent node
	xml_parse.goto_parent();
	// goto the nextsib with endtoken flag handle
	xmlrpc_parse.goto_nextsib_optional();
	// return the object itself
	return xmlrpc_parse;
}

/** \brief Parse a std::string
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, std::string &val)	throw(xml_except_t)
{
	xml_parse_t &	xml_parse	= xmlrpc_parse.xml_parse;
	// log to debug
	KLOG_DBG("enter");
	// init the endtoken flag for non endtoken
	xmlrpc_parse.header_non_endtoken();
	// goto the "value" node
	xml_parse.goto_firstsib("value");
	// goto the children node
	xml_parse.goto_children().goto_firstsib("string");
	// convert the node_content() into a std::string
	val	= xml_parse.node_content();
	// goto the parent node
	xml_parse.goto_parent();
	// goto the nextsib with endtoken flag handle
	xmlrpc_parse.goto_nextsib_optional();
	// return the object itself
	return xmlrpc_parse;
}

/** \brief Parse a double
 */
xmlrpc_parse_t& operator >> (xmlrpc_parse_t& xmlrpc_parse, double &val)	throw(xml_except_t)
{
	xml_parse_t &	xml_parse	= xmlrpc_parse.xml_parse;
	// log to debug
	KLOG_DBG("enter");
	// init the endtoken flag for non endtoken
	xmlrpc_parse.header_non_endtoken();
	// goto the "value" node
	xml_parse.goto_firstsib("value");
	// goto the children node
	xml_parse.goto_children().goto_firstsib("double");
	// convert the node_content() into a double
	val	= atof(xml_parse.node_content().c_str());
	// log to debug
	KLOG_DBG("double=" << xml_parse.node_content());
	// goto the parent node
	xml_parse.goto_parent();
	// goto the nextsib with endtoken flag handle
	xmlrpc_parse.goto_nextsib_optional();
	// return the object itself
	return xmlrpc_parse;
}


NEOIP_NAMESPACE_END

