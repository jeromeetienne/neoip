/*! \file
    \brief Header for handling the convertion of bt_peerid_t from/to torrent file
    
*/


#ifndef __NEOIP_BT_PEERID_HELPER_HPP__ 
#define __NEOIP_BT_PEERID_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_bt_peerid_style.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_id_t;

/** \brief static helpers to manipulate bt_peerid_t
 */
class bt_peerid_helper_t {
private:
	/*************** Version encode/decode	*******************************/
	static std::string	encode_version_packed(const std::string &progvers)	throw();
	static std::string	decode_version_packed(const std::string &progvers)	throw();
	static std::string	encode_version_dashed(const std::string &progvers)	throw();
	static std::string	decode_version_dashed(const std::string &progvers)	throw();
	static std::string	encode_version_dotted(const std::string &progvers)	throw();
	static std::string	decode_version_dotted(const std::string &progvers)	throw();
	
	static bt_id_t		build_peerid_suffix(const std::string &prefix)		throw();
	static std::string	progname_from_tagstyle(const std::string &tag
					, const bt_peerid_style_t &peerid_style)	throw();
public:
	/*************** build high level function	***********************/
	static bt_id_t	build(const std::string &progname, const std::string &progvers)	throw();
	static bt_id_t	build_style(const bt_peerid_style_t &peerid_style, const std::string &tag
								, const std::string &progvers)	throw();
	/*************** build function style specific	***********************/
	static bt_id_t	build_style_azureus(const std::string &tag, const std::string &progvers)	throw();
	static bt_id_t	build_style_shadow(const std::string &tag, const std::string &progvers)		throw();
	static bt_id_t	build_style_mainline(const std::string &tag, const std::string &progvers)	throw();
	static bt_id_t	build_style_xbt(const std::string &tag, const std::string &progvers)		throw();
	static bt_id_t	build_style_opera(const std::string &tag, const std::string &progvers)		throw();
	static bt_id_t	build_style_mldonkey(const std::string &tag, const std::string &progvers)	throw();
	static bt_id_t	build_style_bitonwheels(const std::string &tag, const std::string &progvers)	throw();


	
	static bt_peerid_style_t parse_style(const bt_id_t &peerid)			throw();
	
	/*************** parse_progname function	***********************/
	static std::string	parse_progname(const bt_id_t &peerid)			throw();
	static std::string	parse_progname_style_azureus(const bt_id_t &peerid)	throw();
	static std::string	parse_progname_style_shadow(const bt_id_t &peerid)	throw();
	static std::string	parse_progname_style_mainline(const bt_id_t &peerid)	throw();
	static std::string	parse_progname_style_xbt(const bt_id_t &peerid)	throw();
	static std::string	parse_progname_style_opera(const bt_id_t &peerid)	throw();
	static std::string	parse_progname_style_mldonkey(const bt_id_t &peerid)	throw();
	static std::string	parse_progname_style_bitonwheels(const bt_id_t &peerid)	throw();

	/*************** parse_progvers function	***********************/
	static std::string	parse_progvers(const bt_id_t &peerid)			throw();
	static std::string	parse_progvers_style_azureus(const bt_id_t &peerid)	throw();
	static std::string	parse_progvers_style_shadow(const bt_id_t &peerid)	throw();
	static std::string	parse_progvers_style_mainline(const bt_id_t &peerid)	throw();
	static std::string	parse_progvers_style_xbt(const bt_id_t &peerid)	throw();
	static std::string	parse_progvers_style_opera(const bt_id_t &peerid)	throw();
	static std::string	parse_progvers_style_mldonkey(const bt_id_t &peerid)	throw();
	static std::string	parse_progvers_style_bitonwheels(const bt_id_t &peerid)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERID_HELPER_HPP__  */










