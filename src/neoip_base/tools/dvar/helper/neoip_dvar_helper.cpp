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
				const dvar_t &		item	= iter->second;				
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
				const dvar_t &		item	= iter->second;				
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//		from_http_query
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief parse the keyval for dvar_helper_t::from_http_query
 * 
 * - NOTE: handle all keys as string. aka dont handle dvar_arr_t
*/
dvar_t	dvar_helper_t::http_query_parsekeyval(std::string key, dvar_t val)	throw(serial_except_t)
{
	// determine if key contains a map/arr
	size_t	idx_beg	= key.find_first_of("[");
	size_t	idx_end	= key.find_last_of("]");
	// if key contains a map/arr, reccurse
	if( idx_beg != std::string::npos && idx_end == key.size()-1 ){
		std::string	outter_key	= key.substr(0, idx_beg);
		std::string	inner_key	= key.substr(idx_beg+1, idx_end-idx_beg-1);
		key	= outter_key;
		val	= http_query_parsekeyval(inner_key, val);
	}
	// else key is processed as string, return a dvar_map_t
	return dvar_map_t().insert(key, val);
}

/** \brief merge the keyval for dvar_helper_t::from_http_query
 *
 * - NOTE: handle all keys as string. aka dont handle dvar_arr_t
*/
void	dvar_helper_t::http_query_mergekeyval(dvar_t &dvar1, dvar_t dvar2)	throw(serial_except_t)
{
	// go thru all dvar2 keys
	const std::map<std::string, dvar_t> &		inmap	=  dvar2.map().get_inmap();
	std::map<std::string, dvar_t>::const_iterator	iter;
	for(iter = inmap.begin(); iter != inmap.end(); iter++ ){
		const std::string &	key	= iter->first;
		const dvar_t &		item	= iter->second;
		KLOG_ERR("key="<<key << " item="<< item);
		if( dvar1.map().contain(key) )	http_query_mergekeyval(dvar1.map()[key], dvar2.map()[key]);
		else				dvar1.map().insert(key, item);
	}
}


/** \brief convert dvar_map_t's into dvar_arr_t when possible
 *
 * - this function is needed because dvar_helper_t::http_query_mergekeyval and
 *   dvar_helper_t::http_query_parsekeyval handles only dvar_map_t()
 * - part of the dvar_helper_t::from_http_query
*/
dvar_t	dvar_helper_t::http_query_convert_map2arr(const dvar_t &dvar)	throw()
{
	// if dvar_type_t is atomic, return it now
	if( dvar.type().is_atomic() )	return dvar;
	// if dvar is a dvar_arr_t, return a deepcopy of it
	if( dvar.type().is_arr() ){
		dvar_t res	= dvar_arr_t();
		// go thru all items of dvar_arr_t
		for(size_t i = 0; i < dvar.arr().size(); i++){
			// do a deep copy of this item
			const dvar_t &res_item	= http_query_convert_map2arr(dvar.arr()[i]);
			// append res_item to res
			res.arr().append(res_item);
		}
		// return the just built result
		return res;
	}
	
	// if dvar is a dvar_map_t, test if it is convertible, else return a deepcopy
	if( dvar.type().is_map() ){
		const std::map<std::string, dvar_t> &		inmap	=  dvar.map().get_inmap();
		std::map<std::string, dvar_t>::const_iterator	iter;

		// test if this dvar_map_t is convertible into a dvar_arr_t
		size_t	maxidx	= 0;
		for(maxidx = 0;  dvar.map().contain(OSTREAMSTR(maxidx)); maxidx++);
		bool	is_convertible	= maxidx == inmap.size();

		// if not convertible, return a deepcopy of dvar
		if( is_convertible == false ){
			dvar_t res	= dvar_map_t();
			for(iter = inmap.begin(); iter != inmap.end(); iter++ ){
				const std::string &	key	= iter->first;
				const dvar_t &		item	= iter->second;
				res.map().insert(key, http_query_convert_map2arr(item));
			}
			return res;
		}

		// convert dvar into a dvar_arr_t
		dvar_t res	= dvar_arr_t();
		for(size_t i = 0;  dvar.map().contain(OSTREAMSTR(i)); i++){
			const dvar_t &	item	= dvar.map()[OSTREAMSTR(i)];
			res.arr().append( http_query_convert_map2arr(item) );
		}
		return res;
	}
	// sanity check - this point MUST never be reached
	DBG_ASSERT(false);
}

/** \brief Helper on top of the other to_dvar
 *
 * - inverse of http://php.net/http_build_query
 */
dvar_t	dvar_helper_t::from_http_query(const std::string &query_str)	throw(serial_except_t)
{
	dvar_t				result		= dvar_map_t();
	std::vector<std::string>	keyval_arr	= string_t::split(query_str, "&");
	for(size_t i = 0; i < keyval_arr.size(); i++ ){
		const std::string &		keyval	= keyval_arr[i];
		std::vector<std::string>	tmp	= string_t::split(keyval, "=", 2);
		// sanity check - tmp MUST contain 2 elements
		if( tmp.size() != 2)	nthrow_serial_plain("impossible to parse http_query: "+ query_str+". keyval=" + keyval);
		// do some aliases to ease readability		
		const std::string &	key	= tmp[0];
		const std::string &	val	= tmp[1];
		// parse the key/val of this item
		dvar_t	keyval_dvar	= http_query_parsekeyval(key, dvar_str_t(val));
		// merge it to result
		http_query_mergekeyval(result, keyval_dvar);
	}
	// convert dvar_map_t's to dvar_arr_t when possible
	result	= http_query_convert_map2arr(result);
	// return the result
	return result;
}

NEOIP_NAMESPACE_END


