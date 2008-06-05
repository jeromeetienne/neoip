/*! \file
    \brief Header of the \ref mlink_url_arr_t
    
*/


#ifndef __NEOIP_MLINK_URL_ARR_HPP__ 
#define __NEOIP_MLINK_URL_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_mlink_url.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_xml_except.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	xml_build_t;
class	xml_parse_t;

NEOIP_ITEM_ARR_DECLARATION_START(mlink_url_arr_t, mlink_url_t);
public:
	/*************** xml serialization	*******************************/
	friend	xml_build_t& operator << (xml_build_t& xm_build, const mlink_url_arr_t &url_arr)throw();	
	friend	xml_parse_t& operator >> (xml_parse_t& xml_parse, mlink_url_arr_t &url_arr)	throw(xml_except_t);

NEOIP_ITEM_ARR_DECLARATION_END(mlink_url_arr_t, mlink_url_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_MLINK_URL_ARR_HPP__  */










