/*! \file
    \brief Definition of the \ref amf0_build_t class

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_amf0_build.hpp"
#include "neoip_amf0_type.hpp"
#include "neoip_dvar.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief Helper on top of the other to_dvar
 *
 * - NOTE: it uses a handcrafted xml and no the libneoip_xml library
 *   - this has less dependancy and allow specific output
 */
void	amf0_build_t::to_amf0(const dvar_t &dvar, bytearray_t &amf0_data)	throw()
{
#if 0
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
#endif
}

/** \brief Helper on top of the other to_dvar
 */
bytearray_t	amf0_build_t::to_amf0(const dvar_t &dvar)	throw()
{
	bytearray_t	amf0_data;
	// convert the dvar_t to amf0 and put it in amf0_data
	to_amf0(dvar, amf0_data);
	// return the result
	return amf0_data;
}

NEOIP_NAMESPACE_END


