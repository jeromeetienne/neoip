/*! \file
    \brief Definition of the \ref dvar_helper_t class

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_dvar_helper.hpp"
#include "neoip_dvar.hpp"
#include "neoip_dvar_int.hpp"
#include "neoip_dvar_dbl.hpp"
#include "neoip_dvar_str.hpp"
#include "neoip_dvar_arr.hpp"
#include "neoip_dvar_map.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//		to_xml
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Helper on top of the other to_dvar
 * 
 * - NOTE: it uses a handcrafted xml and no the libneoip_xml library
 *   - this has less dependancy and allow specific output
 */
void	dvar_helper_t::to_xml(const dvar_t &dvar, std::ostringstream &oss)	throw()
{
	switch( dvar.type().get_value() ){
	case dvar_type_t::INTEGER:
			oss << dvar.integer().to_int64();
			break;
	case dvar_type_t::DOUBLE:
			oss << dvar.dbl().get();
			break;
	case dvar_type_t::BOOLEAN:
			oss << dvar.boolean().get();
			break;
	case dvar_type_t::NIL:
			oss << "NIL";
			break;
	case dvar_type_t::STRING:{
			std::string	str	= dvar.str().get();
			// from http://www.w3.org/TR/REC-xml/#dt-escape
			// - some character need to be escaped
			if(str.find("&") != std::string::npos)	str = string_t::replace(str, "&", "&amp;");
			if(str.find("<") != std::string::npos)	str = string_t::replace(str, "<", "&lt;");
			if(str.find(">") != std::string::npos)	str = string_t::replace(str, ">", "&gt;");
			if(str.find("'") != std::string::npos)	str = string_t::replace(str, "'", "&apos;");
			if(str.find("\"") != std::string::npos)	str = string_t::replace(str, "\"","&quot;");
			// put the escaped string into the stream
			oss << str;
			break;}
	case dvar_type_t::ARRAY:
			for(size_t i = 0; i < dvar.arr().size(); i++){
				oss << "<item>";
				if( !dvar.arr()[i].type().is_atomic() )	oss << "\n";
				to_xml(dvar.arr()[i], oss);
				oss << "</item>\n";
			}
			break;
	case dvar_type_t::MAP:{
			const std::map<std::string, dvar_t> &		inmap	=  dvar.map().get_inmap();
			std::map<std::string, dvar_t>::const_iterator	iter;
			for(iter = inmap.begin(); iter != inmap.end(); iter++ ){
				const std::string &	key	= iter->first;
				const dvar_t		item	= iter->second;				
				oss << "<" << key << ">";
				if( !item.type().is_atomic() )	oss << "\n";
				to_xml(item, oss);
				oss << "</"<< key << ">\n";
			}
			break;}
	default:	KLOG_ERR("dvar="<<dvar);
			DBG_ASSERT( 0 );
	}
}

/** \brief Helper on top of the other to_dvar
 */
std::string	dvar_helper_t::to_xml(const dvar_t &dvar)	throw()
{
	std::ostringstream	oss;
	// convert the dvar_t to xml and put it in oss
	to_xml(dvar, oss);
	// return the result
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//		to_http_query
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Helper on top of the other to_dvar
 * 
 * - based on http://php.net/http_build_query
 */
void	dvar_helper_t::to_http_query(const dvar_t &dvar, std::ostringstream &oss
				, const std::string &key_prefix
				, const std::string &key_suffix)	throw()
{
	switch( dvar.type().get_value() ){
	case dvar_type_t::INTEGER:
			oss << dvar.integer().to_int64();
			break;
	case dvar_type_t::DOUBLE:
			oss << dvar.dbl().get();
			break;
	case dvar_type_t::BOOLEAN:
			oss << dvar.boolean().get();
			break;
	case dvar_type_t::NIL:
			oss << "NIL";
			break;
	case dvar_type_t::STRING:
			oss << dvar.str().get();
			break;
	case dvar_type_t::ARRAY:
			for(size_t i = 0; i < dvar.arr().size(); i++){
				// add a separator if needed
				if( oss.str().size() > 0 )	oss << "&";
				// handle dvar_type_t::is_atomic or not
				if( dvar.arr()[i].type().is_atomic() ){
					// build the string to encode this variable
					oss << key_prefix << i << key_suffix << "=" << dvar.arr()[i];
				}else{
					// recurssion inside the non atomic dvar_t
					to_http_query(dvar.arr()[i], oss, key_prefix + OSTREAMSTR(i) + "[", "]" + key_suffix);
				}
			}
			break;
	case dvar_type_t::MAP:{
			const std::map<std::string, dvar_t> &		inmap	=  dvar.map().get_inmap();
			std::map<std::string, dvar_t>::const_iterator	iter;
			for(iter = inmap.begin(); iter != inmap.end(); iter++ ){
				const std::string &	key	= iter->first;
				const dvar_t		item	= iter->second;				
				// add a separator if needed
				if( oss.str().size() > 0 )	oss << "&";
				// handle dvar_type_t::is_atomic or not
				if( item.type().is_atomic() ){
					// build the string to encode this variable
					oss << key_prefix << key << key_suffix << "=" << item;
				}else{
					// recurssion inside the non atomic dvar_t
					to_http_query(item, oss, key_prefix + key + "[", "]" + key_suffix);
				}
			}
			break;}
	default:	KLOG_ERR("dvar="<<dvar);
			DBG_ASSERT( 0 );
	}	
}

/** \brief Helper on top of the other to_dvar
 *
 * - based on http://php.net/http_build_query
 */
std::string	dvar_helper_t::to_http_query(const dvar_t &dvar)	throw()
{
	std::ostringstream	oss;
	// convert the dvar_t to a http_ and put it in oss
	to_http_query(dvar, oss, "", "");
	// return the result
	return oss.str();
}

NEOIP_NAMESPACE_END


