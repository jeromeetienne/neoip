/*! \file
    \brief Definition for helper for bt_id_t peerid

- TODO do the bitcomet with exbc and LORD
- TODO bug in the packed version parsing
  - it assumed an hexa number
  - while bittornado use some T03H with H = 18

*/

/* system include */
/* local include */
#include "neoip_bt_peerid_helper.hpp"
#include "neoip_bt_peerid_style.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

struct bt_peerid_record_t {
	char *			progname;
	char *			tag;
	bt_peerid_style_t	style;
};

static bt_peerid_record_t peerid_record_db[] = {
	  { "Ares"		, "AG"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Ares"		, "A~"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Artic"		, "AR"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Azureus"		, "AZ"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "BitPump"		, "AX"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "BitBuddy"		, "BB"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "BitComet"		, "BC"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Bitflu"		, "BF"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "BTG"		, "BG"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "BitRocket"		, "BR"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "BitSlave"		, "BS"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "BittorrentX"	, "BX"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Enhanced CTorrent"	, "CD"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "CTorrent"		, "CT"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Deluge"		, "DE"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "EBit"		, "EB"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Electrnic Sheep"	, "ES"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Halite"		, "HL"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Ktorrent"		, "KT"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Lphant"		, "LP"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "rb ltorrent"	, "LT"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "rk ltorrent"	, "lt"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "MooPolice"		, "MP"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "MoonlightTorrent"	, "MT"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "qBittorrent"	, "qB"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Qt4 example"	, "QT"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Retriever"		, "RT"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Swiftbit"		, "SB"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "SwarmScope"	, "SS"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Shareaza"		, "SZ"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Shareaza alpha/beta","S~"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "TorrentDotNet"	, "TN"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "Transmission"	, "TR"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "TorrentStorm"	, "TS"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "uLeecher"		, "UL"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "uTorrent"		, "UT"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "XanTorrent"	, "XT"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "ZipTorrent"	, "ZT"	, strtype_bt_peerid_style_t::AZUREUS	}
	, { "ABC"		, "A"	, strtype_bt_peerid_style_t::SHADOW	}
	, { "Ospread permaseed"	, "O"	, strtype_bt_peerid_style_t::SHADOW	}
	, { "Tribler"		, "R"	, strtype_bt_peerid_style_t::SHADOW	}
	, { "Shadow"		, "S"	, strtype_bt_peerid_style_t::SHADOW	}
	, { "BitTornado"	, "T"	, strtype_bt_peerid_style_t::SHADOW	}
	, { "Upnp Nat Bittorren", "U"	, strtype_bt_peerid_style_t::SHADOW	}
	, { "Mainline"		, "M"	, strtype_bt_peerid_style_t::MAINLINE	}
	, { "QueenBee"		, "Q"	, strtype_bt_peerid_style_t::MAINLINE	}
	, { "XBT"		, "XBT"	, strtype_bt_peerid_style_t::XBT	}
	, { "Opera"		, "OP"	, strtype_bt_peerid_style_t::OPERA	}
	, { "MLDonkey"		, "ML"	, strtype_bt_peerid_style_t::MLDONKEY	}
	, { "BitOnWheels"	, "BOWA", strtype_bt_peerid_style_t::BITONWHEELS}
	, { NULL		, NULL	, strtype_bt_peerid_style_t::NONE	}
	};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			version packed handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Encode the progvers in a "packed" format (aka number in hexa without separator)
 */
std::string	bt_peerid_helper_t::encode_version_packed(const std::string &progvers)		throw()
{
	std::vector<std::string> words	= string_t::split(progvers, ".");
	std::ostringstream	oss;
	// go thru all the elements of the progvers
	for(size_t i = 0; i < words.size(); i++ ){
		size_t	value	= strtoul(words[i].c_str(), NULL, 16);
		oss << std::hex << value;
	}
	// return the build string
	return oss.str();
}

/** \brief Decode the progvers from a "packed" format (aka number in hexa without separator)
 */
std::string	bt_peerid_helper_t::decode_version_packed(const std::string &progvers)		throw()
{
	std::ostringstream	oss;
	// go thru all the elements of the progvers
	for(size_t i = 0; i < progvers.size(); i++ ){
		uint8_t	value	= progvers[i];
		if( value >= '0' && value <= '9' )	value	= value - '0' + 0;
		else if( value >= 'A' && value <= 'Z' )	value	= value - 'A' + 10;
		else if( value >= 'a' && value <= 'z' )	value	= value - 'a' + 36;
		if( i != 0 )	oss << ".";
		oss << int(value);
	}
	// return the build string
	return oss.str();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			version dashed handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Encode the progvers in a "dashed" format (aka number in decimal with '-' as separator)
 */
std::string	bt_peerid_helper_t::encode_version_dashed(const std::string &progvers)		throw()
{
	std::vector<std::string> words	= string_t::split(progvers, ".");
	std::ostringstream	oss;
	// go thru all the elements of the progvers
	for(size_t i = 0; i < words.size(); i++ ){
		if( i != 0 )	oss << "-";
		oss << words[i];
	}
	// return the build string
	return oss.str();
}

/** \brief Decode the progvers from a "dashed" format (aka number in decimal without separator)
 */
std::string	bt_peerid_helper_t::decode_version_dashed(const std::string &progvers)		throw()
{
	std::vector<std::string> words	= string_t::split(progvers, "-");
	std::ostringstream	oss;
	// go thru all the elements of the progvers
	for(size_t i = 0; i < words.size(); i++ ){
		if( i != 0 )	oss << ".";
		oss << words[i];
	}
	// return the build string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			version dotted handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Encode the progvers in a "dotted" format (aka number in decimal with '.' as separator)
 */
std::string	bt_peerid_helper_t::encode_version_dotted(const std::string &progvers)		throw()
{
	// return the string as-is as the progvers is dotted internally
	return progvers;
}

/** \brief Decode the progvers from a "dotted" format (aka number in decimal with '.' as separator)
 */
std::string	bt_peerid_helper_t::decode_version_dotted(const std::string &progvers)		throw()
{
	// return the string as-is as the progvers is dotted internally
	return progvers;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			TODO to comment
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a random peerid with a fixed prefix
 */
bt_id_t		bt_peerid_helper_t::build_peerid_suffix(const std::string &prefix)	throw()
{
	// sanity check - the prefix MUST NOT be larger than the bt_id_t size
	DBG_ASSERT( prefix.size() <= bt_id_t::size() );
	// get the initial randomness
	bt_id_t	peerid	= bt_id_t::build_random();
	// put the prefix at the begining of the peerid
	datum_t	datum	= peerid.to_datum();
	memcpy(datum.void_ptr(), prefix.c_str(), prefix.size());
	peerid		= bt_id_t(datum);
	// return the just build peerid
	return peerid;
}

/** \brief Return a progname string matching this tag and bt_peerid_style_t
 */
std::string	bt_peerid_helper_t::progname_from_tagstyle(const std::string &tag
					, const bt_peerid_style_t &peerid_style)	throw()
{
	// go thru all the peerid_record_t
	for(size_t i = 0; peerid_record_db[i].progname; i++){
		const bt_peerid_record_t &	peerid_record	= peerid_record_db[i];
		// if this peerid_record.tag DOES NOT match, goto the next
		if( tag != peerid_record.tag )			continue;
		// if this peerid_record.style DOES NOT match, goto the next
		if( peerid_style != peerid_record.style )	continue;
		// if this peerid_record.name DOES match, call build_style with its bt_peerid_style_t
		return peerid_record.progname;
	}
	// if none have been found
	return "unknown " + tag + " in " + peerid_style.to_string() + " style"; 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         high level builder
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a peerid bt_id_t as if it cames from this progname/progvers
 * 
 * @return the built bt_id_t. or a null one if an error occurs
 */
bt_id_t	bt_peerid_helper_t::build(const std::string &progname, const std::string &progvers) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// go thru all the peerid_record_t
	for(size_t i = 0; peerid_record_db[i].progname; i++){
		const bt_peerid_record_t &	peerid_record	= peerid_record_db[i];
		// if this peerid_record.name DOES NOT match, goto the next
		if( string_t::casecmp(progname, peerid_record.progname) )	continue;
		// if this peerid_record.name DOES match, call build_style with its bt_peerid_style_t
		return build_style(peerid_record.style, peerid_record.tag, progvers);
	}	
	// if this point is reached, progname has no matches, so return a null bt_id_t
	return bt_id_t();
}

/** \brief Build a peerid base on the bt_peerid_style_t/tag/vers
 */
bt_id_t	bt_peerid_helper_t::build_style(const bt_peerid_style_t &peerid_style
			, const std::string &tag, const std::string &progvers)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// handle it according to the bt_peerid_style_t
	switch( peerid_style.get_value() ){
	case bt_peerid_style_t::AZUREUS:	return build_style_azureus(tag, progvers);
	case bt_peerid_style_t::SHADOW:		return build_style_shadow(tag, progvers);
	case bt_peerid_style_t::MAINLINE:	return build_style_mainline(tag, progvers);
	case bt_peerid_style_t::XBT:		return build_style_xbt(tag, progvers);
	case bt_peerid_style_t::OPERA:		return build_style_opera(tag, progvers);
	case bt_peerid_style_t::MLDONKEY:	return build_style_mldonkey(tag, progvers);
	case bt_peerid_style_t::BITONWHEELS:	return build_style_bitonwheels(tag, progvers);
	default:	DBG_ASSERT(0);
	}
	// NOTE: this point MUST never be reached
	DBG_ASSERT(0);
	return bt_id_t();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			build function for each style
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a peerid using the bt_peerid_style_t::AZUREUS
 */
bt_id_t	bt_peerid_helper_t::build_style_azureus(const std::string &tag, const std::string &progvers)	throw()
{
	std::string		version_enc	= encode_version_packed(progvers);
	std::ostringstream	oss;
	// sanity check - check the validity of the parameter
	DBG_ASSERT( tag.size() == 2 );
	DBG_ASSERT( version_enc.size() == 4 );
	// build the prefix_str
	oss << "-" << tag << version_enc << "-";
	// build and return the peerid with the randomness at the end
	return build_peerid_suffix(oss.str());
}

/** \brief Build a peerid using the bt_peerid_style_t::SHADOW
 */
bt_id_t	bt_peerid_helper_t::build_style_shadow(const std::string &tag, const std::string &progvers)	throw()
{
	std::string		version_enc	= encode_version_packed(progvers);
	std::ostringstream	oss;
	// sanity check - check the validity of the parameter
	DBG_ASSERT( tag.size() == 1 );
	DBG_ASSERT( version_enc.size() == 3 );
	// build the prefix_str
	oss << tag << version_enc << "----";
	// build and return the peerid with the randomness at the end
	return build_peerid_suffix(oss.str());
}

/** \brief Build a peerid using the bt_peerid_style_t::MAINLINE
 */
bt_id_t	bt_peerid_helper_t::build_style_mainline(const std::string &tag, const std::string &progvers)	throw()
{
	std::string		version_enc	= encode_version_dashed(progvers);
	std::ostringstream	oss;
	// sanity check - check the validity of the parameter
	DBG_ASSERT( tag.size() == 1 );
	DBG_ASSERT( version_enc.size() < 8 );
	// build the prefix_str
	oss << tag << version_enc << "--";
	// build and return the peerid with the randomness at the end
	return build_peerid_suffix(oss.str());
}

/** \brief Build a peerid using the bt_peerid_style_t::XBT
 */
bt_id_t	bt_peerid_helper_t::build_style_xbt(const std::string &tag, const std::string &progvers)	throw()
{
	std::string		version_enc	= encode_version_packed(progvers);
	std::ostringstream	oss;
	// sanity check - check the validity of the parameter
	DBG_ASSERT( tag.size() == 3 );
	DBG_ASSERT( version_enc.size() == 3 );
	// build the prefix_str
	oss << tag << version_enc << "-";
	// build and return the peerid with the randomness at the end
	return build_peerid_suffix(oss.str());
}

/** \brief Build a peerid using the bt_peerid_style_t::OPERA
 */
bt_id_t	bt_peerid_helper_t::build_style_opera(const std::string &tag, const std::string &progvers)	throw()
{
	std::string		version_enc	= encode_version_packed(progvers);
	std::ostringstream	oss;
	// sanity check - check the validity of the parameter
	DBG_ASSERT( tag.size() == 2 );
	DBG_ASSERT( version_enc.size() == 4 );
	// build the prefix_str
	oss << tag << version_enc;
	// build and return the peerid with the randomness at the end
	return build_peerid_suffix(oss.str());
}

/** \brief Build a peerid using the bt_peerid_style_t::MLDONKEY
 */
bt_id_t	bt_peerid_helper_t::build_style_mldonkey(const std::string &tag, const std::string &progvers)	throw()
{
	std::string		version_enc	= encode_version_dotted(progvers);
	std::ostringstream	oss;
	// sanity check - check the validity of the parameter
	DBG_ASSERT( tag.size() == 2 );
	// build the prefix_str
	oss << "-" << tag << version_enc << "-";
	// now build the peerid with the randomness at the end
	return build_peerid_suffix(oss.str());
}

/** \brief Build a peerid using the bt_peerid_style_t::BITONWHEELS
 */
bt_id_t	bt_peerid_helper_t::build_style_bitonwheels(const std::string &tag, const std::string &progvers)throw()
{
	std::string		version_enc	= encode_version_packed(progvers);
	std::ostringstream	oss;
	// sanity check - check the validity of the parameter
	DBG_ASSERT( tag.size() == 4 );
	DBG_ASSERT( version_enc.size() == 2 );
	// build the prefix_str
	oss << "-" << tag << version_enc << "-";
	// build and return the peerid with the randomness at the end
	return build_peerid_suffix(oss.str());
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parse style
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the bt_peerid_style_t from a bt_id_t peerid
 */
bt_peerid_style_t	bt_peerid_helper_t::parse_style(const bt_id_t &peerid)	throw()
{
	// convert the peerid into a string
	std::string s	= peerid.to_stdstring();
	
	// recognize the bt_peerid_style_t::AZUREUS
	if( s[0] == '-' && isalpha(s[1]) && isalpha(s[2]) && isxdigit(s[3]) && isxdigit(s[4]) 
			&& isxdigit(s[5]) && isxdigit(s[6]) && s[7] == '-' ){
		return bt_peerid_style_t::AZUREUS;
	}
	
	// recognize the bt_peerid_style_t::SHADOW
	// - NOTE: the http://wiki.theory.org/BitTorrentSpecification#peer_id said there is 4 dashes
	//   - but i saw A310--XXXXX in field
	if( isalpha(s[0]) && isalnum(s[1]) && isalnum(s[2]) && isalnum(s[3]) && s.substr(4, 2) == "--" ){
		return bt_peerid_style_t::SHADOW;
	}

	// recognize the bt_peerid_style_t::MAINLINE
	if( isalpha(s[0]) && isxdigit(s[1]) && (s[2] == '-' || s[3] == '-') ){
		return bt_peerid_style_t::MAINLINE;
	}

	// recognize the bt_peerid_style_t::XBT
	if( s.substr(0,3) == "XBT" ){
		return bt_peerid_style_t::XBT;
	}
	
	// recognize the bt_peerid_style_t::OPERA
	if( s.substr(0,2) == "OP" ){
		return bt_peerid_style_t::OPERA;
	}

	// recognize the bt_peerid_style_t::MLDONKEY
	if( s.substr(0,3) == "-ML" ){
		return bt_peerid_style_t::MLDONKEY;
	}

	// recognize the bt_peerid_style_t::BITONWHEELS
	if( s.substr(0,5) == "-BOWA" ){
		return bt_peerid_style_t::BITONWHEELS;
	}

	// if this point is reached, no bt_peerid_style_t have been found, so return none
	return bt_peerid_style_t::NONE;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parse progname main function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the string of the progname for this bt_id_t peerid ("unknown" if not known)
 */
std::string	bt_peerid_helper_t::parse_progname(const bt_id_t &peerid)	throw()
{
	// parse the bt_peerid_style_t for this peerid
	bt_peerid_style_t	peerid_style	= parse_style(peerid);
	// if the bt_peerid_style_t is not recognized, return unknown
	if( peerid_style == bt_peerid_style_t::NONE )	return "unknown";
	// handle it according to the bt_peerid_style_t
	switch( peerid_style.get_value() ){
	case bt_peerid_style_t::AZUREUS:	return parse_progname_style_azureus(peerid);
	case bt_peerid_style_t::SHADOW:		return parse_progname_style_shadow(peerid);
	case bt_peerid_style_t::MAINLINE:	return parse_progname_style_mainline(peerid);
	case bt_peerid_style_t::XBT:		return parse_progname_style_xbt(peerid);
	case bt_peerid_style_t::OPERA:		return parse_progname_style_opera(peerid);
	case bt_peerid_style_t::MLDONKEY:	return parse_progname_style_mldonkey(peerid);
	case bt_peerid_style_t::BITONWHEELS:	return parse_progname_style_bitonwheels(peerid);
	default:	DBG_ASSERT(0);
	}
	return "";	// just to avoid a compiler warning
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parse_progname function for each style
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the progname for a bt_peerid_style_t::AZUREUS
 */
std::string	bt_peerid_helper_t::parse_progname_style_azureus(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return progname_from_tagstyle(s.substr(1,2), bt_peerid_style_t::AZUREUS);
}

/** \brief Parse the progname for a bt_peerid_style_t::SHADOW
 */
std::string	bt_peerid_helper_t::parse_progname_style_shadow(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return progname_from_tagstyle(s.substr(0,1), bt_peerid_style_t::SHADOW);
}

/** \brief Parse the progname for a bt_peerid_style_t::MAINLINE
 */
std::string	bt_peerid_helper_t::parse_progname_style_mainline(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return progname_from_tagstyle(s.substr(0,1), bt_peerid_style_t::MAINLINE);
}

/** \brief Parse the progname for a bt_peerid_style_t::XBT
 */
std::string	bt_peerid_helper_t::parse_progname_style_xbt(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return progname_from_tagstyle(s.substr(0,3), bt_peerid_style_t::XBT);
}

/** \brief Parse the progname for a bt_peerid_style_t::OPERA
 */
std::string	bt_peerid_helper_t::parse_progname_style_opera(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return progname_from_tagstyle(s.substr(0,2), bt_peerid_style_t::OPERA);
}

/** \brief Parse the progname for a bt_peerid_style_t::MLDONKEY
 */
std::string	bt_peerid_helper_t::parse_progname_style_mldonkey(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return progname_from_tagstyle(s.substr(1,2), bt_peerid_style_t::MLDONKEY);
}

/** \brief Parse the progname for a bt_peerid_style_t::BITONWHEELS
 */
std::string	bt_peerid_helper_t::parse_progname_style_bitonwheels(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return progname_from_tagstyle(s.substr(1,4), bt_peerid_style_t::BITONWHEELS);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parse progvers main function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the string of the progvers for this bt_id_t peerid ("unknown" if not known)
 */
std::string	bt_peerid_helper_t::parse_progvers(const bt_id_t &peerid)	throw()
{
	// parse the bt_peerid_style_t for this peerid
	bt_peerid_style_t	peerid_style	= parse_style(peerid);
	// if the bt_peerid_style_t is not recognized, return unknown
	if( peerid_style == bt_peerid_style_t::NONE )	return "0";
	// handle it according to the bt_peerid_style_t
	switch( peerid_style.get_value() ){
	case bt_peerid_style_t::AZUREUS:	return parse_progvers_style_azureus(peerid);
	case bt_peerid_style_t::SHADOW:		return parse_progvers_style_shadow(peerid);
	case bt_peerid_style_t::MAINLINE:	return parse_progvers_style_mainline(peerid);
	case bt_peerid_style_t::XBT:		return parse_progvers_style_xbt(peerid);
	case bt_peerid_style_t::OPERA:		return parse_progvers_style_opera(peerid);
	case bt_peerid_style_t::MLDONKEY:	return parse_progvers_style_mldonkey(peerid);
	case bt_peerid_style_t::BITONWHEELS:	return parse_progvers_style_bitonwheels(peerid);
	default:	DBG_ASSERT(0);
	}
	return "";	// NOTE: just to avoid a compiler warning
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parse_progvers function for each style
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the progname for a bt_peerid_style_t::AZUREUS
 */
std::string	bt_peerid_helper_t::parse_progvers_style_azureus(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return decode_version_packed(s.substr(3,4));
}

/** \brief Parse the progname for a bt_peerid_style_t::SHADOW
 */
std::string	bt_peerid_helper_t::parse_progvers_style_shadow(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return decode_version_packed(s.substr(1,3));
}

/** \brief Parse the progname for a bt_peerid_style_t::MAINLINE
 */
std::string	bt_peerid_helper_t::parse_progvers_style_mainline(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	size_t	pos	= s.find('-');
	if( pos != std::string::npos && pos != s.size()-1 )	pos = s.find('-', pos + 1);
	if( pos != std::string::npos && pos != s.size()-1 )	pos = s.find('-', pos + 1);
	if( pos == std::string::npos )	return "0";
	return decode_version_dashed(s.substr(1,pos - 1));
}

/** \brief Parse the progname for a bt_peerid_style_t::XBT
 */
std::string	bt_peerid_helper_t::parse_progvers_style_xbt(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return decode_version_packed(s.substr(3,3));
}

/** \brief Parse the progname for a bt_peerid_style_t::OPERA
 */
std::string	bt_peerid_helper_t::parse_progvers_style_opera(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return decode_version_packed(s.substr(2,4));
}

/** \brief Parse the progname for a bt_peerid_style_t::MLDONKEY
 */
std::string	bt_peerid_helper_t::parse_progvers_style_mldonkey(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	size_t	pos	= s.find('-', 3);	
	if( pos == std::string::npos )	return "0";
	return decode_version_dotted(s.substr(3,pos - 3));
}

/** \brief Parse the progname for a bt_peerid_style_t::BITONWHEELS
 */
std::string	bt_peerid_helper_t::parse_progvers_style_bitonwheels(const bt_id_t &peerid)	throw()
{
	std::string s	= peerid.to_stdstring();
	return decode_version_packed(s.substr(5,2));
}

NEOIP_NAMESPACE_END;






