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
	default:	DBG_ASSERT( 0 );
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

NEOIP_NAMESPACE_END


